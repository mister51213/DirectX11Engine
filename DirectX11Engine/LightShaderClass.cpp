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
	ID3D11ShaderResourceView** textureArray, XMFLOAT4 ambientColor, LightClass* shadowLight[], 
	XMFLOAT3 cameraPosition, float fogStart, float fogEnd, float translation, float transparency)
{
	// Set the shader parameters that it will use for rendering.
	SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, 
		textureArray, ambientColor, shadowLight, cameraPosition, fogStart, fogEnd, translation, transparency);

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
	deviceContext->PSSetSamplers(0, 1, _sampleStateClamp.GetAddressOf());
	deviceContext->PSSetSamplers(1, 1, _sampleState.GetAddressOf());
	deviceContext->PSSetSamplers(2, 1, _sampleStateComp.GetAddressOf());

	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}

bool LightShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, char* vsFilename, char* psFilename)
{
	ID3D10Blob* errorMessage = 0;
	
	CompileShaders(device, hwnd, vsFilename, psFilename, "LightVertexShader", "LightPixelShader", errorMessage);

	// VERTEX SHADER LAYOUT DESCRIPTION SETUP // 
	//@NOTE - D3D11_APPEND_ALIGNED_ELEMENT is default offset value!
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	D3D11_INPUT_ELEMENT_DESC polygonLayout[5];
	polygonLayout[0] = MakeInputElementDesc("POSITION", DXGI_FORMAT_R32G32B32_FLOAT, 0);
	polygonLayout[1] = MakeInputElementDesc("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT);
	polygonLayout[2] = MakeInputElementDesc("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT);
	polygonLayout[3] = MakeInputElementDesc("TANGENT", DXGI_FORMAT_R32G32B32_FLOAT);
	polygonLayout[4] = MakeInputElementDesc("BINORMAL", DXGI_FORMAT_R32G32B32_FLOAT);

	// Create the vertex input layout.
	unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);
	
	//D3D11_INPUT_ELEMENT_DESC polygonLayout2[2]; // invalid argument for testing purposes
	ThrowHResultIf(device->CreateInputLayout(polygonLayout/*polygonLayout2*/, numElements, _vertexShaderBuffer->GetBufferPointer(), _vertexShaderBuffer->GetBufferSize(), &_layout));
	
	// Create the texture wrap sampler state.
	ThrowHResultIf(device->CreateSamplerState(&MakeSamplerDesc(), &_sampleState));

	// Create the texture clamp sampler state.
	ThrowHResultIf(device->CreateSamplerState(&MakeSamplerDesc(D3D11_TEXTURE_ADDRESS_CLAMP), &_sampleStateClamp));

	D3D11_SAMPLER_DESC comparisonDesc = MakeSamplerDesc(D3D11_TEXTURE_ADDRESS_CLAMP);
	comparisonDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	comparisonDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	ThrowHResultIf(device->CreateSamplerState(&comparisonDesc, &_sampleStateComp));

	// VS Buffers
	_vsBuffers.emplace_back(MakeConstantBuffer<MatrixBufferType>(device));
	_vsBuffers.emplace_back(MakeConstantBuffer<CameraBufferType>(device));
	_vsBuffers.emplace_back(MakeConstantBuffer<FogBufferType>(device));

	// PS Buffers
	_psBuffers.emplace_back(MakeConstantBuffer<SceneLightBufferType>(device));
	_psBuffers.emplace_back(MakeConstantBuffer<TranslateBufferType>(device));
	_psBuffers.emplace_back(MakeConstantBuffer<TransparentBufferType>(device));

	return true;
}

bool LightShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
	ID3D11ShaderResourceView** textureArray, XMFLOAT4 ambientColor,	LightClass* shadowLight[],
	XMFLOAT3 cameraPosition, float fogStart, float fogEnd, float translation, float transparency)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	/////////////////////// SET TEXTURE RESOURCES //////////////////////
	deviceContext->PSSetShaderResources(0, 9, textureArray); // @SHADOWING - TODO: Feed in texture view elsewhere in the framework!!!

	///////////////////////////////////////////////////////////////
	///////////////////////// VS BUFFERS //////////////////////////
	///////////////////////////////////////////////////////////////

	///////////////////// MATRIX INIT - VS BUFFER 0 //////////////////////////////////
	unsigned int bufferNumber = 0;

	// make temp list of transposed light matrices @CAUTION - need to define copy constructor here_????
	LightDataTemplate_VS tempLightsVS[NUM_LIGHTS] = { *shadowLight[0]->GetLightBufferVS(), *shadowLight[1]->GetLightBufferVS(), *shadowLight[2]->GetLightBufferVS() };

	tempLightsVS[0].viewMatrix = XMMatrixTranspose(tempLightsVS[0].viewMatrix);
	tempLightsVS[1].viewMatrix = XMMatrixTranspose(tempLightsVS[1].viewMatrix);
	tempLightsVS[2].viewMatrix = XMMatrixTranspose(tempLightsVS[2].viewMatrix);

	tempLightsVS[0].projectionMatrix = XMMatrixTranspose(tempLightsVS[0].projectionMatrix);
	tempLightsVS[1].projectionMatrix = XMMatrixTranspose(tempLightsVS[1].projectionMatrix);
	tempLightsVS[2].projectionMatrix = XMMatrixTranspose(tempLightsVS[2].projectionMatrix);

	MatrixBufferType tempMatBuff = {
		XMMatrixTranspose(worldMatrix), XMMatrixTranspose(viewMatrix), XMMatrixTranspose(projectionMatrix),
		tempLightsVS[0], tempLightsVS[1], tempLightsVS[2] };
	MapBuffer(tempMatBuff, _vsBuffers[bufferNumber].Get(), deviceContext);
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, _vsBuffers[bufferNumber].GetAddressOf());

	///////////////////// CAM INIT - VS BUFFER 1 //////////////////////////////////
	bufferNumber++;
	CameraBufferType tempCamBuff = { cameraPosition, 0.f };
	MapBuffer(tempCamBuff, _vsBuffers[1].Get(), deviceContext);
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, _vsBuffers[bufferNumber].GetAddressOf());

	/////////// FOG INIT - VS BUFFER 2 /////////////////////////// @TODO: is the data packed correctly???
	bufferNumber++;
	FogBufferType tempFogBuff = { fogStart, fogEnd, 0.f, 0.f };
	MapBuffer(tempFogBuff, _vsBuffers[bufferNumber].Get(), deviceContext);
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, _vsBuffers[bufferNumber].GetAddressOf());

	/////////////////////////////////////////////////////////////
	/////////////////////// PS BUFFERS //////////////////////////
	/////////////////////////////////////////////////////////////

	///////////////////////// LIGHT INIT - PS BUFFER 0 //////////////////////
	bufferNumber = 0;

	//LightDataTemplate_PS tempLightsPS[NUM_LIGHTS] = {*shadowLight[0]->GetLightBufferPS(), *shadowLight[1]->GetLightBufferPS(), *shadowLight[2]->GetLightBufferPS()};

	SceneLightBufferType tempLightBuff = 
	{ ambientColor, *shadowLight[0]->GetLightBufferPS(), *shadowLight[1]->GetLightBufferPS(), *shadowLight[2]->GetLightBufferPS() };
	//{ ambientColor, tempLightsPS[0], tempLightsPS[1], tempLightsPS[2] };

	MapBuffer(tempLightBuff, _psBuffers[bufferNumber].Get(), deviceContext);
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, _psBuffers[bufferNumber].GetAddressOf());
	
	////////////// TEX TRANSLATION - PS BUFFER 1 //////////////////
	bufferNumber++;
	TranslateBufferType tempTransBuff = { translation, XMFLOAT3(0,0,0)};
	MapBuffer(tempTransBuff, _psBuffers[bufferNumber].Get(), deviceContext);
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, _psBuffers[bufferNumber].GetAddressOf());

	/////////////////// TRANSPARENCY BUFFER - PS BUFFER 2 ///////////////////////////
	bufferNumber++;
	TransparentBufferType tempTransparentBuff = { transparency, XMFLOAT3(0, 0, 0) };
	MapBuffer(tempTransparentBuff, _psBuffers[bufferNumber].Get(), deviceContext);
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, _psBuffers[bufferNumber].GetAddressOf());

	return true;
}