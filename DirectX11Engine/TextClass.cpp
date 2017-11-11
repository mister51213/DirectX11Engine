///////////////////////////////////////////////////////////////////////////////
// Filename: textclass.cpp
///////////////////////////////////////////////////////////////////////////////
#include "textclass.h"


TextClass::TextClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_vertexBuffer2 = 0;
	m_indexBuffer2 = 0;
}


TextClass::TextClass(const TextClass& other)
{
}


TextClass::~TextClass()
{
}


bool TextClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int screenWidth, int screenHeight, int maxLength,
	bool shadow, FontClass* Font, char* text, int positionX, int positionY, float red, float green, float blue)
{
	bool result;


	// Store the screen width and height.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// Store the maximum length of the sentence.
	m_maxLength = maxLength;

	// Store if this sentence is shadowed or not.
//	m_shadow = shadow;

	// Initalize the sentence.
	result = InitializeSentence(device, deviceContext, Font, text, positionX, positionY, red, green, blue);
	if (!result)
	{
		return false;
	}

	return true;
}


void TextClass::Shutdown()
{
	// Release the buffers.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	if (m_vertexBuffer2)
	{
		m_vertexBuffer2->Release();
		m_vertexBuffer2 = 0;
	}

	if (m_indexBuffer2)
	{
		m_indexBuffer2->Release();
		m_indexBuffer2 = 0;
	}

	return;
}


void TextClass::Render(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX orthoMatrix, ID3D11ShaderResourceView* fontTexture)
{
	// Draw the sentence.
	RenderSentence(deviceContext, worldMatrix, viewMatrix, orthoMatrix, fontTexture);

	//@TODO: RENDER ALL THE SENTENCES

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
	m_vertexCount = 6 * m_maxLength;
	m_indexCount = 6 * m_maxLength;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// Initialize the index array.
	for (i = 0; i<m_indexCount; i++)
	{
		indices[i] = i;
	}

	// Set up the description of the dynamic vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// If shadowed create the second vertex and index buffer.
	//if (m_shadow)
	//{
	//	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer2);
	//	if (FAILED(result))
	//	{
	//		return false;
	//	}

	//	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer2);
	//	if (FAILED(result))
	//	{
	//		return false;
	//	}
	//}

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
	m_pixelColor = XMFLOAT4(red, green, blue, 1.0f);

	// Get the number of letters in the sentence.
	numLetters = (int)strlen(text);

	// Check for possible buffer overflow.
	if (numLetters > m_maxLength)
	{
		return false;
	}

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// Calculate the X and Y pixel position on the screen to start drawing to.
	drawX = (float)(((m_screenWidth / 2) * -1) + positionX);
	drawY = (float)((m_screenHeight / 2) - positionY);

	// Use the font class to build the vertex array from the sentence text and sentence draw location.
	Font->BuildVertexArray((void*)vertices, text, drawX, drawY);

	// Lock the vertex buffer.
	result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the mapped resource data pointer.
	verticesPtr = (void*)mappedResource.pData;

	// Copy the vertex array into the vertex buffer.
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * m_vertexCount));

	// Unlock the vertex buffer.
	deviceContext->Unmap(m_vertexBuffer, 0);

	// If shadowed then do the same for the second vertex buffer but offset by two pixels on both axis.
	//if (m_shadow)
	//{
	//	memset(vertices, 0, (sizeof(VertexType) * m_vertexCount));

	//	drawX = (float)((((m_screenWidth / 2) * -1) + positionX) + 2);
	//	drawY = (float)(((m_screenHeight / 2) - positionY) - 2);
	//	Font->BuildVertexArray((void*)vertices, text, drawX, drawY);

	//	result = deviceContext->Map(m_vertexBuffer2, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	//	if (FAILED(result))
	//	{
	//		return false;
	//	}
	//	verticesPtr = (void*)mappedResource.pData;
	//	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * m_vertexCount));
	//	deviceContext->Unmap(m_vertexBuffer2, 0);
	//}

	// Release the vertex array as it is no longer needed.
	delete[] vertices;
	vertices = 0;

	return true;
}

