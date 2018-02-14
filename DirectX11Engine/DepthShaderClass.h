#pragma once

#include "ShaderClass.h"

class DepthShaderClass : public ShaderClass
{
public:
	virtual bool InitializeShader(ID3D11Device*, HWND, char*, char*) override;

	bool SetShaderParameters(ID3D11DeviceContext*, MatrixBufferType& transforms);
};