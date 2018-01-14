#pragma once
#include "ShaderClass.h"
class ReflectionShaderClass : public ShaderClass
{
private:
	// cBuffer type that will be used with the vertex shader - typedef must be exactly the same as the one in the vertex shader as the model data needs to match the typedefs in the shader for proper rendering.
	//struct MatrixBufferType
	//{
	//	XMMATRIX world;
	//	XMMATRIX view;
	//	XMMATRIX projection;
	//};

	struct ReflectionBufferType
	{
		XMMATRIX reflectionMatrix;
	};

public:
	ReflectionShaderClass();
	ReflectionShaderClass(const ReflectionShaderClass&);
	~ReflectionShaderClass();

bool Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix,
	XMMATRIX viewMatrix, XMMATRIX projectionMatrix, 
	ID3D11ShaderResourceView* texture, vector<Microsoft::WRL::ComPtr <ID3D11ShaderResourceView>>& texViews,
	ID3D11ShaderResourceView* reflectionTexture, XMMATRIX reflectionMatrix);

private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, char* vsFilename, char* psFilename);

	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
		XMMATRIX projectionMatrix, 
		ID3D11ShaderResourceView* texture, vector<Microsoft::WRL::ComPtr <ID3D11ShaderResourceView>>& texViews,
		ID3D11ShaderResourceView* reflectionTexture, XMMATRIX reflectionMatrix);
};

