////////////////////////////////////////////////////////////////////////////////
// Filename: lightshaderclass.cpp
////////////////////////////////////////////////////////////////////////////////

#include "lightshaderclass.h"

bool LightShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, MatrixBufferType& transforms,
	ID3D11ShaderResourceView** textureArray, vector<Microsoft::WRL::ComPtr <ID3D11ShaderResourceView>>& texViews, XMFLOAT4 ambientColor, 
	LightClass* shadowLight[], XMFLOAT3 cameraPosition, float fogStart, float fogEnd, float translation, float transparency)
{
	// Set the shader parameters that it will use for rendering.
	SetShaderParameters(deviceContext, transforms, textureArray, texViews, ambientColor, shadowLight, cameraPosition, fogStart, fogEnd, translation, transparency);

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

	// Set the sampler states in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, _sampleStateClamp.GetAddressOf());
	deviceContext->PSSetSamplers(1, 1, _sampleState.GetAddressOf());
	deviceContext->PSSetSamplers(2, 1, _sampleStateComp.GetAddressOf());

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
	
	ThrowHResultIf(device->CreateInputLayout(polygonLayout, numElements, _vertexShaderBuffer->GetBufferPointer(), _vertexShaderBuffer->GetBufferSize(), &_layout));
	
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
	_vsBuffers.emplace_back(MakeConstantBuffer<SceneLightBufferType_VS>(device));	
	_vsBuffers.emplace_back(MakeConstantBuffer<CameraBufferType>(device));
	_vsBuffers.emplace_back(MakeConstantBuffer<FogBufferType>(device));

	// PS Buffers
	_psBuffers.emplace_back(MakeConstantBuffer<SceneLightBufferType_PS>(device));
	_psBuffers.emplace_back(MakeConstantBuffer<TranslateBufferType>(device));
	_psBuffers.emplace_back(MakeConstantBuffer<TransparentBufferType>(device));

	return true;
}

bool LightShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, MatrixBufferType& transforms,
	ID3D11ShaderResourceView** textureArray, vector<Microsoft::WRL::ComPtr <ID3D11ShaderResourceView>>& texViews,XMFLOAT4 ambientColor,	
	LightClass* shadowLight[],XMFLOAT3 cameraPosition, float fogStart, float fogEnd, float translation, float transparency)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	/////////////////////// SET TEXTURE RESOURCES //////////////////////
	deviceContext->PSSetShaderResources(0, texViews.size(), texViews.data()->GetAddressOf());

	///////////////////////////////////////////////////////////////
	///////////////////////// VS BUFFERS //////////////////////////
	///////////////////////////////////////////////////////////////

	///////////////////// MATRIX INIT - VS BUFFER 0 //////////////////////////////////
	unsigned int bufferNumber = 0;
	MapBuffer(transforms, _vsBuffers[bufferNumber].Get(), deviceContext);
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, _vsBuffers[bufferNumber].GetAddressOf());

	///////////////////// LIGHT INIT - VS BUFFER 1 //////////////////////////////////
	bufferNumber++;
	SceneLightBufferType_VS sceneLights_VS = 
	{ *shadowLight[0]->GetLightBufferVS(), *shadowLight[1]->GetLightBufferVS(), *shadowLight[2]->GetLightBufferVS() };
	sceneLights_VS.lights[0].viewMatrix = XMMatrixTranspose(sceneLights_VS.lights[0].viewMatrix);
	sceneLights_VS.lights[0].projectionMatrix = XMMatrixTranspose(sceneLights_VS.lights[0].projectionMatrix);
	sceneLights_VS.lights[1].viewMatrix = XMMatrixTranspose(sceneLights_VS.lights[1].viewMatrix);
	sceneLights_VS.lights[1].projectionMatrix = XMMatrixTranspose(sceneLights_VS.lights[1].projectionMatrix);
	sceneLights_VS.lights[2].viewMatrix = XMMatrixTranspose(sceneLights_VS.lights[2].viewMatrix);
	sceneLights_VS.lights[2].projectionMatrix = XMMatrixTranspose(sceneLights_VS.lights[2].projectionMatrix);
	MapBuffer(sceneLights_VS, _vsBuffers[bufferNumber].Get(), deviceContext);
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, _vsBuffers[bufferNumber].GetAddressOf());
	
	///////////////////// CAM INIT - VS BUFFER 1 //////////////////////////////////
	bufferNumber++;
	CameraBufferType tempCamBuff = { cameraPosition, 0.f };
	MapBuffer(tempCamBuff, _vsBuffers[bufferNumber].Get(), deviceContext);
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
	SceneLightBufferType_PS tempLightBuff = 
	{ ambientColor, *shadowLight[0]->GetLightBufferPS(), *shadowLight[1]->GetLightBufferPS(), *shadowLight[2]->GetLightBufferPS() };
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