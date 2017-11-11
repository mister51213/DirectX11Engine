////////////////////////////////////////////////////////////////////////////////
// Filename: fontclass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once


//////////////
// INCLUDES //
//////////////
#include <directxmath.h>
#include <fstream>
using namespace DirectX;
using namespace std;


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "texture.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: FontClass
////////////////////////////////////////////////////////////////////////////////
class FontClass
{
private:
	struct FontType
	{
		float left, right;
		int size;
	};

	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

public:
	FontClass();
	FontClass(const FontClass&);
	~FontClass();

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* fontFilename, char* textureFilename,
		float fontHeight, int spaceSize);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();
	void BuildVertexArray(void*, char*, float, float);
	int GetSentencePixelLength(char*);
	int GetFontHeight();

private:
	bool LoadFontData(char*);
	void ReleaseFontData();
	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, char*);
	void ReleaseTexture();

private:
	FontType* m_Font;
	TextureClass* m_Texture;
	float m_fontHeight;
	int m_spaceSize;
};

//////////////////////////////////////////////////////////////////////////////////
//// Filename: fontclass.h
//////////////////////////////////////////////////////////////////////////////////
//#pragma once
//
////////////////
//// INCLUDES //
////////////////
//#include <d3d11.h>
//#include <DirectXMath.h>
//#include <fstream>
//using namespace std;
//using namespace DirectX;
//
/////////////////////////
//// MY CLASS INCLUDES //
/////////////////////////
//#include "texture.h"
//
//////////////////////////////////////////////////////////////////////////////////
//// Class name: FontClass
//////////////////////////////////////////////////////////////////////////////////
//class FontClass
//{
//private:
//	struct FontType
//	{
//		float left, right;
//		int size;
//	};
//
//	struct VertexType
//	{
//		XMFLOAT3 position;
//		XMFLOAT2 texture;
//	};
//
//public:
//	FontClass();
//	FontClass(const FontClass&);
//	~FontClass();
//
//	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* fontFilename, char* textureFilename);
//	void Shutdown();
//
//	ID3D11ShaderResourceView** GetTexture();
//
//	void BuildVertexArray(void* vertices, char* sentence, float drawX, float drawY);
//
//private:
//	bool LoadFontData(char*);
//	void ReleaseFontData();
//	bool LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename);
//	void ReleaseTexture();
//
//private:
//	FontType* _Font;
//	TextureClass* _Texture;
//};