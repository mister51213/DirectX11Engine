////////////////////////////////////////////////////////////////////////////////
// Filename: texture.cpp
////////////////////////////////////////////////////////////////////////////////
#include "texture.h"

TextureClass::TextureClass()
	:
	_targaData1(0),
	_targaData2(0),
	_texture1(0),
	_texture2(0)
	//_texture(0),
	//_textureView(0),
{
	_textureViews[0] = 0;
	_textureViews[1] = 0;
	_textureViews[2] = 0;
}

TextureClass::TextureClass(const TextureClass& other)
{
}

TextureClass::~TextureClass()
{
}

bool TextureClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename1, char* filename2, char* filename3, char* filename4, char* filename5, char* filename6SpecMap)
{
	bool result;

	int height, width;
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT hResult;
	unsigned int rowPitch;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	
	/*
	TODO: Need to use new version of D3DX11CreateShaderResourceViewFromFile here
	so we can work with any image type
	
	result = D3DCreateShaderResourceViewFromFile(device, filename, NULL, NULL, &m_texture, NULL);
	
	https://www.gamedev.net/forums/topic/656732-solved-d3dx11createshaderresourceviewfromfile-deprecated/
	*/

	// Load the targa image data into memory.
	//result = LoadTarga(filename1, height, width, _targaData1);
	//if (!result)
	//{
	//	return false;
	//}

	_targaData1 = LoadTarga(filename1, height, width, _targaData1);
	if (!_targaData1)
	{
		return false;
	}
	
	_targaData2 = LoadTarga(filename2, height, width, _targaData2);
	if (!_targaData2)
	{
		return false;
	}

	_targaData3 = LoadTarga(filename3, height, width, _targaData3);
	if (!_targaData3)
	{
		return false;
	}

	_targaData4 = LoadTarga(filename4, height, width, _targaData4);
	if (!_targaData4)
	{
		return false;
	}

	_targaData5 = LoadTarga(filename5, height, width, _targaData5);
	if (!_targaData5)
	{
		return false;
	}

	_targaData6 = LoadTarga(filename6SpecMap, height, width, _targaData6);
	if (!_targaData6)
	{
		return false;
	}
	

	// Setup the description of the texture.
	textureDesc.Height = height;
	textureDesc.Width = width;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	// Create the empty texture.
	hResult = device->CreateTexture2D(&textureDesc, NULL, &_texture1/*&_texture*/);
	if (FAILED(hResult))
	{
		return false;
	}

	// Create the empty texture.
	hResult = device->CreateTexture2D(&textureDesc, NULL, &_texture2);
	if (FAILED(hResult))
	{
		return false;
	}

	// Create the empty light map.
	hResult = device->CreateTexture2D(&textureDesc, NULL, &_texture3);
	if (FAILED(hResult))
	{
		return false;
	}

	// Create the empty alpha map.
	hResult = device->CreateTexture2D(&textureDesc, NULL, &_texture4);
	if (FAILED(hResult))
	{
		return false;
	}

	// Create the empty normal map
	hResult = device->CreateTexture2D(&textureDesc, NULL, &_texture5);
	if (FAILED(hResult))
	{
		return false;
	}

	// Create the empty spec map
	hResult = device->CreateTexture2D(&textureDesc, NULL, &_texture6);
	if (FAILED(hResult))
	{
		return false;
	}

	// Set the row pitch of the targa image data.
	rowPitch = (width * 4) * sizeof(unsigned char);

	// should use UpdateSubresource for something that will be loaded once or that gets loaded rarely during loading sequences. 
	// Copy the targa image data into the texture.
	deviceContext->UpdateSubresource(_texture1, 0, NULL, _targaData1, rowPitch, 0);
	deviceContext->UpdateSubresource(_texture2, 0, NULL, _targaData2, rowPitch, 0);
	deviceContext->UpdateSubresource(_texture3, 0, NULL, _targaData3, rowPitch, 0);
	deviceContext->UpdateSubresource(_texture4, 0, NULL, _targaData4, rowPitch, 0);
	deviceContext->UpdateSubresource(_texture5, 0, NULL, _targaData5, rowPitch, 0);
	deviceContext->UpdateSubresource(_texture6, 0, NULL, _targaData6, rowPitch, 0);

	// Setup the shader resource view description.
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	// Create the 1st shader resource view for the texture.
	hResult = device->CreateShaderResourceView(_texture1, &srvDesc, &_textureViews[0]);
	if (FAILED(hResult))
	{
		return false;
	}

	// Create the 2nd shader resource view for the texture.
	hResult = device->CreateShaderResourceView(_texture2, &srvDesc, &_textureViews[1]);
	if (FAILED(hResult))
	{
		return false;
	}

	// Create the 3rd shader resource view for the texture.
	hResult = device->CreateShaderResourceView(_texture3, &srvDesc, &_textureViews[2]);
	if (FAILED(hResult))
	{
		return false;
	}

	// Create the 4th shader resource view for the texture.
	hResult = device->CreateShaderResourceView(_texture4, &srvDesc, &_textureViews[3]);
	if (FAILED(hResult))
	{
		return false;
	}

	// Create the 5th shader resource view for the texture.
	hResult = device->CreateShaderResourceView(_texture5, &srvDesc, &_textureViews[4]);
	if (FAILED(hResult))
	{
		return false;
	}

	// Create the 5th shader resource view for the texture.
	hResult = device->CreateShaderResourceView(_texture6, &srvDesc, &_textureViews[5]);
	if (FAILED(hResult))
	{
		return false;
	}

	// Generate mipmaps for this texture.
	deviceContext->GenerateMips(_textureViews[0]);
	deviceContext->GenerateMips(_textureViews[1]);
	deviceContext->GenerateMips(_textureViews[2]);
	deviceContext->GenerateMips(_textureViews[3]);
	deviceContext->GenerateMips(_textureViews[4]);
	deviceContext->GenerateMips(_textureViews[5]);
	
	// Release the targa image data now that the image data has been loaded into the texture.
	if (_targaData6)
	{
		delete[] _targaData6;
		_targaData6 = 0;
	}

	if (_targaData5)
	{
		delete[] _targaData5;
		_targaData5 = 0;
	}

	if (_targaData4)
	{
		delete[] _targaData4;
		_targaData4 = 0;
	}
	
	if (_targaData3)
	{
		delete[] _targaData3;
		_targaData3 = 0;
	}

	if (_targaData2)
	{
		delete[] _targaData2;
		_targaData2 = 0;
	}

	if (_targaData1)
	{
		delete[] _targaData1;
		_targaData1 = 0;
	}

	return true;
}

