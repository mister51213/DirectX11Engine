////////////////////////////////////////////////////////////////////////////////
// Filename: Graphics.cpp
////////////////////////////////////////////////////////////////////////////////
#include "lightshaderclass.h"
#include "lightclass.h"
#include "Graphics.h"
#include "World.h"


Graphics::Graphics()
	:
	_D3D(nullptr),
	_Camera(nullptr),
	_Model(nullptr),
	//_LightShader(nullptr),
	//_TextureShader(nullptr),
	_Light(nullptr),
	_ModelList(nullptr),
	_GroundModel(nullptr),
	_WallModel(nullptr),
	_BathModel(nullptr),
	_WaterModel(nullptr),
	_RefractionTexture(nullptr),
	//_ReflectionTexture(nullptr),
	_Frustum(nullptr),
	_Font1(nullptr),
	_FpsString(nullptr),
	_VideoStrings(nullptr),
	_PositionStrings(nullptr),
	_RenderCountStrings(nullptr),
	_RenderTexture(nullptr),
	_DebugWindow(nullptr),
	_ReflectionTexture(nullptr),
	_FloorModel(nullptr)
{}

Graphics::Graphics(const Graphics& other)
{}

Graphics::~Graphics()
{
}

bool Graphics::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

	// Create the Direct3D object.
	_D3D = new D3DClass;
	if (!_D3D){return false;}

	// Initialize the Direct3D object.
	result = _D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result){MessageBox(hwnd, "Could not initialize Direct3D", "Error", MB_OK);return false;}

	// Create / initialize the shader manager object.
	_ShaderManager = new ShaderManagerClass;	if (!_ShaderManager){return false;}
	result = _ShaderManager->Initialize(_D3D->GetDevice(), hwnd);
	if (!result){MessageBox(hwnd, "Could not initialize the shader manager object.", "Error", MB_OK);	return false;}

	// Create the camera object.
	_Camera = new Camera;if (!_Camera){	return false;}	_Camera->SetPosition(0.0f, 0.0f, -4.f);	_Camera->UpdateViewFromPosition();

	// InitializeModels
	result = InitializeModels(hwnd, screenWidth, screenHeight);

	// Initialize Lights
	result = InitializeLights();

	// Initialize UI
	result = InitializeUI(screenWidth, screenHeight);
	
	return true;
}

bool Graphics::InitializeLights()
{
	// Create the light object.
	_Light = new LightClass; if (!_Light) { return false; }

	// Initialize the light object.
	_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	_Light->SetDirection(0.0f, -1.0f, 0.5f);
	_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	_Light->SetSpecularPower(16.0f); // the lower the power, the higher the effect intensity

	return true;
}