// HIS @CUSTOM
void TextClass::RenderSentence(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX orthoMatrix, ID3D11ShaderResourceView* fontTexture)
{
	unsigned int stride, offset;
	XMFLOAT4 shadowColor;
	bool result;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// If shadowed then render the shadow text first.
	//if (m_shadow)
	//{
	//	shadowColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	//	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer2, &stride, &offset);
	//	deviceContext->IASetIndexBuffer(m_indexBuffer2, DXGI_FORMAT_R32_UINT, 0);
	//	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//	ShaderManager->RenderFontShader(deviceContext, m_indexCount, worldMatrix, viewMatrix, orthoMatrix, fontTexture, shadowColor);
	//}

	// Render the text buffers.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//ShaderManager->RenderFontShader(deviceContext, m_indexCount, worldMatrix, viewMatrix, orthoMatrix, fontTexture, m_pixelColor);

	// Render the text using the font shader.
	result = _FontShader->Render(deviceContext, m_indexCount, worldMatrix, viewMatrix, orthoMatrix, fontTexture, m_pixelColor);
	if (!result)
	{
		false;
	}

	return;
}

//MINE
//bool TextClass::RenderSentence(ID3D11DeviceContext* deviceContext, SentenceType* sentence, XMMATRIX worldMatrix,
//	XMMATRIX orthoMatrix)
//{
//	unsigned int stride, offset;
//	XMFLOAT4 pixelColor;
//	bool result;
//
//
//	// Set vertex buffer stride and offset.
//	stride = sizeof(VertexType);
//	offset = 0;
//
//	// Set the vertex buffer to active in the input assembler so it can be rendered.
//	deviceContext->IASetVertexBuffers(0, 1, &sentence->vertexBuffer, &stride, &offset);
//
//	// Set the index buffer to active in the input assembler so it can be rendered.
//	deviceContext->IASetIndexBuffer(sentence->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
//
//	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
//	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//
//	// Create a pixel color vector with the input sentence color.
//	pixelColor = XMFLOAT4(sentence->red, sentence->green, sentence->blue, 1.0f);
//
//	// Render the text using the font shader.
//	result = _FontShader->Render(deviceContext, sentence->indexCount, worldMatrix, _baseViewMatrix, orthoMatrix, *_Font->GetTexture(),
//		pixelColor);
//	if (!result)
//	{
//		false;
//	}
//
//	return true;
//}




