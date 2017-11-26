#pragma once

////////////////////////////////////////////////////////////////////////////////
// Filename: ShaderClass.h
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
// Class name: ShaderClass
// Description: Used to invoke the HLSL shaders for drawing the 3D models that are on the GPU.
////////////////////////////////////////////////////////////////////////////////
class ShaderClass
{
protected:
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
	ShaderClass();
	ShaderClass(const ShaderClass&);
	~ShaderClass();
	void Shutdown();
	bool Initialize(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename);

protected:
	virtual bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFileName, WCHAR* psFileName);

	bool CompileShaders(ID3D11Device * device, ID3D10Blob* vertexShaderBuffer, ID3D10Blob* pixelShaderBuffer, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename, char* vsDesc, char* psDesc, ID3D10Blob* errorMessage);

	virtual void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND hwnd, WCHAR*);
	virtual bool SetBaseParameters(D3D11_MAPPED_SUBRESOURCE* const mappedResource, ID3D11DeviceContext*, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, /*ID3D11ShaderResourceView* texture*/unsigned int& bufferNumber);
	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);

protected:
	ID3D11VertexShader* _vertexShader;
	ID3D11PixelShader* _pixelShader;
	ID3D11InputLayout* _layout;
	ID3D11Buffer* _matrixBuffer;
	ID3D11SamplerState* _sampleState;
};