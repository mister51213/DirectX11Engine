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
	_LightShader(nullptr),
	_Light(nullptr),
	_ModelList(nullptr),
	_Frustum(nullptr),
	m_Font1(nullptr),
	m_FpsString(nullptr),
	m_VideoStrings(nullptr),
	m_PositionStrings(nullptr),
	m_RenderCountStrings(nullptr),
	_RenderTexture(nullptr),
	_DebugWindow(nullptr),
	_TextureShader(nullptr)
{}

Graphics::Graphics(const Graphics& other)
{
}

Graphics::~Graphics()
{
}

bool Graphics::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	////////////////
	// UI RELATED //
	////////////////
	bool result;
	char videoCard[128];
	int videoMemory;
	char videoString[144];
	char memoryString[32];
	char tempString[16];
	////////////////
	// UI RELATED //
	////////////////

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
	_Camera->Render();
	//_Camera->RenderBaseViewMatrix();
	//_Camera->GetViewMatrix(baseViewMatrix); // needed for text class

	////////////////
	// UI RELATED //
	////////////////
	// Create the render to texture object.
	_RenderTexture = new RenderTextureClass;
	if (!_RenderTexture)
	{
		return false;
	}

	// Initialize the render to texture object.
	result = _RenderTexture->Initialize(_D3D->GetDevice(), screenWidth, screenHeight);
	if (!result)
	{
		return false;
	}

	// Create the debug window object.
	_DebugWindow = new DebugWindowClass;
	if (!_DebugWindow)
	{
		return false;
	}

	// Initialize the debug window object.
	result = _DebugWindow->Initialize(_D3D->GetDevice(), screenWidth, screenHeight, 100, 100);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the debug window object.", L"Error", MB_OK);
		return false;
	}

	// Create the texture shader object.
	_TextureShader = new TextureShaderClass;
	if (!_TextureShader)
	{
		return false;
	}

	// Initialize the texture shader object.
	result = _TextureShader->Initialize(_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}


	// Create the first font object.
	m_Font1 = new FontClass;
	if (!m_Font1)
	{
		return false;
	}

	// Initialize the first font object.
	result = m_Font1->Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(), "../DirectX11Engine/data/font.txt",
		"../DirectX11Engine/data/font.tga", 32.0f, 3);
	if (!result)
	{
		return false;
	}

	// Create the font shader object.
	_FontShader = new FontShaderClass;
	if (!_FontShader)
	{
		return false;
	}
	
	// Initialize the font shader object.
	result = _FontShader->Initialize(_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the font shader object.", L"Error", MB_OK);
		return false;
	}//@TODO: release this object

	// Create the text object for the fps string.
	m_FpsString = new TextClass;
	if (!m_FpsString)
	{
		return false;
	}

	// Initialize the fps text string.
	result = m_FpsString->Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(), screenWidth, screenHeight, 16, false, m_Font1,
		"Fps: 0", 10, 50, 0.0f, 1.0f, 0.0f);
	if (!result)
	{
		return false;
	}

	// Initial the previous frame fps.
	m_previousFps = -1;

	// Create the text objects for the position strings.
	m_PositionStrings = new TextClass[6];
	if (!m_PositionStrings)
	{
		return false;
	}

	// Initialize the position text strings.
	result = m_PositionStrings[0].Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(), screenWidth, screenHeight, 16, false, m_Font1,
		"X: 0", 10, 310, 1.0f, 1.0f, 1.0f);
	if (!result)
	{
		return false;
	}

	result = m_PositionStrings[1].Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(), screenWidth, screenHeight, 16, false, m_Font1,
		"Y: 0", 10, 330, 1.0f, 1.0f, 1.0f);
	if (!result)
	{
		return false;
	}

	result = m_PositionStrings[2].Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(), screenWidth, screenHeight, 16, false, m_Font1,
		"Z: 0", 10, 350, 1.0f, 1.0f, 1.0f);
	if (!result)
	{
		return false;
	}

	result = m_PositionStrings[3].Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(), screenWidth, screenHeight, 16, false, m_Font1,
		"rX: 0", 10, 370, 1.0f, 1.0f, 1.0f);
	if (!result)
	{
		return false;
	}

	result = m_PositionStrings[4].Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(), screenWidth, screenHeight, 16, false, m_Font1,
		"rY: 0", 10, 390, 1.0f, 1.0f, 1.0f);
	if (!result)
	{
		return false;
	}

	result = m_PositionStrings[5].Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(), screenWidth, screenHeight, 16, false, m_Font1,
		"rZ: 0", 10, 410, 1.0f, 1.0f, 1.0f);
	if (!result)
	{
		return false;
	}

	// Initialize the previous frame position.
	for (int i = 0; i<6; i++)
	{
		m_previousPosition[i] = -1;
	}

	// Create the text objects for the render count strings.
	m_RenderCountStrings = new TextClass[3];
	if (!m_RenderCountStrings)
	{
		return false;
	}

	// Initialize the render count strings.
	result = m_RenderCountStrings[0].Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(), screenWidth, screenHeight, 32, false, m_Font1,	"Polys Drawn: 0", 10, 260, 1.0f, 1.0f, 1.0f);
	if (!result)
	{
		return false;
	}

	result = m_RenderCountStrings[1].Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(), screenWidth, screenHeight, 32, false, m_Font1,	"Cells Drawn: 0", 10, 280, 1.0f, 1.0f, 1.0f);
	if (!result)
	{
		return false;
	}

	result = m_RenderCountStrings[2].Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(), screenWidth, screenHeight, 32, false, m_Font1,	"Cells Culled: 0", 10, 300, 1.0f, 1.0f, 1.0f);
	if (!result)
	{
		return false;
	}

	////////////////
	// UI RELATED //
	////////////////

	// Create the model object.
	_Model = new Model;
	if (!_Model)
	{
		return false;
	}

	// Initialize the model object.
	result = _Model->Initialize(
		_D3D->GetDevice(),
		_D3D->GetDeviceContext(),
		"../DirectX11Engine/data/sphere.txt",
			"../DirectX11Engine/data/stone.tga", // tex1
			"../DirectX11Engine/data/dirt.tga", // tex2
				"../DirectX11Engine/data/light.tga", // lightmap
		"../DirectX11Engine/data/alpha.tga", // alpha
			"../DirectX11Engine/data/bumpMap.tga", // normal map
		"../DirectX11Engine/data/specMap.tga"); // specMap
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
	_Light->SetAmbientColor(.2f, .2f, .2f, 1.0f);
	_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	_Light->SetDirection(0.0f, 0.0f, 1.0f);
	_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	_Light->SetSpecularPower(16.0f); // the lower the power, the higher the effect intensity

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

