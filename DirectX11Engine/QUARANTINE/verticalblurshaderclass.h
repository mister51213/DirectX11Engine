////////////////////////////////////////////////////////////////////////////////
// Filename: verticalblurshaderclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _VERTICALBLURSHADERCLASS_H_
#define _VERTICALBLURSHADERCLASS_H_


#pragma comment ( lib, "d3dcompiler.lib" )
#pragma comment ( lib, "d3d11.lib" )

//////////////
// INCLUDES //
//////////////
#include <d3dcompiler.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <fstream>
#include "../ShaderClass.h"
//#include "ShaderBase.h"

using namespace std;
using namespace DirectX;


////////////////////////////////////////////////////////////////////////////////
// Class name: VerticalBlurShaderClass
////////////////////////////////////////////////////////////////////////////////
class VerticalBlurShaderClass : public ShaderClass //ShaderBase
{
private:
	struct ScreenSizeBufferType
	{
		float screenHeight;
		XMFLOAT3 padding;
	};

public:
	VerticalBlurShaderClass();
	VerticalBlurShaderClass(const VerticalBlurShaderClass&);

	bool Initialize(ID3D11Device*, HWND);
	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, float);

	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, float);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_screenSizeBuffer;
};

#endif