////////////////////////////////////////////////////////////////////////////////
// Filename: textureshaderclass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

//////////////
// INCLUDES //
//////////////
#include "ShaderClass.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: TextureShaderClass
// Description: Used to invoke the HLSL shaders for drawing the 3D models that are on the GPU.
////////////////////////////////////////////////////////////////////////////////

class TextureShaderClass : public ShaderClass
{
private:
	///////////////////////// PS BUFFER TYPES //////////////////////////
	struct TexParamBufferType
	{
		XMFLOAT2 translation2D;
		float scale;
		float gamma;
	};

public:
	/** actually loads the shader files and makes it usable to DirectX and the GPU. */
	virtual bool InitializeShader(ID3D11Device* device, HWND hwnd, char* vsFileName, char* psFileName) override;

	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, MatrixBufferType& transforms, vector<ComPtr <ID3D11ShaderResourceView>>& texViews, 
		XMFLOAT2& translation = XMFLOAT2(0.f, 0.f), float scale = 1.f, float gamma = 1.f);
};