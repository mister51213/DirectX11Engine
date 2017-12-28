////////////////////////////////////////////////////////////////////////////////
// Filename: texture.cpp
////////////////////////////////////////////////////////////////////////////////
#pragma comment(lib, "Shlwapi.lib")

#include "texture.h"
#include "DDSTextureLoader.h"
#include "WICTextureLoader.h"
#include <stdlib.h>
#include <stdexcept>
#include <string>
#include "GfxUtil.h"
#include "Shlwapi.h"

using namespace std;
using namespace DirectX;
using namespace GfxUtil;

TextureClass::TextureClass(){}
TextureClass::TextureClass(const TextureClass& other){}
TextureClass::~TextureClass(){}

//@TODO - DONT USE YET! something not working - not initializing all the files in the array for some reason
bool TextureClass::InitializeArray(ID3D11Device* device, ID3D11DeviceContext* deviceContext, /*vector<char*>*/vector<string> filenames)
{
	for (int i = 0; i < filenames.size(); ++i)
	{
		//@TEMP - convert to c-string here
		char* fileName;
		size_t len = filenames[i].copy(fileName, filenames[i].length());
		fileName[len] = '\0';
		//@TEMP - convert to c-string here


		_textureViews.push_back(Microsoft::WRL::ComPtr <ID3D11ShaderResourceView>());
		InitializeTexture(device, deviceContext, /*filenames[i]*/fileName, i);
	}
	return true;
}

bool TextureClass::InitializeTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename, int i)
{
	string extension = PathFindExtension(filename);
	
	if (extension == ".tga")
	{
		unsigned char* ptr1;	_targaData.push_back(ptr1);
		ID3D11Texture2D* ptr2;	_targaTextures.push_back(ptr2);
		InitializeTexTga(device, deviceContext, filename, &_targaData[i], &_targaTextures[i], &_textureViews[i]);
		return false;
	}
	else if (extension == ".dds")
	{
		_resourceArray.push_back(Microsoft::WRL::ComPtr <ID3D11Resource>());
		bool result = CreateDDSTextureFromFile(device, deviceContext, charToWChar(filename), &_resourceArray[i], &_textureViews[i]);
		if (FAILED(result))
		{
			throw std::runtime_error("Failed to create dds texture: " + std::to_string(__LINE__));
			return false;
		}

		//@REFERENCE
		//	HRESULT __cdecl CreateDDSTextureFromFile(
		//		_In_ ID3D11Device* d3dDevice,
		//		_In_opt_ ID3D11DeviceContext* d3dContext,
		//		_In_z_ const wchar_t* szFileName,
		//		_Outptr_opt_ ID3D11Resource** texture,
		//		_Outptr_opt_ ID3D11ShaderResourceView** textureView,
		//		_In_ size_t maxsize = 0,
		//		_Out_opt_ DDS_ALPHA_MODE* alphaMode = nullptr);
	}
	else
	{
		_resourceArray.push_back(Microsoft::WRL::ComPtr <ID3D11Resource>());
		bool result = CreateWICTextureFromFile(device, deviceContext, charToWChar(filename), &_resourceArray[i], &_textureViews[i]);
		if (FAILED(result))
		{
			throw std::runtime_error("Failed to create WIC texture: " + std::to_string(__LINE__));
			return false;
		}
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

ID3D11ShaderResourceView** TextureClass::GetTextureArray()
{
	return _textureViews.data()->GetAddressOf();
}

void TextureClass::ResetTextureArray(ID3D11ShaderResourceView ** array, int size)
{
	_textureViews.clear();

	for (int i = 0; i < size; ++i)
	{
		_textureViews.push_back(Microsoft::WRL::ComPtr <ID3D11ShaderResourceView>());
		_textureViews[i] = array[i];
	}
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
