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

	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView** textureArray, float, float);

private:
	bool InitializeShader(ID3D11Device*, HWND, char*, char*);
	void ShutdownShader();
	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, XMMATRIX, 
		ID3D11ShaderResourceView** textureArray, float, float);

private:
	const int _numBufferDescs = 3;

//	ID3D11Buffer* _reflectionBuffer;
//	ID3D11Buffer* _waterBuffer;
};

