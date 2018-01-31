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
class TextureShaderClass:public ShaderClass
{
private:
	///////////////////////// PS BUFFER TYPES //////////////////////////
	struct TexParamBufferType
	{
		XMFLOAT2 translation2D;
		float scale;
		float padding;
	};

public:
	TextureShaderClass();
	TextureShaderClass(const TextureShaderClass&);

	/** pass in the name of the HLSL shader files inside this function */
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, MatrixBufferType& transforms,
	ID3D11ShaderResourceView** textureArray, 
	vector<Microsoft::WRL::ComPtr <ID3D11ShaderResourceView>>& texViews, const XMFLOAT2& translation = XMFLOAT2(0.f,0.f), const float scale = 1.f);

private:
	/** actually loads the shader files and makes it usable to DirectX and the GPU. */
	virtual bool InitializeShader(ID3D11Device* device, HWND hwnd, char* vsFileName, char* psFileName) override;

	bool SetShaderParameters(ID3D11DeviceContext*, MatrixBufferType& transforms,
		ID3D11ShaderResourceView** textureArray, 
		vector<Microsoft::WRL::ComPtr <ID3D11ShaderResourceView>>& texViews, const XMFLOAT2& translation, const float scale);
};