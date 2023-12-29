#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <DirectXMath.h>
#include <memory>
#include <string>

using namespace DirectX;
using namespace Microsoft::WRL;

struct Vertex
{
    DirectX::XMFLOAT3 Position;
    DirectX::XMFLOAT2 UV;

    Vertex(
        const DirectX::XMFLOAT3& p,
        const DirectX::XMFLOAT2& uv) :
        Position(p),
        UV(uv) {}
    Vertex(
        float px, float py, float pz,
        float u, float v) :
        Position(px, py, pz),
        UV(u, v) {}
};


class Mesh
{
public:
    Mesh();
    ~Mesh();
    void Update() {};
    void Clear();
    static std::unique_ptr<Mesh> CreateQuad(ComPtr<ID3D12Device>&, ComPtr <ID3D12GraphicsCommandList>&);
    void Draw(ComPtr<ID3D12GraphicsCommandList> commandList, XMMATRIX* posMatrix);

    std::string Name;
    int IndexCount = 0;
    int VertexCount = 0;


    D3D12_VERTEX_BUFFER_VIEW VertexBufferView()const
    {
        D3D12_VERTEX_BUFFER_VIEW vbv;
        vbv.BufferLocation = v_bufferGPU->GetGPUVirtualAddress();
        vbv.StrideInBytes = VertexByteStride;
        vbv.SizeInBytes = VertexBufferByteSize;

        return vbv;
    }

    D3D12_INDEX_BUFFER_VIEW IndexBufferView()const
    {
        D3D12_INDEX_BUFFER_VIEW ibv;
        ibv.BufferLocation = i_bufferGPU->GetGPUVirtualAddress();
        ibv.Format = IndexFormat;
        ibv.SizeInBytes = IndexBufferByteSize;

        return ibv;
    }
    
    // Data about buffers
    UINT VertexByteStride = 0;
    UINT VertexBufferByteSize = 0;
    DXGI_FORMAT IndexFormat = DXGI_FORMAT_R16_UINT;
    UINT IndexBufferByteSize = 0;

    ComPtr<ID3D12Resource> v_bufferGPU = nullptr;
    ComPtr<ID3D12Resource> i_bufferGPU = nullptr;

    ComPtr<ID3DBlob> v_bufferCPU = nullptr;
    ComPtr<ID3DBlob> i_bufferCPU = nullptr;

    ComPtr<ID3D12Resource> v_bufferUploader = nullptr;
    ComPtr<ID3D12Resource> i_bufferUploader = nullptr;

    D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
};
