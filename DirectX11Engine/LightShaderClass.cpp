////////////////////////////////////////////////////////////////////////////////
// Filename: lightshaderclass.cpp
////////////////////////////////////////////////////////////////////////////////

#include "lightshaderclass.h"

LightShaderClass::LightShaderClass()
{}

LightShaderClass::LightShaderClass(const LightShaderClass& other)
{}

LightShaderClass::~LightShaderClass()
{}

bool LightShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, ID3D11ShaderResourceView** textureArray, XMFLOAT3 lightDirection, XMFLOAT4 ambientColor, XMFLOAT4 diffuseColor, LightClass* lights[],
	XMFLOAT3 cameraPosition, XMFLOAT4 specularColor, float specularPower, float fogStart, float fogEnd, XMFLOAT4 clipPlane, float translation, float transparency)
{
	// Set the shader parameters that it will use for rendering.
	bool result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, textureArray, lightDirection, ambientColor, diffuseColor, lights, cameraPosition, specularColor, specularPower, fogStart, fogEnd, clipPlane, translation, transparency/*, reflectionTexture, reflectionMatrix*/);
	if (!result)
	{
		return false;
	}

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext, indexCount);

	return true;
}

bool LightShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, char* vsFilename, char* psFilename)
{
	WCHAR* vsFilenameW = charToWChar(vsFilename);
	WCHAR* psFilenameW = charToWChar(psFilename);

	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;

	// Now set to 5 to accommodate tangent and binormal
	D3D11_INPUT_ELEMENT_DESC polygonLayout[5];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;

	// Initialize the pointers this function will use to null.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

#pragma region COMPILE
	// Compile the vertex shader code.
	result = 
		D3DCompileFromFile(
			vsFilenameW,
			NULL, 
			NULL, 
			"LightVertexShader", 
			"vs_5_0", 
			D3D10_SHADER_ENABLE_STRICTNESS, 
			0, 
			&vertexShaderBuffer, 
			&errorMessage);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// If there was nothing in the error message then it simply could not find the shader file itself.
		else
		{
			throw std::runtime_error("Missing Shader File - " + string(vsFilename));
		}

		return false;
	}

		// Compile the pixel shader code.
		result = D3DCompileFromFile(
			psFilenameW, 
			NULL, 
			NULL, 
			"LightPixelShader", 
			"ps_5_0", 
			D3D10_SHADER_ENABLE_STRICTNESS, 
			0, 
			&pixelShaderBuffer, 
			&errorMessage);
		if (FAILED(result))
		{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// If there was nothing in the error message then it simply could not find the file itself.
		else
		{
			throw std::runtime_error("Missing Shader File - " + string(psFilename));
		}

		return false;
	}

	// Create the vertex shader from the buffer.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	// Create the pixel shader from the buffer.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

