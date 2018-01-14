#include "ReflectionShaderClass.h"

ReflectionShaderClass::ReflectionShaderClass()
{}

ReflectionShaderClass::ReflectionShaderClass(const ReflectionShaderClass &)
{}

ReflectionShaderClass::~ReflectionShaderClass()
{}

bool ReflectionShaderClass::Render(ID3D11DeviceContext * deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, 
	ID3D11ShaderResourceView * texture, vector<Microsoft::WRL::ComPtr <ID3D11ShaderResourceView>>& texViews, 
	ID3D11ShaderResourceView * reflectionTexture, XMMATRIX reflectionMatrix)
{
	// Set the shader parameters that it will use for rendering.
	SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture, texViews, reflectionTexture, reflectionMatrix);

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext, indexCount);

	return true;
}

bool ReflectionShaderClass::InitializeShader(ID3D11Device * device, HWND hwnd, char * vsFilename, char * psFilename)
{
	ID3D10Blob* errorMessage = 0;
	HRESULT result = CompileShaders(device, hwnd, vsFilename, psFilename, "ReflectionVertexShader", "ReflectionPixelShader", errorMessage);

	// Create the vertex input layout description - needs to match the VertexType stucture in the ModelClass and in the shader.
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	polygonLayout[0] = MakeInputElementDesc("POSITION", DXGI_FORMAT_R32G32B32_FLOAT, 0);
	polygonLayout[1] = MakeInputElementDesc("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT);
		
	// Create the vertex input layout.
	unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);
	ThrowHResultIf(device->CreateInputLayout(polygonLayout, numElements, _vertexShaderBuffer->GetBufferPointer(),
		_vertexShaderBuffer->GetBufferSize(), &_layout));

	// Create the texture sampler state.
	ThrowHResultIf(device->CreateSamplerState(&MakeSamplerDesc(), &_sampleState));

	// VS Buffers
	_vsBuffers.emplace_back(MakeConstantBuffer<MatrixBufferType>(device));
	_vsBuffers.emplace_back(MakeConstantBuffer<ReflectionBufferType>(device));

	return true;
}

bool ReflectionShaderClass::SetShaderParameters(ID3D11DeviceContext * deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, 
	ID3D11ShaderResourceView * texture, vector<Microsoft::WRL::ComPtr <ID3D11ShaderResourceView>>& texViews, ID3D11ShaderResourceView * reflectionTexture, XMMATRIX reflectionMatrix)
{
	HRESULT result;

	D3D11_MAPPED_SUBRESOURCE mappedResource;

	//// Set shader texture resource in the pixel shader.
	//deviceContext->PSSetShaderResources(0, 1, &texture);
	////Set the reflection texture as the second texture inside the pixel shader.
	//deviceContext->PSSetShaderResources(1, 1, &reflectionTexture);

	//@REFACTOR - should actually take in an array!!!!
	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, texViews[0].GetAddressOf());
	//Set the reflection texture as the second texture inside the pixel shader.
	deviceContext->PSSetShaderResources(1, 1, texViews[1].GetAddressOf());

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

