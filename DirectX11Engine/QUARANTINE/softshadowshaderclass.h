////////////////////////////////////////////////////////////////////////////////
// Filename: softshadowshaderclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _SOFTSHADOWSHADERCLASS_H_
#define _SOFTSHADOWSHADERCLASS_H_

#pragma comment ( lib, "d3dcompiler.lib" )
#pragma comment ( lib, "d3d11.lib" )

//////////////
// INCLUDES //
//////////////
#include <d3dcompiler.h>
#include <d3d11.h>
#include <DirectXMath.h>
//#include <d3dx11async.h>
#include <fstream>
//#include "ShaderBase.h"
#include "../ShaderClass.h"

using namespace std;
using namespace DirectX;


////////////////////////////////////////////////////////////////////////////////
// Class name: SoftShadowShaderClass
////////////////////////////////////////////////////////////////////////////////
class SoftShadowShaderClass:public ShaderClass //ShaderBase
{
private:
	struct LightBufferType
	{
		XMFLOAT4 ambientColor;
		XMFLOAT4 diffuseColor;
	};

	struct LightBufferType2
	{
		XMFLOAT3 lightPosition;
		float padding;
	};

public:
	SoftShadowShaderClass();
	SoftShadowShaderClass(const SoftShadowShaderClass&);

	bool Initialize(ID3D11Device*, HWND);
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
		XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* shadowTexture,
		XMFLOAT3 lightPosition, XMFLOAT4 ambientColor, XMFLOAT4 diffuseColor);

private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd,/* WCHAR* */wstring vsFilename, /*WCHAR* */wstring psFilename);

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, XMFLOAT3, 
							 XMFLOAT4, XMFLOAT4);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_sampleStateWrap;
	ID3D11SamplerState* m_sampleStateClamp;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightBuffer;
	ID3D11Buffer* m_lightBuffer2;
};

#endif