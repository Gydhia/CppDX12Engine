#include "Mesh.h"
#include <d3d12.h>
#include <wrl.h>
#include <DirectXMath.h>

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
	v_buffer.Reset();
	i_buffer.Reset();
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
