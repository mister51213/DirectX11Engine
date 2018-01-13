////////////////////////////////////////////////////////////////////////////////
// Filename: rendertextureclass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

/////////////
// LINKING //
/////////////
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")

//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include "GlobalIncludes.h"

//using namespace DirectX;
//using namespace Microsoft::WRL;

////////////////////////////////////////////////////////////////////////////////
// Class name: RenderTextureClass
////////////////////////////////////////////////////////////////////////////////
class RenderTextureClass
{
public:
	RenderTextureClass();
	RenderTextureClass(ID3D11Device * const device, const int textureWidth, const int textureHeight, const float screenDepth, const float screenNear);
	RenderTextureClass(const RenderTextureClass&);
	~RenderTextureClass();

	bool Initialize(ID3D11Device* const device, const int textureWidth, const int textureHeight, const float screenDepth, const float screenNear);

	void SetRenderTarget(ID3D11DeviceContext* const deviceContext);
	void ClearRenderTarget(ID3D11DeviceContext* const deviceContext,
		const float red, const float green, const float blue, const float alpha);
	ID3D11ShaderResourceView* GetShaderResourceView();
	
	void GetProjectionMatrix(XMMATRIX&);
	void GetOrthoMatrix(XMMATRIX&);

private:
	ComPtr<ID3D11Texture2D> _renderTargetTexture;
	ComPtr<ID3D11RenderTargetView> _renderTargetView;
	ComPtr<ID3D11ShaderResourceView> _shaderResourceView;

	//////// Shadowing ///////////////
	ComPtr<ID3D11Texture2D> _depthStencilBuffer;
	ComPtr<ID3D11DepthStencilView> _depthStencilView;

	D3D11_VIEWPORT _viewport;
	XMMATRIX _projectionMatrix;
	XMMATRIX _orthoMatrix;
};

#pragma region LEGACY VERSION
//
//////////////////////////////////////////////////////////////////////////////////
//// Filename: rendertextureclass.h
//////////////////////////////////////////////////////////////////////////////////
//#ifndef _RENDERTEXTURECLASS_H_
//#define _RENDERTEXTURECLASS_H_
//
//
////////////////
//// INCLUDES //
////////////////
//#include <d3d11.h>
//#include <DirectXMath.h>
//using namespace DirectX;
//
//////////////////////////////////////////////////////////////////////////////////
//// Class name: RenderTextureClass
//////////////////////////////////////////////////////////////////////////////////
//class RenderTextureClass
//{
//public:
//	RenderTextureClass();
//	RenderTextureClass(const RenderTextureClass&);
//	~RenderTextureClass();
//
//	bool Initialize(ID3D11Device*, int, int, float, float);
//	void Shutdown();
//
//	void SetRenderTarget(ID3D11DeviceContext*);
//	void ClearRenderTarget(ID3D11DeviceContext*, float, float, float, float);
//	ID3D11ShaderResourceView* GetShaderResourceView();
//	void GetProjectionMatrix(XMMATRIX&);
//	void GetOrthoMatrix(XMMATRIX&);
//
//private:
//	ID3D11Texture2D* m_renderTargetTexture;
//	ID3D11RenderTargetView* m_renderTargetView;
//	ID3D11ShaderResourceView* m_shaderResourceView;
//	ID3D11Texture2D* m_depthStencilBuffer;
//	ID3D11DepthStencilView* m_depthStencilView;
//	D3D11_VIEWPORT m_viewport;
//	XMMATRIX m_projectionMatrix;
//	XMMATRIX m_orthoMatrix;
//
//};
//
//#endif
#pragma endregion