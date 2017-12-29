////////////////////////////////////////////////////////////////////////////////
// Filename: lightshaderinterface.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

/////////////
// GLOBALS //
/////////////
//const int NUM_LIGHTS = 4;
const int NUM_LIGHTS = 3;

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
	// cBuffer type that will be used with the vertex shader.
	// This typedef must be exactly the same as the one in the vertex shader as the model data needs to match the typedefs in the shader for proper rendering.
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMMATRIX lightView[NUM_LIGHTS];
		XMMATRIX lightProjection[NUM_LIGHTS];
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
		XMFLOAT4 diffuseColor2;
		XMFLOAT4 diffuseColor3;
		XMFLOAT3 lightDirection;
		float specularPower; // placed the specular power by the light direction to form a 4 float slot instead of using padding so that the structure could be kept in multiples of 16 bytes
		XMFLOAT4 specularColor;
			XMFLOAT4 diffuseCols[NUM_LIGHTS];
	};

	struct LightColorBufferType
	{
		XMFLOAT4 diffuseColor[NUM_LIGHTS];
	};

	struct LightPositionBufferType
	{
		XMFLOAT4 lightPosition[NUM_LIGHTS]; //@TODO: does this have to be the SAME NAME AS INSIDE SHADER????
	};
	struct LightShadowBufferType
	{
		//XMFLOAT3 lightShadowPos;
		//float padding;
		XMFLOAT3 lightShadowPos1;
		float padding1;
		
		XMFLOAT3 lightShadowPos2;
		float padding2;

		XMFLOAT3 lightShadowPos3;
		float padding3;

			XMFLOAT4 lightShadowPositions[NUM_LIGHTS];
	};
	struct FogBufferType
	{
		float fogStart;
		float fogEnd;
		float padding1, padding2;
	};
	//struct ClipPlaneBufferType
	//{
	//	XMFLOAT4 clipPlane;
	//};
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

	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,XMMATRIX projectionMatrix,
		/*XMMATRIX lightViewMatrix, XMMATRIX lightProjectionMatrix, */
		XMMATRIX lightViewMatrix[], XMMATRIX lightProjectionMatrix[],
		ID3D11ShaderResourceView** textureArray, XMFLOAT3 lightDirection, XMFLOAT4 ambientColor, XMFLOAT4 diffuseColor, XMFLOAT4 diffuseColor2,
		/*LightClass* shadowLight,*/LightClass* shadowLight[],
		/*LightClass* lights[],*/
		XMFLOAT3 cameraPosition, XMFLOAT4 specularColor, float specularPower, float fogStart, float fogEnd, float translation, float transparency);

	virtual void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount) override;

private:
	bool InitializeShader(ID3D11Device*, HWND, char*, char*);

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, 
		/*XMMATRIX lightViewMatrix, XMMATRIX lightProjectionMatrix,*/
		XMMATRIX lightViewMatrix[], XMMATRIX lightProjectionMatrix[],
		ID3D11ShaderResourceView** textureArray, XMFLOAT3 lightDirection, XMFLOAT4 ambientColor, XMFLOAT4 diffuseColor, XMFLOAT4 diffuseColor2,
		/*LightClass* shadowLight,*/LightClass* shadowLight[],
		/*LightClass* lights[],*/
		XMFLOAT3 cameraPosition, XMFLOAT4 specularColor, float specularPower, float fogStart, float fogEnd, float texTranslation, float transparency);

private:
	// Total number of buffers including parent matrix buffer
	const int _numBufferDescs = 8;
	ComPtr <ID3D11SamplerState> _sampleStateClamp;

};