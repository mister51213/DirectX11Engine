///////////////////////////////////////////////////////////////////////////////
// Filename: textclass.cpp
///////////////////////////////////////////////////////////////////////////////
#include "textclass.h"

TextClass::TextClass()
{}

TextClass::TextClass(const TextClass& other)
{}

TextClass::~TextClass()
{}

bool TextClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int screenWidth, int screenHeight, int maxLength,
	bool shadow, FontClass* Font, char* text, int positionX, int positionY, float red, float green, float blue)
{
	bool result;


	// Store the screen width and height.
	_screenWidth = screenWidth;
	_screenHeight = screenHeight;

	// Store the maximum length of the sentence.
	_maxLength = maxLength;

	// Initalize the sentence.
	result = InitializeSentence(device, deviceContext, Font, text, positionX, positionY, red, green, blue);
	if (!result)
	{
		return false;
	}

	return true;
}

void TextClass::Render(ID3D11DeviceContext* deviceContext, ShaderManagerClass* pShaderManager, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX orthoMatrix, ID3D11ShaderResourceView* fontTexture, vector<Microsoft::WRL::ComPtr <ID3D11ShaderResourceView>>& texViews)
{
	// Draw the sentence.
	RenderSentence(deviceContext, pShaderManager, worldMatrix, viewMatrix, orthoMatrix, fontTexture, texViews);

	return;
}

bool TextClass::InitializeSentence(ID3D11Device* device, ID3D11DeviceContext* deviceContext, FontClass* Font, char* text, int positionX,
	int positionY, float red, float green, float blue)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;


	// Set the vertex and index count.
	_vertexCount = 6 * _maxLength;
	_indexCount = 6 * _maxLength;

	// Create the vertex array.
	vertices = new VertexType[_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[_indexCount];
	if (!indices)
	{
		return false;
	}

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VertexType) * _vertexCount));

	// Initialize the index array.
	for (i = 0; i<_indexCount; i++)
	{
		indices[i] = i;
	}

	// Set up the description of the dynamic vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * _vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * _indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &_vertexBuffer2);
	if (FAILED(result))
	{
		return false;
	}

	result = device->CreateBuffer(&indexBufferDesc, &indexData, &_indexBuffer2);
	if (FAILED(result))
	{
		return false;
	}

	// Release the vertex array as it is no longer needed.
	delete[] vertices;
	vertices = 0;

	// Release the index array as it is no longer needed.
	delete[] indices;
	indices = 0;

	// Now add the text data to the sentence buffers.
	result = UpdateSentence(deviceContext, Font, text, positionX, positionY, red, green, blue);
	if (!result)
	{
		return false;
	}

	return true;
}


bool TextClass::UpdateSentence(ID3D11DeviceContext* deviceContext, FontClass* Font, char* text, int positionX, int positionY, float red,
	float green, float blue)
{
	int numLetters;
	VertexType* vertices;
	float drawX, drawY;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	void* verticesPtr;
	HRESULT result;


	// Store the color of the sentence.
	_pixelColor = XMFLOAT4(red, green, blue, 1.0f);

	// Get the number of letters in the sentence.
	numLetters = (int)strlen(text);

	// Check for possible buffer overflow.
	if (numLetters > _maxLength)
	{
		return false;
	}

	// Create the vertex array.
	vertices = new VertexType[_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VertexType) * _vertexCount));

	// Calculate the X and Y pixel position on the screen to start drawing to.
	drawX = (float)(((_screenWidth / 2) * -1) + positionX);
	drawY = (float)((_screenHeight / 2) - positionY);

	// Use the font class to build the vertex array from the sentence text and sentence draw location.
	Font->BuildVertexArray((void*)vertices, text, drawX, drawY);

	// Lock the vertex buffer.
	result = deviceContext->Map(_vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the mapped resource data pointer.
	verticesPtr = (void*)mappedResource.pData;

	// Copy the vertex array into the vertex buffer.
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * _vertexCount));

	// Unlock the vertex buffer.
	deviceContext->Unmap(_vertexBuffer.Get(), 0);

	//SHADOWING
	memset(vertices, 0, (sizeof(VertexType) * _vertexCount));

	drawX = (float)((((_screenWidth / 2) * -1) + positionX) + 2);
	drawY = (float)(((_screenHeight / 2) - positionY) - 2);
	Font->BuildVertexArray((void*)vertices, text, drawX, drawY);

	result = deviceContext->Map(_vertexBuffer2.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}
	verticesPtr = (void*)mappedResource.pData;
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * _vertexCount));
	deviceContext->Unmap(_vertexBuffer2.Get(), 0);

	// Release the vertex array as it is no longer needed.
	delete[] vertices;
	vertices = 0;

	return true;
}

// HIS @CUSTOM
void TextClass::RenderSentence(ID3D11DeviceContext* deviceContext, ShaderManagerClass* pShaderManager, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX orthoMatrix, ID3D11ShaderResourceView* fontTexture, vector<Microsoft::WRL::ComPtr <ID3D11ShaderResourceView>>& texViews)
{
	unsigned int stride, offset;
	XMFLOAT4 shadowColor;
	bool result;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	shadowColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	deviceContext->IASetVertexBuffers(0, 1, _vertexBuffer2.GetAddressOf(), &stride, &offset);
	deviceContext->IASetIndexBuffer(_indexBuffer2.Get(), DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pShaderManager->RenderFontShader(deviceContext, _indexCount, worldMatrix, viewMatrix, orthoMatrix, fontTexture, texViews, shadowColor);

	// Render the text buffers.
	deviceContext->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset);
	deviceContext->IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Render the text using the font shader.
	//result = pShaderManager->RenderFontShader(deviceContext, _indexCount, worldMatrix, viewMatrix, orthoMatrix, fontTexture, _pixelColor); //@TODO: implement generic function
	//if (!result)
	//{
	//	false;
	//}
	result = pShaderManager->_FontShader->Render(deviceContext, _indexCount, worldMatrix, viewMatrix, orthoMatrix, fontTexture, texViews, _pixelColor); //@TODO: implement generic function
	if (!result)
	{
		false;
	}

	deviceContext->DrawIndexed(_indexCount, 0, 0);

	return;
}