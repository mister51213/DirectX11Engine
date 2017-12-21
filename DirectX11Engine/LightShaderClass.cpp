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

	// VERTEX SHADER LAYOUT DESCRIPTION SETUP // //@NOTE - D3D11_APPEND_ALIGNED_ELEMENT is default offset value!
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	polygonLayout[0] = MakeInputElementDesc("POSITION", DXGI_FORMAT_R32G32B32_FLOAT, 0);
	polygonLayout[1] = MakeInputElementDesc("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT);
	polygonLayout[2] = MakeInputElementDesc("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT);
	polygonLayout[3] = MakeInputElementDesc("TANGENT", DXGI_FORMAT_R32G32B32_FLOAT);
	polygonLayout[4] = MakeInputElementDesc("BINORMAL", DXGI_FORMAT_R32G32B32_FLOAT);
	
#pragma region LEGACY
	//polygonLayout[0].SemanticName = "POSITION";
	//polygonLayout[0].SemanticIndex = 0;
	//polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	//polygonLayout[0].InputSlot = 0;
	//polygonLayout[0].AlignedByteOffset = 0;
	//polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	//polygonLayout[0].InstanceDataStepRate = 0;
	//polygonLayout[1].SemanticName = "TEXCOORD";
	//polygonLayout[1].SemanticIndex = 0;
	//polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	//polygonLayout[1].InputSlot = 0;
	//polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	//polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	//polygonLayout[1].InstanceDataStepRate = 0;
	//polygonLayout[2].SemanticName = "NORMAL";
	//polygonLayout[2].SemanticIndex = 0;
	//polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	//polygonLayout[2].InputSlot = 0;
	//polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	//polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	//polygonLayout[2].InstanceDataStepRate = 0;
	//polygonLayout[3].SemanticName = "TANGENT";
	//polygonLayout[3].SemanticIndex = 0;
	//polygonLayout[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	//polygonLayout[3].InputSlot = 0;
	//polygonLayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	//polygonLayout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	//polygonLayout[3].InstanceDataStepRate = 0;
	//polygonLayout[4].SemanticName = "BINORMAL";
	//polygonLayout[4].SemanticIndex = 0;
	//polygonLayout[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	//polygonLayout[4].InputSlot = 0;
	//polygonLayout[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	//polygonLayout[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	//polygonLayout[4].InstanceDataStepRate = 0;
#pragma endregion

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
	samplerDesc = MakeSamplerDesc();

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
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	/////////////////////// SET TEXTURE RESOURCES //////////////////////
	deviceContext->PSSetShaderResources(0, 6, textureArray); // sextuple tex with lightmap

	///////////////////////////////////////////////////////////////
	///////////////////////// VS BUFFERS //////////////////////////
	///////////////////////////////////////////////////////////////

	///////////////////// MATRIX INIT - VS BUFFER 0 //////////////////////////////////
	unsigned int bufferNumber = 0;
	MatrixBufferType tempMatBuff = { XMMatrixTranspose(worldMatrix),XMMatrixTranspose(viewMatrix),XMMatrixTranspose(projectionMatrix) };
	MapBuffer(tempMatBuff, _vsBuffers[bufferNumber].Get(), deviceContext);
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, _vsBuffers[bufferNumber].GetAddressOf());

	///////////////////// CAM INIT - VS BUFFER 1 //////////////////////////////////
	bufferNumber++;
	CameraBufferType tempCamBuff = {cameraPosition, 0.f};
	MapBuffer(tempCamBuff, _vsBuffers[1].Get(), deviceContext);
	deviceContext->VSSetConstantBuffers(bufferNumber, 1,_vsBuffers[bufferNumber].GetAddressOf());

	//////////////// LIGHT POSITION - VS BUFFER 2 ///////////////////////
	bufferNumber++;
	LightPositionBufferType tempLightPosBuff = { lights[0]->GetPosition(), lights[1]->GetPosition(), lights[2]->GetPosition(), lights[3]->GetPosition() };
	MapBuffer(tempLightPosBuff, _vsBuffers[bufferNumber].Get(), deviceContext);
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, _vsBuffers[bufferNumber].GetAddressOf());

	//////////////// CLIP PLANE - VS BUFFER 3 ///////////////////////
	bufferNumber++;
	ClipPlaneBufferType tempClipBuff = { clipPlane };
	MapBuffer(tempClipBuff, _vsBuffers[bufferNumber].Get(), deviceContext);
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, _vsBuffers[bufferNumber].GetAddressOf());

	/////////// FOG INIT - VS BUFFER 4 /////////////////////////// @TODO: is the data packed correctly???
	bufferNumber++;
	FogBufferType tempFogBuff = { fogStart, fogEnd, 0.f, 0.f };
	MapBuffer(tempFogBuff, _vsBuffers[bufferNumber].Get(), deviceContext);
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, _vsBuffers[bufferNumber].GetAddressOf());

	/////////////////////////////////////////////////////////////
	/////////////////////// PS BUFFERS //////////////////////////
	/////////////////////////////////////////////////////////////
	
	///////////////////////// LIGHT INIT - PS BUFFER 0 //////////////////////
	bufferNumber = 0;
	LightBufferType tempLightBuff = {ambientColor,diffuseColor,lightDirection,specularPower,specularColor};
	MapBuffer(tempLightBuff, _psBuffers[bufferNumber].Get(), deviceContext);
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, _psBuffers[bufferNumber].GetAddressOf());
	
	////////////// LIGHT COLOR - PS BUFFER 1 //////////////////
	bufferNumber++;
	LightColorBufferType tempLightColBuff = { lights[0]->GetDiffuseColor(), lights[1]->GetDiffuseColor(), lights[2]->GetDiffuseColor(), lights[3]->GetDiffuseColor() }; 
	MapBuffer(tempLightColBuff, _psBuffers[bufferNumber].Get(), deviceContext);
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, _psBuffers[bufferNumber].GetAddressOf());

	////////////// TEX TRANSLATION - PS BUFFER 2 //////////////////
	bufferNumber++;
	TranslateBufferType tempTransBuff = { translation, XMFLOAT3(0,0,0)};
	MapBuffer(tempTransBuff, _psBuffers[bufferNumber].Get(), deviceContext);
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, _psBuffers[bufferNumber].GetAddressOf());

	/////////////////// TRANSPARENCY BUFFER - PS BUFFER 3 ///////////////////////////
	bufferNumber++;
	TransparentBufferType tempTransparentBuff = { transparency, XMFLOAT3(0, 0, 0) };
	MapBuffer(tempTransparentBuff, _psBuffers[bufferNumber].Get(), deviceContext);
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, _psBuffers[bufferNumber].GetAddressOf());

	return true;
}