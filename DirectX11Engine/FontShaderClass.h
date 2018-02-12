////////////////////////////////////////////////////////////////////////////////
// Filename: fontshaderclass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

//////////////
// INCLUDES //
//////////////
#include "ShaderClass.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: FontShaderClass
////////////////////////////////////////////////////////////////////////////////
class FontShaderClass: public ShaderClass
{
private:
	struct PixelBufferType
	{
		XMFLOAT4 pixelColor;
	};

public:
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, MatrixBufferType transforms,ID3D11ShaderResourceView* texture, 
		vector<Microsoft::WRL::ComPtr <ID3D11ShaderResourceView>>& texViews, XMFLOAT4 pixelColor);

//private:
	bool InitializeShader(ID3D11Device*, HWND, char*, char*);
	bool SetShaderParameters(ID3D11DeviceContext*, MatrixBufferType transforms, ID3D11ShaderResourceView*, 
		vector<Microsoft::WRL::ComPtr <ID3D11ShaderResourceView>>& texViews, XMFLOAT4);
};