#pragma endregion

	// VERTEX SHADER LAYOUT DESCRIPTION SETUP //
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

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	polygonLayout[3].SemanticName = "TANGENT";
	polygonLayout[3].SemanticIndex = 0;
	polygonLayout[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[3].InputSlot = 0;
	polygonLayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[3].InstanceDataStepRate = 0;

	polygonLayout[4].SemanticName = "BINORMAL";
	polygonLayout[4].SemanticIndex = 0;
	polygonLayout[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[4].InputSlot = 0;
	polygonLayout[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[4].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &_layout);
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

	// VS Buffers
	_vsBuffers.emplace_back(MakeConstantBuffer<MatrixBufferType>(device));
	_vsBuffers.emplace_back(MakeConstantBuffer<CameraBufferType>(device));
	_vsBuffers.emplace_back(MakeConstantBuffer<LightPositionBufferType>(device));
	_vsBuffers.emplace_back(MakeConstantBuffer<ClipPlaneBufferType>(device));
	_vsBuffers.emplace_back(MakeConstantBuffer<FogBufferType>(device));

	// PS Buffers
	_psBuffers.emplace_back(MakeConstantBuffer<LightBufferType>(device));
	_psBuffers.emplace_back(MakeConstantBuffer<LightColorBufferType>(device));
	_psBuffers.emplace_back(MakeConstantBuffer<TranslateBufferType>(device));
	_psBuffers.emplace_back(MakeConstantBuffer<TransparentBufferType>(device));

#pragma region LEGACY
	//// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	//matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	//matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	//matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//matrixBufferDesc.MiscFlags = 0;
	//matrixBufferDesc.StructureByteStride = 0;

	//// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	//result = device->CreateBuffer(&matrixBufferDesc, NULL, &_matrixBuffer);
	//if (FAILED(result))
	//{
	//	return false;
	//}

	////////////////// MATRIX BUFFER TYPE UNIQUE TO THIS CLASS /////////////

	//// Setup the description of the camera dynamic constant buffer that is in the vertex shader.
	//cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	//cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	//cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//cameraBufferDesc.MiscFlags = 0;
	//cameraBufferDesc.StructureByteStride = 0;

	//// Create the camera constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	//result = device->CreateBuffer(&cameraBufferDesc, NULL, _cameraBuffer.GetAddressOf());
	//if (FAILED(result))
	//{
	//	return false;
	//}

	//// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	//// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	//lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	//lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	//lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//lightBufferDesc.MiscFlags = 0;
	//lightBufferDesc.StructureByteStride = 0;

	//// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	//result = device->CreateBuffer(&lightBufferDesc, NULL, _lightBuffer.GetAddressOf());
	//if (FAILED(result))
	//{
	//	return false;
	//}

	////////////////////////////////////////////
	/////////////// POINT LIGHTS ///////////////
	//// Setup the description of the dynamic constant buffer that is in the pixel shader.
	//lightColorBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	//lightColorBufferDesc.ByteWidth = sizeof(LightColorBufferType);
	//lightColorBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//lightColorBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//lightColorBufferDesc.MiscFlags = 0;
	//lightColorBufferDesc.StructureByteStride = 0;

	//// Create the constant buffer pointer so we can access the pixel shader constant buffer from within this class.
	//result = device->CreateBuffer(&lightColorBufferDesc, NULL, _lightColorBuffer.GetAddressOf());
	//if (FAILED(result))
	//{
	//	return false;
	//}

	//// Setup the description of the dynamic constant buffer that is in the vertex shader.
	//lightPositionBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	//lightPositionBufferDesc.ByteWidth = sizeof(LightPositionBufferType);
	//lightPositionBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//lightPositionBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//lightPositionBufferDesc.MiscFlags = 0;
	//lightPositionBufferDesc.StructureByteStride = 0;

	//// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	//result = device->CreateBuffer(&lightPositionBufferDesc, NULL, _lightPositionBuffer.GetAddressOf());
	//if (FAILED(result))
	//{
	//	return false;
	//}
	/////////////// POINT LIGHTS ///////////////
	////////////////////////////////////////////

	//// Setup the description of the dynamic fog constant buffer that is in the vertex shader.
	//fogBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	//fogBufferDesc.ByteWidth = sizeof(FogBufferType);
	//fogBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//fogBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//fogBufferDesc.MiscFlags = 0;
	//fogBufferDesc.StructureByteStride = 0;

	//// Create the fog buffer pointer so we can access the vertex shader fog constant buffer from within this class.
	//result = device->CreateBuffer(&fogBufferDesc, NULL, _fogBuffer.GetAddressOf());
	//if (FAILED(result))
	//{
	//	return false;
	//}

	//// Setup the description of the clip plane dynamic constant buffer that is in the vertex shader.
	//clipPlaneBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	//clipPlaneBufferDesc.ByteWidth = sizeof(ClipPlaneBufferType);
	//clipPlaneBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//clipPlaneBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//clipPlaneBufferDesc.MiscFlags = 0;
	//clipPlaneBufferDesc.StructureByteStride = 0;

	//// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	//result = device->CreateBuffer(&clipPlaneBufferDesc, NULL, _clipPlaneBuffer.GetAddressOf());
	//if (FAILED(result))
	//{
	//	return false;
	//}

	//// Setup the description of the texture translation dynamic constant buffer that is in the pixel shader.
	//translateBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	//translateBufferDesc.ByteWidth = sizeof(TranslateBufferType);
	//translateBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//translateBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//translateBufferDesc.MiscFlags = 0;
	//translateBufferDesc.StructureByteStride = 0;

	//// Create the constant buffer pointer so we can access the pixel shader constant buffer from within this class.
	//result = device->CreateBuffer(&translateBufferDesc, NULL, _translateBuffer.GetAddressOf());
	//if (FAILED(result))
	//{
	//	return false;
	//}

	//// Setup the description of the transparent dynamic constant buffer that is in the pixel shader.
	//transparentBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	//transparentBufferDesc.ByteWidth = sizeof(TransparentBufferType);
	//transparentBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//transparentBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//transparentBufferDesc.MiscFlags = 0;
	//transparentBufferDesc.StructureByteStride = 0;

	//// Create the constant buffer pointer so we can access the pixel shader constant buffer from within this class.
	//result = device->CreateBuffer(&transparentBufferDesc, NULL, _transparentBuffer.GetAddressOf());
	//if (FAILED(result))
	//{
	//	return false;
	//}

#pragma endregion
	return true;
}

void LightShaderClass::ShutdownShader()
{

	ShaderClass::ShutdownShader();

	return;
}

bool LightShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, ID3D11ShaderResourceView** textureArray, XMFLOAT3 lightDirection, XMFLOAT4 ambientColor, XMFLOAT4 diffuseColor,
	LightClass* lights[],
	XMFLOAT3 cameraPosition, XMFLOAT4 specularColor, float specularPower, float fogStart, float fogEnd, XMFLOAT4 clipPlane, float translation, float transparency)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	/////////////////////// SET TEXTURE RESOURCES //////////////////////
	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 6, textureArray); // sextuple tex with lightmap

	LightBufferType* pLightBuff; //NOTE - dataPtr1 define in parent class

	LightPositionBufferType* pLightPosBuff;
	LightColorBufferType* pLightColBuff;

	CameraBufferType* pCamBuff;
	FogBufferType* pFogBuff;
	ClipPlaneBufferType* pClipPlaneBuff;
	TranslateBufferType* pTranslateBuff;
	TransparentBufferType* pTransparentBuff;

	///////////////////////////////////////////////////////////////
	///////////////////////// VS BUFFERS //////////////////////////
	///////////////////////////////////////////////////////////////
	//// Set the position of the constant buffer in the vertex shader.
	//@TODO just increment buffer number each time, and separate vertex and pixel buffers into 2 functions






	///////////////////// MATRIX INIT - VS BUFFER 0 //////////////////////////////////
	unsigned int bufferNumber = 0;

	//MatrixBufferType* pMatBuff;

	//@NEW GENERIC

	MatrixBufferType tempMatBuff = { XMMatrixTranspose(worldMatrix),
	XMMatrixTranspose(viewMatrix),
	XMMatrixTranspose(projectionMatrix) };

	MapBuffer(tempMatBuff, _vsBuffers[0].Get(), deviceContext);

	// Transpose the matrices to prepare them for the shader.
	//worldMatrix =         XMMatrixTranspose(worldMatrix);
	//viewMatrix =          XMMatrixTranspose(viewMatrix);
	//projectionMatrix =    XMMatrixTranspose(projectionMatrix);

	// Lock the constant buffer so it can be written to.
	//result = deviceContext->Map(_vsBuffers[0].Get()/*_matrixBuffer*/, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	//if (FAILED(result))
	//{
	//	return false;
	//}

	// Get a pointer to the data in the constant buffer.
	//pMatBuff = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	//pMatBuff->world = worldMatrix;
	//pMatBuff->view = viewMatrix;
	//pMatBuff->projection = projectionMatrix;

	// Unlock the constant buffer.
	//deviceContext->Unmap(/*_matrixBuffer*/_vsBuffers[0].Get(), 0);

	deviceContext->VSSetConstantBuffers(bufferNumber, 1, _vsBuffers[0].GetAddressOf());
	
	///////////////////// CAM INIT - VS BUFFER 1 //////////////////////////////////
	// Lock the camera constant buffer so it can be written to.
	bufferNumber = 1;
	
	result = deviceContext->Map(/*_cameraBuffer.Get()*/_vsBuffers[1].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	pCamBuff = (CameraBufferType*)mappedResource.pData;

	// Copy the camera position into the constant buffer.
	pCamBuff->cameraPosition = cameraPosition;
	pCamBuff->padding = 0.0f;

	// Unlock the camera constant buffer.
	deviceContext->Unmap(/*_cameraBuffer.Get()*/_vsBuffers[1].Get(), 0);

	// Set the position of the camera constant buffer in the vertex shader.

	// Now set the camera constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, /*_cameraBuffer.GetAddressOf()*/_vsBuffers[1].GetAddressOf());


	//////////////// LIGHT POSITION - VS BUFFER 2 ///////////////////////
	bufferNumber = 2;

	// Lock the light position constant buffer so it can be written to.
	//result = deviceContext->Map(/*_lightPositionBuffer.Get()*/_buffers[4].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	result = deviceContext->Map(/*_lightPositionBuffer.Get()*/_vsBuffers[2].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	pLightPosBuff = (LightPositionBufferType*)mappedResource.pData;

	// Copy the light position variables into the constant buffer.
	pLightPosBuff->lightPosition[0] = lights[0]->GetPosition();
	pLightPosBuff->lightPosition[1] = lights[1]->GetPosition();
	pLightPosBuff->lightPosition[2] = lights[2]->GetPosition();
	pLightPosBuff->lightPosition[3] = lights[3]->GetPosition();

	// Unlock the constant buffer.
	//deviceContext->Unmap(/*_lightPositionBuffer.Get()*/_buffers[4].Get(), 0);
	deviceContext->Unmap(/*_lightPositionBuffer.Get()*/_vsBuffers[2].Get(), 0);

	// Finally set the constant buffer in the vertex shader with the updated values.
//	deviceContext->VSSetConstantBuffers(bufferNumber, 1, /*_lightPositionBuffer.GetAddressOf()*/_buffers[4].GetAddressOf());
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, /*_lightPositionBuffer.GetAddressOf()*/_vsBuffers[2].GetAddressOf());


	//////////////// CLIP PLANE - VS BUFFER 3 ///////////////////////
	// Lock the clip plane constant buffer so it can be written to.
	bufferNumber = 3;