/////////////////////////////////////////////////////////////////////////////////
//// Filename: textclass.cpp
/////////////////////////////////////////////////////////////////////////////////
//#include "textclass.h"
//
//TextClass::TextClass()
//{
//	_Font = 0;
//	_FontShader = 0;
//
//	_sentence1 = 0;
//	_sentence2 = 0;
//}
//
//TextClass::TextClass(const TextClass& other)
//{
//}
//
//TextClass::~TextClass()
//{
//}
//
//bool TextClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd, int screenWidth, int screenHeight,
//	XMMATRIX baseViewMatrix)
//{
//	bool result;
//
//
//	// Store the screen width and height.
//	_screenWidth = screenWidth;
//	_screenHeight = screenHeight;
//
//	// Store the base view matrix.
//	_baseViewMatrix = baseViewMatrix;
//
//	// Create the font object.
//	_Font = new FontClass;
//	if (!_Font)
//	{
//		return false;
//	}
//
//	// Initialize the font object.
//	// @CUSTOM : removed L from second string param
//	result = _Font->Initialize(device, deviceContext, "../DirectX11Engine/data/fontdata.txt", "../DirectX11Engine/data/font.tga");
//	if (!result)
//	{
//		MessageBox(hwnd, L"Could not initialize the font object.", L"Error", MB_OK);
//		return false;
//	}
//
//	// Create the font shader object.
//	_FontShader = new FontShaderClass;
//	if (!_FontShader)
//	{
//		return false;
//	}
//
//	// Initialize the font shader object.
//	result = _FontShader->Initialize(device, hwnd);
//	if (!result)
//	{
//		MessageBox(hwnd, L"Could not initialize the font shader object.", L"Error", MB_OK);
//		return false;
//	}
//
//	// Initialize the first sentence. @TODO* why 32 not 16?
//	result = InitializeSentence(&_sentence1, 32, device);
//	if (!result)
//	{
//		return false;
//	}
//
//	// Now update the sentence vertex buffer with the new string information.
//	result = UpdateSentence(_sentence1, "Render Count: ", 20, 20, 1.0f, 1.0f, 1.0f, deviceContext);
//	if (!result)
//	{
//		return false;
//	}
//
//	// Initialize the second sentence.
//	result = InitializeSentence(&_sentence2, 16, device);
//	if (!result)
//	{
//		return false;
//	}
//
//	// Now update the sentence vertex buffer with the new string information.
//	result = UpdateSentence(_sentence2, "Goodbye", 100, 200, 1.0f, 1.0f, 0.0f, deviceContext);
//	if (!result)
//	{
//		return false;
//	}
//
//
//	return true;
//}
//
//
//void TextClass::Shutdown()
//{
//	// Release the first sentence.
//	ReleaseSentence(&_sentence1);
//
//	// Release the font shader object.
//	if (_FontShader)
//	{
//		_FontShader->Shutdown();
//		delete _FontShader;
//		_FontShader = 0;
//	}
//
//	// Release the font object.
//	if (_Font)
//	{
//		_Font->Shutdown();
//		delete _Font;
//		_Font = 0;
//	}
//
//	return;
//}
//
//
//bool TextClass::Render(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX orthoMatrix)
//{
//	bool result;
//
//
//	// Draw the first sentence.
//	result = RenderSentence(deviceContext, _sentence1, worldMatrix, orthoMatrix);
//	if (!result)
//	{
//		return false;
//	}
//
//	return true;
//}
//
//
//bool TextClass::InitializeSentence(SentenceType** sentence, int maxLength, ID3D11Device* device)
//{
//	VertexType* vertices;
//	unsigned long* indices;
//	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
//	D3D11_SUBRESOURCE_DATA vertexData, indexData;
//	HRESULT result;
//	int i;
//
//
//	// Create a new sentence object.
//	*sentence = new SentenceType;
//	if (!*sentence)
//	{
//		return false;
//	}
//
//	// Initialize the sentence buffers to null.
//	(*sentence)->vertexBuffer = 0;
//	(*sentence)->indexBuffer = 0;
//
//	// Set the maximum length of the sentence.
//	(*sentence)->maxLength = maxLength;
//
//	// Set the number of vertices in the vertex array.
//	(*sentence)->vertexCount = 6 * maxLength;
//
//	// Set the number of indexes in the index array.
//	(*sentence)->indexCount = (*sentence)->vertexCount;
//
//	// Create the vertex array.
//	vertices = new VertexType[(*sentence)->vertexCount];
//	if (!vertices)
//	{
//		return false;
//	}
//
//	// Create the index array.
//	indices = new unsigned long[(*sentence)->indexCount];
//	if (!indices)
//	{
//		return false;
//	}
//
//	// Initialize vertex array to zeros at first.
//	memset(vertices, 0, (sizeof(VertexType) * (*sentence)->vertexCount));
//
//	// Initialize the index array.
//	for (i = 0; i<(*sentence)->indexCount; i++)
//	{
//		indices[i] = i;
//	}
//
//	// Set up the description of the dynamic vertex buffer.
//	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
//	vertexBufferDesc.ByteWidth = sizeof(VertexType) * (*sentence)->vertexCount;
//	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
//	vertexBufferDesc.MiscFlags = 0;
//	vertexBufferDesc.StructureByteStride = 0;
//
//	// Give the subresource structure a pointer to the vertex data.
//	vertexData.pSysMem = vertices;
//	vertexData.SysMemPitch = 0;
//	vertexData.SysMemSlicePitch = 0;
//
//	// Create the vertex buffer.
//	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &(*sentence)->vertexBuffer);
//	if (FAILED(result))
//	{
//		return false;
//	}
//
//	// Set up the description of the static index buffer.
//	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
//	indexBufferDesc.ByteWidth = sizeof(unsigned long) * (*sentence)->indexCount;
//	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
//	indexBufferDesc.CPUAccessFlags = 0;
//	indexBufferDesc.MiscFlags = 0;
//	indexBufferDesc.StructureByteStride = 0;
//
//	// Give the subresource structure a pointer to the index data.
//	indexData.pSysMem = indices;
//	indexData.SysMemPitch = 0;
//	indexData.SysMemSlicePitch = 0;
//
//	// Create the index buffer.
//	result = device->CreateBuffer(&indexBufferDesc, &indexData, &(*sentence)->indexBuffer);
//	if (FAILED(result))
//	{
//		return false;
//	}
//
//	// Release the vertex array as it is no longer needed.
//	delete[] vertices;
//	vertices = 0;
//
//	// Release the index array as it is no longer needed.
//	delete[] indices;
//	indices = 0;
//
//	return true;
//}
//
//
//bool TextClass::UpdateSentence(SentenceType* sentence, char* text, int positionX, int positionY, float red, float green, float blue,
//	ID3D11DeviceContext* deviceContext)
//{
//	int numLetters;
//	VertexType* vertices;
//	float drawX, drawY;
//	HRESULT result;
//	D3D11_MAPPED_SUBRESOURCE mappedResource;
//	VertexType* verticesPtr;
//
//
//	// Store the color of the sentence.
//	sentence->red = red;
//	sentence->green = green;
//	sentence->blue = blue;
//
//	// Get the number of letters in the sentence.
//	numLetters = (int)strlen(text);
//
//	// Check for possible buffer overflow.
//	if (numLetters > sentence->maxLength)
//	{
//		return false;
//	}
//
//	// Create the vertex array.
//	vertices = new VertexType[sentence->vertexCount];
//	if (!vertices)
//	{
//		return false;
//	}
//
//	// Initialize vertex array to zeros at first.
//	memset(vertices, 0, (sizeof(VertexType) * sentence->vertexCount));
//
//	// Calculate the X and Y pixel position on the screen to start drawing to.
//	drawX = (float)(((_screenWidth / 2) * -1) + positionX);
//	drawY = (float)((_screenHeight / 2) - positionY);
//
//	// Use the font class to build the vertex array from the sentence text and sentence draw location.
//	_Font->BuildVertexArray((void*)vertices, text, drawX, drawY);
//
//	// Lock the vertex buffer so it can be written to.
//	result = deviceContext->Map(sentence->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
//	if (FAILED(result))
//	{
//		return false;
//	}
//
//	// Get a pointer to the data in the vertex buffer.
//	verticesPtr = (VertexType*)mappedResource.pData;
//
//	// Copy the data into the vertex buffer.
//	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * sentence->vertexCount));
//
//	// Unlock the vertex buffer.
//	deviceContext->Unmap(sentence->vertexBuffer, 0);
//
//	// Release the vertex array as it is no longer needed.
//	delete[] vertices;
//	vertices = 0;
//
//	return true;
//}
//
//
//void TextClass::ReleaseSentence(SentenceType** sentence)
//{
//	if (*sentence)
//	{
//		// Release the sentence vertex buffer.
//		if ((*sentence)->vertexBuffer)
//		{
//			(*sentence)->vertexBuffer->Release();
//			(*sentence)->vertexBuffer = 0;
//		}
//
//		// Release the sentence index buffer.
//		if ((*sentence)->indexBuffer)
//		{
//			(*sentence)->indexBuffer->Release();
//			(*sentence)->indexBuffer = 0;
//		}
//
//		// Release the sentence.
//		delete *sentence;
//		*sentence = 0;
//	}
//
//	return;
//}
//
//
//bool TextClass::RenderSentence(ID3D11DeviceContext* deviceContext, SentenceType* sentence, XMMATRIX worldMatrix,
//	XMMATRIX orthoMatrix)
//{
//	unsigned int stride, offset;
//	XMFLOAT4 pixelColor;
//	bool result;
//
//
//	// Set vertex buffer stride and offset.
//	stride = sizeof(VertexType);
//	offset = 0;
//
//	// Set the vertex buffer to active in the input assembler so it can be rendered.
//	deviceContext->IASetVertexBuffers(0, 1, &sentence->vertexBuffer, &stride, &offset);
//
//	// Set the index buffer to active in the input assembler so it can be rendered.
//	deviceContext->IASetIndexBuffer(sentence->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
//
//	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
//	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//
//	// Create a pixel color vector with the input sentence color.
//	pixelColor = XMFLOAT4(sentence->red, sentence->green, sentence->blue, 1.0f);
//
//	// Render the text using the font shader.
//	result = _FontShader->Render(deviceContext, sentence->indexCount, worldMatrix, _baseViewMatrix, orthoMatrix, *_Font->GetTexture(),
//		pixelColor);
//	if (!result)
//	{
//		false;
//	}
//
//	return true;
//}
//
//
//bool TextClass::SetRenderCount(int count, ID3D11DeviceContext* deviceContext)
//{
//	char tempString[32];
//	char countString[32];
//	bool result;
//
//
//	// Convert the count integer to string format.
//	_itoa_s(count, tempString, 10);
//
//	// Setup the render count string.
//	strcpy_s(countString, "Render Count: ");
//	strcat_s(countString, tempString);
//
//	// Update the sentence vertex buffer with the new string information.
//	result = UpdateSentence(_sentence1, countString, 20, 20, 1.0f, 1.0f, 1.0f, deviceContext);
//	if (!result)
//	{
//		return false;
//	}
//
//	return true;
//}
//
//bool TextClass::SetMousePosition(int mouseX, int mouseY, ID3D11DeviceContext* deviceContext)
//{
//	char tempString[16];
//	char mouseString[16];
//	bool result;
//
//	// Convert the mouseX integer to string format.
//	_itoa_s(mouseX, tempString, 10);
//
//	// Setup the mouseX string.
//	strcpy_s(mouseString, "Mouse X: ");
//	strcat_s(mouseString, tempString);
//
//	// Update the sentence vertex buffer with the new string information.
//	result = UpdateSentence(_sentence1, mouseString, 20, 20, 1.0f, 1.0f, 1.0f, deviceContext);
//	if (!result)
//	{
//		return false;
//	}
//
//	// Convert the mouseY integer to string format.
//	_itoa_s(mouseY, tempString, 10);
//
//	// Setup the mouseY string.
//	strcpy_s(mouseString, "Mouse Y: ");
//	strcat_s(mouseString, tempString);
//
//	// Update the sentence vertex buffer with the new string information.
//	result = UpdateSentence(_sentence2, mouseString, 20, 40, 1.0f, 1.0f, 1.0f, deviceContext);
//	if (!result)
//	{
//		return false;
//	}
//
//	return true;
//}
//
//bool TextClass::SetFps(int fps, ID3D11DeviceContext* deviceContext)
//{
//	char tempString[16];
//	char fpsString[16];
//	float red, green, blue;
//	bool result;
//
//
//	// Truncate the fps to below 10,000.
//	if (fps > 9999)
//	{
//		fps = 9999;
//	}
//
//	// Convert the fps integer to string format.
//	_itoa_s(fps, tempString, 10);
//
//	// Setup the fps string.
//	strcpy_s(fpsString, "Fps: ");
//	strcat_s(fpsString, tempString);
//
//	// If fps is 60 or above set the fps color to green.
//	if (fps >= 60)
//	{
//		red = 0.0f;
//		green = 1.0f;
//		blue = 0.0f;
//	}
//
//	// If fps is below 60 set the fps color to yellow.
//	if (fps < 60)
//	{
//		red = 1.0f;
//		green = 1.0f;
//		blue = 0.0f;
//	}
//
//	// If fps is below 30 set the fps color to red.
//	if (fps < 30)
//	{
//		red = 1.0f;
//		green = 0.0f;
//		blue = 0.0f;
//	}
//
//	// Update the sentence vertex buffer with the new string information.
//	result = UpdateSentence(_sentence1, fpsString, 20, 20, red, green, blue, deviceContext);
//	if (!result)
//	{
//		return false;
//	}
//
//	return true;
//}
//
//bool TextClass::SetCpu(int cpu, ID3D11DeviceContext* deviceContext)
//{
//	char tempString[16];
//	char cpuString[16];
//	bool result;
//
//
//	// Convert the cpu integer to string format.
//	_itoa_s(cpu, tempString, 10);
//
//	// Setup the cpu string.
//	strcpy_s(cpuString, "Cpu: ");
//	strcat_s(cpuString, tempString);
//	strcat_s(cpuString, "%");
//
//	// Update the sentence vertex buffer with the new string information.
//	result = UpdateSentence(_sentence2, cpuString, 20, 40, 0.0f, 1.0f, 0.0f, deviceContext);
//	if (!result)
//	{
//		return false;
//	}
//
//	return true;
//}