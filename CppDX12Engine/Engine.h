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

// Link necessary d3d12 libraries.
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")


class Engine
{
public:
    Engine(UINT width, UINT height, std::wstring name);

    void OnInit();
    void OnUpdate();
    void OnRender();
    void OnDestroy();

    void BuildDescriptorHeaps();
    void BuildConstantBuffers();
    void BuildRootSignature();
    void BuildShadersAndInputLayout();
    void BuildBoxGeometry();
    void BuildRenderItems();
    void BuildPSO();
    void CreateTextureAndMaterial(std::string name);

    void OnKeyDown(UINT8 /*key*/) {}
    void OnKeyUp(UINT8 /*key*/) {}

    // Accessors.
    UINT GetWidth() const { return m_width; }
    UINT GetHeight() const { return m_height; }
    const WCHAR* GetTitle() const { return m_title.c_str(); }


protected:
    static Engine* m_Engine;

    Microsoft::WRL::ComPtr<IDXGIFactory4> m_dxgiFactory;
    Microsoft::WRL::ComPtr<ID3D12Device> m_device;
    Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;

    Microsoft::WRL::ComPtr<ID3D12Fence> m_Fence;
    UINT64 mCurrentFence = 0;

    Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_CommandQueue;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_DirectCmdListAlloc;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_CommandList;

    std::unordered_map<std::string, std::unique_ptr<Material>> mMaterials;
    std::unordered_map<std::string, std::unique_ptr<Texture>> m_Textures;
    std::unordered_map<std::string, ComPtr<ID3DBlob>> mShaders;


    // Viewport dimensions.
    UINT m_width;
    UINT m_height;
    float m_aspectRatio;

    // Adapter info.
    bool m_useWarpDevice;

private:
    ComPtr<ID3D12RootSignature> m_RootSignature = nullptr;
    ComPtr<ID3D12DescriptorHeap> m_DescHeap = nullptr;
    ComPtr<ID3D12PipelineState> m_PSO = nullptr;
    ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap = nullptr;
    ComPtr<ID3D12PipelineState> m_OpaquePSO = nullptr;

    std::vector<D3D12_INPUT_ELEMENT_DESC> m_InputLayout;

    UINT m_CbvSrvDescriptorSize = 0;


    // Root assets path.
    std::wstring m_assetsPath;

    // Window title.
    std::wstring m_title;
};