//	result = deviceContext->Map(/*_clipPlaneBuffer.Get()*/_buffers[6].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	result = deviceContext->Map(/*_clipPlaneBuffer.Get()*/_vsBuffers[3].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the clip plane constant buffer.
	pClipPlaneBuff = (ClipPlaneBufferType*)mappedResource.pData;

	// Copy the clip plane into the clip plane constant buffer.
	pClipPlaneBuff->clipPlane = clipPlane;

	// Unlock the buffer.
//	deviceContext->Unmap(/*_clipPlaneBuffer.Get()*/_buffers[6].Get(), 0);
	deviceContext->Unmap(/*_clipPlaneBuffer.Get()*/_vsBuffers[3].Get(), 0);

	// Now set the clip plane constant buffer in the vertex shader with the updated values.
	//deviceContext->VSSetConstantBuffers(bufferNumber, 1, /*_clipPlaneBuffer.GetAddressOf()*/_buffers[6].GetAddressOf());
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, /*_clipPlaneBuffer.GetAddressOf()*/_vsBuffers[3].GetAddressOf());

	/////////// FOG INIT - VS BUFFER 4 /////////////////////////// @TODO: is the data packed correctly???
	// Lock the fog constant buffer so it can be written to.
	bufferNumber = 4; ///////////@TODO: fix these numbers in correct order