bool Graphics::InitializeModels(const HWND &hwnd, int screenWidth, int screenHeight)
{
	///////////////// WATER /////////////////////
	// Create the ground model object.
	bool result = _GroundModel = new Model;
	if (!_GroundModel)
	{
		return false;
	}

	// Initialize the ground model object.
	result = _GroundModel->InitializeDDS(_D3D->GetDevice(), _D3D->GetDeviceContext(),
		"../DirectX11Engine/data/ground.txt",
		"../DirectX11Engine/data/ground.dds",
		"../DirectX11Engine/data/dirt.dds", // tex2
		"../DirectX11Engine/data/light.dds", // lightmap
		"../DirectX11Engine/data/alpha.dds", // alpha
		"../DirectX11Engine/data/blue.dds", // normal map
		"../DirectX11Engine/data/specMap.dds"); // specMap
	if (!result){MessageBox(hwnd, "Could not initialize the ground model object.", "Error", MB_OK);
		return false;}

	// Create the wall model object.
	_WallModel = new Model;
	if (!_WallModel){return false;}

	// Initialize the wall model object.
	result = _WallModel->InitializeDDS(_D3D->GetDevice(), _D3D->GetDeviceContext(),
		"../DirectX11Engine/data/wall.txt",
		"../DirectX11Engine/data/wall.dds",
		"../DirectX11Engine/data/dirt.dds", // tex2
		"../DirectX11Engine/data/light.dds", // lightmap
		"../DirectX11Engine/data/alpha.dds", // alpha
		"../DirectX11Engine/data/bumpMap.dds", // normal map
		"../DirectX11Engine/data/specMap.dds"); // specMap
	if (!result)
	{
		MessageBox(hwnd, "Could not initialize the wall model object.", "Error", MB_OK);
		return false;
	}

	// Create the bath model object.
	_BathModel = new Model;
	if (!_BathModel)
	{
		return false;
	}

	// Initialize the bath model object.
	result = _BathModel->InitializeDDS(_D3D->GetDevice(), _D3D->GetDeviceContext(),
		"../DirectX11Engine/data/bath.txt",
		"../DirectX11Engine/data/marble.dds",
		"../DirectX11Engine/data/dirt.dds", // tex2
		"../DirectX11Engine/data/light.dds", // lightmap
		"../DirectX11Engine/data/alpha.dds", // alpha
		"../DirectX11Engine/data/blue.dds", // normal map
		"../DirectX11Engine/data/specMap.dds"); // specMap
	if (!result)
	{
		MessageBox(hwnd, "Could not initialize the bath model object.", "Error", MB_OK);
		return false;
	}

	// Create the water model object.
	_WaterModel = new Model;
	if (!_WaterModel)
	{
		return false;
	}

	// Initialize the water model object.
	result = _WaterModel->InitializeDDS(_D3D->GetDevice(), _D3D->GetDeviceContext(),
		"../DirectX11Engine/data/water.txt",
		////"../DirectX11Engine/data/water.dds",
		////"../DirectX11Engine/data/dirt.dds", // tex2
		////"../DirectX11Engine/data/light.dds", // lightmap
		////"../DirectX11Engine/data/alpha.dds", // alpha
		////"../DirectX11Engine/data/bumpMap.dds", // normal map
		////"../DirectX11Engine/data/specMap.dds"); // specMap
		"../DirectX11Engine/data/water.dds", 
		"../DirectX11Engine/data/water.dds", 
		"../DirectX11Engine/data/water.dds", 
		"../DirectX11Engine/data/water.dds", 
		"../DirectX11Engine/data/water.dds", 
		"../DirectX11Engine/data/water.dds");
	if (!result)
	{
		MessageBox(hwnd, "Could not initialize the water model object.", "Error", MB_OK);
		return false;
	}

	// Create the refraction render to texture object.
	_RefractionTexture = new RenderTextureClass;
	if (!_RefractionTexture)
	{
		return false;
	}

	// Initialize the refraction render to texture object.
	result = _RefractionTexture->Initialize(_D3D->GetDevice(), screenWidth, screenHeight);
	if (!result)
	{
		MessageBox(hwnd, "Could not initialize the refraction render to texture object.", "Error", MB_OK);
		return false;
	}

	// Create the reflection render to texture object.
	_ReflectionTexture = new RenderTextureClass;
	if (!_ReflectionTexture)
	{
		return false;
	}

	// Initialize the reflection render to texture object.
	result = _ReflectionTexture->Initialize(_D3D->GetDevice(), screenWidth, screenHeight);
	if (!result)
	{
		MessageBox(hwnd, "Could not initialize the reflection render to texture object.", "Error", MB_OK);
		return false;
	}

	// Set the height of the water.
	_waterHeight = 2.75f;

	// Initialize the position of the water.
	_waterTranslation = 0.0f;

	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^//
	///////////////////////// WATER ////////////////////////

	// Create the model object.
	_Model = new Model;
	if (!_Model)
	{
		return false;
	}

	// Initialize the model object.
	result = _Model->InitializeDDS(
		_D3D->GetDevice(),
		_D3D->GetDeviceContext(),
		"../DirectX11Engine/data/sphere.txt",
		"../DirectX11Engine/data/stone.dds", // tex1
		"../DirectX11Engine/data/dirt.dds", // tex2
		"../DirectX11Engine/data/light.dds", // lightmap
		"../DirectX11Engine/data/alpha.dds", // alpha
		"../DirectX11Engine/data/bumpMap.dds", // normal map
		"../DirectX11Engine/data/specMap.dds"); // specMap
	if (!result)
	{
		MessageBox(hwnd, "Could not initialize the model object.", "Error", MB_OK);
		return false;
	}

	// Create the frustum object.
	_Frustum = new FrustumClass;
	if (!_Frustum)
	{
		return false;
	}

	// Create the model list object.
	_ModelList = new ModelListClass;
	if (!_ModelList)
	{
		return false;
	}

	// Initialize the model list object.
	result = _ModelList->Initialize(20);
	if (!result)
	{
		MessageBox(hwnd, "Could not initialize the model list object.", "Error", MB_OK);
		return false;
	}

	// Create the render to texture object.
	//_ReflectionTexture = new RenderTextureClass;
	//if (!_ReflectionTexture)
	//{
	//	return false;
	//}

	_ModelSingle = new Model;
	if (!_ModelSingle)
	{
		return false;
	}

	result = _ModelSingle->InitializeDDS(
		_D3D->GetDevice(),
		_D3D->GetDeviceContext(),
		"../DirectX11Engine/data/sphere.txt",
		"../DirectX11Engine/data/stone.dds", // tex1
		"../DirectX11Engine/data/dirt.dds", // tex2
		"../DirectX11Engine/data/light.dds", // lightmap
		"../DirectX11Engine/data/alpha.dds", // alpha
		"../DirectX11Engine/data/bumpMap.dds", // normal map
		"../DirectX11Engine/data/specMap.dds"); // specMap
	if (!result)
	{
		MessageBox(hwnd, "Could not initialize the model object.", "Error", MB_OK);
		return false;
	}

	//// Initialize the render to texture object.
	//result = _ReflectionTexture->Initialize(_D3D->GetDevice(), screenWidth, screenHeight);
	//if (!result)
	//{
	//	return false;
	//}

	////Create and initialize the blue floor model object.
	//_FloorModel = new Model;
	//if (!_FloorModel)
	//{
	//	return false;
	//}

	//// Initialize the floor model object. //TODO: make tga
	//result = _FloorModel->Initialize(_D3D->GetDevice(),
	//	_D3D->GetDeviceContext(),
	//	"../DirectX11Engine/data/floor.txt",
	//	"../DirectX11Engine/data/blue.dds", 
	//	"../DirectX11Engine/data/blue.dds",
	//	"../DirectX11Engine/data/blue.dds",
	//	"../DirectX11Engine/data/blue.dds",
	//	"../DirectX11Engine/data/blue.dds",
	//	"../DirectX11Engine/data/blue.dds");
	//if (!result)
	//{
	//	MessageBox(hwnd, "Could not initialize the floor model object.", "Error", MB_OK);
	//	return false;
	//}

	return true;
}

