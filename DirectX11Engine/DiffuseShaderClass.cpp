#include "DiffuseShaderClass.h"

bool DiffuseShaderClass::Render(
	ID3D11DeviceContext * deviceContext, int indexCount, MatrixBufferType& transforms,ID3D11ShaderResourceView * texture, 
	vector<Microsoft::WRL::ComPtr <ID3D11ShaderResourceView>>& texViews,XMFLOAT3 lightDirection, XMFLOAT4 ambientColor, 
	XMFLOAT4 diffuseColor, XMFLOAT4 clipPlane)
{
	// Set the shader parameters that it will use for rendering.
	ThrowHResultIf(SetShaderParameters(deviceContext, transforms,/*worldMatrix, viewMatrix, projectionMatrix, */texture, texViews, lightDirection, ambientColor, diffuseColor, clipPlane));

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext, indexCount);

	return true;
}

bool DiffuseShaderClass::InitializeShader(ID3D11Device * device, HWND hwnd, char * vsFilename, char * psFilename)
{
	// Compile the vertex shader code.
	ID3D10Blob* errorMessage = 0;
	CompileShaders(device, hwnd, vsFilename, psFilename, "RefractionVertexShader", "RefractionPixelShader", errorMessage);

	// Create the vertex input layout description - needs to match the VertexType stucture in the ModelClass and in the shader.
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	polygonLayout[0] = MakeInputElementDesc("POSITION", DXGI_FORMAT_R32G32B32_FLOAT, 0);
	polygonLayout[1] = MakeInputElementDesc("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT);
	polygonLayout[2] = MakeInputElementDesc("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT);
	
	// Get a count of the elements in the layout.
	unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	ThrowHResultIf(device->CreateInputLayout(polygonLayout, numElements, _vertexShaderBuffer->GetBufferPointer(),
		_vertexShaderBuffer->GetBufferSize(), &_layout));

	D3D11_SAMPLER_DESC samplerDesc = MakeSamplerDesc();

	// Create the texture sampler state.
	ThrowHResultIf(device->CreateSamplerState(&samplerDesc, &_sampleState));

	// VS Buffers
	_vsBuffers.emplace_back(MakeConstantBuffer<MatrixBufferType>(device));
	_vsBuffers.emplace_back(MakeConstantBuffer<ClipPlaneBufferType>(device));

	// PS Buffers
	_psBuffers.emplace_back(MakeConstantBuffer<LightBufferType>(device));

	return true;
}

bool DiffuseShaderClass::SetShaderParameters(
	ID3D11DeviceContext * deviceContext, MatrixBufferType& transforms,ID3D11ShaderResourceView * texture, 
	vector<Microsoft::WRL::ComPtr <ID3D11ShaderResourceView>>& texViews, XMFLOAT3 lightDirection, 
	XMFLOAT4 ambientColor, XMFLOAT4 diffuseColor, XMFLOAT4 clipPlane)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, texViews.size(), texViews.data()->GetAddressOf());

	///////////////////////////////////////////////////////////////
	///////////////////////// VS BUFFERS //////////////////////////
	///////////////////////////////////////////////////////////////

	///////////////////// MATRIX INIT - VS BUFFER 0 //////////////////////////////////
	unsigned int bufferNumber = 0;

	//MatrixBufferType tempMatBuff = { XMMatrixTranspose(worldMatrix), XMMatrixTranspose(viewMatrix), XMMatrixTranspose(projectionMatrix) };
	//MapBuffer(tempMatBuff, _vsBuffers[bufferNumber].Get(), deviceContext);

	MapBuffer(transforms, _vsBuffers[bufferNumber].Get(), deviceContext);
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, _vsBuffers[bufferNumber].GetAddressOf());

	///////////////////// CLIP INIT - VS BUFFER 1 //////////////////////////////////
	bufferNumber++;
	ClipPlaneBufferType tempClipBuff = { clipPlane };
	MapBuffer(tempClipBuff, _vsBuffers[bufferNumber].Get(), deviceContext);
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, _vsBuffers[bufferNumber].GetAddressOf());

	///////////////////////////////////////////////////////////////
	///////////////////////// PS BUFFERS //////////////////////////
	///////////////////////////////////////////////////////////////

	///////////////////// LIGHT INIT - PS BUFFER 0 //////////////////////////////////
	bufferNumber = 0;
	LightBufferType tempLightBuff = { ambientColor, diffuseColor, lightDirection };
	MapBuffer(tempLightBuff, _psBuffers[bufferNumber].Get(), deviceContext);
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, _psBuffers[bufferNumber].GetAddressOf());

	return true;
}
