#include <vector>
#include <string>
#include <wrl.h>
#include <DirectXMath.h>
#include <iostream>
#include <d3d12.h>
#include "d3dUtil.h"

using Microsoft::WRL::ComPtr;

struct ShaderData 
{
public: 
	std::string Name;
	Microsoft::WRL::ComPtr<ID3DBlob> m_vertexShader;
	Microsoft::WRL::ComPtr<ID3DBlob> m_pixelShader;
	std::vector<D3D12_INPUT_ELEMENT_DESC> m_InputLayout;
};
