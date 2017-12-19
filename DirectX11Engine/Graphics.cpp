////////////////////////////////////////////////////////////////////////////////
// Filename: Graphics.cpp
////////////////////////////////////////////////////////////////////////////////
#include "lightshaderclass.h"
#include "lightclass.h"
#include "Graphics.h"
#include "Scene.h"

Graphics::Graphics(){}

Graphics::Graphics(const Graphics& other)
{}

Graphics::~Graphics()
{}

bool Graphics::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

	// Create the Direct3D object.
	_D3D.reset(new D3DClass);
	if (!_D3D){return false;}

	// Initialize the Direct3D object.
	result = _D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (FAILED(result))
	{
		throw std::runtime_error("Could not initialize Direct3D - line " + std::to_string(__LINE__));
		return false;
	}

	// Create / initialize the shader manager object.
	_ShaderManager.reset(new ShaderManagerClass);	if (!_ShaderManager){return false;}
	result = _ShaderManager->Initialize(_D3D->GetDevice(), hwnd);
	if (FAILED(result))
	{
		throw std::runtime_error("Could not initialize the shader manager object. - line " + std::to_string(__LINE__));
		return false;
	}

	// Create the camera object.
	_Camera.reset(new Camera);if (!_Camera){ return false;}	_Camera->SetPosition(0.0f, 0.0f, -4.f);	_Camera->UpdateViewFromPosition();

	// InitializeModels
	result = InitializeModels(hwnd, screenWidth, screenHeight);

	// Initialize Lights
	result = InitializeLights();

	// Initialize UI
	result = InitializeUI(screenWidth, screenHeight);
	

	// Initialize global effects
	XMFLOAT3 fogColor(.6f, .6f, .6f);	float fogStart = 0.0f;	float fogEnd = 3.f;
	_globalEffects.clipPlane = XMFLOAT4(0.0f, 0.f, 0.0f, 0.0f);
	_globalEffects.fogStart = fogStart;
	_globalEffects.fogEnd = fogEnd;

	return true;
}

