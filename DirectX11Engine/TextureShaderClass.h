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
public:
	// cBuffer type that will be used with the vertex shader.
	// This typedef must be exactly the same as the one in the vertex shader as the model data needs to match the typedefs in the shader for proper rendering.
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	TextureShaderClass();
	TextureShaderClass(const TextureShaderClass&);
	~TextureShaderClass();

	/** pass in the name of the HLSL shader files inside this function */
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView** textureArray);

private:
	/** actually loads the shader files and makes it usable to DirectX and the GPU. */
	virtual bool InitializeShader(ID3D11Device* device, HWND hwnd, char* vsFileName, char* psFileName) override;

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView** textureArray);
};