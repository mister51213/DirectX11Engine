////////////////////////////////////////////////////////////////////////////////
// Filename: d3dclass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

/////////////
// LINKING //
/////////////
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <directxmath.h>
#include <wrl/client.h>
#include "GlobalIncludes.h"

using namespace DirectX;
using namespace Microsoft::WRL;


////////////////////////////////////////////////////////////////////////////////
// Class name: D3DClass
////////////////////////////////////////////////////////////////////////////////
class D3DClass
{

friend class System;

public:
	D3DClass();
	D3DClass(const D3DClass&);
	~D3DClass();

	std::vector<IDXGIAdapter*> EnumerateAdapters(int& adapterToUse);

	bool Initialize(const int screenWidth, const int screenHeight, const bool vsync, const HWND hwnd, const bool fullscreen, const float screenDepth, const float screenNear);

	void BeginScene(const float red, const float green, const float blue, const float alpha);
	void EndScene();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	//void GetProjectionMatrix(XMMATRIX&);
	//void GetWorldMatrix(XMMATRIX&);
	XMMATRIX GetProjectionMatrix();
	XMMATRIX GetWorldMatrix();

	void GetOrthoMatrix(XMMATRIX&);

	void GetVideoCardInfo(char* const cardName, int& memory);

	void TurnZBufferOn();
	void TurnZBufferOff();

	void TurnOnCulling();
	void TurnOffCulling();

	void EnableAlphaBlending();
	void EnableAlphaToCoverageBlending();
	void DisableAlphaBlending();

	void EnableWireframe();
	void DisableWireframe();

	ID3D11DepthStencilView* GetDepthStencilView();
	void SetBackBufferRenderTarget();
	void ResetViewport();
private:
	bool _vsync_enabled;
	int _videoCardMemory;
	char _videoCardDescription[128];

	ComPtr<IDXGISwapChain> _swapChain;
	ComPtr<ID3D11Device> _device;
	ComPtr<ID3D11DeviceContext> _deviceContext;
	ComPtr<ID3D11RenderTargetView> _renderTargetView;
	ComPtr<ID3D11Texture2D> _depthStencilBuffer;
	ComPtr<ID3D11DepthStencilState> _depthStencilState;
	ComPtr<ID3D11DepthStencilView> _depthStencilView;
	ComPtr<ID3D11RasterizerState> _rasterState;
	ComPtr<ID3D11RasterizerState> _rasterStateNoCulling;
	ComPtr<ID3D11RasterizerState> _rasterStateWireframe;
	ComPtr<ID3D11DepthStencilState> _depthDisabledStencilState;
	ComPtr<ID3D11BlendState> _alphaEnableBlendingState;
	ComPtr<ID3D11BlendState> _alphaDisableBlendingState;
	ComPtr<ID3D11BlendState> _alphaEnableBlendingState2;
	
	D3D11_VIEWPORT _viewport;
	XMMATRIX _projectionMatrix;
	XMMATRIX _worldMatrix;
	XMMATRIX _orthoMatrix;
};






//// WORKING VERSION // // WORKING VERSION // // WORKING VERSION //
//// WORKING VERSION // // WORKING VERSION // // WORKING VERSION //
//// WORKING VERSION // // WORKING VERSION // // WORKING VERSION //
//
//////////////////////////////////////////////////////////////////////////////////
//// Filename: d3dclass.h
//////////////////////////////////////////////////////////////////////////////////
//#ifndef _D3DCLASS_H_
//#define _D3DCLASS_H_
//
//
///////////////
//// LINKING //
///////////////
//#pragma comment(lib, "dxgi.lib")
//#pragma comment(lib, "d3d11.lib")
////#pragma comment(lib, "d3dx11.lib")
////#pragma comment(lib, "d3dx10.lib")
//
//
////////////////
//// INCLUDES //
////////////////
//#include <Windows.h>
//#include <dxgi.h>
//#include <d3dcommon.h>
//#include <d3d11.h>
////#include <DirectXMath.h>
//#include <DirectXMath.h>
//using namespace DirectX;
//
//////////////////////////////////////////////////////////////////////////////////
//// Class name: D3DClass
//////////////////////////////////////////////////////////////////////////////////
//class D3DClass
//{
//public:
//	D3DClass();
//	D3DClass(const D3DClass&);
//	~D3DClass();
//
//	bool Initialize(int, int, bool, HWND, bool, float, float);
//	void Shutdown();
//
//	void BeginScene(float, float, float, float);
//	void EndScene();
//
//	ID3D11Device* GetDevice();
//	ID3D11DeviceContext* GetDeviceContext();
//
//	void GetProjectionMatrix(XMMATRIX&);
//	void GetWorldMatrix(XMMATRIX&);
//	void GetOrthoMatrix(XMMATRIX&);
//
//	void GetVideoCardInfo(char*, int&);
//
//	void SetBackBufferRenderTarget();
//	void ResetViewport();
//
//private:
//	bool m_vsync_enabled;
//	int m_videoCardMemory;
//	char m_videoCardDescription[128];
//	IDXGISwapChain* m_swapChain;
//	ID3D11Device* m_device;
//	ID3D11DeviceContext* m_deviceContext;
//	ID3D11RenderTargetView* m_renderTargetView;
//	ID3D11Texture2D* m_depthStencilBuffer;
//	ID3D11DepthStencilState* m_depthStencilState;
//	ID3D11DepthStencilView* m_depthStencilView;
//	ID3D11RasterizerState* m_rasterState;
//	XMMATRIX m_projectionMatrix;
//	XMMATRIX m_worldMatrix;
//	XMMATRIX m_orthoMatrix;
//	D3D11_VIEWPORT m_viewport;
//};
//
//#endif