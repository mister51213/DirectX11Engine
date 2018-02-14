#pragma once

#include "ShaderClass.h"

class WaterShaderClass : public ShaderClass
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
		float lerpRatio;
		float padding;
	};

public:
	bool InitializeShader(ID3D11Device*, HWND, char*, char*);

	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, MatrixBufferType& transforms, XMMATRIX reflectionMatrix, ID3D11ShaderResourceView** textureArray, 
		vector<ComPtr <ID3D11ShaderResourceView>>& texViews, float waterTranslation, float reflectRefractScale, float lerpRatio = .8f);
};