//	result = deviceContext->Map(/*_fogBuffer.Get()*/_buffers[5].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	result = deviceContext->Map(/*_fogBuffer.Get()*/_vsBuffers[4].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	pFogBuff = (FogBufferType*)mappedResource.pData;

	// Copy the fog information into the fog constant buffer.
	pFogBuff->fogStart = fogStart;
	pFogBuff->fogEnd = fogEnd;

	// Unlock the constant buffer.
	//deviceContext->Unmap(/*_fogBuffer.Get()*/_vsBuffers[5].Get(), 0);
	deviceContext->Unmap(/*_fogBuffer.Get()*/_vsBuffers[4].Get(), 0);

	// Now set the fog buffer in the vertex shader with the updated values.
	//deviceContext->VSSetConstantBuffers(bufferNumber, 1, /*_fogBuffer.GetAddressOf()*/_vsBuffers[5].GetAddressOf());
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, /*_fogBuffer.GetAddressOf()*/_vsBuffers[4].GetAddressOf());

	/////////////////////////////////////////////////////////////
	/////////////////////// PS BUFFERS //////////////////////////
	/////////////////////////////////////////////////////////////
	
	///////////////////////// LIGHT INIT - PS BUFFER 0 //////////////////////
	// Lock the light constant buffer so it can be written to.
	bufferNumber = 0;

	//result = deviceContext->Map(/*_lightBuffer.Get()*/_buffers[2].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	result = deviceContext->Map(_psBuffers[0].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	pLightBuff = (LightBufferType*)mappedResource.pData;

	// Copy the lighting variables into the constant buffer.
	pLightBuff->ambientColor = ambientColor;
	pLightBuff->diffuseColor = diffuseColor;
	pLightBuff->lightDirection = lightDirection;
	pLightBuff->specularColor = specularColor;
	pLightBuff->specularPower = specularPower;

	// Unlock the constant buffer.
