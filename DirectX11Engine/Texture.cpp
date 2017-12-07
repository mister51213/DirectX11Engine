////////////////////////////////////////////////////////////////////////////////
// Filename: texture.cpp
////////////////////////////////////////////////////////////////////////////////
#include "texture.h"
#include "DDSTextureLoader.h"
#include <stdlib.h>
#include <stdexcept>
#include <string>
#include "GfxUtil.h"

using namespace std;
using namespace DirectX;
using namespace GfxUtil;

TextureClass::TextureClass()
	:
	//_targaData1(0),
	//_targaData2(0),
	//_texture1(0),
	//_texture2(0),
	_textureView(0)

	//_texture(0),
	//_textureView(0),
{
	int numElements = sizeof(_textureViews) / sizeof(ID3D11ShaderResourceView*);

	for (int i = 0; i < numElements; i++)
	{
		_textureViews[0] = 0;
		_textureViews[1] = 0;
		_textureViews[2] = 0;
		_textureViews[3] = 0;
		_textureViews[4] = 0;
		_textureViews[5] = 0;
	}
}

TextureClass::TextureClass(const TextureClass& other)
{}

TextureClass::~TextureClass()
{}

bool TextureClass::InitializeArrayTga(ID3D11Device* device, ID3D11DeviceContext* deviceContext, vector<char*> filenames, char* filename1, char* filename2, char* filename3, char* filename4, char* filename5, char* filename6)
{
	for (int i = 0; i < filenames.size(); ++i)
	{
		InitializeTexTga(device, deviceContext, filenames[i], &_targaData[i], &_textures[i], &_textureViews[i]);
	}

	return true;
}

bool TextureClass::InitializeTexTga(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename, unsigned char** targaData, ID3D11Texture2D** pTexture, ID3D11ShaderResourceView** pTexView)
{
	bool result;

	int height, width;
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT hResult;
	unsigned int rowPitch;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	*targaData = LoadTarga(filename, height, width, *targaData);
	if (!*targaData)
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
	hResult = device->CreateTexture2D(&textureDesc, NULL, pTexture);
	if (FAILED(hResult))
	{
		return false;
	}

	// Set the row pitch of the targa image data.
	rowPitch = (width * 4) * sizeof(unsigned char);

	deviceContext->UpdateSubresource(*pTexture, 0, NULL, *targaData, rowPitch, 0);

	// Setup the shader resource view description.
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	// Create the 1st shader resource view for the texture.
	hResult = device->CreateShaderResourceView(*pTexture, &srvDesc, pTexView);
	if (FAILED(hResult))
	{
		return false;
	}

	// Generate mipmaps for this texture.
	deviceContext->GenerateMips(*pTexView);

	// Release the targa image data now that the image data has been loaded into the texture.
	if (*targaData)
	{
		delete[] *targaData;
		*targaData = 0;
	}

	return true;
}

bool TextureClass::InitializeArrayDDS(ID3D11Device * device, ID3D11DeviceContext * deviceContext, char * filename1, char * filename2, char * filename3, char * filename4, char * filename5, char * filename6)
{
	//@CAUTION @CAUTION @CAUTION For creation functions, use &texDDS1, for set functions use .GetAddressOf
	// IASetVertexBuffers, SetRenderTargets, ID3D11Resource** tex1 = texDDS1.GetAddressOf();

	bool result = CreateDDSTextureFromFile(device, deviceContext, charToWChar(filename1), &_texDDS1, &_textureViewsDDS[0]);
	if (FAILED(result))
	{
		throw std::runtime_error("Failed to create dds texture: " + std::to_string(__LINE__));
		return false;
	}

	result = CreateDDSTextureFromFile(device, deviceContext, charToWChar(filename2), &_texDDS2, &_textureViewsDDS[1]);
	if (FAILED(result))
	{
		throw std::runtime_error("Failed to create dds texture: "  + std::to_string(__LINE__));
		return false;
	}

	result = CreateDDSTextureFromFile(device, deviceContext, charToWChar(filename3), &_texDDS3, &_textureViewsDDS[2]);
	if (FAILED(result))
	{
		throw std::runtime_error("Failed to create dds texture: "  + std::to_string(__LINE__));
		return false;
	}

	result = CreateDDSTextureFromFile(device, deviceContext, charToWChar(filename4), &_texDDS4, &_textureViewsDDS[3]);
	if (FAILED(result))
	{
		throw std::runtime_error("Failed to create dds texture: "  + std::to_string(__LINE__));
		return false;
	}

	result = CreateDDSTextureFromFile(device, deviceContext, charToWChar(filename5), &_texDDS5, &_textureViewsDDS[4]);
	if (FAILED(result))
	{
		throw std::runtime_error("Failed to create dds texture: "  + std::to_string(__LINE__));
		return false;
	}

	result = CreateDDSTextureFromFile(device, deviceContext, charToWChar(filename6), &_texDDS6, &_textureViewsDDS[5]);
	if (FAILED(result))
	{
		throw std::runtime_error("Failed to create dds texture: " + std::to_string(__LINE__));
		return false;
	}

	//@REFERENCE
	//HRESULT __cdecl CreateDDSTextureFromFile(
	//	_In_ ID3D11Device* d3dDevice,
	//	_In_opt_ ID3D11DeviceContext* d3dContext,
	//	_In_z_ const wchar_t* szFileName,
	//	_Outptr_opt_ ID3D11Resource** texture,
	//	_Outptr_opt_ ID3D11ShaderResourceView** textureView,
	//	_In_ size_t maxsize = 0,
	//	_Out_opt_ DDS_ALPHA_MODE* alphaMode = nullptr);

	return true;
}


