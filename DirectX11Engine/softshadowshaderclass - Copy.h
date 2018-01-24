////////////////////////////////////////////////////////////////////////////////
// Filename: SoftShadowShaderClass_Multi.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

#pragma comment ( lib, "d3dcompiler.lib" )
#pragma comment ( lib, "d3d11.lib" )

/////////////
// GLOBALS //
/////////////
const int NUM_LIGHTS = 3;

//////////////
// INCLUDES //
//////////////
#include <d3dcompiler.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <fstream>
#include "../ShaderClass.h"
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

public:
	SoftShadowShaderClass_Multi();
	SoftShadowShaderClass_Multi(const SoftShadowShaderClass_Multi&);

	bool Initialize(ID3D11Device*, HWND);
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, MatrixBufferType& transforms,
		vector<Microsoft::WRL::ComPtr <ID3D11ShaderResourceView>>& texViews,
		XMFLOAT4 ambientColor, LightClass* shadowLight[],
		XMFLOAT3 cameraPosition, float translation, float transparency);

private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd,/* WCHAR* */wstring vsFilename, /*WCHAR* */wstring psFilename);

	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, MatrixBufferType& transforms,
		vector<Microsoft::WRL::ComPtr <ID3D11ShaderResourceView>>& texViews,
		XMFLOAT4 ambientColor, LightClass* shadowLight[],
		XMFLOAT3 cameraPosition, float translation, float transparency);

	virtual void RenderShader(ID3D11DeviceContext*, int) override;

private:
	ComPtr <ID3D11SamplerState> _sampleStateClamp;
};