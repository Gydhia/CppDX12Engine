#include "Mesh.h"
#include <d3d12.h>
#include <wrl.h>
#include <DirectXMath.h>
#include "d3dUtil.h"

using namespace Microsoft::WRL;
using namespace DirectX;

Mesh::Mesh()
{

}

Mesh::~Mesh()
{
	Clear();
}

void Mesh::Clear()
{
	// Release the resources using ComPtr's automatic Release method
	v_bufferGPU.Reset();
	i_bufferGPU.Reset();
	v_bufferUploader.Reset();
	i_bufferUploader.Reset();
}

void Mesh::SetTexture(ComPtr<ID3D12Device> device, LPCWSTR source)
{
	// For simplicity, texture creation is not provided here.
	// You need to implement texture loading for DirectX 12 separately.
	// Consider using DirectXTex or another texture loading library for DirectX 12.
}

void Mesh::Draw(ComPtr<ID3D12GraphicsCommandList> commandList, XMMATRIX* posMatrix)
{
	// Assuming you have created and recorded commands into the command list elsewhere
	/*

	// Set necessary states and resources
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(0, 1, &v_buffer);
	commandList->IASetIndexBuffer(&i_buffer);

	// Assuming you have created and set the appropriate descriptor heap for the texture
	commandList->SetGraphicsRootDescriptorTable(,);

	// Set transformation matrix
	commandList->SetGraphicsRoot32BitConstants(,);

	// Draw indexed primitives
	commandList->DrawIndexedInstanced(m_indexCount, 1, 0, 0, 0);

	*/
}

std::unique_ptr<Mesh> Mesh::CreateQuad(ComPtr<ID3D12Device>& device, ComPtr<ID3D12GraphicsCommandList>& clist)
{
	auto mesh = std::make_unique<Mesh>();

	// Create the vertices
	std::vector<Vertex> vertices;

	mesh->VertexCount = 4;
	vertices.push_back(Vertex(0, 0, 0, 0, 1));
	vertices.push_back(Vertex(0, 1, 0, 0, 1));
	vertices.push_back(Vertex(1, 1, 0, 0, 1));
	vertices.push_back(Vertex(1, 0, 0, 0, 1));

	// Create indices
	std::vector<std::uint16_t> indices;

	mesh->IndexCount = 4;
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(0);
	indices.push_back(2);
	indices.push_back(3);

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	mesh->Name = "base_quad";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &mesh->v_bufferCPU));
	CopyMemory(mesh->v_bufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &mesh->i_bufferCPU));
	CopyMemory(mesh->i_bufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	mesh->v_bufferGPU = d3dUtil::CreateDefaultBuffer(device.Get(),
		clist.Get(), vertices.data(), vbByteSize, mesh->v_bufferUploader);

	mesh->i_bufferGPU = d3dUtil::CreateDefaultBuffer(device.Get(),
		clist.Get(), indices.data(), ibByteSize, mesh->i_bufferUploader);

	mesh->VertexByteStride = sizeof(Vertex);
	mesh->VertexBufferByteSize = vbByteSize;
	mesh->IndexFormat = DXGI_FORMAT_R16_UINT;
	mesh->IndexBufferByteSize = ibByteSize;

	return mesh;
}