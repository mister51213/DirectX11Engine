////////////////////////////////////////////////////////////////////////////////
// Filename: lightshaderinterface.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

/////////////
// GLOBALS //
/////////////
const int NUM_LIGHTS = 4;

//////////////
// INCLUDES //
//////////////
#include "ShaderClass.h"
#include "LightClass.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: LightShaderClass
////////////////////////////////////////////////////////////////////////////////
class LightShaderClass: public ShaderClass
{
private:
	struct ClipPlaneBufferType
	{
		XMFLOAT4 clipPlane;
	};
	struct CameraBufferType
	{
		XMFLOAT3 cameraPosition;
		float padding;
	};
	struct LightBufferType
	{
		XMFLOAT4 ambientColor;
		XMFLOAT4 diffuseColor;
		XMFLOAT3 lightDirection;
		float specularPower; // placed the specular power by the light direction to form a 4 float slot instead of using padding so that the structure could be kept in multiples of 16 bytes
		XMFLOAT4 specularColor;
	};
	struct LightColorBufferType
	{
		XMFLOAT4 diffuseColor[NUM_LIGHTS];
	};

	struct LightPositionBufferType
	{
		XMFLOAT4 lightPosition[NUM_LIGHTS]; //@TODO: does this have to be the SAME NAME AS INSIDE SHADER????
	};
	struct FogBufferType
	{
		float fogStart;
		float fogEnd;
		float padding1, padding2;
	};
	struct TranslateBufferType
	{
		float translation;
		XMFLOAT3 padding;
	};
	struct TransparentBufferType
	{
		float blendAmount;
		XMFLOAT3 padding;
	};

public:
	LightShaderClass();
	LightShaderClass(const LightShaderClass&);
	~LightShaderClass();

	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
		XMMATRIX projectionMatrix, ID3D11ShaderResourceView** textureArray, XMFLOAT3 lightDirection, XMFLOAT4 ambientColor, XMFLOAT4 diffuseColor, 
		LightClass* lights[],
		XMFLOAT3 cameraPosition, XMFLOAT4 specularColor, float specularPower, float fogStart, float fogEnd, XMFLOAT4 clipPlane, float translation, float transparency);

private:
	bool InitializeShader(ID3D11Device*, HWND, char*, char*);
	virtual void ShutdownShader() override;

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView** textureArray, XMFLOAT3 lightDirection, XMFLOAT4 ambientColor, XMFLOAT4 diffuseColor,
		LightClass* lights[],
		XMFLOAT3 cameraPosition, XMFLOAT4 specularColor, float specularPower, float fogStart, float fogEnd, XMFLOAT4 clipPlane, float texTranslation, float transparency);

private:
	ID3D11Buffer* _cameraBuffer;
	ID3D11Buffer* _lightBuffer;
		ID3D11Buffer* _lightColorBuffer;	// NEW
		ID3D11Buffer* _lightPositionBuffer;	// NEW
	ID3D11Buffer* _fogBuffer;
	ID3D11Buffer* _clipPlaneBuffer;
	ID3D11Buffer* _translateBuffer;
	ID3D11Buffer* _transparentBuffer;
};