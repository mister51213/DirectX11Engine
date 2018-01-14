#pragma once
#include "ShaderClass.h"

class WaterShaderClass :
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

	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView** textureArray, vector<Microsoft::WRL::ComPtr <ID3D11ShaderResourceView>>& texViews, float, float);

private:
	bool InitializeShader(ID3D11Device*, HWND, char*, char*);
	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, XMMATRIX, 
		ID3D11ShaderResourceView** textureArray, vector<Microsoft::WRL::ComPtr <ID3D11ShaderResourceView>>& texViews, float, float);

private:
	const int _numBufferDescs = 3;
};

