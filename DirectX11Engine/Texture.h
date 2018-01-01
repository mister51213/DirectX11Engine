////////////////////////////////////////////////////////////////////////////////
// Filename: texture.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

//////////////
// INCLUDES //
//////////////
#include "GlobalIncludes.h"
#include <d3d11.h>
#include <stdio.h>
#include <wrl/client.h>
#include <vector>

#include <memory>

using namespace std;

class TextureClass
{
private:
	struct TargaHeader
	{
		unsigned char data1[12];
		unsigned short width;
		unsigned short height;
		unsigned char bpp;
		unsigned char data2;
	};

public:
	TextureClass();
	TextureClass(const TextureClass&);
	~TextureClass();

	bool InitializeTexture(ID3D11Device * device, ID3D11DeviceContext * deviceContext, char * filename, int i);
	bool InitializeArray(ID3D11Device* device, ID3D11DeviceContext* deviceContext, /*vector<char*>*/vector<string> filenames);
	bool InitializeTexTga(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename, unsigned char** targaData, ID3D11Texture2D** pTexture, ID3D11ShaderResourceView** pTexView);
	
	ID3D11ShaderResourceView** GetTextureArray();
	void ResetTextureArray(ID3D11ShaderResourceView** array, int size);

private:
	unsigned char* LoadTarga(char*, int&, int&, unsigned char* pTargaData);
	vector<Microsoft::WRL::ComPtr <ID3D11ShaderResourceView>> _textureViews;
	
	// for dds and everything else
	vector<Microsoft::WRL::ComPtr <ID3D11Resource>> _resourceArray;

	// targa
	vector<ID3D11Texture2D*> _targaTextures;
	vector<unsigned char*> _targaData;
};