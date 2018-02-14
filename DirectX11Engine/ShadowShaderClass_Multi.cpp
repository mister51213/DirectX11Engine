////////////////////////////////////////////////////////////////////////////////
// Filename: ShadowShaderClass_Multi.cpp
////////////////////////////////////////////////////////////////////////////////
#include "ShadowShaderClass_Multi.h"

//bool ShadowShaderClass_Multi::Render(ID3D11DeviceContext* deviceContext, int indexCount, MatrixBufferType transforms, LightClass* shadowLight[], vector<Microsoft::WRL::ComPtr <ID3D11ShaderResourceView>>& texViews)
//{
//	// Set the shader parameters that it will use for rendering.
//	SetShaderParameters(deviceContext, transforms, shadowLight, texViews);
//
//	// Now render the prepared buffers with the shader.
//	RenderShader(deviceContext, indexCount);
//
//	return true;
//}

bool ShadowShaderClass_Multi::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
    D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc2;

	CreateShaders(device, hwnd, vsFilename, psFilename);

	// Create the vertex input layout description.
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	polygonLayout[0] = MakeInputElementDesc("POSITION", DXGI_FORMAT_R32G32B32_FLOAT, 0);
	polygonLayout[1] = MakeInputElementDesc("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT);
	polygonLayout[2] = MakeInputElementDesc("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT);


	// Get a count of the elements in the layout.
	unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	ThrowHResultIf(device->CreateInputLayout(polygonLayout, numElements, _vertexShaderBuffer->GetBufferPointer(), _vertexShaderBuffer->GetBufferSize(),&_layout));

	// Create a clamp texture sampler state description.
	ThrowHResultIf(device->CreateSamplerState(&MakeSamplerDesc(D3D11_TEXTURE_ADDRESS_CLAMP), &_sampleStateClamp));

	// VS Buffers
	_vsBuffers.emplace_back(MakeConstantBuffer<MatrixBufferType>(device));
	_vsBuffers.emplace_back(MakeConstantBuffer<SceneLightBufferType_VS>(device));

	// PS Buffers
	_psBuffers.emplace_back(MakeConstantBuffer<SceneLightBufferType_PS>(device));

	return true;
}

bool ShadowShaderClass_Multi::SetShaderParameters(ID3D11DeviceContext* deviceContext, MatrixBufferType transforms, LightClass* shadowLight[], vector<Microsoft::WRL::ComPtr <ID3D11ShaderResourceView>>& texViews)
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

	/////////////////////////////////////////////////////////////
	/////////////////////// PS BUFFERS //////////////////////////
	/////////////////////////////////////////////////////////////

	///////////////////////// LIGHT INIT - PS BUFFER 0 //////////////////////
	bufferNumber = 0;

	SceneLightBufferType_PS tempLightBuff =
	{ XMFLOAT4(.15,.15,.15,1), *shadowLight[0]->GetLightBufferPS(), *shadowLight[1]->GetLightBufferPS(), *shadowLight[2]->GetLightBufferPS() };

	MapBuffer(tempLightBuff, _psBuffers[bufferNumber].Get(), deviceContext);
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, _psBuffers[bufferNumber].GetAddressOf());


	return true;
}

void ShadowShaderClass_Multi::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(_layout.Get());

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(_vertexShader.Get(), NULL, 0);
	deviceContext->PSSetShader(_pixelShader.Get(), NULL, 0);

	// Set the sampler states in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, _sampleStateClamp.GetAddressOf());
	deviceContext->PSSetSamplers(1, 1, _sampleState.GetAddressOf());

	return;
}