bool Graphics::InitializeUI(int screenWidth, int screenHeight)
{
	bool result;

	char videoCard[128];
	int videoMemory;
	char videoString[144];
	char memoryString[32];
	char tempString[16];
	////////////////
	// UI RELATED //
	////////////////
	// Create the render to texture object.
	//_RenderTexture = new RenderTextureClass;
	//if (!_RenderTexture)
	//{
	//	return false;
	//}
	//// Initialize the render to texture object.
	//result = _RenderTexture->Initialize(_D3D->GetDevice(), screenWidth, screenHeight);
	//if (!result)
	//{
	//	return false;
	//}
	// Create the debug window object.
	//_DebugWindow = new DebugWindowClass;
	//if (!_DebugWindow)
	//{
	//	return false;
	//}
	//// Initialize the debug window object.
	//result = _DebugWindow->Initialize(_D3D->GetDevice(), screenWidth, screenHeight, 100, 100);
	//if (!result)
	//{
	//	MessageBox(hwnd, "Could not initialize the debug window object.", "Error", MB_OK);
	//	return false;
	//}

	//// Create the texture shader object.
	//_TextureShader = new TextureShaderClass;
	//if (!_TextureShader)
	//{
	//	return false;
	//}

	//// Initialize the texture shader object.
	//result = _TextureShader->Initialize(_D3D->GetDevice(), hwnd);
	//if (!result)
	//{
	//	MessageBox(hwnd, "Could not initialize the texture shader object.", "Error", MB_OK);
	//	return false;
	//}

	// Create the first font object.
	_Font1 = new FontClass;
	if (!_Font1)
	{
		return false;
	}

	// Initialize the first font object.
	result = _Font1->Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(), "../DirectX11Engine/data/font.txt",
		"../DirectX11Engine/data/font.tga", 32.0f, 3);
	if (!result)
	{
		return false;
	}

	//// Create the font shader object.
	//_FontShader = new FontShaderClass;
	//if (!_FontShader)
	//{
	//	return false;
	//}
	//
	//// Initialize the font shader object.
	//result = _FontShader->Initialize(_D3D->GetDevice(), hwnd);
	//if (!result)
	//{
	//	MessageBox(hwnd, "Could not initialize the font shader object.", "Error", MB_OK);
	//	return false;
	//}

	// Create the text object for the fps string.
	_FpsString = new TextClass;
	if (!_FpsString)
	{
		return false;
	}

	// Initialize the fps text string.
	result = _FpsString->Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(), screenWidth, screenHeight, 16, false, _Font1,
		"Fps: 0", 10, 50, 0.0f, 1.0f, 0.0f);
	if (!result)
	{
		return false;
	}

	// Initial the previous frame fps.
	_previousFps = -1;

	// Create the text objects for the position strings.
	_PositionStrings = new TextClass[6];
	if (!_PositionStrings)
	{
		return false;
	}

	// Initialize the position text strings.
	result = _PositionStrings[0].Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(), screenWidth, screenHeight, 16, false, _Font1,
		"X: 0", 10, 310, 1.0f, 1.0f, 1.0f);
	if (!result)
	{
		return false;
	}

	result = _PositionStrings[1].Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(), screenWidth, screenHeight, 16, false, _Font1,
		"Y: 0", 10, 330, 1.0f, 1.0f, 1.0f);
	if (!result)
	{
		return false;
	}

	result = _PositionStrings[2].Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(), screenWidth, screenHeight, 16, false, _Font1,
		"Z: 0", 10, 350, 1.0f, 1.0f, 1.0f);
	if (!result)
	{
		return false;
	}

	result = _PositionStrings[3].Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(), screenWidth, screenHeight, 16, false, _Font1,
		"rX: 0", 10, 370, 1.0f, 1.0f, 1.0f);
	if (!result)
	{
		return false;
	}

	result = _PositionStrings[4].Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(), screenWidth, screenHeight, 16, false, _Font1,
		"rY: 0", 10, 390, 1.0f, 1.0f, 1.0f);
	if (!result)
	{
		return false;
	}

	result = _PositionStrings[5].Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(), screenWidth, screenHeight, 16, false, _Font1,
		"rZ: 0", 10, 410, 1.0f, 1.0f, 1.0f);
	if (!result)
	{
		return false;
	}

	// Initialize the previous frame position.
	for (int i = 0; i<6; i++)
	{
		_previousPosition[i] = -1;
	}

	// Create the text objects for the render count strings.
	_RenderCountStrings = new TextClass[3];
	if (!_RenderCountStrings)
	{
		return false;
	}

	// Initialize the render count strings.
	result = _RenderCountStrings[0].Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(), screenWidth, screenHeight, 32, false, _Font1, "Polys Drawn: 0", 10, 260, 1.0f, 1.0f, 1.0f);
	if (!result)
	{
		return false;
	}

	result = _RenderCountStrings[1].Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(), screenWidth, screenHeight, 32, false, _Font1, "Cells Drawn: 0", 10, 280, 1.0f, 1.0f, 1.0f);
	if (!result)
	{
		return false;
	}

	result = _RenderCountStrings[2].Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(), screenWidth, screenHeight, 32, false, _Font1, "Cells Culled: 0", 10, 300, 1.0f, 1.0f, 1.0f);
	if (!result)
	{
		return false;
	}
	
	return true;
}

