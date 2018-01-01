
///////////////////////////////////////////////////////////////////////////////
// Filename: fontclass.cpp
///////////////////////////////////////////////////////////////////////////////
#include "fontclass.h"

FontClass::FontClass()
//	:
//_Font(0),
//_Texture(0)
{}

FontClass::FontClass(const FontClass& other)
{
}

FontClass::~FontClass()
{
}

bool FontClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* fontFilename, char* textureFilename,
	float fontHeight, int spaceSize)
{
	bool result;


	// Store the height of the font.
	_fontHeight = fontHeight;

	// Store the size of spaces in pixel size.
	_spaceSize = spaceSize;

	// Load in the text file containing the font data.
	result = LoadFontData(fontFilename);
	if (!result)
	{
		return false;
	}

	// Load the texture that has the font characters on it.
	result = LoadTexture(device, deviceContext, textureFilename);
	if (!result)
	{
		return false;
	}

	return true;
}

bool FontClass::LoadFontData(char* filename)
{
	ifstream fin;
	int i;
	char temp;

	// Create the font spacing buffer.
	_Font.reset(new FontType[95]);
	if (!_Font)
	{
		return false;
	}

	// Read in the font size and spacing between chars.
	fin.open(filename);
	if (fin.fail())
	{
		return false;
	}

	// Read in the 95 used ascii characters for text.
	for (i = 0; i<95; i++)
	{
		fin.get(temp);
		while (temp != ' ')
		{
			fin.get(temp);
		}
		fin.get(temp);
		while (temp != ' ')
		{
			fin.get(temp);
		}

		fin >> _Font.get()[i].left;
		fin >> _Font.get()[i].right;
		fin >> _Font.get()[i].size;
	}

	// Close the file.
	fin.close();

	return true;
}


bool FontClass::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	bool result;


	// Create the texture object.
	//_Texture = new TextureClass;
	_Texture.reset(new TextureClass);
	if (!_Texture)
	{
		return false;
	}

	//vector<char*> filenames{ filename };
	vector<string> filenames{ filename };
	result = _Texture->InitializeArray(device, deviceContext, filenames);

	if (!result)
	{
		return false;
	}

	return true;
}

ID3D11ShaderResourceView* FontClass::GetTexture()
{
	ID3D11ShaderResourceView** views = _Texture->GetTextureArray();
	return views[0];
}


void FontClass::BuildVertexArray(void* vertices, char* sentence, float drawX, float drawY)
{
	VertexType* vertexPtr;
	int numLetters, index, i, letter;


	// Coerce the input vertices into a VertexType structure.
	vertexPtr = (VertexType*)vertices;

	// Get the number of letters in the sentence.
	numLetters = (int)strlen(sentence);

	// Initialize the index to the vertex array.
	index = 0;

	// Draw each letter onto a quad.
	for (i = 0; i<numLetters; i++)
	{
		letter = ((int)sentence[i]) - 32;

		// If the letter is a space then just move over three pixels.
		if (letter == 0)
		{
			drawX = drawX + (float)_spaceSize;
		}
		else
		{
			// First triangle in quad.
			vertexPtr[index].position = XMFLOAT3(drawX, drawY, 0.0f);  // Top left.
			vertexPtr[index].texture = XMFLOAT2(_Font.get()[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = XMFLOAT3((drawX + _Font.get()[letter].size), (drawY - _fontHeight), 0.0f);  // Bottom right.
			vertexPtr[index].texture = XMFLOAT2(_Font.get()[letter].right, 1.0f);
			index++;

			vertexPtr[index].position = XMFLOAT3(drawX, (drawY - _fontHeight), 0.0f);  // Bottom left.
			vertexPtr[index].texture = XMFLOAT2(_Font.get()[letter].left, 1.0f);
			index++;

			// Second triangle in quad.
			vertexPtr[index].position = XMFLOAT3(drawX, drawY, 0.0f);  // Top left.
			vertexPtr[index].texture = XMFLOAT2(_Font.get()[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = XMFLOAT3(drawX + _Font.get()[letter].size, drawY, 0.0f);  // Top right.
			vertexPtr[index].texture = XMFLOAT2(_Font.get()[letter].right, 0.0f);
			index++;

			vertexPtr[index].position = XMFLOAT3((drawX + _Font.get()[letter].size), (drawY - _fontHeight), 0.0f);  // Bottom right.
			vertexPtr[index].texture = XMFLOAT2(_Font.get()[letter].right, 1.0f);
			index++;

			// Update the x location for drawing by the size of the letter and one pixel.
			drawX = drawX + _Font.get()[letter].size + 1.0f;
		}
	}

	return;
}


int FontClass::GetSentencePixelLength(char* sentence)
{
	int pixelLength, numLetters, i, letter;


	pixelLength = 0;
	numLetters = (int)strlen(sentence);

	for (i = 0; i<numLetters; i++)
	{
		letter = ((int)sentence[i]) - 32;

		// If the letter is a space then count it as three pixels.
		if (letter == 0)
		{
			pixelLength += _spaceSize;
		}
		else
		{
			pixelLength += (_Font.get()[letter].size + 1);
		}
	}

	return pixelLength;
}

int FontClass::GetFontHeight()
{
	return (int)_fontHeight;
}