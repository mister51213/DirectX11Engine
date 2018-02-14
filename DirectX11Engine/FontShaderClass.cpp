////////////////////////////////////////////////////////////////////////////////
// Filename: fontshaderclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "fontshaderclass.h"

//bool FontShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, MatrixBufferType transforms,
//	ID3D11ShaderResourceView* texture, vector<Microsoft::WRL::ComPtr <ID3D11ShaderResourceView>>& texViews, XMFLOAT4 pixelColor)
//{
//	bool result;
//
//	// Set the shader parameters that it will use for rendering.
//	result = SetShaderParameters(deviceContext, transforms, texture, texViews, pixelColor);
//	if (!result)
//	{
//		return false;
//	}
//
//	// Now render the prepared buffers with the shader.
//	RenderShader(deviceContext, indexCount);
//
//	return true;
//}

bool FontShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, char* vsFilename, char* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage = 0;

	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;

	// Compile the vertex shader code.
	result = CompileShaders(device, hwnd, vsFilename, psFilename, "FontVertexShader", "FontPixelShader", errorMessage);

	// Create the vertex input layout description.
	polygonLayout[0] = MakeInputElementDesc("POSITION", DXGI_FORMAT_R32G32B32_FLOAT, 0);
	polygonLayout[1] = MakeInputElementDesc("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT);

	// Get a count of the elements in the layout.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	ThrowHResultIf(device->CreateInputLayout(polygonLayout, numElements, _vertexShaderBuffer->GetBufferPointer(),
		_vertexShaderBuffer->GetBufferSize(), &_layout));

	D3D11_SAMPLER_DESC samplerDesc = MakeSamplerDesc();

	// Create the texture sampler state.
	ThrowHResultIf(device->CreateSamplerState(&samplerDesc, &_sampleState));

	// VS Buffers
	_vsBuffers.emplace_back(MakeConstantBuffer<MatrixBufferType>(device));

	// PS Buffers
	_psBuffers.emplace_back(MakeConstantBuffer<PixelBufferType>(device));

	return true;
}

bool FontShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, MatrixBufferType transforms,
	ID3D11ShaderResourceView* texture, vector<Microsoft::WRL::ComPtr <ID3D11ShaderResourceView>>& texViews, XMFLOAT4 pixelColor)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	
	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, texViews.size(), texViews.data()->GetAddressOf());

	///////////////////////////////////////////////////////////////
	///////////////////////// VS BUFFERS //////////////////////////
	///////////////////////////////////////////////////////////////

	///////////////////// MATRIX INIT - VS BUFFER 0 //////////////////////////////////
	unsigned int bufferNumber = 0;
	MatrixBufferType tempMatBuff = transforms;
	MapBuffer(tempMatBuff, _vsBuffers[bufferNumber].Get(), deviceContext);
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, _vsBuffers[bufferNumber].GetAddressOf());

	/////////////////////////////////////////////////////////////
	/////////////////////// PS BUFFERS //////////////////////////
	/////////////////////////////////////////////////////////////
	bufferNumber = 0;
	PixelBufferType tempPixBuff = { pixelColor };
	MapBuffer(tempPixBuff, _psBuffers[bufferNumber].Get(), deviceContext);
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, _psBuffers[bufferNumber].GetAddressOf());

	return true;
}