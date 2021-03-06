////////////////////////////////////////////////////////////////////////////////
// Filename: lightshaderinterface.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

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
	///////////////////////// VS BUFFER TYPES //////////////////////////
	struct SceneLightBufferType_VS
	{
		LightDataTemplate_VS lights[NUM_LIGHTS];
	};

	struct CameraBufferType
	{
		XMFLOAT3 cameraPosition;
		float padding;
	};

	struct FogBufferType
	{
		float fogStart;
		float fogEnd;
		float padding1, padding2;
	};

	///////////////////////// PS BUFFER TYPES //////////////////////////
	struct SceneLightBufferType_PS
	{
		XMFLOAT4 ambientColor;
		LightDataTemplate_PS lights[NUM_LIGHTS];
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
	virtual void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount) override;

	bool InitializeShader(ID3D11Device*, HWND, char*, char*);

	bool SetShaderParameters(ID3D11DeviceContext* deviceContext,MatrixBufferType& transforms, ID3D11ShaderResourceView** textureArray, 
		vector<Microsoft::WRL::ComPtr <ID3D11ShaderResourceView>>& texViews,XMFLOAT4 ambientColor, LightClass* shadowLight[], XMFLOAT3 cameraPosition, 
		float fogStart, float fogEnd, float translation, float transparency);

private:
	ComPtr <ID3D11SamplerState> _sampleStateClamp;
	ComPtr <ID3D11SamplerState> _sampleStateComp;
};