bool TextureClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* d3dContext, WCHAR* filename, ID3D11ShaderResourceView** textureView)
{
	HRESULT result;

	// Load the texture in.
	result = CreateDDSTextureFromFile(device, d3dContext, filename, &_texture, &_textureView);

	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void TextureClass::Shutdown()
{
	// release the texture view resource.
	if (_textureView)
	{
		_textureView->Release();
		_textureView = 0;
	}
	//// Release the texture.
	//if (_texture)
	//{
	//	_texture->Release();
	//	_texture = 0;
	//}

	// Release the texture resources.
	int numElements = sizeof(_textureViews) / sizeof(ID3D11ShaderResourceView*);
	for (int i = numElements-1; i >= 0; i--)
	{
		if (_textureViews[i])
		{
			_textureViews[i]->Release();
			_textureViews[i] = 0;
		}
	}

	//if (_textureViews[0])
	//{
	//	_textureViews[0]->Release();
	//	_textureViews[0] = 0;
	//}
	//if (_textureViews[1])
	//{
	//	_textureViews[1]->Release();
	//	_textureViews[1] = 0;
	//}
	//if (_textureViews[2])
	//{
	//	_textureViews[2]->Release();
	//	_textureViews[2] = 0;
	//}
	//if (_textureViews[3])
	//{
	//	_textureViews[3]->Release();
	//	_textureViews[3] = 0;
	//}
	//if (_textureViews[4])
	//{
	//	_textureViews[4]->Release();
	//	_textureViews[4] = 0;
	//}
	//if (_textureViews[5])
	//{
	//	_textureViews[5]->Release();
	//	_textureViews[5] = 0;
	//}

	// Release the targa data. //@CLEANUP: Why done twice?
	//numElements = sizeof(_textureViews) / sizeof(ID3D11ShaderResourceView*);

	
	//if (_targaData6)
	//{
	//	delete[] _targaData6;
	//	_targaData6 = 0;
	//}

	//if (_targaData5)
	//{
	//	delete[] _targaData5;
	//	_targaData5 = 0;
	//}

	//if (_targaData4)
	//{
	//	delete[] _targaData4;
	//	_targaData4 = 0;
	//}

	//if (_targaData3)
	//{
	//	delete[] _targaData3;
	//	_targaData3 = 0;
	//}

	//if (_targaData2)
	//{
	//	delete[] _targaData2;
	//	_targaData2 = 0;
	//}

	//if (_targaData1)
	//{
	//	delete[] _targaData1;
	//	_targaData1 = 0;
	//}
	
	return;
}

//ID3D11ShaderResourceView* TextureClass::GetTexture()
ID3D11ShaderResourceView** TextureClass::GetTextureArray()
{
	//return _textureView;
	return _textureViews;
}

ID3D11ShaderResourceView** TextureClass::GetTextureArrayDDS()
{
	//return _textureView;

	//ID3D11ShaderResourceView* _textureViews[6];

	//return _textureViewsDDS;
	return _textureViewsDDS;

}

ID3D11ShaderResourceView* TextureClass::GetTexture()
{
	return _textureView;
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

