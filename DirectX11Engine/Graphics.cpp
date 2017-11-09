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
	_Text(nullptr),
	_Model(nullptr),
	_LightShader(nullptr),
	_Light(nullptr),
	_ModelList(nullptr),
	_Frustum(nullptr)
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
	XMMATRIX baseViewMatrix;

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
	_Camera->Render(); //@TODO: why called again here?
	_Camera->GetViewMatrix(baseViewMatrix); // needed for text class

	// Create the text object.
	_Text = new TextClass;
	if (!_Text)
	{
		return false;
	}

	// Initialize the text object.
	result = _Text->Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(), hwnd, screenWidth, screenHeight, baseViewMatrix);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the text object.", L"Error", MB_OK);
		return false;
	}

	// Create the model object.
	_Model = new Model;
	if (!_Model)
	{
		return false;
	}

	// Initialize the model object.
	//result = _Model->Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(), "../DirectX11Engine/data/stone01.tga");
	result = _Model->Initialize(
			_D3D->GetDevice(), 
			_D3D->GetDeviceContext(), 
			"../DirectX11Engine/data/sphere.txt", 
			"../DirectX11Engine/data/fire2.tga");
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
	_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	_Light->SetDiffuseColor(1.0f, 0.5f, 1.0f, 1.0f);
	_Light->SetDirection(1.0f, 0.0f, 0.0f);
	_Light->SetDirection(0.0f, 0.0f, 1.0f);
	_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	_Light->SetSpecularPower(30.0f); // the lower the power, the higher the effect intensity

	// Create the model list object.
	_ModelList = new ModelListClass;
	if (!_ModelList)
	{
		return false;
	}

	// Initialize the model list object.
	result = _ModelList->Initialize(100);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model list object.", L"Error", MB_OK);
		return false;
	}

	// Create the frustum object.
	_Frustum = new FrustumClass;
	if (!_Frustum)
	{
		return false;
	}

	return true;
}

void Graphics::Shutdown()
{
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

	// Release the text object.
	if (_Text)
	{
		_Text->Shutdown();
		delete _Text;
		_Text = 0;
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

bool Graphics::Frame(float rotationY, int mouseX, int mouseY, int fps, int cpu, float frameTime)
{
	bool result;

	// @TODO: update textclass according to tutorial 15 after making it
	// Set the frames per second.
	result = _Text->SetFps(fps, _D3D->GetDeviceContext());
	if (!result)
	{
		return false;
	}

	// Set the cpu usage.
	result = _Text->SetCpu(cpu, _D3D->GetDeviceContext());
	if (!result)
	{
		return false;
	}

	// Set the location of the mouse.
	result = _Text->SetMousePosition(mouseX, mouseY, _D3D->GetDeviceContext());
	if (!result)
	{
		return false;
	}

	// Set the position of the camera.
	_Camera->SetPosition(0.0f, 0.0f, -10.0f);

	// Set the rotation of the camera.
	_Camera->SetRotation(0.0f, rotationY, 0.0f);

	// Update the rotation variable each frame.
	//_modelRotation += (float)XM_PI * 0.0003f;
	//if (_modelRotation > 360.0f)
	//{
	//	_modelRotation -= 360.0f;
	//}

	// @DEBUG why do they now disable rendering inside frame?
	// Render the graphics scene.
	//result = Render(/*_modelRotation*/);
	//if (!result)
	//{
	//	return false;
	//}

	return true;
}

bool Graphics::Render()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix; //@NEW
	int modelCount, renderCount, index;
	float positionX, positionY, positionZ, radius;
	XMFLOAT4 color;
	bool renderModel, result;

	// Clear the buffers to begin the scene.
	_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	_D3D->GetWorldMatrix(worldMatrix);
	_Camera->GetViewMatrix(viewMatrix);
	_D3D->GetProjectionMatrix(projectionMatrix);
	_D3D->GetOrthoMatrix(orthoMatrix); //@NEW

	// Construct the frustum.
	_Frustum->ConstructFrustum(SCREEN_DEPTH, projectionMatrix, viewMatrix);

	// Get the number of models that will be rendered.
	modelCount = _ModelList->GetModelCount();

	// Initialize the count of models that have been rendered.
	renderCount = 0;

	// Go through all the models and render them only if they can be seen by the camera view.
	for (index = 0; index < modelCount; index++)
	{
		// Get the position and color of the sphere model at this index.
		_ModelList->GetData(index, positionX, positionY, positionZ, color);

		// Set the radius of the sphere to 1.0 since this is already known.
		radius = 1.0f;

		// Check if the sphere model is in the view frustum.
		renderModel = _Frustum->CheckSphere(positionX, positionY, positionZ, radius);

		// If it can be seen then render it, if not skip this model and check the next sphere.
		if (renderModel)
		{
			//Rotate the world matrix by the rotation value so that the triangle will spin.
			worldMatrix = DirectX::XMMatrixRotationY(_modelRotation);
			//Move the model to the location it should be rendered at.
			worldMatrix = DirectX::XMMatrixTranslation(positionX, positionY, positionZ);

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
				_Light->GetAmbientColor(),
				_Light->GetDiffuseColor(),
				_Camera->GetPosition(),
				_Light->GetSpecularColor(),
				_Light->GetSpecularPower());
			if (!result)
			{
				return false;
			}

			// Reset to the original world matrix.
			_D3D->GetWorldMatrix(worldMatrix);

			// Since this model was rendered then increase the count for this frame.
			renderCount++;
		}
	}
	
	// Set the number of models that was actually rendered this frame.
	result = _Text->SetRenderCount(renderCount, _D3D->GetDeviceContext());
	if (!result)
	{
		return false;
	}

	// @DEBUG @CUSTOM RENDER THE NEW 2D OVERLAY HERE
	// Turn off the Z buffer to begin all 2D rendering.
	_D3D->TurnZBufferOff();

	// Turn on the alpha blending before rendering the text.
	_D3D->TurnOnAlphaBlending();

	// Render the text strings.
	result = _Text->Render(_D3D->GetDeviceContext(), worldMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}

	// Turn off alpha blending after rendering the text.
	_D3D->TurnOffAlphaBlending();

	// Turn the Z buffer back on now that all 2D rendering has completed.
	_D3D->TurnZBufferOn();

	// Present the rendered scene to the screen.
	_D3D->EndScene();

	return true;
}