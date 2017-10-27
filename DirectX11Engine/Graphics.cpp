////////////////////////////////////////////////////////////////////////////////
// Filename: Graphics.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Graphics.h"

Graphics::Graphics()
	:
	_D3D(nullptr),
	_Camera(nullptr),
	_Model(nullptr),
	_ColorShader(nullptr)
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
	_Camera->SetPosition(0.0f, 0.0f, -8.f);

	// Create the model object.
	_Model = new Model;
	if (!_Model)
	{
		return false;
	}

	// Initialize the model object.
	result = _Model->Initialize(_D3D->GetDevice());
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// Create the color shader object.
	_ColorShader = new ColorShaderClass;
	if (!_ColorShader)
	{
		return false;
	}

	// Initialize the color shader object.
	result = _ColorShader->Initialize(_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}

void Graphics::Shutdown()
{
	// Release the color shader object.
	if (_ColorShader)
	{
		_ColorShader->Shutdown();
		delete _ColorShader;
		_ColorShader = 0;
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

	// Render the graphics scene.
	result = Render();
	if (!result)
	{
		return false;
	}

	return true;
}

bool Graphics::Render()
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

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	_Model->Render(_D3D->GetDeviceContext());

	// Render the model using the color shader.
	result = _ColorShader->Render(_D3D->GetDeviceContext(), _Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}

	// Present the rendered scene to the screen.
	_D3D->EndScene();

	return true;
}