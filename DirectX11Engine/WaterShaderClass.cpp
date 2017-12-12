#include "WaterShaderClass.h"



WaterShaderClass::WaterShaderClass()
	:
	_reflectionBuffer(0),
	_waterBuffer(0)
{}

WaterShaderClass::WaterShaderClass(const WaterShaderClass &)
{}

WaterShaderClass::~WaterShaderClass()
{}

bool WaterShaderClass::Render(ID3D11DeviceContext * deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMMATRIX reflectionMatrix, ID3D11ShaderResourceView * reflectionTexture, ID3D11ShaderResourceView * refractionTexture, ID3D11ShaderResourceView * normalTexture, float waterTranslation, float reflectRefractScale)
{
	bool result;

	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, reflectionMatrix, reflectionTexture, refractionTexture, normalTexture, waterTranslation, reflectRefractScale);
	if (!result)
	{
		return false;
	}

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext, indexCount);

	return true;
}

bool WaterShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, char* vsFilename, char* psFilename)
{
	WCHAR* vsFilenameW = charToWChar(vsFilename);
	WCHAR* psFilenameW = charToWChar(psFilename);

	HRESULT result;
	ID3D10Blob* errorMessage;
	//ID3D10Blob* vertexShaderBuffer;
	//ID3D10Blob* pixelShaderBuffer;

	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC reflectionBufferDesc;
	D3D11_BUFFER_DESC waterBufferDesc;

	// Initialize the pointers this function will use to null.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	// Compile the vertex shader code.
	result = D3DCompileFromFile(vsFilenameW, NULL, NULL, "WaterVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
			&vertexShaderBuffer, &errorMessage);
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
			MessageBox(hwnd, vsFilename, "Missing Shader File", MB_OK);
		}

		return false;
	}

	// Compile the pixel shader code.
	result = D3DCompileFromFile(psFilenameW, NULL, NULL, "WaterPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
			&pixelShaderBuffer, &errorMessage);
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
			MessageBox(hwnd, psFilename, "Missing Shader File", MB_OK);
		}

		return false;
	}

	// Create the vertex shader from the buffer.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL,
		&_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	// Create the vertex shader from the buffer.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL,
		&_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	// Create the vertex input layout description.
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

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
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	result = device->CreateSamplerState(&samplerDesc, &_sampleState);
	if (FAILED(result))
	{
		return false;
	}

	// Setup the description of the matrix dynamic constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the matrix constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Setup the description of the reflection dynamic constant buffer that is in the vertex shader.
	reflectionBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	reflectionBufferDesc.ByteWidth = sizeof(ReflectionBufferType);
	reflectionBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	reflectionBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	reflectionBufferDesc.MiscFlags = 0;
	reflectionBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&reflectionBufferDesc, NULL, &_reflectionBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Setup the description of the water dynamic constant buffer that is in the pixel shader.
	waterBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	waterBufferDesc.ByteWidth = sizeof(WaterBufferType);
	waterBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	waterBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	waterBufferDesc.MiscFlags = 0;
	waterBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the pixel shader constant buffer from within this class.
	result = device->CreateBuffer(&waterBufferDesc, NULL, &_waterBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void WaterShaderClass::ShutdownShader()
{
	// Release the water constant buffer.
	if (_waterBuffer)
	{
		_waterBuffer->Release();
		_waterBuffer = 0;
	}

	// Release the reflection constant buffer.
	if (_reflectionBuffer)
	{
		_reflectionBuffer->Release();
		_reflectionBuffer = 0;
	}

	ShaderClass::ShutdownShader();
}

bool WaterShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, XMMATRIX reflectionMatrix,
	ID3D11ShaderResourceView* reflectionTexture,
	ID3D11ShaderResourceView* refractionTexture, ID3D11ShaderResourceView* normalTexture,
	float waterTranslation, float reflectRefractScale)
{
	HRESULT result;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber;

	//MatrixBufferType* dataPtr;
	ReflectionBufferType* dataPtr2;
	WaterBufferType* dataPtr3;

	result = SetBaseParameters(&mappedResource, deviceContext, worldMatrix, viewMatrix, projectionMatrix, bufferNumber);
	if (FAILED(result))
	{
		return false;
	}

	// Set the reflection texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &reflectionTexture);

	// Set the refraction texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(1, 1, &refractionTexture);

	// Set the normal map texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(2, 1, &normalTexture);

	// Transpose all the input matrices to prepare them for the shader.
	reflectionMatrix = XMMatrixTranspose(reflectionMatrix);

	// Lock the reflection constant buffer so it can be written to.
	result = deviceContext->Map(_reflectionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr2 = (ReflectionBufferType*)mappedResource.pData;

	// Copy the reflection matrix into the constant buffer.
	dataPtr2->reflection = reflectionMatrix;

	// Unlock the constant buffer.
	deviceContext->Unmap(_reflectionBuffer, 0);

	// Set the position of the reflection constant buffer in the vertex shader.
	bufferNumber = 1;

	// Finally set the reflection constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &_reflectionBuffer);

	// Lock the water constant buffer so it can be written to.
	result = deviceContext->Map(_waterBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr3 = (WaterBufferType*)mappedResource.pData;

	// Copy the water data into the constant buffer.
	dataPtr3->waterTranslation = waterTranslation;
	dataPtr3->reflectRefractScale = reflectRefractScale;
	dataPtr3->padding = XMFLOAT2(0.0f, 0.0f);

	// Unlock the constant buffer.
	deviceContext->Unmap(_waterBuffer, 0);

	// Set the position of the water constant buffer in the pixel shader.
	bufferNumber = 0;

	// Finally set the water constant buffer in the pixel shader with the updated values.
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &_waterBuffer);

	return true;
}
