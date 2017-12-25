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

bool LightShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,XMMATRIX projectionMatrix, 
	XMMATRIX lightViewMatrix, XMMATRIX lightProjectionMatrix, 
	ID3D11ShaderResourceView** textureArray, XMFLOAT3 lightDirection, XMFLOAT4 ambientColor, XMFLOAT4 diffuseColor, 
	LightClass* shadowLight, 
	LightClass* lights[],
	XMFLOAT3 cameraPosition, XMFLOAT4 specularColor, float specularPower, float fogStart, float fogEnd, float translation, float transparency)
{
	// Set the shader parameters that it will use for rendering.
	bool result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, 
		lightViewMatrix, lightProjectionMatrix,
		textureArray, lightDirection, ambientColor, diffuseColor, 
		shadowLight,
		lights, cameraPosition, specularColor, specularPower, fogStart, fogEnd, translation, transparency);
	if (!result)
	{
		return false;
	}

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext, indexCount);

	return true;
}

void LightShaderClass::RenderShader(ID3D11DeviceContext * deviceContext, int indexCount)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(_layout.Get());

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(_vertexShader.Get(), NULL, 0);
	deviceContext->PSSetShader(_pixelShader.Get(), NULL, 0);

	// The new clamp sampler state is set in the pixel shader here.

	// Set the sampler states in the pixel shader.
	deviceContext->PSSetSamplers(1, 1, _sampleState.GetAddressOf());
	deviceContext->PSSetSamplers(0, 1, _sampleStateClamp.GetAddressOf());

	// Render the triangle.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}

