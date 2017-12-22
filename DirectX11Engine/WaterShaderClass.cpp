#include "WaterShaderClass.h"

WaterShaderClass::WaterShaderClass()
{}

WaterShaderClass::WaterShaderClass(const WaterShaderClass &)
{}

WaterShaderClass::~WaterShaderClass()
{}

bool WaterShaderClass::Render(ID3D11DeviceContext * deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, 
	XMMATRIX reflectionMatrix, ID3D11ShaderResourceView** textureArray, float waterTranslation, float reflectRefractScale)
{
	bool result;

	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, reflectionMatrix, textureArray, waterTranslation, reflectRefractScale);
	if (!result)
	{
		return false;
	}

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext, indexCount);

	return true;
}

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
	result = device->CreateInputLayout(polygonLayout, numElements, _vertexShaderBuffer->GetBufferPointer(),
		_vertexShaderBuffer->GetBufferSize(), &_layout);
	if (FAILED(result))
	{
		return false;
	}

	// Create a texture sampler state description.
	D3D11_SAMPLER_DESC samplerDesc = MakeSamplerDesc();
	// Create the texture sampler state.
	result = device->CreateSamplerState(&samplerDesc, &_sampleState);
	if (FAILED(result))
	{
		return false;
	}
	//@TODO  - NOT WORKING!
	//ComPtr<ID3D11SamplerState> sampler = MakeSamplerState(device);
	
	// VS Buffers
	_vsBuffers.emplace_back(MakeConstantBuffer<MatrixBufferType>(device));
	_vsBuffers.emplace_back(MakeConstantBuffer<ReflectionBufferType>(device));

	// PS Buffers
	_psBuffers.emplace_back(MakeConstantBuffer<WaterBufferType>(device));


	return true;
}

bool WaterShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, XMMATRIX reflectionMatrix,
	ID3D11ShaderResourceView** textureArray,
	float waterTranslation, float reflectRefractScale)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	deviceContext->PSSetShaderResources(0, 3, textureArray);
	
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
	ReflectionBufferType tempReflectBuff = { XMMatrixTranspose(reflectionMatrix)};
	MapBuffer(tempReflectBuff, _vsBuffers[bufferNumber].Get(), deviceContext);
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, _vsBuffers[bufferNumber].GetAddressOf());

	///////////////////////////////////////////////////////////////
	///////////////////////// PS BUFFERS //////////////////////////
	///////////////////////////////////////////////////////////////

	///////////////////// WATER INIT - PS BUFFER 0 //////////////////////////////////
	bufferNumber = 0;
	WaterBufferType tempWaterBuff = { waterTranslation, reflectRefractScale, XMFLOAT2(0.0f, 0.0f) };
	MapBuffer(tempWaterBuff, _psBuffers[bufferNumber].Get(), deviceContext);
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, _psBuffers[bufferNumber].GetAddressOf());

	return true;
}
