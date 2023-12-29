#include <wrl.h>
#include <DirectXMath.h>
#include "Engine.h"
#include "Mesh.h"
#include <iostream>
#include "Shader.h"
#include "TextureShader.h"

#define MAX_NAME_STRING 256
#define HInstance() GetModuleHandle(NULL)

WCHAR			WindowClass[MAX_NAME_STRING];
WCHAR			WindowTitle[MAX_NAME_STRING];

INT				WindowWidth;
INT				WindowHeight;

using Microsoft::WRL::ComPtr;

struct ObjectConstants
{
	DirectX::XMFLOAT4X4 World = MathHelper::Identity4x4();
};


struct TextureMaterial
{
	TextureMaterial() = default;

	std::string Name;

	Shader* Shader = nullptr;
	Texture* Texture = nullptr;
};

struct RenderItem
{
	RenderItem() = default;

	std::string Name;

	DirectX::XMFLOAT4X4 World = MathHelper::Identity4x4();

	TextureMaterial* Mat = nullptr;
	Mesh* Mesh = nullptr;
};

Engine::Engine(HINSTANCE hInstance)
{
}

Engine::~Engine()
{
	if (m_device != nullptr)
		FlushCommandQueue();
}

int WINAPI main(HINSTANCE, HINSTANCE hInstance, LPSTR, INT)
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	wcscpy_s(WindowClass, TEXT("TutorialOneClass"));
	wcscpy_s(WindowTitle, TEXT("Our First Window"));
	
	try
	{
		Engine theApp(hInstance);
		if (!theApp.OnInit())
			return 0;

		return theApp.Run();
	}
	catch (DxException& e)
	{
		MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
		return 0;
	}

	return 0;
}


int Engine::Run()
{
	MSG msg = { 0 };

	while (msg.message != WM_QUIT)
	{
		// If there are Window messages then process them.
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// Otherwise, do animation/game stuff.
		else
		{
			Update();
			Draw();
		}
	}

	return (int)msg.wParam;
}


bool Engine::OnInit()
{
	if (!InitMainWindow())
		return false;
	
	if(!InitPipeline())
		return false;
	
	try 
	{
		ThrowIfFailed(m_CommandList->Reset(m_DirectCmdListAlloc.Get(), nullptr));

		m_CbvSrvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		BuildDescriptorHeaps();
		BuildShaders();
		BuildShapeGeometry();
		BuildRenderItems();

		// Execute the initialization commands.
		ThrowIfFailed(m_CommandList->Close());
		ID3D12CommandList* cmdsLists[] = { m_CommandList.Get() };
		m_CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

		// Wait until initialization is complete.
		FlushCommandQueue();
	}
	catch (DxException& e)
	{
		// Log the exception details (you can modify this based on your logging mechanism)
		std::cout << "Exception: " << e.ToString().c_str() << " | " << e.FunctionName.c_str() << " | " << e.LineNumber << std::endl;
		
		return false;
	}
	
	return true;
}

bool Engine::InitMainWindow()
{
	WindowWidth = 1366;
	WindowHeight = 768;

	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = DefWindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = HInstance();
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"MainWnd";

	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}

	// Compute window rectangle dimensions based on requested client area dimensions.
	RECT R = { 0, 0, WindowWidth, WindowHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	MainWnd = CreateWindow(L"MainWnd", WindowTitle,
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, HInstance(), 0);

	if (!MainWnd)
	{
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	ShowWindow(MainWnd, SW_SHOW);
	UpdateWindow(MainWnd);

	return true;
}

bool Engine::InitPipeline() 
{
	#if defined(DEBUG) || defined(_DEBUG) 
		// Enable the D3D12 debug layer.
		{
			ComPtr<ID3D12Debug> debugController;
			ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
			debugController->EnableDebugLayer();
		}
	#endif

	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&m_dxgiFactory)));

	// Try to create hardware device.
	HRESULT hardwareResult = D3D12CreateDevice(
		nullptr,             // default adapter
		D3D_FEATURE_LEVEL_12_0,
		IID_PPV_ARGS(&m_device));

	// Fallback to WARP device.
	if (FAILED(hardwareResult))
	{
		ComPtr<IDXGIAdapter> pWarpAdapter;
		ThrowIfFailed(m_dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));

		ThrowIfFailed(D3D12CreateDevice(
			pWarpAdapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&m_device)));
	}

	ThrowIfFailed(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(&m_Fence)));

	m_RtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_DsvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	m_CbvSrvUavDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// Check 4X MSAA quality support for our back buffer format.
	// All Direct3D 11 capable devices support 4X MSAA for all render 
	// target formats, so we only need to check quality support.

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
	msQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	msQualityLevels.SampleCount = 4;
	msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	msQualityLevels.NumQualityLevels = 0;
	ThrowIfFailed(m_device->CheckFeatureSupport(
		D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&msQualityLevels,
		sizeof(msQualityLevels)));

	CreateCommandObjects();
	CreateSwapChain();

	return true;
}

