////////////////////////////////////////////////////////////////////////////////
// Filename: d3dclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "d3dclass.h"
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <algorithm>
#include "WICTextureLoader.h"


using namespace std;

D3DClass::D3DClass()
{}

D3DClass::D3DClass(const D3DClass& other)
{
}

D3DClass::~D3DClass()
{
}

std::vector <IDXGIAdapter*> D3DClass::EnumerateAdapters(int& adapterIndexToUse)
{
	IDXGIAdapter* pAdapter;
	std::vector <IDXGIAdapter*> vAdapters;
	IDXGIFactory* pFactory = NULL;


	// Create a DXGIFactory object.
	if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory)))
	{
		return vAdapters;
	}

	adapterIndexToUse = 0;
	for (UINT i = 0;
		pFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND;
		++i)
	{
		vAdapters.push_back(pAdapter);
	
		//DXGI_ADAPTER_DESC adapterDesc;
		//vAdapters[i]->GetDesc(&adapterDesc);
		//if ((int)adapterDesc.DedicatedVideoMemory > (int)vAdapters[adapterIndexToUse]->GetDesc.DedicatedVideoMemory)
		//{
		//	adapterIndexToUse = i;
		//}
	}

	if (pFactory)
	{
		pFactory->Release();
	}

	return vAdapters;
}

