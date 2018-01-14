////////////////////////////////////////////////////////////////////////////////
// Filename: fontclass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

//////////////
// INCLUDES //
//////////////
#include <directxmath.h>
#include <fstream>
#include <vector>
#include <wrl/client.h>
#include "texture.h"

using namespace DirectX;
using namespace std;
using namespace Microsoft::WRL;

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

	TextureClass* GetTextureObject();
	ID3D11ShaderResourceView* GetTexture();
	void BuildVertexArray(void*, char*, float, float);
	int GetSentencePixelLength(char*);
	int GetFontHeight();

private:
	bool LoadFontData(char*);
	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, char*);

private:
	unique_ptr<FontType[]> _Font;
	unique_ptr<TextureClass> _Texture;
	float _fontHeight;
	int _spaceSize;
};