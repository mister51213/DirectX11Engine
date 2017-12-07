////////////////////////////////////////////////////////////////////////////////
// Filename: texture.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

//////////////
// INCLUDES //
//////////////
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
	bool InitializeArray(ID3D11Device* device, ID3D11DeviceContext* deviceContext, vector<char*> filenames);
	bool InitializeArrayTga(ID3D11Device* device, ID3D11DeviceContext* deviceContext, vector<char*> filenames);
	bool InitializeArrayDDS(ID3D11Device* device, ID3D11DeviceContext* deviceContext, vector<char*> fileNames);
	bool InitializeTexTga(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename, unsigned char** targaData, ID3D11Texture2D** pTexture, ID3D11ShaderResourceView** pTexView);
	
	ID3D11ShaderResourceView** GetTextureArray();
	ID3D11ShaderResourceView** GetTextureArrayDDS();

private:
	unsigned char* LoadTarga(char*, int&, int&, unsigned char* pTargaData);

	unsigned char* _targaData[6];
	ID3D11Texture2D* _textures[6];

	Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> _textureViews[6];
	Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> _textureViewsDDS[6];

	////////// DDS VERSION ////////////////
	Microsoft::WRL::ComPtr <ID3D11Resource> _texDDS[6];
};