#pragma once

#include <string>
#include <wrl.h>
#include <DirectXMath.h>
#include <vector>
#include <d3d12.h>
#include "d3dx12.h"
#include "d3dUtil.h"

using Microsoft::WRL::ComPtr;

class Shader
{
public:
	virtual ~Shader() = default;
	Shader(std::string name);

	void BuildShader(ComPtr<ID3D12Device> device);
	
	std::string Name;

	ComPtr<ID3D12RootSignature> GetRootSignature() { return m_rootSignature; }
	ComPtr<ID3D12PipelineState> GetPSO() { return m_pso; }
	ComPtr<ID3DBlob> GetVertexShader() { return m_vertexShader; }
	ComPtr<ID3DBlob> GetPixelShader() { return m_pixelShader; }
	std::vector<D3D12_INPUT_ELEMENT_DESC> GetInputLayout() { return m_inputLayout; }

protected:
	ComPtr<ID3D12RootSignature> m_rootSignature;
	ComPtr<ID3D12PipelineState> m_pso;
	ComPtr<ID3DBlob> m_vertexShader;
	ComPtr<ID3DBlob> m_pixelShader;
	std::vector<D3D12_INPUT_ELEMENT_DESC> m_inputLayout;

	virtual void buildShaderAndInput() = 0;

	virtual void buildRootSignature(ComPtr<ID3D12Device> device) = 0;

	static std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();

private : 
	void _buildPSO(ComPtr<ID3D12Device> device);
};