bool D3DClass::Initialize(const int screenWidth, const int screenHeight, const bool vsync, const HWND hwnd, const bool fullscreen, const float screenDepth, const float screenNear)
{
	HRESULT result;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes, i, numerator, denominator;
	unsigned long long stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Texture2D* backBufferPtr;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;
	float fieldOfView, screenAspect;
	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
	D3D11_BLEND_DESC blendStateDescription;

	// Store the vsync setting.
	_vsync_enabled = vsync;

	//////////////////////////////////////////////////////////////////
	// CHOOSE TEH BEST ADAPTER WITH MOST MEMORY //
	/////////////////////////////////////////////////////////////////////////////
	/////// PRINT ADAPTERS /////////
	int adapterIndexToUse = 0;
	std::vector <IDXGIAdapter*> adapters = EnumerateAdapters(adapterIndexToUse);
	std::ofstream file("adapter_output.txt");
	for (auto adapter : adapters)
	{
		adapter->GetDesc(&adapterDesc);
		std::wstring wDesc(adapterDesc.Description);
		std::string desc(wDesc.begin(), wDesc.end());
		file << desc << "\n";
	}
	file.close();

	auto bestAdapter = *(std::max_element(adapters.begin(), adapters.end(), [](IDXGIAdapter* a1, IDXGIAdapter* a2)
	{
		DXGI_ADAPTER_DESC aDesc;

		a1->GetDesc(&aDesc);
		int mem1 = aDesc.DedicatedVideoMemory;

		a2->GetDesc(&aDesc);
		int mem2 = aDesc.DedicatedVideoMemory;

		return mem1 > mem2;
	}));

	DXGI_ADAPTER_DESC testDesc;
	bestAdapter->GetDesc(&testDesc);
	//////////////////////////////////////////////////////////////////

	// Create a DirectX graphics interface factory.
	ThrowHResultIf(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory));

	// Use the factory to create an adapter for the primary graphics interface (video card).
	ThrowHResultIf(factory->EnumAdapters(0 /*adapterIndexToUse*/, &adapter));

	// Enumerate the primary adapter output (monitor).
	ThrowHResultIf(adapter->EnumOutputs(0, &adapterOutput));

	// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
	ThrowHResultIf(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL));

	// Create a list to hold all the possible display modes for this monitor/video card combination.
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
	{
		return false;
	}

	// Now fill the display mode list structures.
	ThrowHResultIf(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList));

	// Now go through all the display modes and find the one that matches the screen width and height.
	// When a match is found store the numerator and denominator of the refresh rate for that monitor.
	for (i = 0; i<numModes; i++)
	{
		if (displayModeList[i].Width == (unsigned int)screenWidth)
		{
			if (displayModeList[i].Height == (unsigned int)screenHeight)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	// Get the adapter (video card) description.
	ThrowHResultIf(adapter->GetDesc(&adapterDesc));

	// Store the dedicated video card memory in megabytes.
	_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// Convert the name of the video card to a character array and store it.
	error = wcstombs_s(&stringLength, _videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
	{
		ThrowRuntime("Failed to store video card name.");
	}

	// Release the display mode list.
	delete[] displayModeList;
	displayModeList = 0;

	// Release the adapter output.
	adapterOutput->Release();
	adapterOutput = 0;

	// Release the adapter.
	adapter->Release();
	adapter = 0;

	// Release the factory.
	factory->Release();
	factory = 0;

	// Initialize the swap chain description.
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// Set to a single back buffer.
	swapChainDesc.BufferCount = 1;

	// Set the width and height of the back buffer.
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	// Set regular 32-bit surface for the back buffer.
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// Set the refresh rate of the back buffer.
	if (_vsync_enabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// Set the usage of the back buffer.
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// Set the handle for the window to render to.
	swapChainDesc.OutputWindow = hwnd;

	// Turn multisampling off.
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// Set to full screen or windowed mode.
	if (fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	// Set the scan line ordering and scaling to unspecified.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Discard the back buffer contents after presenting.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// Don't set the advanced flags.
	swapChainDesc.Flags = 0;

	// Set the feature level to DirectX 11.
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	// Create the swap chain, Direct3D device, and Direct3D device context.
	ThrowHResultIf(D3D11CreateDeviceAndSwapChain(bestAdapter /*NULL*/, D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_UNKNOWN/*D3D_DRIVER_TYPE_HARDWARE*/, NULL, 0, 
		/*NULL, 0,*/ &featureLevel, 1,
		D3D11_SDK_VERSION, &swapChainDesc, &_swapChain, &_device, NULL, &_deviceContext));

	// Get the pointer to the back buffer.
	ThrowHResultIf(_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr));

	// Create the render target view with the back buffer pointer.
	ThrowHResultIf(_device->CreateRenderTargetView(backBufferPtr, NULL, &_renderTargetView));

	// Release pointer to the back buffer as we no longer need it.
	backBufferPtr->Release();
	backBufferPtr = 0;

	// Initialize the description of the depth buffer.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// Set up the description of the depth buffer.
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// Create the texture for the depth buffer using the filled out description.
	ThrowHResultIf(_device->CreateTexture2D(&depthBufferDesc, NULL, &_depthStencilBuffer));

	// Initialize the description of the stencil state.
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create the depth stencil state.
	ThrowHResultIf(_device->CreateDepthStencilState(&depthStencilDesc, &_depthStencilState));

	// Set the depth stencil state.
	_deviceContext->OMSetDepthStencilState(_depthStencilState.Get(), 1);

	// Initialize the depth stencil view.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// Set up the depth stencil view description.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	ThrowHResultIf(_device->CreateDepthStencilView(_depthStencilBuffer.Get(), &depthStencilViewDesc, &_depthStencilView));

	// Bind the render target view and depth stencil buffer to the output render pipeline.
	_deviceContext->OMSetRenderTargets(1, _renderTargetView.GetAddressOf(), _depthStencilView.Get());

	// Setup the raster description which will determine how and what polygons will be drawn.
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID; //D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out.
	ThrowHResultIf(_device->CreateRasterizerState(&rasterDesc, &_rasterState));

	// Now set the rasterizer state.
	_deviceContext->RSSetState(_rasterState.Get());

	// SETUP RASTER STATE FOR FRONT FACE CULLING
	// SETUP RASTER STATE FOR FRONT FACE CULLING
	// Setup the raster description which will determine how and what polygons will be drawn.
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_NONE; //D3D11_CULL_FRONT;
	rasterDesc.DepthBias = 1000;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 1.0f;
	ThrowHResultIf(_device->CreateRasterizerState(&rasterDesc, &_rasterStateFrontFaceCulling));
	// SETUP RASTER STATE FOR FRONT FACE CULLING
	// SETUP RASTER STATE FOR FRONT FACE CULLING

	// Setup a raster description which turns off back face culling.
	rasterDesc.CullMode = D3D11_CULL_NONE;

	// Create the no culling rasterizer state.
	ThrowHResultIf(_device->CreateRasterizerState(&rasterDesc, &_rasterStateNoCulling));

	// Setup a raster description which enables wire frame rendering.
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the wire frame rasterizer state.
	ThrowHResultIf(_device->CreateRasterizerState(&rasterDesc, &_rasterStateWireframe));

	// Setup the viewport for rendering.
	_viewport.Width = (float)screenWidth;
	_viewport.Height = (float)screenHeight;
	_viewport.MinDepth = 0.0f;
	_viewport.MaxDepth = 1.0f;
	_viewport.TopLeftX = 0.0f;
	_viewport.TopLeftY = 0.0f;

	// Create the viewport.
	_deviceContext->RSSetViewports(1, &_viewport);

	// Setup the projection matrix.
	fieldOfView = 3.141592654f / 4.0f;
	screenAspect = (float)screenWidth / (float)screenHeight;

	// Create the projection matrix for 3D rendering.
	_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

	// Initialize the world matrix to the identity matrix.
	_worldMatrix = XMMatrixIdentity();

	// Create an orthographic projection matrix for 2D rendering.
	_orthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);

	// Now create a second depth stencil state which turns off the Z buffer for 2D rendering.  The only difference is 
	// that DepthEnable is set to false, all other parameters are the same as the other depth stencil state.
	depthDisabledStencilDesc.DepthEnable = false;
	depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDisabledStencilDesc.StencilEnable = true;
	depthDisabledStencilDesc.StencilReadMask = 0xFF;
	depthDisabledStencilDesc.StencilWriteMask = 0xFF;
	depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create the state using the device.
	ThrowHResultIf(_device->CreateDepthStencilState(&depthDisabledStencilDesc, &_depthDisabledStencilState));

	// Clear the blend state description.
	ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));

	// Create an alpha enabled blend state description.
	//blendStateDescription.AlphaToCoverageEnable = FALSE;
	//blendStateDescription.IndependentBlendEnable = false;
	//blendStateDescription.RenderTarget[0].BlendEnable = true;
	//blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	//blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	//blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	//blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	//blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	//blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	//blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	// FOR ALPHA BLENDING TEXTURES
	// Create an alpha enabled blend state description.
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	//blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	// Create the blend state using the description.
	ThrowHResultIf(_device->CreateBlendState(&blendStateDescription, &_alphaEnableBlendingState));

	// Modify the description to create an alpha disabled blend state description.
	blendStateDescription.RenderTarget[0].BlendEnable = false;
	blendStateDescription.AlphaToCoverageEnable = false;

	// Create the blend state using the description.
	ThrowHResultIf(_device->CreateBlendState(&blendStateDescription, &_alphaDisableBlendingState));

	// Create a blend state description for the alpha-to-coverage blending mode.
	blendStateDescription.AlphaToCoverageEnable = true;
	blendStateDescription.IndependentBlendEnable = false;
	blendStateDescription.RenderTarget[0].BlendEnable = true;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	// Create the blend state using the description.
	ThrowHResultIf(_device->CreateBlendState(&blendStateDescription, &_alphaEnableBlendingState2));

	return true;
}

