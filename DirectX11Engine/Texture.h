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

	bool InitializeArrayTga(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename1, char* filename2, char* filename3, char* filename4, char* filename5, char* filename6);
	bool InitializeTexTga(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename, unsigned char** targaData, ID3D11Texture2D** pTexture, ID3D11ShaderResourceView** pTexView);
	bool InitializeArrayDDS(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename1, char* filename2, char* filename3, char* filename4, char* filename5, char* filename6);
	
	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* d3dContext, WCHAR* filename, ID3D11ShaderResourceView** textureView);
	
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();
	ID3D11ShaderResourceView** GetTextureArray();
	ID3D11ShaderResourceView** GetTextureArrayDDS();

private:
	unsigned char* LoadTarga(char*, int&, int&, unsigned char* pTargaData);

	unsigned char* _targaData1;
	unsigned char* _targaData2;
	unsigned char* _targaData3;
	unsigned char* _targaData4;
	unsigned char* _targaData5;
	unsigned char* _targaData6;
	ID3D11Texture2D* _texture1;
	ID3D11Texture2D* _texture2;
	ID3D11Texture2D* _texture3;
	ID3D11Texture2D* _texture4;
	ID3D11Texture2D* _texture5;
	ID3D11Texture2D* _texture6;
	ID3D11ShaderResourceView* _textureViews[6];
	ID3D11ShaderResourceView* _textureViewsDDS[6];

	///////// SINGLE TEXTURE /////////////
	ID3D11Resource* _texture;
	ID3D11ShaderResourceView* _textureView;

	////////// DDS VERSION ////////////////
	//@NOTE - it wont work with Texture2D for some reason
	//unique_ptr<ID3D11Resource> texDDS1;
	//Microsoft::WRL::ComPtr <ID3D11Texture2D> texDDS1;
	Microsoft::WRL::ComPtr <ID3D11Resource> _texDDS1;
	Microsoft::WRL::ComPtr <ID3D11Resource> _texDDS2;
	Microsoft::WRL::ComPtr <ID3D11Resource> _texDDS3;
	Microsoft::WRL::ComPtr <ID3D11Resource> _texDDS4;
	Microsoft::WRL::ComPtr <ID3D11Resource> _texDDS5;
	Microsoft::WRL::ComPtr <ID3D11Resource> _texDDS6;

	//Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> _textureViewsDDS[5];
	Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> _textureView1;
	Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> _textureView2;
	Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> _textureView3;
	Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> _textureView4;
	Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> _textureView5;
	Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> _textureView6;
};