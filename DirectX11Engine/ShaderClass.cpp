////////////////////////////////////////////////////////////////////////////////
// Filename: ShaderClass.cpp
////////////////////////////////////////////////////////////////////////////////
//#pragma comment ( lib, "d3dcompiler.lib" )
//#pragma comment ( lib, "d3d11.lib" )

#include "ShaderClass.h"
#include "ShaderCompiler.h"


ShaderClass::ShaderClass()
{}

ShaderClass::ShaderClass(const ShaderClass& other)
{}

ShaderClass::~ShaderClass()
{}

bool ShaderClass::Initialize(ID3D11Device * device, HWND hwnd, char * vsFilename, char * psFilename)
{
	// Initialize the vertex and pixel shaders.
	InitializeShader(device,hwnd,vsFilename,psFilename);

	return true;
}

bool ShaderClass::InitializeShader(ID3D11Device * device, HWND hwnd, char * vsFileName, char * psFileName)
{
	return false;
}

bool ShaderClass::CompileShaders(
	ID3D11Device * device, 
	HWND hwnd, 
	//char* vsFilename,
	//char* psFilename,
	string vsFilename,
	string psFilename,
	char* vsDesc, 
	char* psDesc, 
	ID3D10Blob* errorMessage)
{
	// USING D3D COMPILE FROM FILE
	//ShaderCompiler compiler;
	//_pixelShaderBuffer = compiler.CompileShader(psDesc, "ps_5_0", psFilename, device);
	//_vertexShaderBuffer = compiler.CompileShader(vsDesc, "vs_5_0", vsFilename, device);

	// USING HLSL COMPILER
	#ifdef RELEASE
		vsFilename = "../x64/Release/" + vsFilename;
		psFilename = "../x64/Release/" + psFilename;
	#else
		vsFilename = "../x64/Debug/" + vsFilename;
		psFilename = "../x64/Debug/" + psFilename;
	#endif

	ComPtr<ID3D10Blob> pVertexShaderBuffer, pPixelShaderBuffer;
	ThrowHResultIf(D3DReadFileToBlob(charToWChar(vsFilename.data()), _vertexShaderBuffer.GetAddressOf()));
	ThrowHResultIf(D3DReadFileToBlob(charToWChar(psFilename.data()), _pixelShaderBuffer.GetAddressOf()));

	ThrowHResultIf(device->CreateVertexShader(_vertexShaderBuffer->GetBufferPointer(),_vertexShaderBuffer->GetBufferSize(),nullptr,&_vertexShader));
	ThrowHResultIf(device->CreatePixelShader(_pixelShaderBuffer->GetBufferPointer(),_pixelShaderBuffer->GetBufferSize(),nullptr,&_pixelShader));

	// @LEGACY // @LEGACY // @LEGACY // @LEGACY // @LEGACY // @LEGACY
	//WCHAR* vsFilenameW = charToWChar(vsFilename);
	//WCHAR* psFilenameW = charToWChar(psFilename);

	//// Compile the vertex shader code. @TODO!!!!!! WATCH OUT FOR NULL POINTERS HERE!!!!!
	//HRESULT result = D3DCompileFromFile(vsFilenameW, NULL, NULL, vsDesc, "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
	//	0, &_vertexShaderBuffer, &errorMessage);
	//if (FAILED(result))
	//{
	//	// If the shader failed to compile it should have writen something to the error message.
	//	if (errorMessage)
	//	{
	//		OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
	//	}
	//	// If there was  nothing in the error message then it simply could not find the shader file itself.
	//	else
	//	{
	//		throw std::runtime_error("Missing Shader File - line " + to_string(__LINE__));
	//	}

	//	return false;
	//}

	//// Compile the pixel shader code.
	//result = D3DCompileFromFile(psFilenameW, NULL, NULL, psDesc, "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
	//	0, &_pixelShaderBuffer, &errorMessage);
	//if (FAILED(result))
	//{
	//	// If the shader failed to compile it should have writen something to the error message.
	//	if (errorMessage)
	//	{
	//		OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
	//	}
	//	// If there was  nothing in the error message then it simply could not find the file itself.
	//	else
	//	{
	//		throw std::runtime_error("Missing Shader File - line " + to_string(__LINE__));
	//	}

	//	return false;
	//}

	//// Create the vertex shader from the buffer.
	//ThrowHResultIf(device->CreateVertexShader(_vertexShaderBuffer->GetBufferPointer(), _vertexShaderBuffer->GetBufferSize(), NULL, &_vertexShader));

	//// Create the vertex shader from the buffer.
	//ThrowHResultIf(device->CreatePixelShader(_pixelShaderBuffer->GetBufferPointer(), _pixelShaderBuffer->GetBufferSize(), NULL,	&_pixelShader));

	return true;
}

void ShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, char* shaderFilename)
{
	char* compileErrors;
	unsigned long long bufferSize, i;
	ofstream fout;

	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("shader-error.txt");

	// Write out the error message.
	for (i = 0; i<bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	throw std::runtime_error("Error compiling shader.  Check shader-error.txt for message.");
}

void ShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(_layout.Get());

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(_vertexShader.Get(), NULL, 0);
	deviceContext->PSSetShader(_pixelShader.Get(), NULL, 0);

	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, _sampleState.GetAddressOf());

	deviceContext->DrawIndexed(indexCount, 0, 0);
}

