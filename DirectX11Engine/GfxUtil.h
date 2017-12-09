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
#include "Texture.h"

using namespace DirectX;

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

	struct Material
	{
		TextureClass* _TextureArray;
		EShaderType shaderType;
		int texArraySize;

		Material()
		{
			shaderType = ETEXTURE;
			texArraySize = 1;
		}

		bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, EShaderType inShaderType, vector<char*> fileNames)
		{
			texArraySize = fileNames.size();

			bool result;

			TextureClass* _TextureArray = nullptr;
			shaderType = inShaderType;

			_TextureArray = new TextureClass;
			if (!_TextureArray)
			{
				return false;
			}

			result = _TextureArray->InitializeArray(device, deviceContext, fileNames);
			if (!result)
			{
				return false;
			}

			return true;
		}

		ID3D11ShaderResourceView** GetResourceArray()
		{
			return _TextureArray->GetTextureArray();
		}
	};
}