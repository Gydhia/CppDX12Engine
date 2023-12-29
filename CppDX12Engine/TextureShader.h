#pragma once

#include "Shader.h"

class TextureShader : public Shader
{
public:
	TextureShader(const std::string& name);

protected:
	void buildShaderAndInput() override;
	void buildRootSignature(ComPtr<ID3D12Device> device) override;
};