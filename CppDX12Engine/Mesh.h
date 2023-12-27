#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <DirectXMath.h>

using namespace DirectX;
using namespace Microsoft::WRL;

class Mesh
{
protected:
    ComPtr<ID3D12Resource> texture;
    int m_indexCount = 0;
    int m_vertexCount = 0;
    ComPtr<ID3D12Resource> v_buffer;
    ComPtr<ID3D12Resource> i_buffer;

public:
    Mesh();
    ~Mesh();
    void Update() {};
    void Clear();
    void SetTexture(ComPtr<ID3D12Device> device, LPCWSTR source);
    void Draw(ComPtr<ID3D12GraphicsCommandList> commandList, XMMATRIX* posMatrix);
};
