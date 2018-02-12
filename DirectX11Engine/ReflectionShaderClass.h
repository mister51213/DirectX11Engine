#pragma once

#include "ShaderClass.h"

class ReflectionShaderClass : public ShaderClass
{
private:
	struct ReflectionBufferType
	{
		XMMATRIX reflectionMatrix;
	};

public:
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix,XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, 
		vector<Microsoft::WRL::ComPtr <ID3D11ShaderResourceView>>& texViews, ID3D11ShaderResourceView* reflectionTexture, XMMATRIX reflectionMatrix);

//private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, char* vsFilename, char* psFilename);

	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix,	XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, 
		vector<Microsoft::WRL::ComPtr <ID3D11ShaderResourceView>>& texViews, ID3D11ShaderResourceView* reflectionTexture, XMMATRIX reflectionMatrix);
};