bool LightShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, char* vsFilename, char* psFilename)
{
	ID3D10Blob* errorMessage = 0;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[5];
	
	CompileShaders(device, hwnd, vsFilename, psFilename, "LightVertexShader", "LightPixelShader", errorMessage);

	// VERTEX SHADER LAYOUT DESCRIPTION SETUP // //@NOTE - D3D11_APPEND_ALIGNED_ELEMENT is default offset value!
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	polygonLayout[0] = MakeInputElementDesc("POSITION", DXGI_FORMAT_R32G32B32_FLOAT, 0);
	polygonLayout[1] = MakeInputElementDesc("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT);
	polygonLayout[2] = MakeInputElementDesc("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT);
	polygonLayout[3] = MakeInputElementDesc("TANGENT", DXGI_FORMAT_R32G32B32_FLOAT);
	polygonLayout[4] = MakeInputElementDesc("BINORMAL", DXGI_FORMAT_R32G32B32_FLOAT);

	// Create the vertex input layout.
	unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);
	HRESULT result = device->CreateInputLayout(polygonLayout, numElements, _vertexShaderBuffer->GetBufferPointer(), _vertexShaderBuffer->GetBufferSize(), &_layout);
	CHECK(SUCCEEDED(result), "input layout");

	// Create the texture wrap sampler state.
	result = device->CreateSamplerState(&MakeSamplerDesc(), &_sampleState);
	CHECK(SUCCEEDED(result), "sampler state");

	// Create the texture clamp sampler state.
	result = device->CreateSamplerState(&MakeSamplerDesc(D3D11_TEXTURE_ADDRESS_CLAMP), &_sampleStateClamp);
	CHECK(SUCCEEDED(result), "sampler state");


	// VS Buffers
	_vsBuffers.emplace_back(MakeConstantBuffer<MatrixBufferType>(device));
	_vsBuffers.emplace_back(MakeConstantBuffer<CameraBufferType>(device));
	_vsBuffers.emplace_back(MakeConstantBuffer<LightPositionBufferType>(device));
	_vsBuffers.emplace_back(MakeConstantBuffer<LightShadowBufferType>(device)); // NEW SHADOW BUFFER
	//_vsBuffers.emplace_back(MakeConstantBuffer<ClipPlaneBufferType>(device));
	_vsBuffers.emplace_back(MakeConstantBuffer<FogBufferType>(device));

	// PS Buffers
	_psBuffers.emplace_back(MakeConstantBuffer<LightBufferType>(device));
	_psBuffers.emplace_back(MakeConstantBuffer<LightColorBufferType>(device));
	_psBuffers.emplace_back(MakeConstantBuffer<TranslateBufferType>(device));
	_psBuffers.emplace_back(MakeConstantBuffer<TransparentBufferType>(device));

	return true;
}

bool LightShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix,XMMATRIX projectionMatrix, 
	XMMATRIX lightViewMatrix, XMMATRIX lightProjectionMatrix,
	ID3D11ShaderResourceView** textureArray, XMFLOAT3 lightDirection, XMFLOAT4 ambientColor, XMFLOAT4 diffuseColor, 
	LightClass* shadowLight,
	LightClass* lights[],
	XMFLOAT3 cameraPosition, XMFLOAT4 specularColor, float specularPower, float fogStart, float fogEnd, float translation, float transparency)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	/////////////////////// SET TEXTURE RESOURCES //////////////////////
	deviceContext->PSSetShaderResources(0, 7, textureArray); // @SHADOWING - TODO: Feed in texture view elsewhere in the framework!!!

	///////////////////////////////////////////////////////////////
	///////////////////////// VS BUFFERS //////////////////////////
	///////////////////////////////////////////////////////////////

	///////////////////// MATRIX INIT - VS BUFFER 0 //////////////////////////////////
	unsigned int bufferNumber = 0;
	MatrixBufferType tempMatBuff = { XMMatrixTranspose(worldMatrix),XMMatrixTranspose(viewMatrix),XMMatrixTranspose(projectionMatrix),
		XMMatrixTranspose(lightViewMatrix),				// @SHADOWING
		XMMatrixTranspose(lightProjectionMatrix)};		// @SHADOWING
	MapBuffer(tempMatBuff, _vsBuffers[bufferNumber].Get(), deviceContext);
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, _vsBuffers[bufferNumber].GetAddressOf());

	///////////////////// CAM INIT - VS BUFFER 1 //////////////////////////////////
	bufferNumber++;
	CameraBufferType tempCamBuff = {cameraPosition, 0.f};
	MapBuffer(tempCamBuff, _vsBuffers[1].Get(), deviceContext);
	deviceContext->VSSetConstantBuffers(bufferNumber, 1,_vsBuffers[bufferNumber].GetAddressOf());

	//////////////// LIGHT POSITION - VS BUFFER 2 ///////////////////////
	bufferNumber++;

	XMFLOAT4 pos1(lights[0]->GetPosition().x, lights[0]->GetPosition().y, lights[0]->GetPosition().z, 1);
	XMFLOAT4 pos2(lights[1]->GetPosition().x, lights[1]->GetPosition().y, lights[1]->GetPosition().z, 1);
	XMFLOAT4 pos3(lights[2]->GetPosition().x, lights[2]->GetPosition().y, lights[2]->GetPosition().z, 1);
	XMFLOAT4 pos4(lights[3]->GetPosition().x, lights[3]->GetPosition().y, lights[3]->GetPosition().z, 1);
	//LightPositionBufferType tempLightPosBuff = { lights[0]->GetPosition(), lights[1]->GetPosition(), lights[2]->GetPosition(), lights[3]->GetPosition() };
	LightPositionBufferType tempLightPosBuff = { pos1, pos2, pos3, pos4 };
	MapBuffer(tempLightPosBuff, _vsBuffers[bufferNumber].Get(), deviceContext);
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, _vsBuffers[bufferNumber].GetAddressOf());

	////////////////// LIGHT SHADOW - VS BUFFER 3 ///////////////////////	@SHADOWING
	bufferNumber++;
	LightShadowBufferType tempShadowBuff = { XMFLOAT3(shadowLight->GetPosition().x, shadowLight->GetPosition().y, shadowLight->GetPosition().z) };
	MapBuffer(tempShadowBuff, _vsBuffers[bufferNumber].Get(), deviceContext);
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, _vsBuffers[bufferNumber].GetAddressOf());

	/////////// FOG INIT - VS BUFFER 3 /////////////////////////// @TODO: is the data packed correctly???
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