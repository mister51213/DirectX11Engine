#pragma once
#include "ShaderClass.h"

class WaterShaderClass :
	public ShaderClass
{
private:
	struct ReflectionBufferType
	{
		XMMATRIX reflection;
	};

	struct WaterBufferType
	{
		float waterTranslation;
		float reflectRefractScale;
		XMFLOAT2 padding;
	};

public:
	WaterShaderClass();
	WaterShaderClass(const WaterShaderClass&);
	~WaterShaderClass();

	bool Render(ID3D11DeviceContext * deviceContext, int indexCount,
		MatrixBufferType& transforms,
		//XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, 
		XMMATRIX reflectionMatrix, ID3D11ShaderResourceView** textureArray, vector<Microsoft::WRL::ComPtr <ID3D11ShaderResourceView>>& texViews, float waterTranslation, float reflectRefractScale);

private:
	bool InitializeShader(ID3D11Device*, HWND, char*, char*);
	bool SetShaderParameters(ID3D11DeviceContext* deviceContext,
		MatrixBufferType& transforms,
		//XMMATRIX worldMatrix, XMMATRIX viewMatrix,XMMATRIX projectionMatrix, 
		XMMATRIX reflectionMatrix, ID3D11ShaderResourceView** textureArray, vector<Microsoft::WRL::ComPtr <ID3D11ShaderResourceView>>& texViews, float waterTranslation, float reflectRefractScale);

private:
	const int _numBufferDescs = 3;
};

