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
#include "GlobalIncludes.h"
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

	static D3D11_INPUT_ELEMENT_DESC MakeInputElementDesc(LPCSTR name, DXGI_FORMAT format, UINT offset = D3D11_APPEND_ALIGNED_ELEMENT)
	{
		D3D11_INPUT_ELEMENT_DESC desc;

		desc.SemanticName = name;
		desc.SemanticIndex = 0;
		desc.Format = format;
		desc.InputSlot = 0;
		desc.AlignedByteOffset = offset;
		desc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		desc.InstanceDataStepRate = 0;

		return desc;
	}

	static D3D11_SAMPLER_DESC MakeSamplerDesc(D3D11_TEXTURE_ADDRESS_MODE texMode = D3D11_TEXTURE_ADDRESS_WRAP)
	{
		D3D11_SAMPLER_DESC desc;

		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = texMode;
		desc.AddressV = texMode;
		desc.AddressW = texMode;
		desc.MipLODBias = 0.0f;
		desc.MaxAnisotropy = 1;
		desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		desc.BorderColor[0] = 0;
		desc.BorderColor[1] = 0;
		desc.BorderColor[2] = 0;
		desc.BorderColor[3] = 0;
		desc.MinLOD = 0;
		desc.MaxLOD = D3D11_FLOAT32_MAX;

		return desc;
	}

	//@TODO - behaves really weird, must fix!
	static Microsoft::WRL::ComPtr<ID3D11SamplerState> MakeSamplerState(ID3D11Device* device)
	{
		D3D11_SAMPLER_DESC desc;

		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.MipLODBias = 0.0f;
		desc.MaxAnisotropy = 1;
		desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		desc.BorderColor[0] = 0;
		desc.BorderColor[1] = 0;
		desc.BorderColor[2] = 0;
		desc.BorderColor[3] = 0;
		desc.MinLOD = 0;
		desc.MaxLOD = D3D11_FLOAT32_MAX;

		Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;
		HRESULT result = device->CreateSamplerState(&desc, &sampler);
		CHECK(SUCCEEDED(result), "Failed to create sampler state.");

		return sampler;
	}


	template<class BufferType>
	static Microsoft::WRL::ComPtr<ID3D11Buffer> MakeConstantBuffer(ID3D11Device* device)
	{
		D3D11_BUFFER_DESC desc{};
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = sizeof(BufferType);
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
		HRESULT result = device->CreateBuffer(&desc, nullptr, &buffer);
		CHECK(SUCCEEDED(result), "Failed to create constant buffer.");

		return buffer;
	}

	template<class BufferType>
	static void MapBuffer(const BufferType& inData, ID3D11Buffer* pBuffer, ID3D11DeviceContext* deviceContext)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT result = deviceContext->Map(pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		CHECK(SUCCEEDED(result), "cbuffer");
		memcpy(mappedResource.pData, &inData, sizeof(BufferType));
		deviceContext->Unmap(pBuffer, 0);
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
		
		void Animate(float deltaTime = 1.f);

		unique_ptr<TextureClass> _TextureArray;
		EShaderType shaderType;
		int texArraySize;
		XMFLOAT4 pixelColor = XMFLOAT4(1,1,1,1);

		// Effects @TODO: encapsulate into different struct
		bool bAnimated = false;
		float transparency = 0.f;
		float translation = 0.f;
		float reflectRefractScale = 0.01f;
		float waterHeight = 2.75f;
	};

}