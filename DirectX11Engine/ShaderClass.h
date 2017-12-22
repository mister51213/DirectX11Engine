#pragma once

////////////////////////////////////////////////////////////////////////////////
// Filename: ShaderClass.h
////////////////////////////////////////////////////////////////////////////////

#pragma comment ( lib, "d3dcompiler.lib" )
#pragma comment ( lib, "d3d11.lib" )

//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>
#include "GfxUtil.h"
using namespace DirectX;
using namespace std;
using namespace GfxUtil;

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
	bool Initialize(ID3D11Device* device, HWND hwnd, char* vsFilename, char* psFilename);

protected:
	virtual bool InitializeShader(ID3D11Device* device, HWND hwnd, char* vsFileName, char* psFileName);

	bool CompileShaders(ID3D11Device * device, HWND hwnd, char* vsFilename, char* psFilename, char* vsDesc, char* psDesc, ID3D10Blob* errorMessage);

	void OutputShaderErrorMessage(ID3D10Blob*, HWND hwnd, char*);
	virtual bool SetBaseParameters(D3D11_MAPPED_SUBRESOURCE* const mappedResource, ID3D11DeviceContext*, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, /*ID3D11ShaderResourceView* texture*/unsigned int& bufferNumber);
	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);

protected:
	ComPtr<ID3D10Blob> _vertexShaderBuffer;
	ComPtr<ID3D10Blob> _pixelShaderBuffer;

	ComPtr <ID3D11SamplerState> _sampleState;
	ComPtr <ID3D11VertexShader> _vertexShader;
	ComPtr <ID3D11PixelShader> _pixelShader;
	ComPtr <ID3D11InputLayout> _layout;
	ComPtr <ID3D11Buffer> _matrixBuffer;

	vector<ComPtr <ID3D11Buffer>> _vsBuffers;
	vector<ComPtr <ID3D11Buffer>> _psBuffers;
};