bool Graphics::InitializeLights()
{
	// Create the light object.
	_Light.reset(new LightClass);

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
	///////////////MODEL LIST///////////////////// @TODO - make in scene class
	//vector<Model*> sceneModels;
	//vector<unique_ptr<Actor>> sceneActors;
	for (int i = 0; i < 4; ++i)
	{
		Model* pMod;
		sceneModels.push_back(pMod);

		sceneActors.push_back(unique_ptr<Actor>());
		sceneActors[i].reset(new Actor);
		sceneActors[i]->Initialize();
	}

	///////////////// WATER DEMO /////////////////////
	// Create the ground model object.
	_GroundModel.reset(new Model);
	if (!_GroundModel)
	{
		return false;
	}

	// Initialize the ground model object.
	vector<char*>groundTex{
		"../DirectX11Engine/data/ground.dds",
		"../DirectX11Engine/data/dirt.dds",
		"../DirectX11Engine/data/light.dds",
		"../DirectX11Engine/data/alpha.dds",
		"../DirectX11Engine/data/blue.dds", // normal map
		"../DirectX11Engine/data/specMap.dds" };

	bool result = _GroundModel->Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(),
		"../DirectX11Engine/data/ground.txt",groundTex,	EShaderType::ELIGHT_SPECULAR);
	if (FAILED(result))
	{
		throw std::runtime_error("Could not initialize the ground model object. - line " + std::to_string(__LINE__));
		return false;
	}
	sceneModels[0] = _GroundModel.get();
	sceneActors[0]->GetMovementComponent()->SetPosition(XMFLOAT3(0.0f, 1.0f, 0.0f));

	// Create the wall model object.
	_WallModel.reset(new Model);
	if (!_WallModel){return false;}

	// Initialize the wall model object.
	vector<char*>wallTex{
		"../DirectX11Engine/data/wall.dds",
		"../DirectX11Engine/data/dirt.dds",
		"../DirectX11Engine/data/light.dds",
		"../DirectX11Engine/data/alpha.dds",
		"../DirectX11Engine/data/bumpMap.dds", // normal map
		"../DirectX11Engine/data/specMap.dds" };

	result = _WallModel->Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(),"../DirectX11Engine/data/wall.txt",	wallTex,EShaderType::ELIGHT_SPECULAR);
	if (FAILED(result))
	{
		throw std::runtime_error("Could not initialize the wall model object. - line " + std::to_string(__LINE__));
		return false;
	}
	sceneModels[1] = _WallModel.get();
	sceneActors[1]->GetMovementComponent()->SetPosition(XMFLOAT3(0.0f, 6.0f, 8.0f));

	// Create the bath model object.
	_BathModel.reset(new Model);
	if (!_BathModel)
	{
		return false;
	}
	vector<char*>bathTex{
		"../DirectX11Engine/data/marble.png", 
		"../DirectX11Engine/data/dirt.dds",
		"../DirectX11Engine/data/light.dds",
		"../DirectX11Engine/data/alpha.dds",
		"../DirectX11Engine/data/blue.dds", // normal map
		"../DirectX11Engine/data/specMap.dds" };

	// Initialize the bath model object.
	result = _BathModel->Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(),
		"../DirectX11Engine/data/bath.txt",	bathTex,EShaderType::EREFRACTION);
	if (FAILED(result))
	{
		throw std::runtime_error("Could not initialize the bath model object. - line " + std::to_string(__LINE__));
		return false;
	}
	sceneModels[2] = _BathModel.get();
	sceneActors[2]->GetMovementComponent()->SetPosition(XMFLOAT3(0.0f, 2.0f, 0.0f));

	// Create the water model object.
	_WaterModel.reset(new Model);
	if (!_WaterModel)
	{
		return false;
	}
	vector<char*> waterTextures{ "../DirectX11Engine/data/water.dds", "../DirectX11Engine/data/water.dds" , "../DirectX11Engine/data/water.dds" };
	result = _WaterModel->Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(),
		"../DirectX11Engine/data/water.txt",
		waterTextures,
		EShaderType::EWATER);
	if (FAILED(result))
	{
		throw std::runtime_error("Could not initialize the water model object. - line " + std::to_string(__LINE__));
		return false;
	}
	_WaterModel->GetMaterial()->reflectRefractScale = 0.01f;
	_WaterModel->GetMaterial()->waterHeight = 2.75f;
	sceneModels[3] = _WaterModel.get();
	sceneActors[3]->GetMovementComponent()->SetPosition(XMFLOAT3(0.0f, _WaterModel->GetMaterial()->waterHeight, 0.0f));

	///////////////////////////////////////////////
	///////////// INIT RENDER TEXTURES //////////// (LATER ENCAPASULATE INTO MATERIALS)
	///////////////////////////////////////////////

	// Create the refraction render to texture object.
	_RefractionTexture.reset(new RenderTextureClass);
	if (!_RefractionTexture)
	{
		return false;
	}

	// Initialize the refraction render to texture object.
	result = _RefractionTexture->Initialize(_D3D->GetDevice(), screenWidth, screenHeight);
	if (FAILED(result))
	{
		throw std::runtime_error("Could not initialize the refraction render to texture object. - line " + std::to_string(__LINE__));
		return false;
	}

	// Create the reflection render to texture object.
	_ReflectionTexture.reset(new RenderTextureClass);
	if (!_ReflectionTexture)
	{
		return false;
	}

	// Initialize the reflection render to texture object.
	result = _ReflectionTexture->Initialize(_D3D->GetDevice(), screenWidth, screenHeight);
	if (FAILED(result))
	{
		throw std::runtime_error("Could not initialize the reflection render to texture object. - line " + std::to_string(__LINE__));
		return false;
	}

	//////////////////////////////////////////////////////
	////// GLOBAL OBJECTS ////////////////////////////////
	//////////////////////////////////////////////////////

	// Create the frustum object.
	_Frustum.reset(new FrustumClass);

	// Create the model list object.
	_ModelList.reset(new ModelListClass);
	if (!_ModelList)
	{
		return false;
	}

	// Initialize the model list object.
	result = _ModelList->Initialize(20);
	if (FAILED(result))
	{
		throw std::runtime_error("Could not initialize the model list object - line " + std::to_string(__LINE__));
		return false;
	}

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

	// Create the first font object.
	_Font1.reset(new FontClass);
	if (!_Font1)
	{
		return false;
	}

	// Initialize the first font object.
	result = _Font1->Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(), "../DirectX11Engine/data/font.txt",
		"../DirectX11Engine/data/font.tga", 32.0f, 3);
	if (FAILED(result))
	{
		throw std::runtime_error("Could not initialize the font object. - line " + std::to_string(__LINE__));
		return false;
	}

	// Create the text object for the fps string.
	_FpsString.reset(new TextClass);
	if (!_FpsString)
	{
		return false;
	}

	// Initialize the fps text string.
	result = _FpsString->Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(), screenWidth, screenHeight, 16, false, _Font1.get(),
		"Fps: 0", 10, 50, 0.0f, 1.0f, 0.0f);
	if (FAILED(result))
	{
		throw std::runtime_error("Could not initialize the fps string object. - line " + std::to_string(__LINE__));
		return false;
	}

	// Initial the previous frame fps.
	_previousFps = -1;

	// Initialize the position text strings.
	vector<char*> labels = { "X: 0", "Y: 0", "Z: 0", "rX: 0", "rY: 0", "rZ: 0" };
	char offset = 0;
	for (int i = 0; i < 6; ++i)
	{
		_PositionStrings.push_back(unique_ptr<TextClass>(new TextClass()));
		if (!_PositionStrings[i]) return false;

		result = _PositionStrings[i]->Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(), screenWidth, screenHeight, 16, false, _Font1.get(),
			labels[i], 10, 310 + offset, 1.0f, 1.0f, 1.0f);
		if (FAILED(result))
		{
			throw std::runtime_error("Could not initialize position string number " + to_string(i) + " - line " + std::to_string(__LINE__));
			return false;
		}

		offset += 20;
		_previousPosition[i] = -1;
	}

	// Create the text objects for the render count strings.
	vector<char*> renderLabels = { "Polys Drawn: 0", "Cells Drawn: 0", "Cells Culled: 0" };
	offset = 0;
	for (int i =0; i< 3; ++i)
	{
		_RenderCountStrings.push_back(unique_ptr<TextClass>(new TextClass()));
		if (!_RenderCountStrings[i]) return false;

		result = _RenderCountStrings[i]->Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(), screenWidth, screenHeight, 32, false, _Font1.get(), renderLabels[i], 10, 260 + offset, 1.0f, 1.0f, 1.0f);
		if (FAILED(result))
		{
			throw std::runtime_error("Could not initialize render count string number " + to_string(i) + " - line " + std::to_string(__LINE__));
			return false;
		}

		offset += 20;
	}
	
	return true;
}

