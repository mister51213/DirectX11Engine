////////////////////////////////////////////////////////////////////////////////
// Filename: TextureShaderClass.cpp
////////////////////////////////////////////////////////////////////////////////

#include "TextureShaderClass.h"

TextureShaderClass::TextureShaderClass()
{}

TextureShaderClass::TextureShaderClass(const TextureShaderClass& other)
{
}

bool TextureShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, MatrixBufferType& transforms,
	vector<Microsoft::WRL::ComPtr <ID3D11ShaderResourceView>>& texViews, 
	XMFLOAT2& translation, float scale, float gamma)
{
	bool result;

	// Set the shader parameters that it will use for rendering.
	SetShaderParameters(deviceContext, transforms, /*textureArray, */texViews, translation, scale, gamma);

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

	// Create the texture sampler state.
	D3D11_SAMPLER_DESC samplerDesc = MakeSamplerDesc();
	ThrowHResultIf(device->CreateSamplerState(&samplerDesc, &_sampleState));

	// VS Buffers
	_vsBuffers.emplace_back(MakeConstantBuffer<MatrixBufferType>(device));

	// PS Buffers
	_psBuffers.emplace_back(MakeConstantBuffer<TexParamBufferType>(device));

	return true;
}

bool TextureShaderClass::SetShaderParameters(
	ID3D11DeviceContext* deviceContext, MatrixBufferType& transforms,
	vector<Microsoft::WRL::ComPtr <ID3D11ShaderResourceView>>& texViews, 
	XMFLOAT2& translation, float scale, float gamma)
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

	/////////////////////////////////////////////////////////////
	/////////////////////// PS BUFFERS //////////////////////////
	/////////////////////////////////////////////////////////////

	////////////////////// TEXPARAM INIT - PS BUFFER 0 //////////////////////
	bufferNumber = 0;
	TexParamBufferType tempTexBuffer = { translation , scale, gamma};
	MapBuffer(tempTexBuffer, _psBuffers[bufferNumber].Get(), deviceContext);
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, _psBuffers[bufferNumber].GetAddressOf());

	return true;
}