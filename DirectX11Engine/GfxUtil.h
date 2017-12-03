////////////////////////////////////////////
// Filename: GfxUtilites.h
// Contains useful functions and operator overloads for working with Graphics
///////////////////////////////////////////////
#pragma once

#include <Windows.h>
#define _XM_NO_INTRINSICS_ 
#include <directxmath.h>
#include <stdlib.h>
#include <d3d11.h>
#include <vector>

using namespace DirectX;

namespace GfxUtilities
{
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
		Material::Material(EShaderType inShaderType, ID3D11Resource** inTextures, ID3D11ShaderResourceView** inTextureViews)
		{
			shaderType = inShaderType;
			textures = inTextures;
			textureViews = inTextureViews;
		}

		EShaderType shaderType;
		ID3D11Resource** textures;
		ID3D11ShaderResourceView** textureViews;
	};
}