void Graphics::Shutdown() //TODO - Reorder these in proper reverse order of intialization
{
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

	// Release the light shader object.
	if (_LightShader)
	{
		_LightShader->Shutdown();
		delete _LightShader;
		_LightShader = 0;
	}

	// Release the light shader object.
	if (_FontShader)
	{
		_FontShader->Shutdown();
		delete _FontShader;
		_FontShader = 0;
	}
	
	// Release the texture shader object.
	if (_TextureShader)
	{
		_TextureShader->Shutdown();
		delete _TextureShader;
		_TextureShader = 0;
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
	if (m_RenderCountStrings)
	{
		m_RenderCountStrings[0].Shutdown();
		m_RenderCountStrings[1].Shutdown();
		m_RenderCountStrings[2].Shutdown();

		delete[] m_RenderCountStrings;
		m_RenderCountStrings = 0;
	}

	// Release the position text strings.
	if (m_PositionStrings)
	{
		m_PositionStrings[0].Shutdown();
		m_PositionStrings[1].Shutdown();
		m_PositionStrings[2].Shutdown();
		m_PositionStrings[3].Shutdown();
		m_PositionStrings[4].Shutdown();
		m_PositionStrings[5].Shutdown();

		delete[] m_PositionStrings;
		m_PositionStrings = 0;
	}

	// Release the video card string.
	if (m_VideoStrings)
	{
		m_VideoStrings[0].Shutdown();
		m_VideoStrings[1].Shutdown();

		delete[] m_VideoStrings;
		m_VideoStrings = 0;
	}


	// Release the fps text string.
	if (m_FpsString)
	{
		m_FpsString->Shutdown();
		delete m_FpsString;
		m_FpsString = 0;
	}

	// Release the font object.
	if (m_Font1)
	{
		m_Font1->Shutdown();
		delete m_Font1;
		m_Font1 = 0;
	}

	return;
}

bool Graphics::Frame(float frameTime, int fps, float posX, float posY, float posZ, float rotX, float rotY, float rotZ)
{
	bool result;

	// Set the position of the camera.
	_Camera->SetPosition(posX, posY, posZ);

	// Set the rotation of the camera.
	_Camera->SetRotation(rotX, rotY/*rotationY*/, rotZ);

	if (Camera* cam = GetCamera())
	{
		_Camera->Tick();
	}

	// Update the rotation variable each frame.
	_modelRotation += (float)XM_PI * 0.0003f;
	if (_modelRotation > 360.0f)
	{
		_modelRotation -= 360.0f;
	}

	// Update the fps string. //@TODO
	result = UpdateFpsString(_D3D->GetDeviceContext(), fps);
	if (!result)
	{
		return false;
	}

	// Update the position strings.
	result = UpdatePositionStrings(_D3D->GetDeviceContext(), posX, posY, posZ, rotX, rotY, rotZ);
	if (!result)
	{
		return false;
	}

	// @DEBUG why do they now disable rendering inside frame?
	// Render the graphics scene.
	result = Render(frameTime/*_modelRotation*/);
	if (!result)
	{
		return false;
	}

	return true;
}

bool Graphics::Render(float frameTime)
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;
	
	XMFLOAT3 fogColor(0.5f, 0.3f, 0.3f);
	float fogStart = 0.0f;
	float fogEnd = 3.f;

	bool result;

	// Render the entire scene to the texture first.
	result = RenderToTexture(frameTime);
	if (!result)
	{
		return false;
	}

	// Clear the buffers to begin the scene.
	//_D3D->BeginScene(0.3f, 0.3f, 0.3f, 1.0f);
	_D3D->BeginScene(fogColor.x, fogColor.y, fogColor.z, 1.0f);

	// Render the scene as normal to the back buffer.
	result = RenderScene(fogStart, fogEnd, frameTime);
	if (!result)
	{
		return false;
	}

	// Turn off the Z buffer to begin all 2D rendering.
	_D3D->TurnZBufferOff();

	_D3D->GetWorldMatrix(worldMatrix);
	_Camera->GetBaseViewMatrix(baseViewMatrix);
	_D3D->GetOrthoMatrix(orthoMatrix);

	// Put the debug window vertex and index buffers on the graphics pipeline to prepare them for drawing.
	result = _DebugWindow->Render(_D3D->GetDeviceContext(), 80, 50);
	if (!result)
	{
		return false;
	}

	// Render the debug window using the texture shader.
	result = _TextureShader->Render(
				_D3D->GetDeviceContext(), 
				_DebugWindow->GetIndexCount(), 
				worldMatrix, 
				baseViewMatrix, //viewMatrix,
				orthoMatrix, 
				_RenderTexture->GetShaderResourceView());
	if (!result)
	{
		return false;
	}

	RenderText(worldMatrix, baseViewMatrix, orthoMatrix);

	// Turn the Z buffer back on now that all 2D rendering has completed.
	_D3D->TurnZBufferOn();

	// Present the rendered scene to the screen.
	_D3D->EndScene();

	return true;
}