void D3DClass::BeginScene(const float red, const float green, const float blue, const float alpha)
{
	float color[4];


	// Setup the color to clear the buffer to.
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// Clear the back buffer.
	_deviceContext->ClearRenderTargetView(_renderTargetView.Get(), color);

	// Clear the depth buffer.
	_deviceContext->ClearDepthStencilView(_depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}


void D3DClass::EndScene()
{
	// Present the back buffer to the screen since rendering is complete.
	if (_vsync_enabled)
	{
		// Lock to screen refresh rate.
		_swapChain->Present(1, 0);
	}
	else
	{
		// Present as fast as possible.
		_swapChain->Present(0, 0);
	}
}


ID3D11Device* D3DClass::GetDevice()
{
	return _device.Get();
}


ID3D11DeviceContext* D3DClass::GetDeviceContext()
{
	return _deviceContext.Get();
}


//void D3DClass::GetProjectionMatrix(XMMATRIX& projectionMatrix)
//{
//	projectionMatrix = _projectionMatrix;
//}
XMMATRIX D3DClass::GetProjectionMatrix()
{
	return _projectionMatrix;
}

//void D3DClass::GetWorldMatrix(XMMATRIX& worldMatrix)
//{
//	worldMatrix = _worldMatrix;
//}
XMMATRIX D3DClass::GetWorldMatrix()
{
	return _worldMatrix;
}

void D3DClass::GetOrthoMatrix(XMMATRIX& orthoMatrix)
{
	orthoMatrix = _orthoMatrix;
}


void D3DClass::GetVideoCardInfo(char* const cardName, int& memory)
{
	strcpy_s(cardName, 128, _videoCardDescription);
	memory = _videoCardMemory;
}


void D3DClass::TurnZBufferOn()
{
	_deviceContext->OMSetDepthStencilState(_depthStencilState.Get(), 1);
}


void D3DClass::TurnZBufferOff()
{
	_deviceContext->OMSetDepthStencilState(_depthDisabledStencilState.Get(), 1);
}


void D3DClass::EnableAlphaBlending()
{
	float blendFactor[4];


	// Setup the blend factor.
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;

	// Turn on the alpha blending.
	_deviceContext->OMSetBlendState(_alphaEnableBlendingState.Get(), blendFactor, 0xffffffff);
}


void D3DClass::DisableAlphaBlending()
{
	float blendFactor[4];


	// Setup the blend factor.
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;

	// Turn off the alpha blending.
	_deviceContext->OMSetBlendState(_alphaDisableBlendingState.Get(), blendFactor, 0xffffffff);
}


void D3DClass::TurnOnCulling()
{
	// Set the culling rasterizer state.
	_deviceContext->RSSetState(_rasterState.Get());
}

void D3DClass::TurnOnFrontFaceCulling()
{
	// Set the culling rasterizer state.
	_deviceContext->RSSetState(_rasterStateFrontFaceCulling.Get());
}

void D3DClass::TurnOffCulling()
{
	// Set the no back face culling rasterizer state.
	_deviceContext->RSSetState(_rasterStateNoCulling.Get());
}


void D3DClass::EnableAlphaToCoverageBlending()
{
	float blendFactor[4];


	// Setup the blend factor.
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;

	// Turn on the alpha blending.
	_deviceContext->OMSetBlendState(_alphaEnableBlendingState2.Get(), blendFactor, 0xffffffff);
}


void D3DClass::EnableWireframe()
{
	// Set the wire frame rasterizer state.
	_deviceContext->RSSetState(_rasterStateWireframe.Get());
}


void D3DClass::DisableWireframe()
{
	// Set the solid fill rasterizer state.
	_deviceContext->RSSetState(_rasterState.Get());
}

ID3D11DepthStencilView* D3DClass::GetDepthStencilView()
{
	return _depthStencilView.Get();
}

void D3DClass::SetBackBufferRenderTarget()
{
	// Bind the render target view and depth stencil buffer to the output render pipeline.
	_deviceContext->OMSetRenderTargets(1, _renderTargetView.GetAddressOf(),_depthStencilView.Get());
}

void D3DClass::ResetViewport()
{
	// Set the viewport.
	_deviceContext->RSSetViewports(1, &_viewport);
}