void TextureClass::Shutdown()
{
	//// Release the texture view resource.
	//if (_textureView)
	//{
	//	_textureView->Release();
	//	_textureView = 0;
	//}
	//// Release the texture.
	//if (_texture)
	//{
	//	_texture->Release();
	//	_texture = 0;
	//}

	// Release the texture resources.
	if (_textureViews[0])
	{
		_textureViews[0]->Release();
		_textureViews[0] = 0;
	}

	if (_textureViews[1])
	{
		_textureViews[1]->Release();
		_textureViews[1] = 0;
	}

	if (_textureViews[2])
	{
		_textureViews[2]->Release();
		_textureViews[2] = 0;
	}

	if (_textureViews[3])
	{
		_textureViews[3]->Release();
		_textureViews[3] = 0;
	}

	if (_textureViews[4])
	{
		_textureViews[4]->Release();
		_textureViews[4] = 0;
	}

	if (_textureViews[5])
	{
		_textureViews[5]->Release();
		_textureViews[5] = 0;
	}

	// Release the targa data. //@CLEANUP: Why done twice?
	if (_targaData6)
	{
		delete[] _targaData6;
		_targaData6 = 0;
	}

	if (_targaData5)
	{
		delete[] _targaData5;
		_targaData5 = 0;
	}

	if (_targaData4)
	{
		delete[] _targaData4;
		_targaData4 = 0;
	}

	if (_targaData3)
	{
		delete[] _targaData3;
		_targaData3 = 0;
	}

	if (_targaData2)
	{
		delete[] _targaData2;
		_targaData2 = 0;
	}

	if (_targaData1)
	{
		delete[] _targaData1;
		_targaData1 = 0;
	}
	
	return;
}

//ID3D11ShaderResourceView* TextureClass::GetTexture()
ID3D11ShaderResourceView** TextureClass::GetTextureArray()
{
	//return _textureView;
	return _textureViews;
}

ID3D11ShaderResourceView* TextureClass::GetTexture()
{
	return _textureViewSingle;
}

unsigned char* TextureClass::LoadTarga(char* filename, int& height, int& width, unsigned char* pTargaData)
{
	int error, bpp, imageSize, index, i, j, k;
	FILE* filePtr;
	unsigned int count;
	TargaHeader targaFileHeader;
	unsigned char* targaImage;

	// Open the targa file for reading in binary.
	if (!filename)
	{
		return nullptr;
	}

	error = fopen_s(&filePtr, filename, "rb");
	if (error != 0)
	{
		return false;
	}

	// Read in the file header.
	count = (unsigned int)fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	// Get the important information from the header.
	height = (int)targaFileHeader.height;
	width = (int)targaFileHeader.width;
	bpp = (int)targaFileHeader.bpp;

	// Check that it is 32 bit and not 24 bit.
	if (bpp != 32)
	{
		return false;
	}

	// Calculate the size of the 32 bit image data.
	imageSize = width * height * 4;

	// Allocate memory for the targa image data.
	targaImage = new unsigned char[imageSize];
	if (!targaImage)
	{
		return false;
	}

	// Read in the targa image data.
	count = (unsigned int)fread(targaImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		return false;
	}

	// Close the file.
	error = fclose(filePtr);
	if (error != 0)
	{
		return false;
	}

	// Allocate memory for the targa destination data.
	//_targaData = new unsigned char[imageSize];
	pTargaData = new unsigned char[imageSize];
	if (!*pTargaData)
	{
		return false;
	}

	// Initialize the index into the targa destination data array.
	index = 0;

	// Initialize the index into the targa image data.
	k = (width * height * 4) - (width * 4);

	// Now copy the targa image data into the targa destination array in the correct order since the targa format is stored upside down.
	for (j = 0; j < height; j++)
	{
		for (i = 0; i < width; i++)
		{
			pTargaData[index + 0] = targaImage[k + 2];  // Red.
			pTargaData[index + 1] = targaImage[k + 1];  // Green.
			pTargaData[index + 2] = targaImage[k + 0];  // Blue
			pTargaData[index + 3] = targaImage[k + 3];  // Alpha

														 // Increment the indexes into the targa data.
			k += 4;
			index += 4;
		}

		// Set the targa image data index back to the preceding row at the beginning of the column since its reading it in upside down.
		k -= (width * 8);
	}

	// Release the targa image data now that it was copied into the destination array.
	delete[] targaImage;
	targaImage = 0;

	return pTargaData;
}