void Graphics::Shutdown() //TODO - Reorder these in proper reverse order of intialization
{
	// Release the refraction render to texture object.
	if (_RefractionTexture)
	{
		_RefractionTexture->Shutdown();
		delete _RefractionTexture;
		_RefractionTexture = 0;
	}

	// Release the water model object.
	if (_WaterModel)
	{
		_WaterModel->Shutdown();
		delete _WaterModel;
		_WaterModel = 0;
	}

	// Release the bath model object.
	if (_BathModel)
	{
		_BathModel->Shutdown();
		delete _BathModel;
		_BathModel = 0;
	}

	// Release the wall model object.
	if (_WallModel)
	{
		_WallModel->Shutdown();
		delete _WallModel;
		_WallModel = 0;
	}

	// Release the ground model object.
	if (_GroundModel)
	{
		_GroundModel->Shutdown();
		delete _GroundModel;
		_GroundModel = 0;
	}

	// Release the floor model object.
	if (_ModelSingle)
	{
		_ModelSingle->Shutdown();
		delete _ModelSingle;
		_ModelSingle = 0;
	}

	// Release the floor model object.
	if (_FloorModel)
	{
		_FloorModel->Shutdown();
		delete _FloorModel;
		_FloorModel = 0;
	}

	// Release the render to texture object.
	if (_ReflectionTexture)
	{
		_ReflectionTexture->Shutdown();
		delete _ReflectionTexture;
		_ReflectionTexture = 0;
	}

	// Release the debug window object.
	if (_DebugWindow)
	{
		_DebugWindow->Shutdown();
		delete _DebugWindow;
		_DebugWindow = 0;
	}

	// Release the render to texture object.
	if (_RenderTexture)
	{
		_RenderTexture->Shutdown();
		delete _RenderTexture;
		_RenderTexture = 0;
	}

	// Release the frustum object.
	if (_Frustum)
	{
		delete _Frustum;
		_Frustum = 0;
	}

	// Release the model list object.
	if (_ModelList)
	{
		_ModelList->Shutdown();
		delete _ModelList;
		_ModelList = 0;
	}
	
	// Release the light object.
	if (_Light)
	{
		delete _Light;
		_Light = 0;
	}

	// Release the model object.
	if (_Model)
	{
		_Model->Shutdown();
		delete _Model;
		_Model = 0;
	}

	// Release the camera object.
	if (_Camera)
	{
		delete _Camera;
		_Camera = 0;
	}

	if (_D3D)
	{
		_D3D->Shutdown();
		delete _D3D;
		_D3D = 0;
	}

	// Release the render count strings.
	if (_RenderCountStrings)
	{
		_RenderCountStrings[0].Shutdown();
		_RenderCountStrings[1].Shutdown();
		_RenderCountStrings[2].Shutdown();

		delete[] _RenderCountStrings;
		_RenderCountStrings = 0;
	}

	// Release the position text strings.
	if (_PositionStrings)
	{
		_PositionStrings[0].Shutdown();
		_PositionStrings[1].Shutdown();
		_PositionStrings[2].Shutdown();
		_PositionStrings[3].Shutdown();
		_PositionStrings[4].Shutdown();
		_PositionStrings[5].Shutdown();

		delete[] _PositionStrings;
		_PositionStrings = 0;
	}

	// Release the video card string.
	if (_VideoStrings)
	{
		_VideoStrings[0].Shutdown();
		_VideoStrings[1].Shutdown();

		delete[] _VideoStrings;
		_VideoStrings = 0;
	}
	
	// Release the fps text string.
	if (_FpsString)
	{
		_FpsString->Shutdown();
		delete _FpsString;
		_FpsString = 0;
	}

	// Release the font object.
	if (_Font1)
	{
		_Font1->Shutdown();
		delete _Font1;
		_Font1 = 0;
	}

	return;
}

bool Graphics::UpdateFrame(float frameTime, World* world, int fps, float camX, float camY, float camZ, float rotX, float rotY, float rotZ)
{
	bool result;

	// 1. Update Effects
	_waterTranslation += 0.001f;
	if (_waterTranslation > 1.0f)
	{_waterTranslation -= 1.0f;	}

	// 2. Update Camera
	_Camera->SetPosition(camX, camY, camZ);
	_Camera->SetRotation(rotX, rotY, rotZ);

	// 3. Update World Actors
	//@TODO: SET ALL MODEL POSITIONS HERE
	
	// 4. Update UI
	result = UpdateFpsString(_D3D->GetDeviceContext(), fps);
	if (!result){return false;}
	result = UpdatePositionStrings(_D3D->GetDeviceContext(), camX, camY, camZ, rotX, rotY, rotZ);
	if (!result){return false;}

	result = DrawFrame(frameTime); if (!result)return false;

	return true;
}

bool Graphics::DrawFrame(float frameTime)
{
	XMFLOAT3 fogColor(.6f, .6f, .6f);	float fogStart = 0.0f;	float fogEnd = 3.f;

//	bool result = RenderToReflection(frameTime);
//	if (!result) { return false; }

//	_D3D->BeginScene(fogColor.x, fogColor.y, fogColor.z, 1.0f);

	// Render the refraction of the scene to a texture.
	bool result = RenderRefractionToTexture();
	if (!result){return false;}

	// Render the reflection of the scene to a texture.
	result = RenderReflectionToTexture();
	if (!result){return false;}

	// Render the scene as normal to the back buffer.
	result = RenderScene(fogStart, fogEnd, frameTime);
	if (!result) { return false; }

	_D3D->TurnZBufferOff();

	// Put the debug window vertex and index buffers on the graphics pipeline to prepare them for drawing.
	//result = _DebugWindow->Render(_D3D->GetDeviceContext(), 80, 50);
	//if (!result) {	return false; }
	//result = _ShaderManager->RenderTextureShader(
	//	_D3D->GetDeviceContext(),
	//	_DebugWindow->GetIndexCount(),
	//	worldMatrix,
	//	baseViewMatrix, //viewMatrix,
	//	orthoMatrix,
	//	_RenderTexture->GetShaderResourceView());
	//if (!result)
	//{
	//	return false;
	//}

	RenderText();

	_D3D->TurnZBufferOn(); // Turn the Z buffer back on now that all 2D rendering has completed.

	_D3D->EndScene(); // Present the rendered scene to the screen.

	return true;
}