bool Graphics::RenderToTexture(float frameTime)
{
	// Set the render target to be the render to texture.
	_RenderTexture->SetRenderTarget(_D3D->GetDeviceContext(), _D3D->GetDepthStencilView());

	// Clear the render to texture.
	_RenderTexture->ClearRenderTarget(_D3D->GetDeviceContext(), _D3D->GetDepthStencilView(), 0.0f, 0.0f, 1.0f, 1.0f);

	// Render the scene now and it will draw to the render to texture instead of the back buffer.
	bool result = RenderScene(0.f,10.f, frameTime); //@REFACTOR later
	if (!result)
	{
		return false;
	}

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	_D3D->SetBackBufferRenderTarget();


	return result;
}

void Graphics::RenderText(const DirectX::XMMATRIX &worldMatrix, const DirectX::XMMATRIX &baseViewMatrix, const DirectX::XMMATRIX &orthoMatrix)
{
	// Turn on the alpha blending before rendering the text.
	_D3D->EnableAlphaBlending();

	// Render the fps string.
	m_FpsString->Render(_D3D->GetDeviceContext(), _FontShader, worldMatrix, /*viewMatrix*/baseViewMatrix, orthoMatrix, m_Font1->GetTexture());
	// Render the position and rotation strings.
	for (int i = 0; i<6; i++)
	{
		m_PositionStrings[i].Render(_D3D->GetDeviceContext(), _FontShader, worldMatrix, /*viewMatrix*/baseViewMatrix, orthoMatrix, m_Font1->GetTexture());
	}
	// Render the render count strings.
	for (int i = 0; i<3; i++)
	{
		m_RenderCountStrings[i].Render(_D3D->GetDeviceContext(), _FontShader, worldMatrix, /*viewMatrix*/baseViewMatrix, orthoMatrix, m_Font1->GetTexture());
	}

	// Turn off alpha blending after rendering the text.
	_D3D->DisableAlphaBlending();
}

