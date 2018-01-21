////////////////////////////////////////////////////////////////////////////////
// Filename: horizontalblurshaderclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _HORIZONTALBLURSHADERCLASS_H_
#define _HORIZONTALBLURSHADERCLASS_H_

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
#include "../ShaderClass.h"
//#include "ShaderBase.h"

using namespace std;
using namespace DirectX;


////////////////////////////////////////////////////////////////////////////////
// Class name: HorizontalBlurShaderClass
////////////////////////////////////////////////////////////////////////////////
class HorizontalBlurShaderClass : public ShaderClass//ShaderBase
{
private:
	//struct MatrixBufferType
	//{
	//	XMMATRIX world;
	//	XMMATRIX view;
	//	XMMATRIX projection;
	//};

	struct ScreenSizeBufferType
	{
		float screenWidth;
		XMFLOAT3 padding;
	};

public:
	HorizontalBlurShaderClass();
	HorizontalBlurShaderClass(const HorizontalBlurShaderClass&);
	//~HorizontalBlurShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	//void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
		XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, float screenWidth);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	//void ShutdownShader();

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, float);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	//ID3D11VertexShader* m_vertexShader;
	//ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_screenSizeBuffer;
};

#endif