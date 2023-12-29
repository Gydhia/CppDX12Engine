#pragma once

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <string>
#include <Windows.h>
#include <d3d12.h>
#include <windows.h>
#include <dxgi1_4.h>
#include <unordered_map>
#include <memory>
#include "d3dUtil.h"
#include "Mesh.h"
#include "Shader.h"

// Link necessary d3d12 libraries.
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

struct TextureMaterial;
struct RenderItem;
using Microsoft::WRL::ComPtr;

class Engine
{
public:
    Engine(HINSTANCE hInstance);
    Engine(const Engine& rhs) = delete;
    Engine& operator=(const Engine& rhs) = delete;
    ~Engine();

    static Engine* GetEngine();

    bool OnInit();
    bool InitMainWindow();
    bool InitPipeline();

    int Run();
    void Draw();
    void Update();
    void OnDestroy();
    void FlushCommandQueue();

    void BuildDescriptorHeaps();
    void BuildShaders();
    void BuildShapeGeometry();
    void BuildRenderItems();
    void CreateTextureAndMaterial(std::string name);
    std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();

    void OnKeyDown(UINT8 /*key*/) {}
    void OnKeyUp(UINT8 /*key*/) {}

    // Accessors.
    UINT GetWidth() const { return m_width; }
    UINT GetHeight() const { return m_height; }
    const WCHAR* GetTitle() const { return m_title.c_str(); }


protected:
    static Engine* m_Engine;

    HWND MainWnd = nullptr;

    Microsoft::WRL::ComPtr<IDXGIFactory4> m_dxgiFactory;
    Microsoft::WRL::ComPtr<ID3D12Device> m_device;
    Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;

    Microsoft::WRL::ComPtr<ID3D12Fence> m_Fence;
    UINT64 m_CurrentFence = 0;

    Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_CommandQueue;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_DirectCmdListAlloc;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_CommandList;

    std::unordered_map<std::string, std::unique_ptr<TextureMaterial>> m_Materials;
    std::unordered_map<std::string, std::unique_ptr<Texture>> m_Textures;
    std::unordered_map<std::string, std::unique_ptr<Shader>> m_Shaders;

    int m_CurrBackBuffer = 0;
    Microsoft::WRL::ComPtr<ID3D12Resource> mSwapChainBuffer[2];
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_RtvHeap;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DsvHeap;

    D3D12_VIEWPORT m_ScreenViewport;
    D3D12_RECT m_ScissorRect;

    // Viewport dimensions.
    UINT m_width;
    UINT m_height;
    float m_aspectRatio;

    // Adapter info.
    bool m_useWarpDevice;

    void CreateCommandObjects();
    void CreateSwapChain();


    ID3D12Resource* CurrentBackBuffer()const;
    D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView()const;
    D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView()const;

private:
    ComPtr<ID3D12RootSignature> m_RootSignature = nullptr;
    ComPtr<ID3D12DescriptorHeap> m_DescHeap = nullptr;
    ComPtr<ID3D12DescriptorHeap> m_SrvDescriptorHeap = nullptr;

    std::unordered_map<std::string, std::unique_ptr<Mesh>> m_Meshes;
    std::vector<std::unique_ptr<RenderItem>> m_renderItems;

    UINT m_CbvSrvDescriptorSize = 0;

    UINT m_RtvDescriptorSize = 0;
    UINT m_DsvDescriptorSize = 0;
    UINT m_CbvSrvUavDescriptorSize = 0;


    XMFLOAT4X4 mWorld = MathHelper::Identity4x4();
    XMFLOAT4X4 mView = MathHelper::Identity4x4();
    XMFLOAT4X4 mProj = MathHelper::Identity4x4();

    XMFLOAT3 m_EyePos = { 0.0f, 0.0f, 0.0f };

    float m_Theta = 1.5f * XM_PI;
    float m_Phi = XM_PIDIV4;
    float m_Radius = 5.0f;

    POINT mLastMousePos;

    // Root assets path.
    std::wstring m_assetsPath;

    // Window title.
    std::wstring m_title;
};

Engine* Engine::m_Engine = nullptr;
Engine* Engine::GetEngine()
{
    return m_Engine;
}

ID3D12Resource* Engine::CurrentBackBuffer() const
{
    return mSwapChainBuffer[m_CurrBackBuffer].Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE Engine::CurrentBackBufferView() const
{
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(
        m_RtvHeap->GetCPUDescriptorHandleForHeapStart(),
        m_CurrBackBuffer,
        m_RtvDescriptorSize);
}

D3D12_CPU_DESCRIPTOR_HANDLE Engine::DepthStencilView()const
{
    return m_DsvHeap->GetCPUDescriptorHandleForHeapStart();
}