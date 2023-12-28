#pragma once

#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl/client.h>

class SwapChain
{
public :
	SwapChain(HWND hwnd, ID3D12CommandQueue* cmdQue, UINT width, UINT height);
	~SwapChain();

	void CreateSwapChain();
	void Present();

	ID3D12Resource* GetCurrentBackBuffer() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferView() const;

private:
	Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain;
	Microsoft::WRL::ComPtr<ID3D12Resource> backBuffers[2];
	UINT currentBackBufferIndex;
};

