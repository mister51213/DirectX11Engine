#pragma once
#include "ShaderClass.h"

class DepthShaderClass : public ShaderClass
{
public:
	bool Render(ID3D11DeviceContext*, int, MatrixBufferType& transforms);

//private:
	virtual bool InitializeShader(ID3D11Device*, HWND, char*, char*) override;

	bool SetShaderParameters(ID3D11DeviceContext*, MatrixBufferType& transforms);
};