bool Graphics::UpdateFrame(float frameTime, Scene* world, int fps, float camX, float camY, float camZ, float rotX, float rotY, float rotZ)
{
	bool result;

	// 1. Update Effects // @TODO - loop through all models
	_WaterModel->GetMaterial()->Animate(true);

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
	// Render the refraction of the scene to a texture.
	bool result = RenderRefractionToTexture();
	if (!result){return false;}

	// Render the reflection of the scene to a texture.
	result = RenderReflectionToTexture();
	if (!result){return false;}

	// Render the scene as normal to the back buffer.
	result = RenderScene(_globalEffects.fogStart, _globalEffects.fogEnd, frameTime);
	if (!result) { return false; }

	_D3D->TurnZBufferOff();

	RenderText();

	_D3D->TurnZBufferOn(); // Turn the Z buffer back on now that all 2D rendering has completed.

	_D3D->EndScene(); // Present the rendered scene to the screen.

	return true;
}

bool Graphics::RenderRefractionToTexture()
{
	XMFLOAT4 clipPlane;
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	// Setup a clipping plane based on the height of the water to clip everything above it.
	_globalEffects.clipPlane = XMFLOAT4(0.0f, -1.0f, 0.0f, _WaterModel->GetMaterial()->waterHeight + 0.1f);

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
	worldMatrix = DirectX::XMMatrixMultiply(worldMatrix, DirectX::XMMatrixTranslation(0.0f, 2.0f, 0.0f));

	// Put the bath model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	_BathModel->RenderBuffers(_D3D->GetDeviceContext());

	result = _ShaderManager->Render(_D3D->GetDeviceContext(), _BathModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		_BathModel->GetMaterial(), _Light.get(), _globalEffects);
	if (!result)
	{
		return false;
	}

	_globalEffects.clipPlane = XMFLOAT4(0, 0, 0,0);

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
	_Camera->RenderReflection(2.75f);

	// Get the camera reflection view matrix instead of the normal view matrix.
	reflectionViewMatrix = _Camera->GetReflectionViewMatrix();

	// Get the world and projection matrices from the d3d object.
	_D3D->GetWorldMatrix(worldMatrix);
	_D3D->GetProjectionMatrix(projectionMatrix);

	// Translate to where the wall model will be rendered.
	worldMatrix = DirectX::XMMatrixMultiply(worldMatrix, DirectX::XMMatrixTranslation(0.0f, 6.0f, 8.0f));

	// Put the wall model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	_WallModel->RenderBuffers(_D3D->GetDeviceContext());

	result = _ShaderManager->Render(_D3D->GetDeviceContext(), _WallModel->GetIndexCount(), worldMatrix, reflectionViewMatrix, projectionMatrix,
		_WallModel->GetMaterial(), _Light.get(), _globalEffects);
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

	// Clear the buffers to begin the scene.
	_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f); //@EFFECT - init to fog color here if you want to use fog

	// Generate the view matrix based on the camera's position.
	_Camera->UpdateViewFromPosition();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	_D3D->GetWorldMatrix(worldMatrix);
	_Camera->GetViewMatrix(viewMatrix);
	_D3D->GetProjectionMatrix(projectionMatrix);

	//@TODO: TEMP HACK!!!!!! - MUST ENCAPSULATE!!!!!!!
	sceneModels[3]->GetMaterial()->GetTextureObject()->GetTextureArray()[0] = _ReflectionTexture->GetShaderResourceView();
	sceneModels[3]->GetMaterial()->GetTextureObject()->GetTextureArray()[1] = _RefractionTexture->GetShaderResourceView();
	//@TODO: TEMP HACK!!!!!! - MUST ENCAPSULATE!!!!!!!

	for (int i = 0; i < sceneActors.size(); ++i)
	{
		XMFLOAT3 translation = sceneActors[i]->GetMovementComponent()->GetPosition();
		worldMatrix = DirectX::XMMatrixMultiply(worldMatrix, DirectX::XMMatrixTranslation(translation.x, translation.y, translation.z));

		sceneModels[i]->RenderBuffers(_D3D->GetDeviceContext());

		result = _ShaderManager->Render(_D3D->GetDeviceContext(), sceneModels[i]->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
			sceneModels[i]->GetMaterial(), _Light.get(), _globalEffects, XMFLOAT3(0,0,0), _Camera->GetReflectionViewMatrix());
		if (!result)
		{
			return false;
		}

		_D3D->GetWorldMatrix(worldMatrix);
	}

