////////////////////////////////////////////////////////////////////////////////
// Filename: ShaderClass.cpp
////////////////////////////////////////////////////////////////////////////////
//#pragma comment ( lib, "d3dcompiler.lib" )
//#pragma comment ( lib, "d3d11.lib" )

#include "ShaderClass.h"

ShaderClass::ShaderClass()
{}

ShaderClass::ShaderClass(const ShaderClass& other)
{}

ShaderClass::~ShaderClass()
{}

void ShaderClass::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects.
	ShutdownShader();

	return;
}

bool ShaderClass::Initialize(ID3D11Device * device, HWND hwnd, char * vsFilename, char * psFilename)
{
	bool result;

	// Initialize the vertex and pixel shaders.
	result = InitializeShader(
		device,
		hwnd,
		vsFilename,
		psFilename);
	if (!result)
	{
		return false;
	}

	return true;
}

bool ShaderClass::InitializeShader(ID3D11Device * device, HWND hwnd, char * vsFileName, char * psFileName)
{
	return false;
}

bool ShaderClass::CompileShaders(ID3D11Device * device, HWND hwnd, char* vsFilename, char* psFilename, char* vsDesc, char* psDesc, ID3D10Blob* errorMessage)
{
	WCHAR* vsFilenameW = charToWChar(vsFilename);
	WCHAR* psFilenameW = charToWChar(psFilename);

	// Compile the vertex shader code. @TODO!!!!!! WATCH OUT FOR NULL POINTERS HERE!!!!!
	bool result = D3DCompileFromFile(vsFilenameW, NULL, NULL, vsDesc, "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
		0, &_vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// If there was  nothing in the error message then it simply could not find the shader file itself.
		else
		{
			throw std::runtime_error("Missing Shader File - line " + to_string(__LINE__));
		}

		return false;
	}

	// Compile the pixel shader code.
	result = D3DCompileFromFile(psFilenameW, NULL, NULL, psDesc, "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
		0, &_pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// If there was  nothing in the error message then it simply could not find the file itself.
		else
		{
			throw std::runtime_error("Missing Shader File - line " + to_string(__LINE__));
		}

		return false;
	}

	// Create the vertex shader from the buffer.
	result = device->CreateVertexShader(_vertexShaderBuffer->GetBufferPointer(), _vertexShaderBuffer->GetBufferSize(), NULL,
		&_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	// Create the vertex shader from the buffer.
	result = device->CreatePixelShader(_pixelShaderBuffer->GetBufferPointer(), _pixelShaderBuffer->GetBufferSize(), NULL,
		&_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void ShaderClass::ShutdownShader()
{
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

	return;
}

bool ShaderClass::SetBaseParameters(D3D11_MAPPED_SUBRESOURCE* const mappedResource, ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, /*ID3D11ShaderResourceView* texture*/unsigned int& bufferNumber)
{
	HRESULT result;
	//D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	//unsigned int bufferNumber;

	// Transpose the matrices to prepare them for the shader.
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(_matrixBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, /*&*/mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)(*mappedResource).pData;

	// Copy the matrices into the constant buffer.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// Unlock the constant buffer.
	deviceContext->Unmap(_matrixBuffer.Get(), 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	/////////////////////////////////////////////////////////////
	/////////////////////// VS BUFFERS //////////////////////////
	/////////////////////////////////////////////////////////////

	// Finanly set the constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &_matrixBuffer);

	/////////////////////////////////////////////////////////////
	/////////////////////// PS BUFFERS //////////////////////////
	/////////////////////////////////////////////////////////////
	// Set shader texture resource in the pixel shader.
	//deviceContext->PSSetShaderResources(0, 1, &texture);

	return true;
}

void ShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(_layout.Get());

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(_vertexShader.Get(), NULL, 0);
	deviceContext->PSSetShader(_pixelShader.Get(), NULL, 0);

	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, /*&_sampleState*/_sampleState.GetAddressOf());

	// Render the triangle.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}

