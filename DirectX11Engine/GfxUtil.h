////////////////////////////////////////////
// Filename: GfxUtil.h
// Contains useful functions and operator overloads for working with Graphics
///////////////////////////////////////////////
#pragma once

#include <Windows.h>
#define _XM_NO_INTRINSICS_ 
//#include <directxmath.h>
//#include <stdlib.h>
#include <d3d11.h>
#include <vector>
#include <string>
#include "Texture.h"
#include "RenderTextureClass.h"
#include "GlobalIncludes.h"
//#include <memory>
//#include <wrl/client.h>

//using namespace Microsoft::WRL;

//using namespace DirectX;
//using namespace std;

namespace GfxUtil
{
	constexpr float DEGTORAD = XM_PI / 180.f;

	// TEXT RELATED
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

	// GPU PIPELINE RELATED //
	// VS CBUFFER TYPES
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;

		MatrixBufferType()
		{
			XMMATRIX world = XMMatrixIdentity();
			XMMATRIX view = XMMatrixIdentity();
			XMMATRIX projection = XMMatrixIdentity();
		}

		MatrixBufferType(const XMMATRIX& wrld, const XMMATRIX& vw, const XMMATRIX& proj)
		{
			world = wrld;
			view = vw;
			projection = proj;
		}
	};

	struct LightDataTemplate_VS
	{
		XMMATRIX viewMatrix;
		XMMATRIX projectionMatrix;
		XMFLOAT3 position;
		float padding;

		LightDataTemplate_VS()
		{
			viewMatrix = XMMatrixIdentity();
			projectionMatrix = XMMatrixIdentity();
			position = XMFLOAT3(0,0,0);
			padding = 0;
		}

		LightDataTemplate_VS(const XMMATRIX& view, const XMMATRIX& projection,const XMFLOAT3& pos)
		{
			viewMatrix = view;
			projectionMatrix = projection;
			position = pos;
			padding = 0;
		}

		LightDataTemplate_VS(const LightDataTemplate_VS& otherBuff)
		{

			viewMatrix = otherBuff.viewMatrix;
			projectionMatrix = otherBuff.projectionMatrix;
			position = otherBuff.position;
		}
	};

	// PS CBUFFER TYPES
	struct LightDataTemplate_PS
	{
		int type;
		XMFLOAT3 padding;
		XMFLOAT4 diffuseColor;
		XMFLOAT3 direction; //(lookat?) //@TODO pass from VS BUFFER?
		float specularPower;
		XMFLOAT4 specularColor;

		LightDataTemplate_PS()
		{
			int type = 0;
			XMFLOAT3 padding = XMFLOAT3();

			XMFLOAT4 diffuseColor = XMFLOAT4();
			XMFLOAT3 direction = XMFLOAT3(); //(lookat?) //@TODO pass from VS BUFFER?

			float specularPower = 0;
			XMFLOAT4 specularColor = XMFLOAT4();
		}

		LightDataTemplate_PS(const LightDataTemplate_PS& otherBuff)
		{
			type = otherBuff.type;
			diffuseColor = otherBuff.diffuseColor;
			direction = otherBuff.direction;
			specularPower = otherBuff.specularPower;
			specularColor = otherBuff.specularColor;
		}
	};

	// DIRECT3D UTILITY FUNCTIONS
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
		ThrowHResultIf(device->CreateBuffer(&desc, nullptr, &buffer));

		return buffer;
	}

	template<class BufferType>
	static void MapBuffer(const BufferType& inData, ID3D11Buffer* pBuffer, ID3D11DeviceContext* deviceContext)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		ThrowHResultIf(deviceContext->Map(pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));
		memcpy(mappedResource.pData, &inData, sizeof(BufferType));
		deviceContext->Unmap(pBuffer, 0);
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
		ThrowHResultIf(device->CreateSamplerState(&desc, &sampler));

		return sampler;
	}

	// CPU SIDE GFX RELATED
	enum EShaderType
	{
		ETEXTURE = 0,
		ELIGHT_DIFFUSE = 1,
		ELIGHT_SPECULAR = 2,
		EREFLECTION = 3,
		EWATER = 4,
		EFONT = 5,
		EDEPTH = 6,
		EMATERIAL_DEFAULT = 7,
		ESIZE = 8
	};

	struct SceneEffects
	{
		XMFLOAT4 ambientColor = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
		XMFLOAT4 clipPlane = XMFLOAT4(0.0f, 0.f, 0.0f, 0.0f);
		float fogStart = 0.f;
		float fogEnd = 3.f;

		SceneEffects()
		{
			ambientColor = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
			clipPlane = XMFLOAT4(0.0f, 0.f, 0.0f, 0.0f);
			fogStart = 0.f;
			fogEnd = 3.f;
		}
	};

	class Material
	{
	public:
		Material();
		Material(EShaderType inShaderType, int numRenderTex);

		bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, EShaderType inShaderType, vector<string> fileNames);

		ID3D11ShaderResourceView* AddRenderTexture(ID3D11Device* const device, const int textureWidth, const int textureHeight, const float screenDepth, const float screenNear);

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
		XMFLOAT2 translation2D = XMFLOAT2(0.f,0.f);
		float reflectRefractScale = 0.01f;
		float waterHeight = 2.75f;
		float textureScale = 1.f;
		float gamma = 1.f;
		unsigned int bBlendTexture = 0;

		// RENDER TEXTURES - for shadowing, reflection, and refraction
		vector<unique_ptr<RenderTextureClass>> _RenderTextures;

	private:
		int numRenderTextures = 0;
	};

	///////////// MATRIX MATH /////////////
	XMMATRIX ComputeWorldTransform(XMFLOAT3& rotation, XMFLOAT3& scale, XMFLOAT3& translation);
}