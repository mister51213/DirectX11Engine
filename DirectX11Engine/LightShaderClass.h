////////////////////////////////////////////////////////////////////////////////
// Filename: lightshaderinterface.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <directxmath.h>
#include <fstream>
using namespace std;
using namespace DirectX;


////////////////////////////////////////////////////////////////////////////////
// Class name: LightShaderClass
////////////////////////////////////////////////////////////////////////////////
class LightShaderClass
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

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

	struct FogBufferType
	{
		float fogStart;
		float fogEnd;
		float padding1, padding2;
	};

	// for tex translation
	struct TranslateBufferType
	{
		float translation;
		XMFLOAT3 padding;
	};

	// for alpha blending textures
	struct TransparentBufferType
	{
		float blendAmount;
		XMFLOAT3 padding;
	};

public:
	LightShaderClass();
	LightShaderClass(const LightShaderClass&);
	~LightShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
		XMMATRIX projectionMatrix, ID3D11ShaderResourceView** textureArray, XMFLOAT3 lightDirection, XMFLOAT4 ambientColor, XMFLOAT4 diffuseColor,
		XMFLOAT3 cameraPosition, XMFLOAT4 specularColor, float specularPower, float fogStart, float fogEnd, XMFLOAT4 clipPlane, float translation, float transparency);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView** textureArray, XMFLOAT3, XMFLOAT4 ambientColor, XMFLOAT4, XMFLOAT3 cameraPosition, XMFLOAT4 specularColor, float specularPower, float fogStart, float fogEnd, XMFLOAT4 clipPlane, float texTranslation, float transparency);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* _vertexShader;
	ID3D11PixelShader* _pixelShader;
	ID3D11InputLayout* _layout;
	ID3D11Buffer* _matrixBuffer;

	ID3D11SamplerState* _sampleState;
	ID3D11Buffer* _cameraBuffer;
	ID3D11Buffer* _lightBuffer;
	ID3D11Buffer* _fogBuffer;

	ID3D11Buffer* _clipPlaneBuffer;

	ID3D11Buffer* _translateBuffer;

	ID3D11Buffer* _transparentBuffer;
};