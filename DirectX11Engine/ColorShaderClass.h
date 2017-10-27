#pragma once

////////////////////////////////////////////////////////////////////////////////
// Filename: colorshaderclass.h
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>
using namespace DirectX;
using namespace std;

////////////////////////////////////////////////////////////////////////////////
// Class name: ColorShaderClass
// Description: Used to invoke the HLSL shaders for drawing the 3D models that are on the GPU.
////////////////////////////////////////////////////////////////////////////////
class ColorShaderClass
{
private:
	// cBuffer type that will be used with the vertex shader.
	// This typedef must be exactly the same as the one in the vertex shader as the model data needs to match the typedefs in the shader for proper rendering.
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	// record the location of each vertex that is in the vertex buffer.
	// The GPU then uses the index buffer to quickly find specific vertices in the vertex buffer.

public:
	ColorShaderClass();
	ColorShaderClass(const ColorShaderClass&);
	~ColorShaderClass();

	/** pass in the name of the HLSL shader files inside this function */
	bool Initialize(ID3D11Device* device, HWND hwnd);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX);

private:
	/** actually loads the shader files and makes it usable to DirectX and the GPU. */
	bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFileName, WCHAR* psFileName);
	//bool InitializeShader(ID3D11Device*, HWND, std::wstring vsFilename, std::wstring psFilename);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND hwnd, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX);
	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);

private:
	ID3D11VertexShader* _vertexShader;
	ID3D11PixelShader* _pixelShader;
	ID3D11InputLayout* _layout;
	ID3D11Buffer* _matrixBuffer;
};