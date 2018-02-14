#include "DepthShaderClass.h"

//bool DepthShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, MatrixBufferType& transforms/*,XMMATRIX worldMatrix, XMMATRIX viewMatrix,XMMATRIX projectionMatrix*/)
//{
//	// Set the shader parameters that it will use for rendering.
//	SetShaderParameters(deviceContext, transforms/*,worldMatrix, viewMatrix, projectionMatrix*/);
//
//	// Now render the prepared buffers with the shader.
//	RenderShader(deviceContext, indexCount);
//
//	return true;
//}

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

bool DepthShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, MatrixBufferType& transforms/*, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix*/)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	///////////////////// MATRIX INIT - VS BUFFER 0 //////////////////////////////////
	unsigned int bufferNumber = 0;

	//MatrixBufferType tempMatBuff = { XMMatrixTranspose(worldMatrix), XMMatrixTranspose(viewMatrix), XMMatrixTranspose(projectionMatrix)};		// @SHADOWING
	//MapBuffer(tempMatBuff, _vsBuffers[bufferNumber].Get(), deviceContext);

	MapBuffer(transforms, _vsBuffers[bufferNumber].Get(), deviceContext);

	deviceContext->VSSetConstantBuffers(bufferNumber, 1, _vsBuffers[bufferNumber].GetAddressOf());

	return true;
}