#pragma region WATER

	//// Translate to where the ground model will be rendered.
	//worldMatrix = DirectX::XMMatrixMultiply(worldMatrix, DirectX::XMMatrixTranslation(0.0f, 1.0f, 0.0f));

	//// Put the ground model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	//_GroundModel->RenderBuffers(_D3D->GetDeviceContext());

	//result = _ShaderManager->Render(_D3D->GetDeviceContext(), _GroundModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
	//	_GroundModel->GetMaterial(), _Light.get(), _globalEffects);
	//if (!result)
	//{
	//	return false;
	//}

	//// Reset the world matrix.
	//_D3D->GetWorldMatrix(worldMatrix);

	//// Translate to where the wall model will be rendered.
	//worldMatrix = DirectX::XMMatrixMultiply(worldMatrix, DirectX::XMMatrixTranslation(0.0f, 6.0f, 8.0f));

	//// Put the wall model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	//_WallModel->RenderBuffers(_D3D->GetDeviceContext());

	//result = _ShaderManager->Render(_D3D->GetDeviceContext(), _WallModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
	//	_WallModel->GetMaterial(), _Light.get(), _globalEffects);
	//if (!result)
	//{
	//	return false;
	//}

	//// Reset the world matrix.
	//_D3D->GetWorldMatrix(worldMatrix);

	//// Translate to where the bath model will be rendered.
	//worldMatrix = DirectX::XMMatrixMultiply(worldMatrix, DirectX::XMMatrixTranslation(0.0f, 2.0f, 0.0f));

	//// Put the bath model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	//_BathModel->RenderBuffers(_D3D->GetDeviceContext());

	//result = _ShaderManager->Render(_D3D->GetDeviceContext(), _BathModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
	//	_BathModel->GetMaterial(), _Light.get(), _globalEffects);
	//if (!result)
	//{
	//	return false;
	//}

	//// Reset the world matrix.
	//_D3D->GetWorldMatrix(worldMatrix);

	//// Get the camera reflection view matrix.
	//reflectionMatrix = _Camera->GetReflectionViewMatrix();

	//// Translate to where the water model will be rendered.
	//worldMatrix = DirectX::XMMatrixMultiply(worldMatrix, DirectX::XMMatrixTranslation(0.0f, _WaterModel->GetMaterial()->waterHeight, 0.0f));

	//// Put the water model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	//_WaterModel->RenderBuffers(_D3D->GetDeviceContext());

	////_WaterModel->GetMaterial()->GetTextureObject()->GetTextureArray()[0] = _ReflectionTexture->GetShaderResourceView();
	////_WaterModel->GetMaterial()->GetTextureObject()->GetTextureArray()[1] = _RefractionTexture->GetShaderResourceView();

	//result =_ShaderManager->Render(_D3D->GetDeviceContext(), _WaterModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, 		
	//	_WaterModel->GetMaterial(), _Light.get(), _globalEffects, 
	//	XMFLOAT3(0,0,0), reflectionMatrix); // need cleanup
	//if (!result)
	//{
	//	return false;
	//}

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

