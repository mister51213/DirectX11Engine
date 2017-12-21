#include "ReflectionShaderClass.h"

ReflectionShaderClass::ReflectionShaderClass()
{}

ReflectionShaderClass::ReflectionShaderClass(const ReflectionShaderClass &)
{}

ReflectionShaderClass::~ReflectionShaderClass()
{}

bool ReflectionShaderClass::Render(ID3D11DeviceContext * deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView * texture, ID3D11ShaderResourceView * reflectionTexture, XMMATRIX reflectionMatrix)
{
	bool result;
	
	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture, reflectionTexture, reflectionMatrix);
	if (!result)
	{
		return false;
	}

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext, indexCount);

	return true;
}

bool ReflectionShaderClass::InitializeShader(ID3D11Device * device, HWND hwnd, char * vsFilename, char * psFilename)
{

	WCHAR* vsFilenameW = charToWChar(vsFilename);
	WCHAR* psFilenameW = charToWChar(psFilename);

	HRESULT result;
	ID3D10Blob* errorMessage;
	//ID3D10Blob* vertexShaderBuffer;
	//ID3D10Blob* pixelShaderBuffer;

	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;


	D3D11_BUFFER_DESC reflectionBufferDesc;

	// Initialize the pointers this function will use to null.
	errorMessage = 0;
	//vertexShaderBuffer = 0;
	//pixelShaderBuffer = 0;

	result = CompileShaders(device, hwnd, vsFilename, psFilename, "ReflectionVertexShader", "ReflectionPixelShader", errorMessage);

	//// Compile the vertex shader code.
	//result = D3DCompileFromFile(vsFilename, NULL, NULL, "ReflectionVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
	//		0, &vertexShaderBuffer, &errorMessage);
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
	//		MessageBox(hwnd, vsFilename, "Missing Shader File", MB_OK);
	//	}
	//	return false;
	//}
	//// Compile the pixel shader code.
	//result = D3DCompileFromFile(psFilename, NULL, NULL, "ReflectionPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
	//	0, &pixelShaderBuffer, &errorMessage);
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
	//		MessageBox(hwnd, psFilename, "Missing Shader File", MB_OK);
	//	}
	//	return false;
	//}
	//// Create the vertex shader from the buffer.
	//result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL,
	//	&_vertexShader);
	//if (FAILED(result))
	//{
	//	return false;
	//}
	//// Create the vertex shader from the buffer.
	//result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL,
	//	&_pixelShader);
	//if (FAILED(result))
	//{
	//	return false;
	//}

	// Create the vertex input layout description.
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	polygonLayout[0] = MakeInputElementDesc("POSITION", DXGI_FORMAT_R32G32B32_FLOAT, 0);
	polygonLayout[1] = MakeInputElementDesc("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT);
		
	// Get a count of the elements in the layout.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &_layout);
	if (FAILED(result))
	{
		return false;
	}

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// Create a texture sampler state description.
	samplerDesc = MakeSamplerDesc();
	// Create the texture sampler state.
	result = device->CreateSamplerState(&samplerDesc, &_sampleState);
	if (FAILED(result))
	{
		return false;
	}

	// VS Buffers
	_vsBuffers.emplace_back(MakeConstantBuffer<MatrixBufferType>(device));
	_vsBuffers.emplace_back(MakeConstantBuffer<ReflectionBufferType>(device));

	return true;
}

void ReflectionShaderClass::ShutdownShader()
{
	ShaderClass::ShutdownShader();
}

bool ReflectionShaderClass::SetShaderParameters(ID3D11DeviceContext * deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView * texture, ID3D11ShaderResourceView * reflectionTexture, XMMATRIX reflectionMatrix)
{
	HRESULT result;

	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);
	//Set the reflection texture as the second texture inside the pixel shader.
	deviceContext->PSSetShaderResources(1, 1, &reflectionTexture);

	///////////////////////////////////////////////////////////////
	///////////////////////// VS BUFFERS //////////////////////////
	///////////////////////////////////////////////////////////////

	///////////////////// MATRIX INIT - VS BUFFER 0 //////////////////////////////////
	unsigned int bufferNumber = 0;
	MatrixBufferType tempMatBuff = { XMMatrixTranspose(worldMatrix), XMMatrixTranspose(viewMatrix), XMMatrixTranspose(projectionMatrix) };
	MapBuffer(tempMatBuff, _vsBuffers[bufferNumber].Get(), deviceContext);
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, _vsBuffers[bufferNumber].GetAddressOf());

	///////////////////// REFLECTION INIT - VS BUFFER 1 //////////////////////////////////
	bufferNumber++;
	ReflectionBufferType tempRefBuff = { XMMatrixTranspose(reflectionMatrix) };
	MapBuffer(tempRefBuff, _vsBuffers[bufferNumber].Get(), deviceContext);
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, _vsBuffers[bufferNumber].GetAddressOf());
	
	return true;
}

