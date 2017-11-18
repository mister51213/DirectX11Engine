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
using namespace DirectX;


////////////////////////////////////////////////////////////////////////////////
// Class name: D3DClass
////////////////////////////////////////////////////////////////////////////////
class D3DClass
{
public:
	D3DClass();
	D3DClass(const D3DClass&);
	~D3DClass();

	bool Initialize(int, int, bool, HWND, bool, float, float);
	void Shutdown();

	void BeginScene(float, float, float, float);
	void EndScene();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	void GetProjectionMatrix(XMMATRIX&);
	void GetWorldMatrix(XMMATRIX&);
	void GetOrthoMatrix(XMMATRIX&);

	void GetVideoCardInfo(char*, int&);

	void TurnZBufferOn();
	void TurnZBufferOff();

	void TurnOnCulling();
	void TurnOffCulling();

	//	void TurnOnAlphaBlending();
	//	void TurnOffAlphaBlending();
	void EnableAlphaBlending();
	void EnableAlphaToCoverageBlending();
	void DisableAlphaBlending();

	void EnableWireframe();
	void DisableWireframe();

	ID3D11DepthStencilView* GetDepthStencilView();
	void SetBackBufferRenderTarget();
private:
	bool m_vsync_enabled;
	int m_videoCardMemory;
	char m_videoCardDescription[128];
	IDXGISwapChain* m_swapChain;
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilState* m_depthStencilState;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11RasterizerState* m_rasterState;
	ID3D11RasterizerState* m_rasterStateNoCulling;
	ID3D11RasterizerState* m_rasterStateWireframe;
	XMMATRIX m_projectionMatrix;
	XMMATRIX m_worldMatrix;
	XMMATRIX m_orthoMatrix;
	ID3D11DepthStencilState* m_depthDisabledStencilState;
	ID3D11BlendState* m_alphaEnableBlendingState;
	ID3D11BlendState* m_alphaDisableBlendingState;
	ID3D11BlendState* m_alphaEnableBlendingState2;
};


//////////////////////////////////////////////////////////////////////////////////
//// Filename: d3dclass.h
//////////////////////////////////////////////////////////////////////////////////
//#pragma once
//
///////////////
//// LINKING //
///////////////
//#pragma comment(lib, "dxgi.lib")
//#pragma comment(lib, "d3d11.lib")
////#pragma comment(lib, "d3dx11.lib")
////#pragma comment(lib, "d3dx10.lib")
//
////////////////
//// INCLUDES //
////////////////
//#include <string>
//#include <dxgi.h>
//#include <d3dcommon.h>
//#include <d3d11.h>
////#include <d3dx10math.h>
//#include <DirectXMath.h>
//
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
//	bool Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear);
//	void Shutdown();
//
//	void BeginScene(float red, float green, float blue, float alpha);
//	void EndScene();
//
//	ID3D11Device* GetDevice();
//	ID3D11DeviceContext* GetDeviceContext();
//
//	void GetProjectionMatrix(DirectX::XMMATRIX&);
//	void GetWorldMatrix(DirectX::XMMATRIX&);
//	void GetOrthoMatrix(DirectX::XMMATRIX&);
//
//	void GetVideoCardInfo(char* Description, int& memory);
//	//void GetVideoCardInfo(std::wstring &Description, int& memory);
//
//	void TurnZBufferOn();
//	void TurnZBufferOff();
//
//	void TurnOnAlphaBlending();
//	void TurnOffAlphaBlending();
//
//	ID3D11DepthStencilView* GetDepthStencilView();
//	void SetBackBufferRenderTarget();
//private:
//	bool _vsync_enabled;
//	int _videoCardMemory;
//	char _videoCardDescription[128];
//	//std::wstring _videoCardDescription;
//
//	IDXGISwapChain* _swapChain;
//	ID3D11Device* _device;
//	ID3D11DeviceContext* _deviceContext;
//	ID3D11RenderTargetView* _renderTargetView;
//	ID3D11Texture2D* _depthStencilBuffer;
//	ID3D11DepthStencilState* _depthStencilState;
//	ID3D11DepthStencilView* _depthStencilView;
//	ID3D11RasterizerState* _rasterState;
//
//	DirectX::XMMATRIX _projectionMatrix;
//	DirectX::XMMATRIX _worldMatrix;
//	DirectX::XMMATRIX _orthoMatrix;
//
//	ID3D11DepthStencilState* _depthDisabledStencilState;
//	ID3D11BlendState* _alphaEnableBlendingState;
//	ID3D11BlendState* _alphaDisableBlendingState;
//};