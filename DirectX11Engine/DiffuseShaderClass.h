#pragma once
#include "ShaderClass.h"
class DiffuseShaderClass :
	public ShaderClass
{
private:
	// cBuffer type that will be used with the vertex shader - typedef must be exactly the same as the one in the vertex shader as the model data needs to match the typedefs in the shader for proper rendering.
	//struct MatrixBufferType
	//{
	//	XMMATRIX world;
	//	XMMATRIX view;
	//	XMMATRIX projection;
	//};

	struct LightBufferType
	{
		XMFLOAT4 ambientColor;
		XMFLOAT4 diffuseColor;
		XMFLOAT3 lightDirection;
		float padding;
	};

	struct ClipPlaneBufferType
	{
		XMFLOAT4 clipPlane;
	};

public:
	DiffuseShaderClass();
	DiffuseShaderClass(const DiffuseShaderClass& other);
	~DiffuseShaderClass();

	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix,XMMATRIX viewMatrix, XMMATRIX projectionMatrix, 
		ID3D11ShaderResourceView* texture, vector<Microsoft::WRL::ComPtr <ID3D11ShaderResourceView>>& texViews,
		XMFLOAT3 lightDirection, XMFLOAT4 ambientColor, XMFLOAT4 diffuseColor,
		XMFLOAT4 clipPlane);

private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, char* vsFilename, char* psFilename);
	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix,	XMMATRIX projectionMatrix, 
		ID3D11ShaderResourceView* texture, vector<Microsoft::WRL::ComPtr <ID3D11ShaderResourceView>>& texViews,
		XMFLOAT3 lightDirection, XMFLOAT4 ambientColor, XMFLOAT4 diffuseColor,
		XMFLOAT4 clipPlane);

private:
	const int _numBufferDescs = 3;
};