void Engine::CreateCommandObjects()
{
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_CommandQueue)));

	ThrowIfFailed(m_device->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(m_DirectCmdListAlloc.GetAddressOf())));

	ThrowIfFailed(m_device->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		m_DirectCmdListAlloc.Get(), // Associated command allocator
		nullptr,                   // Initial PipelineStateObject
		IID_PPV_ARGS(m_CommandList.GetAddressOf())));

	// Start off in a closed state.  This is because the first time we refer 
	// to the command list we will Reset it, and it needs to be closed before
	// calling Reset.
	m_CommandList->Close();
}
void Engine::CreateSwapChain()
{
	// Release the previous swapchain we will be recreating.
	m_swapChain.Reset();
	
	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = WindowWidth;
	sd.BufferDesc.Height = WindowHeight;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 2;
	sd.OutputWindow = MainWnd;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// Note: Swap chain uses queue to perform flush.
	ThrowIfFailed(m_dxgiFactory->CreateSwapChain(
		m_CommandQueue.Get(),
		&sd,
		m_swapChain.GetAddressOf()));
}

void Engine::Update() 
{
	// Convert Spherical to Cartesian coordinates.
	m_EyePos.x = m_Radius * sinf(m_Phi) * cosf(m_Theta);
	m_EyePos.z = m_Radius * sinf(m_Phi) * sinf(m_Theta);
	m_EyePos.y = m_Radius * cosf(m_Phi);

	// Build the view matrix.
	XMVECTOR pos = XMVectorSet(m_EyePos.x, m_EyePos.y, m_EyePos.z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, view);

	// Update materials
	for (auto& e : m_renderItems)
	{
		XMMATRIX world = XMLoadFloat4x4(&e->World);

		ObjectConstants objConstants;
		XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(world));
	}
}

void Engine::Draw()
{
	// Reset the command list and set necessary states
	ThrowIfFailed(m_CommandList->Reset(m_DirectCmdListAlloc.Get(), nullptr));

	// Set the viewport and scissor rectangle
	m_CommandList->RSSetViewports(1, &m_ScreenViewport);
	m_CommandList->RSSetScissorRects(1, &m_ScissorRect);

	// Transition the swap chain back buffer to the render target state
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	m_CommandList->ResourceBarrier(1, &barrier);

	// Clear the back buffer and depth buffer
	m_CommandList->ClearRenderTargetView(CurrentBackBufferView(), Colors::LightSteelBlue, 0, nullptr);
	m_CommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	// Set the render target and depth/stencil buffer
	D3D12_CPU_DESCRIPTOR_HANDLE cBBView = CurrentBackBufferView();
	D3D12_CPU_DESCRIPTOR_HANDLE dSView = DepthStencilView();
	m_CommandList->OMSetRenderTargets(1, &cBBView, true, &dSView);

	// Iterate through each RenderItem
	for (const auto& renderItem : m_renderItems)
	{
		// Set pipeline state and root signature for the RenderItem's material
		m_CommandList->SetPipelineState(renderItem->Mat->Shader->GetPSO().Get());
		m_CommandList->SetGraphicsRootSignature(renderItem->Mat->Shader->GetRootSignature().Get());

		// Set descriptor heaps for the RenderItem's material
		ID3D12DescriptorHeap* heaps[] = { m_SrvDescriptorHeap.Get() };
		m_CommandList->SetDescriptorHeaps(_countof(heaps), heaps);

		// Update constant buffers or other shader resources if needed
		// For example, you might update the world matrix constant buffer here

		// Iterate through each mesh in the RenderItem
		D3D12_VERTEX_BUFFER_VIEW vBView = renderItem->Mesh->VertexBufferView();
		m_CommandList->IASetVertexBuffers(0, 1, &vBView);
		D3D12_INDEX_BUFFER_VIEW iBView = renderItem->Mesh->IndexBufferView();
		m_CommandList->IASetIndexBuffer(&iBView);
		m_CommandList->IASetPrimitiveTopology(renderItem->Mesh->PrimitiveType);

		// Draw the mesh
		m_CommandList->DrawIndexedInstanced(renderItem->Mesh->IndexCount, 1, 0, 0, 0);
	}

	// Transition the swap chain back buffer to the present state
	barrier = CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	m_CommandList->ResourceBarrier(1, &barrier);

	// Close the command list
	ThrowIfFailed(m_CommandList->Close());

	// Execute the command list
	ID3D12CommandList* cmdLists[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);

	// Present the frame
	ThrowIfFailed(m_swapChain->Present(0, 0));

	// Advance the fence value to mark the current frame
	m_CurrentFence++;

	// Wait for the GPU to finish rendering this frame
	FlushCommandQueue();
}