bool Graphics::RenderScene(float fogStart, float fogEnd, float frameTime)
{
	XMMATRIX worldPosition, viewMatrix, projectionMatrix;
	// Setup a clipping plane.
	XMFLOAT4 clipPlane = XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f);

	int modelCount, renderCount, index;
	float positionX, positionY, positionZ, radius;
	XMFLOAT4 color;
	bool renderModel, result;
	
	// Tex translation
	//static float textureTranslation = 0.f;
	textureTranslation += .004f;
	//textureTranslation += 0.08f * frameTime;
	if (textureTranslation > 1.0f)
	{
		textureTranslation -= 1.0f;
	}


	// Generate the view matrix based on the camera's position.
	_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	_D3D->GetWorldMatrix(worldPosition);
	_Camera->GetViewMatrix(viewMatrix);
	_D3D->GetProjectionMatrix(projectionMatrix);
	//_D3D->GetOrthoMatrix(orthoMatrix); //@NEW

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
			//worldMatrix = DirectX::XMMatrixRotationY(_modelRotation);
			//Move the model to the location it should be rendered at.
			worldPosition = DirectX::XMMatrixTranslation(positionX, positionY, positionZ);

			// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
			_Model->Render(_D3D->GetDeviceContext());

			// Render the model using the color shader.
			result = _LightShader->Render(
				_D3D->GetDeviceContext(),
				_Model->GetIndexCount(),
				worldPosition,
				viewMatrix,
				projectionMatrix,
				_Model->GetTextureArray(),
				_Light->GetDirection(),
				/*color,*/ _Light->GetAmbientColor(),
				color, //_Light->GetDiffuseColor(), 
				_Camera->GetPosition(),
				/*color,*/ _Light->GetSpecularColor(),
				_Light->GetSpecularPower(),
				fogStart,
				fogEnd,
				clipPlane,
				textureTranslation);
			if (!result)
			{
				return false;
			}

			// Reset to the original world matrix.
			_D3D->GetWorldMatrix(worldPosition);

			// Since this model was rendered then increase the count for this frame.
			renderCount++;
		}
	}

	return true;
}

