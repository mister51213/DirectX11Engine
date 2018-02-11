////////////////////////////////////////////////////////////////////////////////
// Filename: SoftShadowShaderClass_Multi.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

//#pragma comment ( lib, "d3dcompiler.lib" )
//#pragma comment ( lib, "d3d11.lib" )

//////////////
// INCLUDES //
//////////////
#include <d3dcompiler.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <fstream>
#include "ShaderClass.h"
#include "LightClass.h"

using namespace std;
using namespace DirectX;

////////////////////////////////////////////////////////////////////////////////
// Class name: SoftShadowShaderClass_Multi
////////////////////////////////////////////////////////////////////////////////
class SoftShadowShaderClass_Multi:public ShaderClass //ShaderBase
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

	// All in one tex param type
	struct TexParamBufferType
	{
		float translation = 0.f;
		float blendAmount = 1.f;
		float gamma = 1.f;
		unsigned int bBlendTexture;
	};

public:
	bool Initialize(ID3D11Device*, HWND);

	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, MatrixBufferType& transforms, vector<Microsoft::WRL::ComPtr <ID3D11ShaderResourceView>>& texViews,
		XMFLOAT4 ambientColor, LightClass* shadowLight[], XMFLOAT3 cameraPosition, float translation, float transparency, float gamma = 1.f, unsigned int bBlendTexture = 0);

	virtual bool InitializeShader(ID3D11Device* device, HWND hwnd,/* WCHAR* */wstring vsFilename, /*WCHAR* */wstring psFilename);

private:
	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, MatrixBufferType& transforms, vector<Microsoft::WRL::ComPtr <ID3D11ShaderResourceView>>& texViews,
		XMFLOAT4 ambientColor, LightClass* shadowLight[], XMFLOAT3 cameraPosition, float translation, float transparency, float gamma = 1.f, unsigned int bBlendTexture = 0);

	virtual void RenderShader(ID3D11DeviceContext*, int) override;

private:
	ComPtr <ID3D11SamplerState> _sampleStateClamp;
};