////////////////////////////////////////////////////////////////////////////////
// Filename: Graphics.cpp
////////////////////////////////////////////////////////////////////////////////
#include "lightshaderclass.h"
#include "lightclass.h"
#include "Graphics.h"

Graphics::Graphics()
	:
	_D3D(nullptr),
	_Camera(nullptr),
	_Model(nullptr),
	_LightShader(0),
	_Light(0)
{}

Graphics::Graphics(const Graphics& other)
{
}

Graphics::~Graphics()
{
}

bool Graphics::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

	// Create the Direct3D object.
	_D3D = new D3DClass;
	if (!_D3D)
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = _D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	// Create the camera object.
	_Camera = new Camera;
	if (!_Camera)
	{
		return false;
	}

	// Set the initial position of the camera.
	_Camera->SetPosition(0.0f, 0.0f, -4.f);

	// Create the model object.
	_Model = new Model;
	if (!_Model)
	{
		return false;
	}

	// Initialize the model object.
	//result = _Model->Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(), "../DirectX11Engine/data/stone01.tga");
	result = _Model->Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(), "../DirectX11Engine/data/cube.txt", "../Engine/data/seafloor.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// Create the light shader object.
	_LightShader = new LightShaderClass;
	if (!_LightShader)
	{
		return false;
	}

	// Initialize the light shader object.
	result = _LightShader->Initialize(_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the light object.
	_Light = new LightClass;
	if (!_Light)
	{
		return false;
	}

	// Initialize the light object.
	_Light->SetDiffuseColor(1.0f, 0.5f, 1.0f, 1.0f);
	_Light->SetDirection(0.0f, 0.0f, 1.0f);

	//// Create the color shader object.
	//_TextureShader = new TextureShaderClass;
	//if (!_TextureShader)
	//{
	//	return false;
	//}
	//// Initialize the color shader object.
	//result = _TextureShader->Initialize(_D3D->GetDevice(), hwnd);
	//if (!result)
	//{
	//	MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
	//	return false;
	//}

	return true;
}

void Graphics::Shutdown()
{
	//// Release the color shader object.
	//if (_TextureShader)
	//{
	//	_TextureShader->Shutdown();
	//	delete _TextureShader;
	//	_TextureShader = 0;
	//}

	// Release the light object.
	if (_Light)
	{
		delete _Light;
		_Light = 0;
	}

	// Release the light shader object.
	if (_LightShader)
	{
		_LightShader->Shutdown();
		delete _LightShader;
		_LightShader = 0;
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

	return;
}

bool Graphics::Frame()
{
	bool result;

	// Update the rotation variable each frame.
	_modelRotation += (float)XM_PI * 0.01f;
	if (_modelRotation > 360.0f)
	{
		_modelRotation -= 360.0f;
	}

	// Render the graphics scene.
	result = Render(_modelRotation);
	if (!result)
	{
		return false;
	}

	return true;
}

bool Graphics::Render(float lightRotation)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	// Clear the buffers to begin the scene.
	_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	_D3D->GetWorldMatrix(worldMatrix);
	_Camera->GetViewMatrix(viewMatrix);
	_D3D->GetProjectionMatrix(projectionMatrix);

	// Rotate the world matrix by the rotation value so that the triangle will spin.
	worldMatrix = DirectX::XMMatrixRotationY(_modelRotation);

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	_Model->Render(_D3D->GetDeviceContext());

	// Render the model using the color shader.
	result = _LightShader->Render(
			_D3D->GetDeviceContext(), 
			_Model->GetIndexCount(), 
			worldMatrix, 
			viewMatrix, 
			projectionMatrix,
			_Model->GetTexture(),
			_Light->GetDirection(), 
			_Light->GetDiffuseColor());
	if (!result)
	{
		return false;
	}

	// Present the rendered scene to the screen.
	_D3D->EndScene();

	return true;
}