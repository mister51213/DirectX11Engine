////////////////////////////////////////////////////////////////////////////////
// Filename: texture.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <stdio.h>

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

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename1, char* filename2);
	void Shutdown();

	//ID3D11ShaderResourceView* GetTexture();
	ID3D11ShaderResourceView** GetTextureArray();

private:
	bool LoadTarga(char*, int&, int&, unsigned char** pTargaData);

	unsigned char* _targaData1;
	unsigned char* _targaData2;
	//ID3D11Texture2D* _texture;
	ID3D11Texture2D* _texture1;
	ID3D11Texture2D* _texture2;
	//ID3D11ShaderResourceView* _textureView;
	ID3D11ShaderResourceView* _textureViews[2];

};