bool Graphics::RenderToReflection(float time) // same as rendertotexture in rastertek
{
	XMMATRIX worldMatrix, reflectionViewMatrix, projectionMatrix;
	static float rotation = 0.0f;
	bool result;

	// Set the render target to be the render to texture.
	_RenderTexture->SetRenderTarget(_D3D->GetDeviceContext(), _D3D->GetDepthStencilView());

	// Clear the render to texture.
	_RenderTexture->ClearRenderTarget(_D3D->GetDeviceContext(), _D3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Use the camera to calculate the reflection matrix.
	_Camera->RenderReflection(-1.f); //@TODO - must be same as rendering height
	
	// Get the camera reflection view matrix instead of the normal view matrix.
	reflectionViewMatrix = _Camera->GetReflectionViewMatrix();

	// Get the world and projection matrices.
	_D3D->GetWorldMatrix(worldMatrix);
	_D3D->GetProjectionMatrix(projectionMatrix);

	// Update the rotation variable each frame.
	rotation += (float)XM_PI * 0.001f;
	if (rotation > 360.0f){rotation -= 360.0f;}
	XMMATRIX rotMat = XMMatrixRotationY(rotation);
	worldMatrix = XMMatrixMultiply(worldMatrix, rotMat);

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	_ModelSingle->Render(_D3D->GetDeviceContext());

	// Render the model IMAGE upside down in the reflective surface.
	XMFLOAT3 fogColor(.6f, .6f, .6f);	float fogStart = 0.0f;	float fogEnd = 3.f;
	XMFLOAT4 clipPlane = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	result = _ShaderManager->RenderLightShader(
		_D3D->GetDeviceContext(),
		_Model->GetIndexCount(),
		worldMatrix,
		reflectionViewMatrix,
		projectionMatrix,
		_ModelSingle->GetTextureArray(),
		_Light->GetDirection(),
		_Light->GetAmbientColor(),
		_Light->GetDiffuseColor(),
		_Camera->GetPosition(),
		_Light->GetSpecularColor(),
		_Light->GetSpecularPower(),
		fogStart,
		fogEnd,
		clipPlane,
		0.f,
		.5f);
	if (!result)
	{
		return false;
	}

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	_D3D->SetBackBufferRenderTarget();

	return true;
}

bool Graphics::RenderRefractionToTexture()
{
	XMFLOAT4 clipPlane;
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	// Setup a clipping plane based on the height of the water to clip everything above it.
	clipPlane = XMFLOAT4(0.0f, -1.0f, 0.0f, _waterHeight + 0.1f);

	// Set the render target to be the refraction render to texture.
	_RefractionTexture->SetRenderTarget(_D3D->GetDeviceContext(), _D3D->GetDepthStencilView());

	// Clear the refraction render to texture.
	_RefractionTexture->ClearRenderTarget(_D3D->GetDeviceContext(), _D3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	_Camera->UpdateViewFromPosition();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	_D3D->GetWorldMatrix(worldMatrix);
	_Camera->GetViewMatrix(viewMatrix);
	_D3D->GetProjectionMatrix(projectionMatrix);

	// Translate to where the bath model will be rendered.
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixTranslation(0.0f, 2.0f, 0.0f));

	// Put the bath model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	_BathModel->Render(_D3D->GetDeviceContext());

	// Render the bath model using the light shader.
	result = _ShaderManager->RenderRefractionShader(_D3D->GetDeviceContext(), _BathModel->GetIndexCount(), worldMatrix, viewMatrix,
		projectionMatrix, _BathModel->GetTextureArray()[0], _Light->GetDirection(),
		_Light->GetAmbientColor(), _Light->GetDiffuseColor(), clipPlane);
	if (!result)
	{
		return false;
	}

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	_D3D->SetBackBufferRenderTarget();

	return true;
}

bool Graphics::RenderReflectionToTexture()
{
	XMMATRIX reflectionViewMatrix, worldMatrix, projectionMatrix;
	bool result;
	
	// Set the render target to be the reflection render to texture.
	_ReflectionTexture->SetRenderTarget(_D3D->GetDeviceContext(), _D3D->GetDepthStencilView());

	// Clear the reflection render to texture.
	_ReflectionTexture->ClearRenderTarget(_D3D->GetDeviceContext(), _D3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Use the camera to render the reflection and create a reflection view matrix.
	_Camera->RenderReflection(_waterHeight);

	// Get the camera reflection view matrix instead of the normal view matrix.
	reflectionViewMatrix = _Camera->GetReflectionViewMatrix();

	// Get the world and projection matrices from the d3d object.
	_D3D->GetWorldMatrix(worldMatrix);
	_D3D->GetProjectionMatrix(projectionMatrix);

	// Translate to where the wall model will be rendered.
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixTranslation(0.0f, 6.0f, 8.0f));

	// Put the wall model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	_WallModel->Render(_D3D->GetDeviceContext());

	// Render the wall model using the light shader and the reflection view matrix.
	//result = _ShaderManager->RenderLightShader(_D3D->GetDeviceContext(), _WallModel->GetIndexCount(), worldMatrix, reflectionViewMatrix,
	//	projectionMatrix, _WallModel->GetTextureArrayDDS(), _Light->GetDirection(),
	//	_Light->GetAmbientColor(), _Light->GetDiffuseColor(), _Camera->GetPosition(), _Light->GetSpecularColor(), _Light->GetSpecularPower(), 0, 0, XMFLOAT4(0.0f, 0.f, 0.0f, 0.0f), 0.f, 0.f);

	result = _ShaderManager->RenderLightShader(_D3D->GetDeviceContext(), _WallModel->GetIndexCount(), worldMatrix, reflectionViewMatrix,
		projectionMatrix, _WallModel->GetTextureArray(), _Light->GetDirection(),
		_Light->GetAmbientColor(), _Light->GetDiffuseColor(), _Camera->GetPosition(), _Light->GetSpecularColor(), _Light->GetSpecularPower(), 0, 0, XMFLOAT4(0.0f, 0.f, 0.0f, 0.0f), 0.f, 0.f);
	if (!result)
	{
		return false;
	}


	// Reset the render target back to the original back buffer and not the render to texture anymore.
	_D3D->SetBackBufferRenderTarget();

	return true;
}

bool Graphics::RenderScene(float fogStart, float fogEnd, float frameTime)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, reflectionMatrix;
	bool result;

#pragma region WATER
	// Clear the buffers to begin the scene.
	_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	_Camera->UpdateViewFromPosition();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	_D3D->GetWorldMatrix(worldMatrix);
	_Camera->GetViewMatrix(viewMatrix);
	_D3D->GetProjectionMatrix(projectionMatrix);

	// Translate to where the ground model will be rendered.
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixTranslation(0.0f, 1.0f, 0.0f));

	// Put the ground model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	_GroundModel->Render(_D3D->GetDeviceContext());

	// Render the ground model using the light shader.
	result = _ShaderManager->RenderLightShader(_D3D->GetDeviceContext(), _GroundModel->GetIndexCount(), worldMatrix, viewMatrix,
		projectionMatrix, _GroundModel->GetTextureArray(), _Light->GetDirection(),_Light->GetAmbientColor(), _Light->GetDiffuseColor(),_Camera->GetPosition(), _Light->GetSpecularColor(), _Light->GetSpecularPower(), fogStart, fogEnd, XMFLOAT4(0.0f, 0.f, 0.0f, 0.0f), 0.f, 0.f);
	if (!result)
	{
		return false;
	}

	// Reset the world matrix.
	_D3D->GetWorldMatrix(worldMatrix);

	// Translate to where the wall model will be rendered.
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixTranslation(0.0f, 6.0f, 8.0f));

	// Put the wall model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	_WallModel->Render(_D3D->GetDeviceContext());

	// Render the wall model using the light shader.
	result = _ShaderManager->RenderLightShader(_D3D->GetDeviceContext(), _WallModel->GetIndexCount(), worldMatrix, viewMatrix,
		projectionMatrix, _WallModel->GetTextureArray(),_Light->GetDirection(),
		_Light->GetAmbientColor(), _Light->GetDiffuseColor(),_Camera->GetPosition(), _Light->GetSpecularColor(), _Light->GetSpecularPower(), fogStart, fogEnd, XMFLOAT4(0.0f, 0.f, 0.0f, 0.0f), 0.f, 0.f);
	if (!result)
	{
		return false;
	}

	// Reset the world matrix.
	_D3D->GetWorldMatrix(worldMatrix);

	// Translate to where the bath model will be rendered.
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixTranslation(0.0f, 2.0f, 0.0f));

	// Put the bath model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	_BathModel->Render(_D3D->GetDeviceContext());

	// Render the bath model using the light shader.
	result = _ShaderManager->RenderLightShader(_D3D->GetDeviceContext(), _BathModel->GetIndexCount(), worldMatrix, viewMatrix,
		projectionMatrix, _BathModel->GetTextureArray(), _Light->GetDirection(), _Light->GetAmbientColor(), _Light->GetDiffuseColor(),
		_Camera->GetPosition(), _Light->GetSpecularColor(), _Light->GetSpecularPower(), fogStart, fogEnd, XMFLOAT4(0.0f, 0.f, 0.0f, 0.0f), 0.f, 0.f);

	if (!result)
	{
		return false;
	}

	// Reset the world matrix.
	_D3D->GetWorldMatrix(worldMatrix);

	// Get the camera reflection view matrix.
	reflectionMatrix = _Camera->GetReflectionViewMatrix();

	// Translate to where the water model will be rendered.
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixTranslation(0.0f, _waterHeight, 0.0f));

	// Put the water model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	_WaterModel->Render(_D3D->GetDeviceContext());

	// Render the water model using the water shader.
	result =
		_ShaderManager->RenderWaterShader(_D3D->GetDeviceContext(), _WaterModel->GetIndexCount(), worldMatrix, viewMatrix,
		projectionMatrix, reflectionMatrix, _ReflectionTexture->GetShaderResourceView(), _RefractionTexture->GetShaderResourceView(), 
			_WaterModel->GetTextureArray()[0], _waterTranslation, 0.01f);
	if (!result)
	{
		return false;
	}

	// Present the rendered scene to the screen.
	//_D3D->EndScene();