void Graphics::RenderText()
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	_D3D->GetWorldMatrix(worldMatrix);
	_Camera->GetBaseViewMatrix(baseViewMatrix);
	_D3D->GetOrthoMatrix(orthoMatrix);

	_D3D->EnableAlphaBlending();

	_FpsString->Render(_D3D->GetDeviceContext(), _ShaderManager.get(), worldMatrix, baseViewMatrix, orthoMatrix, _Font1->GetTexture());
	for (int i = 0; i < 6; i++)
	{
		_PositionStrings[i]->Render(_D3D->GetDeviceContext(), _ShaderManager.get(), worldMatrix, baseViewMatrix, orthoMatrix, _Font1->GetTexture());
	}
	for (int i = 0; i < 3; i++)
	{
		_RenderCountStrings[i]->Render(_D3D->GetDeviceContext(), _ShaderManager.get(), worldMatrix, baseViewMatrix, orthoMatrix, _Font1->GetTexture());
	}

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

	// @TODO: Set material values here

	// Update the sentence vertex buffer with the new string information.
	result = _FpsString->UpdateSentence(deviceContext, _Font1.get(), finalString, 10, 50, red, green, blue);
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

	// Initialize the position text strings.
	vector<char*> labels = { "X: ", "Y: ", "Z: ", "rX: ", "rY: ", "rZ: " };
	vector<float> posRot = { posX, posY, posZ, rotX, rotY, rotZ };
	char offset = 0;
	for (int i = 0; i < 6; ++i)
	{
		_previousPosition[i] = (int)posRot[i];
		_itoa_s(posRot[i], tempString, 10);
		strcpy_s(finalString, labels[i]);
		strcat_s(finalString, tempString);
		result = _PositionStrings[i]->UpdateSentence(deviceContext, _Font1.get(), finalString, 10, 100 + offset, 1.0f, 1.0f, 1.0f);
		if (FAILED(result))
		{
			throw std::runtime_error("Could not update sentence number " + to_string(i) + " - line " + std::to_string(__LINE__));
			return false;
		}

		offset += 20;
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
	result = _RenderCountStrings[0]->UpdateSentence(deviceContext, _Font1.get(), finalString, 10, 260, 1.0f, 1.0f, 1.0f);
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
	result = _RenderCountStrings[1]->UpdateSentence(deviceContext, _Font1.get(), finalString, 10, 280, 1.0f, 1.0f, 1.0f);
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
	result = _RenderCountStrings[2]->UpdateSentence(deviceContext, _Font1.get(), finalString, 10, 300, 1.0f, 1.0f, 1.0f);
	if (!result)
	{
		return false;
	}

	return true;
}