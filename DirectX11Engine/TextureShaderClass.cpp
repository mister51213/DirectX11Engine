////////////////////////////////////////////////////////////////////////////////
// Filename: TextureShaderClass.cpp
////////////////////////////////////////////////////////////////////////////////

#include "TextureShaderClass.h"

TextureShaderClass::TextureShaderClass()
{}

TextureShaderClass::TextureShaderClass(const TextureShaderClass& other)
{
}

//TextureShaderClass::~TextureShaderClass()
//{}

bool TextureShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, MatrixBufferType& transforms,/* XMMATRIX worldMatrix, XMMATRIX viewMatrix,	XMMATRIX projectionMatrix,*/
	ID3D11ShaderResourceView** textureArray, vector<Microsoft::WRL::ComPtr <ID3D11ShaderResourceView>>& texViews)
{
	bool result;

	// Set the shader parameters that it will use for rendering.
	SetShaderParameters(deviceContext, transforms, /*worldMatrix, viewMatrix, projectionMatrix,*/ textureArray, texViews);

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext, indexCount);

	return true;
}

// The layout will need the match the VertexType in the modelclass.h file as well as the one defined in the color.vs file.
bool TextureShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, char* vsFilename, char* psFilename)
{
	WCHAR* vsFilenameW = charToWChar(vsFilename);
	WCHAR* psFilenameW = charToWChar(psFilename);

	HRESULT result;
	ID3D10Blob* errorMessage = 0;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	
	result = CompileShaders(device, hwnd, vsFilename, psFilename, "TextureVertexShader", "TexturePixelShader", errorMessage);

	// Create the vertex input layout description. This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	polygonLayout[0] = MakeInputElementDesc("POSITION", DXGI_FORMAT_R32G32B32_FLOAT, 0);
	polygonLayout[1] = MakeInputElementDesc("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT);

	// Get a count of the elements in the layout.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	ThrowHResultIf(device->CreateInputLayout(polygonLayout, numElements, _vertexShaderBuffer->GetBufferPointer(),
		_vertexShaderBuffer->GetBufferSize(), &_layout));

	// VS Buffers
	_vsBuffers.emplace_back(MakeConstantBuffer<MatrixBufferType>(device));

	D3D11_SAMPLER_DESC samplerDesc = MakeSamplerDesc();

	// Create the texture sampler state.
	ThrowHResultIf(device->CreateSamplerState(&samplerDesc, &_sampleState));

	return true;
}

bool TextureShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, MatrixBufferType& transforms,/*XMMATRIX worldMatrix, XMMATRIX viewMatrix,XMMATRIX projectionMatrix, */
	ID3D11ShaderResourceView** textureArray, vector<Microsoft::WRL::ComPtr <ID3D11ShaderResourceView>>& texViews)
{
	// Set shader texture resource in the pixel shader.
	//deviceContext->PSSetShaderResources(0, 1, &texture);
	//deviceContext->PSSetShaderResources(0, 2, textureArray);
	deviceContext->PSSetShaderResources(0, texViews.size(), texViews.data()->GetAddressOf());

	D3D11_MAPPED_SUBRESOURCE mappedResource;

	unsigned int bufferNumber = 0;

	//MatrixBufferType tempMatBuff = { XMMatrixTranspose(worldMatrix), XMMatrixTranspose(viewMatrix), XMMatrixTranspose(projectionMatrix) };
	//MapBuffer(tempMatBuff, _vsBuffers[bufferNumber].Get(), deviceContext);
		
	MapBuffer(transforms, _vsBuffers[bufferNumber].Get(), deviceContext);

	deviceContext->VSSetConstantBuffers(bufferNumber, 1, _vsBuffers[bufferNumber].GetAddressOf());

	return true;
}