//	deviceContext->Unmap(/*_lightBuffer.Get()*/_buffers[2].Get(), 0);
	deviceContext->Unmap(/*_lightBuffer.Get()*/_psBuffers[0].Get(), 0);

	// Finally set the light constant buffer in the pixel shader with the updated values.
	//deviceContext->PSSetConstantBuffers(bufferNumber, 1, /*_lightBuffer.GetAddressOf()*/_buffers[2].GetAddressOf());
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, /*_lightBuffer.GetAddressOf()*/_psBuffers[0].GetAddressOf());


	////////////// LIGHT COLOR - PS BUFFER 1 //////////////////
	bufferNumber = 1;

	// Lock the light color constant buffer so it can be written to.
	//result = deviceContext->Map(/*_lightColorBuffer.Get()*/_buffers[3].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	result = deviceContext->Map(/*_lightColorBuffer.Get()*/_psBuffers[1].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	pLightColBuff = (LightColorBufferType*)mappedResource.pData;

	// Copy the light color variables into the constant buffer.
	pLightColBuff->diffuseColor[0] = lights[0]->GetDiffuseColor();
	pLightColBuff->diffuseColor[1] = lights[1]->GetDiffuseColor();
	pLightColBuff->diffuseColor[2] = lights[2]->GetDiffuseColor();
	pLightColBuff->diffuseColor[3] = lights[3]->GetDiffuseColor();

	// Unlock the constant buffer.
	//deviceContext->Unmap(/*_lightColorBuffer.Get()*/_buffers[3].Get(), 0);
	deviceContext->Unmap(/*_lightColorBuffer.Get()*/_psBuffers[1].Get(), 0);

	// Finally set the constant buffer in the pixel shader with the updated values.
	//deviceContext->PSSetConstantBuffers(bufferNumber, 1, /*_lightColorBuffer.GetAddressOf()*/_buffers[3].GetAddressOf());
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, /*_lightColorBuffer.GetAddressOf()*/_psBuffers[1].GetAddressOf());


	////////////// TEX TRANSLATION - PS BUFFER 2 //////////////////
	bufferNumber = 2;

	// Lock the texture translation constant buffer so it can be written to.
	//result = deviceContext->Map(/*_translateBuffer.Get()*/_buffers[7].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	result = deviceContext->Map(/*_translateBuffer.Get()*/_psBuffers[2].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the texture translation constant buffer.
	pTranslateBuff = (TranslateBufferType*)mappedResource.pData;

	// Copy the translation value into the texture translation constant buffer.
	pTranslateBuff->translation = translation;

	// Unlock the buffer.
	//deviceContext->Unmap(/*_translateBuffer.Get()*/_buffers[7].Get(), 0);
	deviceContext->Unmap(/*_translateBuffer.Get()*/_psBuffers[2].Get(), 0);
	
	// Now set the texture translation constant buffer in the pixel shader with the updated values.
	//deviceContext->PSSetConstantBuffers(bufferNumber, 1, /*_translateBuffer.GetAddressOf()*/_buffers[7].GetAddressOf());
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, /*_translateBuffer.GetAddressOf()*/_psBuffers[2].GetAddressOf());

	/////////////////// TRANSPARENCY BUFFER - PS BUFFER 3 ///////////////////////////
	bufferNumber = 3;

	// Lock the transparent constant buffer so it can be written to.
	//result = deviceContext->Map(/*_transparentBuffer.Get()*/_buffers[8].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	result = deviceContext->Map(/*_transparentBuffer.Get()*/_psBuffers[3].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the transparent constant buffer.
	pTransparentBuff = (TransparentBufferType*)mappedResource.pData;

	// Copy the blend amount value into the transparent constant buffer.
	pTransparentBuff->blendAmount = transparency;

	// Unlock the buffer.
	//deviceContext->Unmap(/*_transparentBuffer.Get()*/_buffers[8].Get(), 0);
	deviceContext->Unmap(/*_transparentBuffer.Get()*/_psBuffers[3].Get(), 0);

	// Now set the texture translation constant buffer in the pixel shader with the updated values.
	//deviceContext->PSSetConstantBuffers(bufferNumber, 1, /*_transparentBuffer.GetAddressOf()*/_buffers[8].GetAddressOf());
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, /*_transparentBuffer.GetAddressOf()*/_psBuffers[3].GetAddressOf());

	return true;
}