#pragma endregion

#pragma region SINGLE_REFLECTION
	//_Camera->Render(); // Generate the view matrix based on the camera's position.

 //    ///////////// RENDER SPINNING CUBE ///////////////////
	//_D3D->GetWorldMatrix(worldPosition);
	//_Camera->GetViewMatrix(viewMatrix);
	//_D3D->GetProjectionMatrix(projectionMatrix);

	//// Update the rotation variable each frame.
	//static float rotation = 0.0f;
	//rotation += (float)XM_PI * 0.001f;
	//if (rotation > 360.0f) { rotation -= 360.0f; }
	//XMMATRIX rotMat = XMMatrixRotationY(rotation);
	//worldPosition = XMMatrixMultiply(worldPosition, rotMat);

	//// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	//_ModelSingle->Render(_D3D->GetDeviceContext());

	//XMMATRIX reflectionMatrix = _Camera->GetReflectionViewMatrix();
	//XMFLOAT4 clipPlane = XMFLOAT4(0.0f, 0.f, 0.0f, 0.0f);
	//result = _ShaderManager->RenderLightShader(
	//	_D3D->GetDeviceContext(),
	//	_Model->GetIndexCount(),
	//	worldPosition,
	//	viewMatrix,
	//	projectionMatrix,
	//	_ModelSingle->GetTextureArrayDDS(),
	//	_Light->GetDirection(),
	//	_Light->GetAmbientColor(),
	//	_Light->GetDiffuseColor(), 
	//	_Camera->GetPosition(),
	//	 _Light->GetSpecularColor(),
	//	_Light->GetSpecularPower(),
	//	fogStart,
	//	fogEnd,
	//	clipPlane,
	//	0.f,
	//	.5f);
	//if (!result)
	//{
	//	return false;
	//}
	//
	/////////////// RENDER REFLECTIVE FLOOR ///////////////////
	//// Get the world matrix again and translate down for the floor model to render underneath the cube.
	//_D3D->GetWorldMatrix(worldPosition);
	//XMMATRIX translation = XMMatrixTranslation(0.0f, -1.f, 0.0f);
	//worldPosition = XMMatrixMultiply(worldPosition, translation);

	//// Get the camera reflection view matrix.
	////XMMATRIX reflectionMatrix = _Camera->GetReflectionViewMatrix();

	//// Put the floor model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	//_FloorModel->Render(_D3D->GetDeviceContext());

	//// Render the floor model using the reflection shader, reflection texture, and reflection view matrix.
	//result = _ShaderManager->RenderReflectionShader(_D3D->GetDeviceContext(), _FloorModel->GetIndexCount(), worldPosition, viewMatrix,
	//	projectionMatrix, _FloorModel->GetTextureArrayDDS()[0], _RenderTexture->GetShaderResourceView(),
	//	reflectionMatrix);

