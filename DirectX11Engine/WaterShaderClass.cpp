#include "WaterShaderClass.h"

bool WaterShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, char* vsFilename, char* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage = 0;

	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;

	CompileShaders(device, hwnd, vsFilename, psFilename, "WaterVertexShader", "WaterPixelShader", errorMessage);
	
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	polygonLayout[0] = MakeInputElementDesc("POSITION", DXGI_FORMAT_R32G32B32_FLOAT, 0);
	polygonLayout[1] = MakeInputElementDesc("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT);
	
	// Get a count of the elements in the layout.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	ThrowHResultIf(device->CreateInputLayout(polygonLayout, numElements, _vertexShaderBuffer->GetBufferPointer(),
		_vertexShaderBuffer->GetBufferSize(), &_layout));

	// Create a texture sampler state description.
	D3D11_SAMPLER_DESC samplerDesc = MakeSamplerDesc();

	// Create the texture sampler state.
	ThrowHResultIf(device->CreateSamplerState(&samplerDesc, &_sampleState));
	
	// VS Buffers
	_vsBuffers.emplace_back(MakeConstantBuffer<MatrixBufferType>(device));
	_vsBuffers.emplace_back(MakeConstantBuffer<ReflectionBufferType>(device));

	// PS Buffers
	_psBuffers.emplace_back(MakeConstantBuffer<WaterBufferType>(device));

	return true;
}

bool WaterShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, MatrixBufferType& transforms, XMMATRIX reflectionMatrix, ID3D11ShaderResourceView** textureArray, 
	vector<ComPtr <ID3D11ShaderResourceView>>& texViews, float waterTranslation, float reflectRefractScale, float lerpRatio)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	deviceContext->PSSetShaderResources(0, texViews.size(), texViews.data()->GetAddressOf());
	
	///////////////////////////////////////////////////////////////
	///////////////////////// VS BUFFERS //////////////////////////
	///////////////////////////////////////////////////////////////

	///////////////////// MATRIX INIT - VS BUFFER 0 //////////////////////////////////
	unsigned int bufferNumber = 0;
	MapBuffer(transforms, _vsBuffers[bufferNumber].Get(), deviceContext);
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, _vsBuffers[bufferNumber].GetAddressOf());

	///////////////////// REFLECTION INIT - VS BUFFER 1 //////////////////////////////////
	bufferNumber++;
	ReflectionBufferType tempReflectBuff = { XMMatrixTranspose(reflectionMatrix)};
	MapBuffer(tempReflectBuff, _vsBuffers[bufferNumber].Get(), deviceContext);
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, _vsBuffers[bufferNumber].GetAddressOf());

	///////////////////////////////////////////////////////////////
	///////////////////////// PS BUFFERS //////////////////////////
	///////////////////////////////////////////////////////////////

	///////////////////// WATER INIT - PS BUFFER 0 //////////////////////////////////
	bufferNumber = 0;
	WaterBufferType tempWaterBuff = { waterTranslation, reflectRefractScale, lerpRatio, 0.f /*XMFLOAT2(0.0f, 0.0f)*/ };
	MapBuffer(tempWaterBuff, _psBuffers[bufferNumber].Get(), deviceContext);
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, _psBuffers[bufferNumber].GetAddressOf());

	return true;
}
