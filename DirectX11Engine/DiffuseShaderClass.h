#pragma once
#include "ShaderClass.h"
class DiffuseShaderClass :
	public ShaderClass
{
private:
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
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, MatrixBufferType& transforms,
		ID3D11ShaderResourceView* texture, vector<Microsoft::WRL::ComPtr <ID3D11ShaderResourceView>>& texViews,
		XMFLOAT3 lightDirection, XMFLOAT4 ambientColor, XMFLOAT4 diffuseColor, XMFLOAT4 clipPlane);

//private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, char* vsFilename, char* psFilename);
	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, MatrixBufferType& transforms,
		ID3D11ShaderResourceView* texture, vector<Microsoft::WRL::ComPtr <ID3D11ShaderResourceView>>& texViews,
		XMFLOAT3 lightDirection, XMFLOAT4 ambientColor, XMFLOAT4 diffuseColor, XMFLOAT4 clipPlane);
};

