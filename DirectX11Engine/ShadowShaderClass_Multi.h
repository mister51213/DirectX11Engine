////////////////////////////////////////////////////////////////////////////////
// Filename: ShadowShaderClass_Multi.h
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
//#include "QUARANTINE/ShaderBase.h"
#include "ShaderClass.h"
#include "LightClass.h"

using namespace std;
using namespace DirectX;

////////////////////////////////////////////////////////////////////////////////
// Class name: ShadowShaderClass_Multi
////////////////////////////////////////////////////////////////////////////////
class ShadowShaderClass_Multi : public ShaderClass
{
private:
	///////////////////////// VS BUFFER TYPES //////////////////////////
	struct SceneLightBufferType_VS
	{
		LightDataTemplate_VS lights[NUM_LIGHTS];
	};

	///////////////////////// PS BUFFER TYPES //////////////////////////
	struct SceneLightBufferType_PS
	{
		XMFLOAT4 ambientColor;
		LightDataTemplate_PS lights[NUM_LIGHTS];
	};

public:
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, MatrixBufferType transforms, LightClass* shadowLight[], vector<Microsoft::WRL::ComPtr <ID3D11ShaderResourceView>>& texViews);

	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);

private:
	bool SetShaderParameters(ID3D11DeviceContext* device, MatrixBufferType transforms, LightClass* shadowLight[], vector<Microsoft::WRL::ComPtr <ID3D11ShaderResourceView>>& texViews);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ComPtr <ID3D11SamplerState> _sampleStateClamp;
};