#pragma endregion

#pragma region MULTIMODELS
	//textureTranslation += .004f;	if (textureTranslation > 1.0f) { textureTranslation -= 1.0f; }

	//XMFLOAT4 clipPlane = XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f);
	//float blendAmount = 0.5f;

	// Construct the frustum. //@TODO : is this also needed for reflection?
	//_Frustum->ConstructFrustum(SCREEN_DEPTH, projectionMatrix, viewMatrix);

	//float positionX, positionY, positionZ, radius;
	//XMFLOAT4 color;
	//bool renderModel;

	// Get the number of models that will be rendered.
	//int modelCount = _ModelList->GetModelCount();

	// Initialize the count of models that have been rendered.
	//int renderCount = 0;

	//XMMATRIX viewMatLocal;
	//_Camera->GetViewMatrix(viewMatLocal);

	//_D3D->EnableAlphaBlending();

	// Go through all the models and render them only if they can be seen by the camera view.
	//for (int index = 0; index < modelCount; index++)
	//{
	//	// Get the position and color of the sphere model at this index.
	//	_ModelList->GetData(index, positionX, positionY, positionZ, color);

	//	// Set the radius of the sphere to 1.0 since this is already known.
	//	radius = 1.0f;

	//	// Check if the sphere model is in the view frustum.
	//	renderModel = _Frustum->CheckSphere(positionX, positionY, positionZ, radius);

	//	// If it can be seen then render it, if not skip this model and check the next sphere.
	//	if (renderModel)
	//	{
	//		//Move the model to the location it should be rendered at.
	//		worldPosition = DirectX::XMMatrixTranslation(positionX, positionY, positionZ);

	//		// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	//		_Model->Render(_D3D->GetDeviceContext());

	//		result = _ShaderManager->RenderLightShader(
	//			_D3D->GetDeviceContext(),
	//			_Model->GetIndexCount(),
	//			worldPosition,
	//			viewMatLocal,//viewMatrix,
	//			projectionMatrix,
	//			_Model->GetTextureArrayDDS(),
	//			_Light->GetDirection(),
	//			/*color,*/ _Light->GetAmbientColor(),
	//			color, //_Light->GetDiffuseColor(), 
	//			_Camera->GetPosition(),
	//			/*color,*/ _Light->GetSpecularColor(),
	//			_Light->GetSpecularPower(),
	//			fogStart,
	//			fogEnd,
	//			clipPlane,
	//			textureTranslation,
	//			blendAmount,
	//			_Model->GetTextureArrayDDS()[0], //@TODO: must fix
	//			_Camera->GetReflectionViewMatrix());//viewMatrix); // @TODO: 
	//		if (!result)
	//		{
	//			return false;
	//		}

	//		// Reset to the original world matrix.
	//		_D3D->GetWorldMatrix(worldPosition);

	//		// Since this model was rendered then increase the count for this frame.
	//		renderCount++;
	//	}
	//}

	//_D3D->DisableAlphaBlending();

#pragma endregion

	return true;
}

//bool Graphics::RenderToTexture(float frameTime)
//{
//	// Set the render target to be the render to texture.
//	_RenderTexture->SetRenderTarget(_D3D->GetDeviceContext(), _D3D->GetDepthStencilView());
//
//	// Clear the render to texture.
//	_RenderTexture->ClearRenderTarget(_D3D->GetDeviceContext(), _D3D->GetDepthStencilView(), 0.0f, 0.0f, 1.0f, 1.0f);
//
//	// Render the scene now and it will draw to the render to texture instead of the back buffer.
//	XMMATRIX viewMatrix;
//	_Camera->GetViewMatrix(viewMatrix);
//	bool result = RenderScene(viewMatrix, 0.f,10.f, frameTime); //@REFACTOR later
//	if (!result)
//	{
//		return false;
//	}
//
//	// Reset the render target back to the original back buffer and not the render to texture anymore.
//	_D3D->SetBackBufferRenderTarget();
//
//	return result;
//}

void Graphics::RenderText(/*const DirectX::XMMATRIX &worldMatrix, const DirectX::XMMATRIX &baseViewMatrix, const DirectX::XMMATRIX &orthoMatrix*/)
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	_D3D->GetWorldMatrix(worldMatrix);
	_Camera->GetBaseViewMatrix(baseViewMatrix);
	_D3D->GetOrthoMatrix(orthoMatrix);

	// Turn on the alpha blending before rendering the text.
	_D3D->EnableAlphaBlending();

	// Render the fps string.
	_FpsString->Render(_D3D->GetDeviceContext(), _ShaderManager,/*_FontShader),*/ worldMatrix, /*viewMatrix*/baseViewMatrix, orthoMatrix, _Font1->GetTexture());
	// Render the position and rotation strings.
	for (int i = 0; i<6; i++)
	{
		_PositionStrings[i].Render(_D3D->GetDeviceContext(), _ShaderManager/*_FontShader*/, worldMatrix, /*viewMatrix*/baseViewMatrix, orthoMatrix, _Font1->GetTexture());
	}
	// Render the render count strings.
	for (int i = 0; i<3; i++)
	{
		_RenderCountStrings[i].Render(_D3D->GetDeviceContext(), _ShaderManager/*_FontShader*/, worldMatrix, /*viewMatrix*/baseViewMatrix, orthoMatrix, _Font1->GetTexture());
	}

	// Turn off alpha blending after rendering the text.
	_D3D->DisableAlphaBlending();
}

