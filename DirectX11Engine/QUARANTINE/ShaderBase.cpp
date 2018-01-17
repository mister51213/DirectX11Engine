////////////////////////////////////////////////////////////////////////////////
// Filename: ShaderBase.cpp
////////////////////////////////////////////////////////////////////////////////
//#pragma comment ( lib, "d3dcompiler.lib" )
//#pragma comment ( lib, "d3d11.lib" )

#include "ShaderBase.h"


ShaderBase::ShaderBase()
{}

ShaderBase::ShaderBase(const ShaderBase& other)
{}

ShaderBase::~ShaderBase()
{}


bool ShaderBase::CreateShaders(
	ID3D11Device * device, 
	HWND hwnd, 
	wstring vsFilename,
	wstring psFilename/*
	char* vsDesc, 
	char* psDesc, 
	ID3D10Blob* errorMessage*/)
{
	HRESULT result;

	ComPtr<ID3D10Blob> pVertexShaderBuffer, pPixelShaderBuffer;
	result = D3DReadFileToBlob((L"../x64/Debug/" + vsFilename).data(), _vertexShaderBuffer.GetAddressOf());
	result = D3DReadFileToBlob((L"../x64/Debug/" + psFilename).data(), _pixelShaderBuffer.GetAddressOf());

	result = device->CreateVertexShader(_vertexShaderBuffer->GetBufferPointer(),_vertexShaderBuffer->GetBufferSize(),nullptr,&m_vertexShader);
	result = device->CreatePixelShader(_pixelShaderBuffer->GetBufferPointer(),_pixelShaderBuffer->GetBufferSize(),nullptr,&m_pixelShader);

	return true;
}