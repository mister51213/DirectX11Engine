////////////////////////////////////////////////////////////////////////////////
// Filename: shadowshaderclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _SHADOWSHADERCLASS_H_
#define _SHADOWSHADERCLASS_H_

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
using namespace std;
using namespace DirectX;


////////////////////////////////////////////////////////////////////////////////
// Class name: ShadowShaderClass
////////////////////////////////////////////////////////////////////////////////
class ShadowShaderClass
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMMATRIX lightView;
		XMMATRIX lightProjection;
	};

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
	ShadowShaderClass();
	ShadowShaderClass(const ShadowShaderClass&);
	~ShadowShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
		XMMATRIX projectionMatrix, XMMATRIX lightViewMatrix, XMMATRIX lightProjectionMatrix,
		ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* depthMapTexture, XMFLOAT3 lightPosition,
		XMFLOAT4 ambientColor, XMFLOAT4 diffuseColor);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, char*);

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*,
		ID3D11ShaderResourceView*, XMFLOAT3, XMFLOAT4, XMFLOAT4);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_sampleStateWrap;
	ID3D11SamplerState* m_sampleStateClamp;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightBuffer;
	ID3D11Buffer* m_lightBuffer2;
};

#endif