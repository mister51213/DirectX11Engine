#pragma once
#include "ShaderClass.h"

class DepthShaderClass : public ShaderClass
{
//private:
//	struct MatrixBufferType
//	{
//		XMMATRIX world;
//		XMMATRIX view;
//		XMMATRIX projection;
//	};

public:
	DepthShaderClass();
	DepthShaderClass(const DepthShaderClass&);
	~DepthShaderClass();

	bool Render(ID3D11DeviceContext*, int, MatrixBufferType& transforms, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix);

private:
	bool InitializeShader(ID3D11Device*, HWND, char*, char*);
	bool SetShaderParameters(ID3D11DeviceContext*, MatrixBufferType& transforms, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix);
};