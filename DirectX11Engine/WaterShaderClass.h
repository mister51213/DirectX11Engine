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

	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView** textureArray, /*ID3D11ShaderResourceView*,
		ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, */float, float);

private:
	bool InitializeShader(ID3D11Device*, HWND, char*, char*);
	void ShutdownShader();
	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, XMMATRIX, 
		ID3D11ShaderResourceView** textureArray, /*
		ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, */float, float);

private:
	//ID3D11VertexShader* _vertexShader;
	//ID3D11PixelShader* _pixelShader;
	//ID3D11InputLayout* _layout;
	//ID3D11SamplerState* _sampleState;
	//ID3D11Buffer* _matrixBuffer;

	ID3D11Buffer* _reflectionBuffer;
	ID3D11Buffer* _waterBuffer;
};

