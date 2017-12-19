////////////////////////////////////////////
// Filename: GfxUtil.h
// Contains useful functions and operator overloads for working with Graphics
///////////////////////////////////////////////
#pragma once

#include <Windows.h>
#define _XM_NO_INTRINSICS_ 
#include <directxmath.h>
#include <stdlib.h>
#include <d3d11.h>
#include <vector>
#include <string>
#include "Texture.h"
#include <memory>
#include <wrl/client.h>

using namespace Microsoft::WRL;

using namespace DirectX;
using namespace std;

namespace GfxUtil
{
	static wchar_t* charToWChar(const char* text)
	{
		const size_t size = strlen(text) +1;
		wchar_t* wText = new wchar_t[size];
		mbstowcs(wText, text, size);
		return wText;
	}

	static wchar_t* charToWChar_S(const char* text)
	{
		size_t origsize = strlen(text) + 1;
		const size_t newsize = 100;
		size_t convertedChars = 0;
		wchar_t wcstring[newsize];
		mbstowcs_s(&convertedChars, wcstring, origsize, text, _TRUNCATE);
		wcscat_s(wcstring, L" (wchar_t *)");

		return wcstring;
	}

	enum EShaderType
	{
		ETEXTURE = 0,
		ELIGHT_SPECULAR = 1,
		EREFLECTION = 2,
		EREFRACTION = 3,
		EWATER = 4,
		EFONT = 5,
		ESIZE = 6
	};

	struct SceneEffects
	{
		float fogStart = 0.f;
		float fogEnd = 0.f;
		XMFLOAT4 clipPlane = XMFLOAT4(0.0f, 0.f, 0.0f, 0.0f);
	};

	class Material
	{
	public:
		Material();

		bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, EShaderType inShaderType, vector<char*> fileNames);

		TextureClass* GetTextureObject();
		ID3D11ShaderResourceView** GetResourceArray();
		
		void Animate(bool bAnimated = true, float deltaTime = 1.f);

		unique_ptr<TextureClass> _TextureArray;
		EShaderType shaderType;
		int texArraySize;
		XMFLOAT4 pixelColor = XMFLOAT4(1,1,1,1);

		// Effects @TODO: encapsulate into different struct
		float transparency = 0.f;
		float translation = 0.f;
		float waterHeight = 2.75f;
		float reflectRefractScale = 0.01f;
	};
}