bool Graphics::UpdateFpsString(ID3D11DeviceContext* deviceContext, int fps)
{
	char tempString[16];
	char finalString[16];
	float red, green, blue;
	bool result;


	// Check if the fps from the previous frame was the same, if so don't need to update the text string.
	if (m_previousFps == fps)
	{
		return true;
	}

	// Store the fps for checking next frame.
	m_previousFps = fps;

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
	result = m_FpsString->UpdateSentence(deviceContext, m_Font1, finalString, 10, 50, red, green, blue);
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
	if (positionX != m_previousPosition[0])
	{
		m_previousPosition[0] = positionX;
		_itoa_s(positionX, tempString, 10);
		strcpy_s(finalString, "X: ");
		strcat_s(finalString, tempString);
		result = m_PositionStrings[0].UpdateSentence(deviceContext, m_Font1, finalString, 10, 100, 1.0f, 1.0f, 1.0f);
		if (!result) { return false; }
	}

	if (positionY != m_previousPosition[1])
	{
		m_previousPosition[1] = positionY;
		_itoa_s(positionY, tempString, 10);
		strcpy_s(finalString, "Y: ");
		strcat_s(finalString, tempString);
		result = m_PositionStrings[1].UpdateSentence(deviceContext, m_Font1, finalString, 10, 120, 1.0f, 1.0f, 1.0f);
		if (!result) { return false; }
	}

	if (positionZ != m_previousPosition[2])
	{
		m_previousPosition[2] = positionZ;
		_itoa_s(positionZ, tempString, 10);
		strcpy_s(finalString, "Z: ");
		strcat_s(finalString, tempString);
		result = m_PositionStrings[2].UpdateSentence(deviceContext, m_Font1, finalString, 10, 140, 1.0f, 1.0f, 1.0f);
		if (!result) { return false; }
	}

	if (rotationX != m_previousPosition[3])
	{
		m_previousPosition[3] = rotationX;
		_itoa_s(rotationX, tempString, 10);
		strcpy_s(finalString, "rX: ");
		strcat_s(finalString, tempString);
		result = m_PositionStrings[3].UpdateSentence(deviceContext, m_Font1, finalString, 10, 180, 1.0f, 1.0f, 1.0f);
		if (!result) { return false; }
	}

	if (rotationY != m_previousPosition[4])
	{
		m_previousPosition[4] = rotationY;
		_itoa_s(rotationY, tempString, 10);
		strcpy_s(finalString, "rY: ");
		strcat_s(finalString, tempString);
		result = m_PositionStrings[4].UpdateSentence(deviceContext, m_Font1, finalString, 10, 200, 1.0f, 1.0f, 1.0f);
		if (!result) { return false; }
	}

	if (rotationZ != m_previousPosition[5])
	{
		m_previousPosition[5] = rotationZ;
		_itoa_s(rotationZ, tempString, 10);
		strcpy_s(finalString, "rZ: ");
		strcat_s(finalString, tempString);
		result = m_PositionStrings[5].UpdateSentence(deviceContext, m_Font1, finalString, 10, 220, 1.0f, 1.0f, 1.0f);
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
	result = m_RenderCountStrings[0].UpdateSentence(deviceContext, m_Font1, finalString, 10, 260, 1.0f, 1.0f, 1.0f);
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
	result = m_RenderCountStrings[1].UpdateSentence(deviceContext, m_Font1, finalString, 10, 280, 1.0f, 1.0f, 1.0f);
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
	result = m_RenderCountStrings[2].UpdateSentence(deviceContext, m_Font1, finalString, 10, 300, 1.0f, 1.0f, 1.0f);
	if (!result)
	{
		return false;
	}

	return true;
}