bool Graphics::UpdateFpsString(ID3D11DeviceContext* deviceContext, int fps)
{
	char tempString[16];
	char finalString[16];
	float red, green, blue;
	bool result;


	// Check if the fps from the previous frame was the same, if so don't need to update the text string.
	if (_previousFps == fps)
	{
		return true;
	}

	// Store the fps for checking next frame.
	_previousFps = fps;

	// Truncate the fps to below 100,000.
	if (fps > 99999)
	{
		fps = 99999;
	}

	// Convert the fps integer to string format.
	_itoa_s(fps, tempString, 10);

	// Setup the fps string.
	strcpy_s(finalString, "Fps: ");
	strcat_s(finalString, tempString);

	// If fps is 60 or above set the fps color to green.
	if (fps >= 60)
	{
		red = 0.0f;
		green = 1.0f;
		blue = 0.0f;
	}

	// If fps is below 60 set the fps color to yellow.
	if (fps < 60)
	{
		red = 1.0f;
		green = 1.0f;
		blue = 0.0f;
	}

	// If fps is below 30 set the fps color to red.
	if (fps < 30)
	{
		red = 1.0f;
		green = 0.0f;
		blue = 0.0f;
	}

	// Update the sentence vertex buffer with the new string information.
	result = _FpsString->UpdateSentence(deviceContext, _Font1, finalString, 10, 50, red, green, blue);
	if (!result)
	{
		return false;
	}

	return true;
}

bool Graphics::UpdatePositionStrings(ID3D11DeviceContext* deviceContext, float posX, float posY, float posZ,
	float rotX, float rotY, float rotZ)
{
	int positionX, positionY, positionZ, rotationX, rotationY, rotationZ;
	char tempString[16];
	char finalString[16];
	bool result;


	// Convert the float values to integers.
	positionX = (int)posX;
	positionY = (int)posY;
	positionZ = (int)posZ;
	rotationX = (int)rotX;
	rotationY = (int)rotY;
	rotationZ = (int)rotZ;

	// Update the position strings if the value has changed since the last frame.
	if (positionX != _previousPosition[0])
	{
		_previousPosition[0] = positionX;
		_itoa_s(positionX, tempString, 10);
		strcpy_s(finalString, "X: ");
		strcat_s(finalString, tempString);
		result = _PositionStrings[0].UpdateSentence(deviceContext, _Font1, finalString, 10, 100, 1.0f, 1.0f, 1.0f);
		if (!result) { return false; }
	}

	if (positionY != _previousPosition[1])
	{
		_previousPosition[1] = positionY;
		_itoa_s(positionY, tempString, 10);
		strcpy_s(finalString, "Y: ");
		strcat_s(finalString, tempString);
		result = _PositionStrings[1].UpdateSentence(deviceContext, _Font1, finalString, 10, 120, 1.0f, 1.0f, 1.0f);
		if (!result) { return false; }
	}

	if (positionZ != _previousPosition[2])
	{
		_previousPosition[2] = positionZ;
		_itoa_s(positionZ, tempString, 10);
		strcpy_s(finalString, "Z: ");
		strcat_s(finalString, tempString);
		result = _PositionStrings[2].UpdateSentence(deviceContext, _Font1, finalString, 10, 140, 1.0f, 1.0f, 1.0f);
		if (!result) { return false; }
	}

	if (rotationX != _previousPosition[3])
	{
		_previousPosition[3] = rotationX;
		_itoa_s(rotationX, tempString, 10);
		strcpy_s(finalString, "rX: ");
		strcat_s(finalString, tempString);
		result = _PositionStrings[3].UpdateSentence(deviceContext, _Font1, finalString, 10, 180, 1.0f, 1.0f, 1.0f);
		if (!result) { return false; }
	}

	if (rotationY != _previousPosition[4])
	{
		_previousPosition[4] = rotationY;
		_itoa_s(rotationY, tempString, 10);
		strcpy_s(finalString, "rY: ");
		strcat_s(finalString, tempString);
		result = _PositionStrings[4].UpdateSentence(deviceContext, _Font1, finalString, 10, 200, 1.0f, 1.0f, 1.0f);
		if (!result) { return false; }
	}

	if (rotationZ != _previousPosition[5])
	{
		_previousPosition[5] = rotationZ;
		_itoa_s(rotationZ, tempString, 10);
		strcpy_s(finalString, "rZ: ");
		strcat_s(finalString, tempString);
		result = _PositionStrings[5].UpdateSentence(deviceContext, _Font1, finalString, 10, 220, 1.0f, 1.0f, 1.0f);
		if (!result) { return false; }
	}

	return true;
}


bool Graphics::UpdateRenderCounts(ID3D11DeviceContext* deviceContext, int renderCount, int nodesDrawn, int nodesCulled)
{
	char tempString[32];
	char finalString[32];
	bool result;


	// Convert the render count integer to string format.
	_itoa_s(renderCount, tempString, 10);

	// Setup the render count string.
	strcpy_s(finalString, "Polys Drawn: ");
	strcat_s(finalString, tempString);

	// Update the sentence vertex buffer with the new string information.
	result = _RenderCountStrings[0].UpdateSentence(deviceContext, _Font1, finalString, 10, 260, 1.0f, 1.0f, 1.0f);
	if (!result)
	{
		return false;
	}

	// Convert the cells drawn integer to string format.
	_itoa_s(nodesDrawn, tempString, 10);

	// Setup the cells drawn string.
	strcpy_s(finalString, "Cells Drawn: ");
	strcat_s(finalString, tempString);

	// Update the sentence vertex buffer with the new string information.
	result = _RenderCountStrings[1].UpdateSentence(deviceContext, _Font1, finalString, 10, 280, 1.0f, 1.0f, 1.0f);
	if (!result)
	{
		return false;
	}

	// Convert the cells culled integer to string format.
	_itoa_s(nodesCulled, tempString, 10);

	// Setup the cells culled string.
	strcpy_s(finalString, "Cells Culled: ");
	strcat_s(finalString, tempString);

	// Update the sentence vertex buffer with the new string information.
	result = _RenderCountStrings[2].UpdateSentence(deviceContext, _Font1, finalString, 10, 300, 1.0f, 1.0f, 1.0f);
	if (!result)
	{
		return false;
	}

	return true;
}