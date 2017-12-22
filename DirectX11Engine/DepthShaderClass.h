#pragma once
#include "ShaderClass.h"

class DepthShaderClass : public ShaderClass
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

public:
	DepthShaderClass();
	DepthShaderClass(const DepthShaderClass&);
	~DepthShaderClass();

	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX);

private:
	bool InitializeShader(ID3D11Device*, HWND, char*, char*);
	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX);

private:
	//ID3D11VertexShader* m_vertexShader;
	//ID3D11PixelShader* m_pixelShader;
	//ID3D11InputLayout* m_layout;
	//ID3D11Buffer* m_matrixBuffer;
};