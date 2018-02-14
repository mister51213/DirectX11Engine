#include "DepthShaderClass.h"

bool DepthShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, char* vsFilename, char* psFilename)
{
	ID3D10Blob* errorMessage = 0;

	CompileShaders(device, hwnd, vsFilename, psFilename, "DepthVertexShader", "DepthPixelShader", errorMessage);

	// Create the vertex input layout description - needs to match the VertexType stucture in the ModelClass and in the shader.
	D3D11_INPUT_ELEMENT_DESC polygonLayout[1];
	polygonLayout[0] = MakeInputElementDesc("POSITION", DXGI_FORMAT_R32G32B32_FLOAT, 0);

	// Get a count of the elements in the layout.
	unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);
	ThrowHResultIf(device->CreateInputLayout(polygonLayout, numElements, _vertexShaderBuffer->GetBufferPointer(), _vertexShaderBuffer->GetBufferSize(), &_layout));

	_vsBuffers.emplace_back(MakeConstantBuffer<MatrixBufferType>(device));

	return true;
}

bool DepthShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, MatrixBufferType& transforms)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	///////////////////// MATRIX INIT - VS BUFFER 0 //////////////////////////////////
	unsigned int bufferNumber = 0;

	MapBuffer(transforms, _vsBuffers[bufferNumber].Get(), deviceContext);

	deviceContext->VSSetConstantBuffers(bufferNumber, 1, _vsBuffers[bufferNumber].GetAddressOf());

	return true;
}