void Engine::BuildDescriptorHeaps()
{
	// Create the SRV heap used localy
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = 1;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	ThrowIfFailed(m_device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_SrvDescriptorHeap)));

	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = 2;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	ThrowIfFailed(m_device->CreateDescriptorHeap(
		&rtvHeapDesc, IID_PPV_ARGS(m_RtvHeap.GetAddressOf())));


	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	ThrowIfFailed(m_device->CreateDescriptorHeap(
		&dsvHeapDesc, IID_PPV_ARGS(m_DsvHeap.GetAddressOf())));

	// Create materials and fill the heap desc with it 
	Engine::CreateTextureAndMaterial("Wood");
	//Engine::CreateTextureAndMaterial("Cobble");
}

void Engine::CreateTextureAndMaterial(std::string name)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(m_SrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	// ----- Load the texture
	auto newTexture = std::make_unique<Texture>();
	newTexture->Name = name;
	newTexture->Filename = L"Textures/" + std::wstring(name.begin(), name.end()) + L".dds";

	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(m_device.Get(),
		m_CommandList.Get(), newTexture->Filename.c_str(),
		newTexture->Resource, newTexture->UploadHeap));

	// ----- Create the texture
	auto textResource = newTexture->Resource;
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDescCobble = {};
	srvDescCobble.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDescCobble.Format = textResource->GetDesc().Format;
	srvDescCobble.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDescCobble.Texture2D.MostDetailedMip = 0;
	srvDescCobble.Texture2D.MipLevels = textResource->GetDesc().MipLevels;
	srvDescCobble.Texture2D.ResourceMinLODClamp = 0.0f;

	m_device->CreateShaderResourceView(textResource.Get(), &srvDescCobble, hDescriptor);

	// ----- Create material
	auto textMaterial = std::make_unique<TextureMaterial>();
	textMaterial->Name = name;
	textMaterial->Texture = newTexture.get();
	textMaterial->Shader = m_Shaders["TextureShader"].get();

	m_Textures[name] = std::move(newTexture);
	m_Materials[name] = std::move(textMaterial);
}

void Engine::BuildShapeGeometry() 
{
	auto mesh = Mesh::CreateQuad(m_device, m_CommandList);
	m_Meshes[mesh->Name] = std::move(mesh);
}

void Engine::BuildShaders()
{
	auto texShader = std::make_unique<TextureShader>("TextureShader");
	texShader.get()->BuildShader(m_device);
	m_Shaders["TextureShader"] = std::move(texShader);
}

void Engine::BuildRenderItems() 
{
	auto woodQuad = std::make_unique<RenderItem>();
	woodQuad->Name = "WoodQuad";
	woodQuad->Mat = m_Materials["Wood"].get();
	woodQuad->Mesh = m_Meshes["base_quad"].get();
	m_renderItems.push_back(std::move(woodQuad));

	auto cobbleQuad = std::make_unique<RenderItem>();
	cobbleQuad->Name = "CobbleQuad";
	cobbleQuad->Mat = m_Materials["Cobble"].get();
	cobbleQuad->Mesh = m_Meshes["base_quad"].get();
	m_renderItems.push_back(std::move(cobbleQuad));
}

void Engine::FlushCommandQueue()
{
	// Advance the fence value to mark commands up to this fence point.
	m_CurrentFence++;

	// Add an instruction to the command queue to set a new fence point.  Because we 
	// are on the GPU timeline, the new fence point won't be set until the GPU finishes
	// processing all the commands prior to this Signal().
	ThrowIfFailed(m_CommandQueue->Signal(m_Fence.Get(), m_CurrentFence));

	// Wait until the GPU has completed commands up to this fence point.
	if (m_Fence->GetCompletedValue() < m_CurrentFence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);

		// Fire event when GPU hits current fence.  
		ThrowIfFailed(m_Fence->SetEventOnCompletion(m_CurrentFence, eventHandle));

		// Wait until the GPU hits current fence event is fired.
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}
}