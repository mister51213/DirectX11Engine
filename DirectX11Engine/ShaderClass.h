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
#include <fstream>
#include "GfxUtil.h"

using namespace GfxUtil;

////////////////////////////////////////////////////////////////////////////////
// Class name: ShaderClass
// Description: Used to invoke the HLSL shaders for drawing the 3D models that are on the GPU.
////////////////////////////////////////////////////////////////////////////////
class ShaderClass
{
public:
	bool Initialize(ID3D11Device* device, HWND hwnd, char* vsFilename, char* psFilename);

	virtual void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);

protected:
	virtual bool InitializeShader(ID3D11Device* device, HWND hwnd, char* vsFileName, char* psFileName);

	bool CreateShaders(ID3D11Device * device, HWND hwnd, wstring vsFilename, wstring psFilename);

	bool CompileShaders(ID3D11Device * device, HWND hwnd, string vsFilename, string psFilename, char* vsDesc, char* psDesc, ID3D10Blob* errorMessage = nullptr);

	void OutputShaderErrorMessage(ID3D10Blob*, HWND hwnd, char*);

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