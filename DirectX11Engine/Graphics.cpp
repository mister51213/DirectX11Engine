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

bool Graphics::Initialize(int screenWidth, int screenHeight, HWND hwnd, Scene* scene)
{
	HRESULT result;

	_D3D.reset(new D3DClass);
	_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);

	_ShaderManager.reset(new ShaderManagerClass);
	_ShaderManager->Initialize(_D3D->GetDevice(), hwnd);

	_ShadowShader.reset(new ShadowShaderClass);
	_ShadowShader->Initialize(_D3D->GetDevice(), hwnd);

	_DepthShader.reset(new depthShaderClassALT);
	_DepthShader->Initialize(_D3D->GetDevice(), hwnd);

	_Camera.reset(new Camera);

	//XMFLOAT3 camPos = scene->GetCamera()->GetMovementComponent()->GetPosition();
	XMFLOAT3 camPos = scene->GetCamera()->GetPosition();
	_Camera->SetPosition(camPos.x, camPos.y, camPos.z);
	_Camera->UpdateViewPoint();

	InitializeModels(hwnd, screenWidth, screenHeight, scene->_Actors);

	InitializeLights(scene);

	InitializeUI(screenWidth, screenHeight);
	
	// Initialize global effects
	_globalEffects = SceneEffects();

	return true;
}

bool Graphics::InitializeModels(const HWND &hwnd, int screenWidth, int screenHeight, vector<unique_ptr<Actor>>& sceneActors)
{
#pragma region DEFAULT DRAWING

	/////////////////// DEFAULT APPEARANCE INIT /////////////////////
	//for (int i = 0; i < sceneActors.size(); ++i)
	//{
	//	if (sceneActors[i]->bCustomAppearance)
	//	{
	//		continue;
	//	}

	//	_DefaultModels.push_back(unique_ptr<Model>());
	//	sceneActors[i]->SetModel(_DefaultModels[i].get());

	//	vector<char*>defaultTex{
	//		"../DirectX11Engine/data/noise.png",
	//		"../DirectX11Engine/data/noise.png",
	//		"../DirectX11Engine/data/noise.png",
	//		"../DirectX11Engine/data/noise.png",
	//		"../DirectX11Engine/data/noise.png", // normal map
	//		"../DirectX11Engine/data/noise.png",
	//		"../DirectX11Engine/data/noise.png"};

	//	bool result = _DefaultModels[i]->Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(), "../DirectX11Engine/data/sphere.txt", defaultTex, EShaderType::ELIGHT_SPECULAR);
	//	CHECK(result, "default model");
	//}

	/////////////////// CUSTOM WATER DEMO /////////////////////
	//_GroundModel.reset(new Model);
	//if (!_GroundModel)
	//{
	//	return false;
	//}

	//// Initialize the ground model object.
	//vector<char*>groundTex{
	//	"../DirectX11Engine/data/stone.dds",
	//	"../DirectX11Engine/data/dirt.dds",
	//	"../DirectX11Engine/data/light.dds", // light map - not used
	//	"../DirectX11Engine/data/noise.png",
	//	"../DirectX11Engine/data/nMap2.png",
	//	"../DirectX11Engine/data/specMap.dds",
	//	"../DirectX11Engine/data/noise.png" }; // TEMP PLACE HOLDER - replace with DEPTH BUFFER!!!

	//bool result = _GroundModel->Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(),
	//	"../DirectX11Engine/data/ground.txt",groundTex,	EShaderType::ELIGHT_SPECULAR);
	//CHECK(result, "ground model");

	//sceneActors[sceneActors.size() - 4]->SetModel(_GroundModel.get());

	//// Create the wall model object.
	//_WallModel.reset(new Model);
	//if (!_WallModel){return false;}

	//// Initialize the wall model object.
	//vector<char*>wallTex{
	//	"../DirectX11Engine/data/wall.dds",
	//	"../DirectX11Engine/data/dirt.dds",
	//	"../DirectX11Engine/data/light.dds",
	//	"../DirectX11Engine/data/alpha.dds",
	//	"../DirectX11Engine/data/bumpMap.dds", // normal map
	//	"../DirectX11Engine/data/specMap.dds",
	//	"../DirectX11Engine/data/noise.png" };

	//result = _WallModel->Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(),"../DirectX11Engine/data/wall.txt",	wallTex,EShaderType::ELIGHT_SPECULAR);
	//CHECK(result, "wall model");
	//sceneActors[sceneActors.size() - 3]->SetModel(_WallModel.get());

	//// Create the bath model object.
	//_BathModel.reset(new Model);
	//if (!_BathModel)
	//{
	//	return false;
	//}
	//vector<char*>bathTex{
	//	"../DirectX11Engine/data/marble.png", 
	//	"../DirectX11Engine/data/dirt.dds",
	//	"../DirectX11Engine/data/light.dds",
	//	"../DirectX11Engine/data/alpha.dds",
	//	"../DirectX11Engine/data/blue.dds", // normal map
	//	"../DirectX11Engine/data/specMap.dds",
	//	"../DirectX11Engine/data/noise.png" };

	//// Initialize the bath model object.
	//result = _BathModel->Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(),
	//	"../DirectX11Engine/data/bath.txt",	bathTex,EShaderType::EREFRACTION);
	//CHECK(result, "bath model");
	//sceneActors[sceneActors.size() - 2]->SetModel(_BathModel.get());

	//// Create the water model object.
	//_WaterModel.reset(new Model);
	//if (!_WaterModel)
	//{
	//	return false;
	//}
	//vector<char*> waterTextures{ "../DirectX11Engine/data/water.dds", "../DirectX11Engine/data/water.dds" , "../DirectX11Engine/data/water.dds" };
	//result = _WaterModel->Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(),
	//	"../DirectX11Engine/data/water.txt",
	//	waterTextures,
	//	EShaderType::EWATER);
	//CHECK(result, "water model");

	//_WaterModel->GetMaterial()->reflectRefractScale = 0.01f;
	////_WaterModel->GetMaterial()->waterHeight = sceneActors[3]->GetMovementComponent()->GetPosition().y;
	//_WaterModel->GetMaterial()->waterHeight = sceneActors[3]->GetPosition().y;
	//_WaterModel->GetMaterial()->bAnimated = true;

	//sceneActors[sceneActors.size() - 1]->SetModel(_WaterModel.get());

	//// Create the model list object.
	//_ModelList.reset(new ModelListClass);
	//if (!_ModelList)
	//{
	//	return false;
	//}

	//// Initialize the model list object.
	//result = _ModelList->Initialize(20);
	//CHECK(result, "model list");

#pragma endregion

	//////////////////////////////////////////////
	///////////// CUSTOM SHADOW DEMO /////////////
	//////////////////////////////////////////////

	//groundTex[0] = "../DirectX11Engine/data/wall01.dds";
	//_CubeModel.reset(new Model);
	//_CubeModel->Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(), "../DirectX11Engine/data/cube2.txt",
	//	groundTex, EShaderType::ELIGHT_SPECULAR);
	//_CubeModel->SetPosition(XMFLOAT3(-2.0f, 1.f, 0.0f));
	_CubeModel.reset(new ModelClass);
	_CubeModel->Initialize(_D3D->GetDevice(), "../DirectX11Engine/data/cube2.txt", _D3D->GetDeviceContext(), L"../DirectX11Engine/data/wall01.dds");
	_CubeModel->SetPosition(-2.0f, 1.f, 0.0f);

	//bathTex[0] = "../DirectX11Engine/data/ice.dds";
	//_SphereModel.reset(new Model);
	//_SphereModel->Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(), "../DirectX11Engine/data/sphere.txt",
	//	bathTex, EShaderType::ELIGHT_SPECULAR);
	//_SphereModel->SetPosition(XMFLOAT3(2.0f, 1.0f, 0.0f));
	_SphereModel.reset(new ModelClass);
	_SphereModel->Initialize(_D3D->GetDevice(), "../DirectX11Engine/data/sphere.txt", _D3D->GetDeviceContext(), L"../DirectX11Engine/data/ice.dds");
	_SphereModel->SetPosition(2.0f, 1.0f, 0.0f);

	//_ShadowGround.reset(new Model);
	//vector<char*>gTex = { "../DirectX11Engine/data/metal001.dds","../DirectX11Engine/data/stone.dds","../DirectX11Engine/data/stone.dds","../DirectX11Engine/data/stone.dds","../DirectX11Engine/data/stone.dds","../DirectX11Engine/data/stone.dds","../DirectX11Engine/data/stone.dds" };
	//_ShadowGround->Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(), "../DirectX11Engine/data/plane01.txt",
	//	gTex, EShaderType::ELIGHT_SPECULAR);
	//_ShadowGround->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	_ShadowGround.reset(new ModelClass);
	_ShadowGround->Initialize(_D3D->GetDevice(), "../DirectX11Engine/data/plane01.txt", _D3D->GetDeviceContext(), L"../DirectX11Engine/data/metal001.dds");
	_ShadowGround->SetPosition(0.0f, 0.0f, 0.0f);

	_ShadowMap.reset(new RenderTextureClass);
	//result = _ShadowMap->Initialize(_D3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR);
	_ShadowMap->Initialize(_D3D->GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_WIDTH, SCREEN_DEPTH, SCREEN_NEAR);
	//result = _ShadowMap->Initialize(_D3D->GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SCREEN_DEPTH, SCREEN_NEAR);

	///////////////////////////////////////////////
	///////////// INIT RENDER TEXTURES //////////// (LATER ENCAPASULATE INTO MATERIALS)
	///////////////////////////////////////////////

	// Create the refraction render to texture object.
	//_RefractionTexture.reset(new RenderTextureClass);
	//if (!_RefractionTexture)
	//{
	//	return false;
	//}

	//// Initialize the refraction render to texture object.
	////result = _RefractionTexture->Initialize(_D3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR);
	//result = _RefractionTexture->Initialize(_D3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR);
	//CHECK(result, "refraction render to texture");

	//// Create the reflection render to texture object.
	//_ReflectionTexture.reset(new RenderTextureClass);
	//if (!_ReflectionTexture)
	//{
	//	return false;
	//}

	//// Initialize the reflection render to texture object.
	//result = _ReflectionTexture->Initialize(_D3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR);
	//CHECK(result, "reflection render to texture");

	//////////////////////////////////////////////////////
	////// GLOBAL OBJECTS ////////////////////////////////
	//////////////////////////////////////////////////////

	// Create the frustum object.
	_Frustum.reset(new FrustumClass);

	return true;
}

bool Graphics::InitializeLights(Scene* pScene)
{
	// Create the skylight object.
	//_Light.reset(new LightClass);
	_Light.reset(new lightclassALT);

	// Initialize the light object.
	_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	//_Light->SetDirection(0.0f, -1.0f, 0.5f);
	//_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	//_Light->SetSpecularPower(16.0f); // the lower the power, the higher the effect intensity

	// Shadows
	_Light->SetLookAt(0.0f, 0.0f, 0.0f);
	_Light->GenerateProjectionMatrix(SCREEN_DEPTH, SCREEN_NEAR);

	// Create list of point lights
	//for (int i = 0; i < NUM_LIGHTS; ++i)
	//{
	//	_Lights.push_back(unique_ptr<LightClass>());
	//	_Lights[i].reset(new LightClass);

	//	//XMFLOAT3 worldPosition = pScene->_LightActors[i]->GetMovementComponent()->GetPosition();
	//	XMFLOAT3 worldPosition = pScene->_LightActors[i]->GetPosition();
	//	_Lights[i]->SetPosition(worldPosition.x, worldPosition.y, worldPosition.z);
	//}

	//_Lights[0]->SetDiffuseColor(1.0f, 0.0f, 0.0f, 1.0f);
	//_Lights[1]->SetDiffuseColor(0.0f, 1.0f, 0.0f, 1.0f);
	//_Lights[2]->SetDiffuseColor(0.0f, 0.0f, 1.0f, 1.0f);
	//_Lights[3]->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);

	// STORE LIGHT DATA
	//for (auto& light : _Lights)
	//{
	//	_LightData.push_back(light.get());
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

	// Create the first font object.
	_Font1.reset(new FontClass);
	if (!_Font1)
	{
		return false;
	}

	// Initialize the first font object.
	result = _Font1->Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(), "../DirectX11Engine/data/font.txt",
		"../DirectX11Engine/data/font.tga", 32.0f, 3);
	CHECK(result, "font");

	// Create the text object for the fps string.
	_FpsString.reset(new TextClass);
	if (!_FpsString)
	{
		return false;
	}

	// Initialize the fps text string.
	result = _FpsString->Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(), screenWidth, screenHeight, 16, false, _Font1.get(),
		"Fps: 0", 10, 50, 0.0f, 1.0f, 0.0f);
	CHECK(result, "fps string");

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
		CHECK(result, "position string number " + to_string(i));

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
		CHECK(result, "render count string number " + to_string(i));

		offset += 20;
	}
	
	return true;
}

bool Graphics::UpdateFrame(float frameTime, Scene* scene, int fps)
{
	// 1. Animate Materials
	for (auto& actor : scene->_Actors)
	{
//		actor->GetModel()->GetMaterial()->Animate();
	}
	
	// 2. Update Camera
	XMFLOAT3 camPos = scene->GetCamera()->GetPosition();
	XMFLOAT3 camRot = scene->GetCamera()->GetOrientation();
	_Camera->SetPosition(camPos.x, camPos.y, camPos.z);
	_Camera->SetRotation(camRot.x, camRot.y, camRot.z);

	// 3. Update Lights
	//for (int i = 0; i < NUM_LIGHTS; ++i)
	//{
	//	//XMFLOAT3 worldPosition = scene->_LightActors[i]->GetMovementComponent()->GetPosition();
	//	XMFLOAT3 worldPosition = scene->_LightActors[i]->GetPosition();
	//	_Lights[i]->SetPosition(worldPosition.x, worldPosition.y, worldPosition.z);
	//}

	// Update the position of the light each frame.
	static float lightPositionX = -5.f;
	lightPositionX += 0.002f * frameTime;
	if (lightPositionX > 5.0f)
	lightPositionX = -5.0f;
	_Light->SetPosition(lightPositionX, 6.0f, -5.0f);

	// 4. Update UI
	UpdateFpsString(_D3D->GetDeviceContext(), fps);
	UpdatePositionStrings(_D3D->GetDeviceContext(), camPos.x, camPos.y, camPos.z, camRot.x, camRot.y, camRot.z);

	// 5. Draw the Actual frame
	//DrawFrame(scene->_Actors, frameTime); 
	//////// TESTING //////////////
	Render();

	return true;
}

void Graphics::Render()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, translateMatrix;
	XMMATRIX lightViewMatrix, lightProjectionMatrix;
	float posX, posY, posZ;
	
	// First render the scene to a texture.
	RenderSceneToTexture();

	// Clear the buffers to begin the scene.
	_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	_Camera->UpdateViewPoint();

	// Generate the light view matrix based on the light's position.
	_Light->GenerateViewMatrix();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	_Camera->GetViewMatrix(viewMatrix);
	_D3D->GetWorldMatrix(worldMatrix);
	_D3D->GetProjectionMatrix(projectionMatrix);

	// Get the light's view and projection matrices from the light object.
	//lightViewMatrix = _Light->GetViewMatrix();
	_Light->GetViewMatrix(lightViewMatrix);
	//lightProjectionMatrix = _Light->GetProjectionMatrix();
	_Light->GetProjectionMatrix(lightProjectionMatrix);

	// Setup the translation matrix for the cube model.
	//XMFLOAT3 cPos = _CubeModel->GetPosition(/*posX, posY, posZ*/);
	_CubeModel->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixTranslation(/*cPos.x, cPos.y, cPos.z*/posX, posY, posZ));


	// Put the cube model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	//_CubeModel->LoadVertices(_D3D->GetDeviceContext());
	_CubeModel->Render(_D3D->GetDeviceContext());

	// Render the model using the shadow shader.
	_ShadowShader->Render(
		_D3D->GetDeviceContext(), _CubeModel->GetIndexCount(),
		worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix,
		lightProjectionMatrix, _CubeModel->/*GetMaterial()->GetResourceArray()[0]*/GetTexture(), _ShadowMap->GetShaderResourceView(), _Light->GetPosition(),
		_Light->GetAmbientColor(), _Light->GetDiffuseColor());

	// Reset the world matrix.
	_D3D->GetWorldMatrix(worldMatrix);

	// Setup the translation matrix for the sphere model.
	//XMFLOAT3 spherePos = _SphereModel->GetPosition(/*posX, posY, posZ*/);
	_SphereModel->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixTranslation(/*spherePos.x, spherePos.y, spherePos.z*/posX, posY, posZ));


	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	//_SphereModel->LoadVertices(_D3D->GetDeviceContext());
	_SphereModel->Render(_D3D->GetDeviceContext());
	_ShadowShader->Render(_D3D->GetDeviceContext(), _SphereModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix,
		lightProjectionMatrix, _SphereModel->/*GetMaterial()->GetResourceArray()[0]*/GetTexture(), _ShadowMap->GetShaderResourceView(), _Light->GetPosition(),
		_Light->GetAmbientColor(), _Light->GetDiffuseColor());

	// Reset the world matrix.
	_D3D->GetWorldMatrix(worldMatrix);

	// Setup the translation matrix for the ground model.
	//XMFLOAT3 grPos = _ShadowGround->GetPosition(/*posX, posY, posZ*/);
	_ShadowGround->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixTranslation(/*grPos.x, grPos.y, grPos.z*/posX, posY, posZ));


	// Render the ground model using the shadow shader.
	//_ShadowGround->LoadVertices(_D3D->GetDeviceContext());
	_ShadowGround->Render(_D3D->GetDeviceContext());
	_ShadowShader->Render(
		_D3D->GetDeviceContext(),
		_ShadowGround->GetIndexCount(),
		worldMatrix, viewMatrix, projectionMatrix,
		lightViewMatrix, lightProjectionMatrix,
		_ShadowGround->GetTexture()/*_ShadowGround->GetMaterial()->GetResourceArray()[0]*/, _ShadowMap->GetShaderResourceView(), _Light->GetPosition(),
		_Light->GetAmbientColor(), _Light->GetDiffuseColor());

	// Present the rendered scene to the screen.
	_D3D->EndScene();

}

void Graphics::RenderSceneToTexture()
{
	XMMATRIX worldMatrix, lightViewMatrix, lightProjectionMatrix, translateMatrix;
	float posX, posY, posZ;

	// Set the render target to be the render to texture.
	_ShadowMap->SetRenderTarget(_D3D->GetDeviceContext());

	// Clear the render to texture.
	_ShadowMap->ClearRenderTarget(_D3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the light view matrix based on the light's position.
	_Light->GenerateViewMatrix();

	// Get the world matrix from the d3d object.
	_D3D->GetWorldMatrix(worldMatrix);

	// Get the view and orthographic matrices from the light object.
	//lightViewMatrix = _Light->GetViewMatrix();
	_Light->GetViewMatrix(lightViewMatrix);
	//lightProjectionMatrix = _Light->GetProjectionMatrix();
	_Light->GetProjectionMatrix(lightProjectionMatrix);

	// Setup the translation matrix for the cube model.
	//XMFLOAT3 cPos = _CubeModel->GetPosition(/*posX, posY, posZ*/);
	_CubeModel->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixTranslation(/*cPos.x, cPos.y, cPos.z*/posX, posY, posZ));

	// Render the cube model with the depth shader.
	//_CubeModel->LoadVertices(_D3D->GetDeviceContext());
	_CubeModel->Render(_D3D->GetDeviceContext());
	_ShaderManager->_DepthShader->Render(_D3D->GetDeviceContext(),_CubeModel->GetIndexCount(), worldMatrix, lightViewMatrix, lightProjectionMatrix);


	// Reset the world matrix.
	_D3D->GetWorldMatrix(worldMatrix);

	// Setup the translation matrix for the sphere model.
	//XMFLOAT3 spPos = _SphereModel->GetPosition(/*posX, posY, posZ*/);
	_SphereModel->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixTranslation(/*spPos.x, spPos.y, spPos.z*/posX, posY, posZ));


	// Render the sphere model with the depth shader.
	//_SphereModel->LoadVertices(_D3D->GetDeviceContext());
	_SphereModel->Render(_D3D->GetDeviceContext());
	_ShaderManager->_DepthShader->Render(_D3D->GetDeviceContext(), _SphereModel->GetIndexCount(), worldMatrix, lightViewMatrix, lightProjectionMatrix);

	// Reset the world matrix.
	_D3D->GetWorldMatrix(worldMatrix);

	// Setup the translation matrix for the ground model.
	//XMFLOAT3 grPos = _ShadowGround->GetPosition(/*posX, posY, posZ*/);
	_ShadowGround->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixTranslation(/*grPos.x, grPos.y, grPos.z*/posX, posY, posZ));


	// Render the ground model with the depth shader.
	//_ShadowGround->LoadVertices(_D3D->GetDeviceContext());
	_ShadowGround->Render(_D3D->GetDeviceContext());
	_ShaderManager->_DepthShader->Render(_D3D->GetDeviceContext(), _ShadowGround->GetIndexCount(), worldMatrix, lightViewMatrix, lightProjectionMatrix);

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	_D3D->SetBackBufferRenderTarget();

	// Reset the viewport back to the original.
	_D3D->ResetViewport();
}

bool Graphics::DrawFrame(vector<unique_ptr<Actor>>& sceneActors, float frameTime)
{
	// Render the refraction of the scene to a texture.
	//RenderRefractionToTexture(_WaterModel->GetMaterial()->waterHeight);

	// Render the reflection of the scene to a texture.
	//result = RenderReflectionToTexture();
	//if (!result){return false;}

	// Render the scene as normal to the back buffer.
	RenderScene(sceneActors, frameTime);

	RenderText();

	_D3D->EndScene(); // Present the rendered scene to the screen.

	return true;
}

bool Graphics::RenderRefractionToTexture(float surfaceHeight)
{
	XMMATRIX worldMatrix;

	////////// WATER REFRACTION ///////////
	//XMMATRIX viewMatrix, projectionMatrix;
	//// Setup a clipping plane based on the height of the water to clip everything above it.
	//_globalEffects.clipPlane = XMFLOAT4(0.0f, -1.0f, 0.0f, surfaceHeight);
	//// Set the render target to be the refraction render to texture.
	//_RefractionTexture->SetRenderTarget(_D3D->GetDeviceContext(), _D3D->GetDepthStencilView());
	//// Clear the refraction render to texture.
	//_RefractionTexture->ClearRenderTarget(_D3D->GetDeviceContext(), _D3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);
	//// Generate the view matrix based on the camera's position.
	//_Camera->UpdateViewPoint();
	//// Get the world, view, and projection matrices from the camera and d3d objects.
	//_D3D->GetWorldMatrix(worldMatrix);
	//_Camera->GetViewMatrix(viewMatrix);
	//_D3D->GetProjectionMatrix(projectionMatrix);
	//// Translate to where the bath model will be rendered.
	//worldMatrix = DirectX::XMMatrixMultiply(worldMatrix, DirectX::XMMatrixTranslation(0.0f, 2.0f, 0.0f));
	//// Put the bath model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	//_BathModel->LoadVertices(_D3D->GetDeviceContext());
	//bool result = _ShaderManager->Render(_D3D->GetDeviceContext(), _BathModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
	//	_BathModel->GetMaterial(), _Light.get(), _LightData.data(), _globalEffects);
	//if (!result)
	//{
	//	return false;
	//}
	//_globalEffects.clipPlane = XMFLOAT4(0, 0, 0,0);

	/////////////////////////////////////
	/////////// SHADOW MAPPING //////////
	/////////////////////////////////////

	//XMMATRIX lightViewMatrix, lightProjectionMatrix, translateMatrix;

	//// Set the render target to be the render to texture.
	//_ShadowMap->SetRenderTarget(_D3D->GetDeviceContext()/*,_D3D->GetDepthStencilView()*/);

	//// Clear the render to texture.
	//_ShadowMap->ClearRenderTarget(_D3D->GetDeviceContext(), /*_D3D->GetDepthStencilView(),*/ 0.0f, 0.0f, 0.0f, 1.0f);

	//// Generate the light view matrix based on the light's position.
	//_Light->GenerateViewMatrix();

	//// Get the world matrix from the d3d object.
	//_D3D->GetWorldMatrix(worldMatrix);

	//// Get the view and orthographic matrices from the light object.
	////lightViewMatrix = _Light->GetViewMatrix();
	//_Light->GetViewMatrix(lightViewMatrix);
	////lightProjectionMatrix = _Light->GetProjectionMatrix();
	//_Light->GetProjectionMatrix(lightProjectionMatrix);

	////////////////////////////////////////////////////////////////////
	//// Setup the translation matrix for the CUBE MODEL.
	//worldMatrix = DirectX::XMMatrixTranslation(_CubeModel->GetPosition().x, _CubeModel->GetPosition().y, _CubeModel->GetPosition().z);
	//_CubeModel->LoadVertices(_D3D->GetDeviceContext());
	//_DepthShader->Render(_D3D->GetDeviceContext(), _CubeModel->GetIndexCount(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	//_D3D->GetWorldMatrix(worldMatrix);

	////////////////////////////////////////////////////////////////////
	//// RENDER THE SPHERE MODEL WITH THE DEPTH SHADER.
	//worldMatrix = DirectX::XMMatrixTranslation(_SphereModel->GetPosition().x, _SphereModel->GetPosition().y, _SphereModel->GetPosition().z);
	//_SphereModel->LoadVertices(_D3D->GetDeviceContext());
	//_DepthShader->Render(_D3D->GetDeviceContext(), _SphereModel->GetIndexCount(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	//_D3D->GetWorldMatrix(worldMatrix);

	////////////////////////////////////////////////////////////////////
	//// RENDER THE GROUND MODEL WITH THE DEPTH SHADER.
	//worldMatrix = DirectX::XMMatrixTranslation(_ShadowGround->GetPosition().x, _ShadowGround->GetPosition().y, _ShadowGround->GetPosition().z);
	//_ShadowGround->LoadVertices(_D3D->GetDeviceContext());
	//_DepthShader->Render(_D3D->GetDeviceContext(), _ShadowGround->GetIndexCount(), worldMatrix, lightViewMatrix, lightProjectionMatrix);

	////////////////////////////////
	/////////// RESET ALL //////////
	////////////////////////////////

	//// Reset the render target back to the original back buffer and not the render to texture anymore.
	//_D3D->SetBackBufferRenderTarget();

	//// Reset the viewport back to the original.
	//_D3D->ResetViewport();

	return true;
}

bool Graphics::RenderReflectionToTexture()
{
	//XMMATRIX reflectionViewMatrix, worldMatrix, projectionMatrix;
	//
	//// Set the render target to be the reflection render to texture.
	//_ReflectionTexture->SetRenderTarget(_D3D->GetDeviceContext(), _D3D->GetDepthStencilView());

	//// Clear the reflection render to texture.
	//_ReflectionTexture->ClearRenderTarget(_D3D->GetDeviceContext(), _D3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);

	//// Use the camera to render the reflection and create a reflection view matrix.
	//_Camera->RenderReflection(2.75f);

	//// Get the camera reflection view matrix instead of the normal view matrix.
	//reflectionViewMatrix = _Camera->GetReflectionViewMatrix();

	//// Get the world and projection matrices from the d3d object.
	//_D3D->GetWorldMatrix(worldMatrix);
	//_D3D->GetProjectionMatrix(projectionMatrix);

	//// Translate to where the wall model will be rendered.
	//worldMatrix = DirectX::XMMatrixMultiply(worldMatrix, DirectX::XMMatrixTranslation(0.0f, 6.0f, 8.0f));

	//// Put the wall model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	//_WallModel->LoadVertices(_D3D->GetDeviceContext());

	//bool result = _ShaderManager->Render(_D3D->GetDeviceContext(), _WallModel->GetIndexCount(), worldMatrix, reflectionViewMatrix, projectionMatrix,
	//	_WallModel->GetMaterial(), _Light.get(), _LightData.data(), _globalEffects);
	//if (!result)
	//{
	//	return false;
	//}

	//// Reset the render target back to the original back buffer and not the render to texture anymore.
	//_D3D->SetBackBufferRenderTarget();

	return true;
}

bool Graphics::RenderScene(vector<unique_ptr<Actor>>& sceneActors, float frameTime)
{
	//XMMATRIX worldMatrix, viewMatrix, projectionMatrix, reflectionMatrix;

	//// Clear the buffers to begin the scene.
	//_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f); //@EFFECT - init to fog color here if you want to use fog

	//// Generate the view matrix based on the camera's position.
	//_Camera->UpdateViewPoint();

	//// Generate the light view matrix based on the light's position.
	////_Light->GenerateViewMatrix();

	//// Get the world, view, and projection matrices from the camera and d3d objects.
	//_Camera->GetViewMatrix(viewMatrix);
	//_D3D->GetWorldMatrix(worldMatrix);
	//_D3D->GetProjectionMatrix(projectionMatrix);

	////@TODO: TEMP HACK!!!!!! - MUST ENCAPSULATE!!!!!!!
	////sceneActors[3]->GetModel()->GetMaterial()->GetTextureObject()->GetTextureArray()[0] = _ReflectionTexture->GetShaderResourceView();
	////sceneActors[3]->GetModel()->GetMaterial()->GetTextureObject()->GetTextureArray()[1] = _RefractionTexture->GetShaderResourceView();
	////@TODO: TEMP HACK!!!!!! - MUST ENCAPSULATE!!!!!!!

	////for (int i = 0; i < (*sceneActors).size(); ++i)
	////{
	////	XMFLOAT3 translation = (*sceneActors)[i]->GetMovementComponent()->GetPosition();
	////	worldMatrix = DirectX::XMMatrixMultiply(worldMatrix, DirectX::XMMatrixTranslation(translation.x, translation.y, translation.z));

	////	(*sceneActors)[i]->GetModel()->LoadVertices(_D3D->GetDeviceContext());

	////	if((*sceneActors)[i]->GetModel()->GetMaterial()->transparency != 0.f)
	////		_D3D->EnableAlphaBlending();

	////	bool result = _ShaderManager->Render(_D3D->GetDeviceContext(), (*sceneActors)[i]->GetModel()->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
	////		(*sceneActors)[i]->GetModel()->GetMaterial(), _Light.get(), _LightData.data(), _globalEffects, XMFLOAT3(0,0,0), _Camera->GetReflectionViewMatrix());
	////	if (!result) return false;

	////	if ((*sceneActors)[i]->GetModel()->GetMaterial()->transparency != 0.f)
	////		_D3D->DisableAlphaBlending();

	////	// reset world matrix
	////	_D3D->GetWorldMatrix(worldMatrix);
	////}
	//
	////@TEST SECTION - Point lights not working
	////XMFLOAT3 translation = (*sceneActors)[0]->GetMovementComponent()->GetPosition();
	////worldMatrix = DirectX::XMMatrixMultiply(worldMatrix, DirectX::XMMatrixTranslation(translation.x, translation.y, translation.z));
	////(*sceneActors)[0]->GetModel()->LoadVertices(_D3D->GetDeviceContext());

	////_ShaderManager->Render(_D3D->GetDeviceContext(), _GroundModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
	////	_GroundModel->GetMaterial(), _Light.get(), _LightData.data(), _globalEffects, XMFLOAT3(0, 0, 0), _Camera->GetReflectionViewMatrix());

	//// TESTING SHADOWS // - WORKING

	//// Generate the light view matrix based on the light's position.
	//_Light->GenerateViewMatrix();
	////_Light->GenerateProjectionMatrix();

	//// Get the light's view and projection matrices from the light object.
	//XMMATRIX lightViewMatrix = _Light->GetViewMatrix();
	//XMMATRIX lightProjectionMatrix = _Light->GetProjectionMatrix();

	//// CUBE
	//_D3D->GetWorldMatrix(worldMatrix);
	//worldMatrix = DirectX::XMMatrixMultiply(worldMatrix, DirectX::XMMatrixTranslation(_CubeModel->GetPosition().x, _CubeModel->GetPosition().y, _CubeModel->GetPosition().z));

	//_CubeModel->LoadVertices(_D3D->GetDeviceContext());
	//	
	//_ShaderManager->Render(_D3D->GetDeviceContext(), _CubeModel->GetIndexCount(), 
	//	worldMatrix, viewMatrix, projectionMatrix,
	//	_CubeModel->GetMaterial(), _Light.get(), _LightData.data(), _globalEffects);
	////_ShaderManager->_LightShader->Render(
	////	_D3D->GetDeviceContext(), _CubeModel->GetIndexCount(),
	////	worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix,
	////	_CubeModel->GetMaterial(), _Light.get(), _LightData.data(), _globalEffects);
	//
	//// SPHERE
	//_D3D->GetWorldMatrix(worldMatrix);
	//worldMatrix = DirectX::XMMatrixMultiply(worldMatrix, DirectX::XMMatrixTranslation(_SphereModel->GetPosition().x, _SphereModel->GetPosition().y, _SphereModel->GetPosition().z));

	//_SphereModel->LoadVertices(_D3D->GetDeviceContext());
	//
	//_ShaderManager->Render(_D3D->GetDeviceContext(), _SphereModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
	//	_SphereModel->GetMaterial(), _Light.get(), _LightData.data(), _globalEffects);
	////_ShaderManager->_LightShader->Render(
	////	_D3D->GetDeviceContext(), _SphereModel->GetIndexCount(), 
	////	worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix,
	////	_SphereModel->GetMaterial(), _Light.get(), _LightData.data(), _globalEffects);

	//// GROUND
	//_D3D->GetWorldMatrix(worldMatrix);
	//worldMatrix = DirectX::XMMatrixMultiply(worldMatrix, DirectX::XMMatrixTranslation(_ShadowGround->GetPosition().x, _ShadowGround->GetPosition().y, _ShadowGround->GetPosition().z));
	//
	//_ShadowGround->LoadVertices(_D3D->GetDeviceContext());

	//_ShadowGround->GetMaterial()->GetTextureObject()->GetTextureArray()[6] = _ShadowMap->GetShaderResourceView();

	//_ShaderManager->Render(_D3D->GetDeviceContext(), _ShadowGround->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
	//	_ShadowGround->GetMaterial(), _Light.get(), _LightData.data(), _globalEffects);
	////_ShaderManager->_LightShader->Render(
	////	_D3D->GetDeviceContext(), _GroundModel->GetIndexCount(), 
	////	worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix,
	////	_GroundModel->GetMaterial(), _Light.get(), _LightData.data(), _globalEffects);


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
	_D3D->TurnZBufferOff();
	_D3D->EnableAlphaBlending();


	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	_D3D->GetWorldMatrix(worldMatrix);
	_Camera->GetBaseViewMatrix(baseViewMatrix);
	_D3D->GetOrthoMatrix(orthoMatrix);


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
	_D3D->TurnZBufferOn(); // Turn the Z buffer back on now that all 2D rendering has completed.
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

#pragma region UNIT_TESTING

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
///////////// SHADOW TESTING /////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

//
//////////////////////////////////////////////////////////////////////////////////
//// Filename: graphicsclass.cpp
//////////////////////////////////////////////////////////////////////////////////
//#include "graphics.h"
//
//
//GraphicsClass::GraphicsClass()
//{
//	m_D3D = 0;
//	m_Camera = 0;
//	m_CubeModel = 0;
//	m_GroundModel = 0;
//	m_SphereModel = 0;
//	m_Light = 0;
//	m_RenderTexture = 0;
//	m_DepthShader = 0;
//	m_ShadowShader = 0;
//}
//
//
//GraphicsClass::GraphicsClass(const GraphicsClass& other)
//{
//}
//
//
//GraphicsClass::~GraphicsClass()
//{
//}
//
//
//bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
//{
//	bool result;
//
//
//	// Create the Direct3D object.
//	m_D3D = new D3DClass;
//	if (!m_D3D)
//	{
//		return false;
//	}
//
//	// Initialize the Direct3D object.
//	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
//	if (!result)
//	{
//		MessageBox(hwnd, "Could not initialize Direct3D.", "Error", MB_OK);
//		return false;
//	}
//
//	// Create the camera object.
//	m_Camera = new CameraClass;
//	if (!m_Camera)
//	{
//		return false;
//	}
//
//	// Set the initial position of the camera.
//	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
//
//	// Create the cube model object.
//	m_CubeModel = new ModelClass;
//	if (!m_CubeModel)
//	{
//		return false;
//	}
//
//	// Initialize the cube model object.
//	result = m_CubeModel->Initialize(m_D3D->GetDevice(), "../DirectX11Engine/data/cube2.txt", m_D3D->GetDeviceContext(), L"../DirectX11Engine/data/wall01.dds");
//	if (!result)
//	{
//		MessageBox(hwnd, "Could not initialize the cube model object.", "Error", MB_OK);
//		return false;
//	}
//
//	// Set the position for the cube model.
//	m_CubeModel->SetPosition(-2.0f, 1.f, 0.0f);
//
//	// Create the sphere model object.
//	m_SphereModel = new ModelClass;
//	if (!m_SphereModel)
//	{
//		return false;
//	}
//
//	// Initialize the sphere model object.
//	result = m_SphereModel->Initialize(m_D3D->GetDevice(), "../DirectX11Engine/data/sphere.txt", m_D3D->GetDeviceContext(), L"../DirectX11Engine/data/ice.dds");
//	if (!result)
//	{
//		MessageBox(hwnd, "Could not initialize the sphere model object.", "Error", MB_OK);
//		return false;
//	}
//
//	// Set the position for the sphere model.
//	m_SphereModel->SetPosition(2.0f, 1.0f, 0.0f);
//
//	// Create the ground model object.
//	m_GroundModel = new ModelClass;
//	if (!m_GroundModel)
//	{
//		return false;
//	}
//
//	// Initialize the ground model object.
//	result = m_GroundModel->Initialize(m_D3D->GetDevice(), "../DirectX11Engine/data/plane01.txt", m_D3D->GetDeviceContext(), L"../DirectX11Engine/data/metal001.dds");
//	if (!result)
//	{
//		MessageBox(hwnd, "Could not initialize the ground model object.", "Error", MB_OK);
//		return false;
//	}
//
//	// Set the position for the ground model.
//	m_GroundModel->SetPosition(0.0f, 0.0f, 0.0f);
//
//	// Create the light object.
//	m_Light = new lightclassALT;
//	if (!m_Light)
//	{
//		return false;
//	}
//
//	// Initialize the light object.
//	m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
//	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
//	m_Light->SetLookAt(0.0f, 0.0f, 0.0f);
//	m_Light->GenerateProjectionMatrix(SCREEN_DEPTH, SCREEN_NEAR);
//
//	// Create the render to texture object.
//	m_RenderTexture = new RenderTextureClass;
//	if (!m_RenderTexture)
//	{
//		return false;
//	}
//
//	// Initialize the render to texture object.
//	//result = m_RenderTexture->Initialize(m_D3D->GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SCREEN_DEPTH, SCREEN_NEAR);
//	result = m_RenderTexture->Initialize(m_D3D->GetDevice(), SHADOWMAP_WIDTH*.1f, SHADOWMAP_HEIGHT*.1f, SCREEN_DEPTH, SCREEN_NEAR);
//	//	result = m_RenderTexture->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR);
//	if (!result)
//	{
//		MessageBox(hwnd, "Could not initialize the render to texture object.", "Error", MB_OK);
//		return false;
//	}
//
//	// Create the depth shader object.
//	//m_DepthShader = new DepthShaderClass;
//	m_DepthShader = new depthShaderClassALT;
//	if (!m_DepthShader)
//	{
//		return false;
//	}
//
//	// Initialize the depth shader object.
//	//result = m_DepthShader->Initialize(m_D3D->GetDevice(), hwnd);
//	result = m_DepthShader->Initialize(m_D3D->GetDevice(), hwnd/*, "../DirectX11Engine/depth.vs", "../DirectX11Engine/depth.ps"*/);
//
//	if (!result)
//	{
//		MessageBox(hwnd, "Could not initialize the depth shader object.", "Error", MB_OK);
//		return false;
//	}
//
//	// Create the shadow shader object.
//	m_ShadowShader = new ShadowShaderClass;
//	if (!m_ShadowShader)
//	{
//		return false;
//	}
//
//	// Initialize the shadow shader object.
//	result = m_ShadowShader->Initialize(m_D3D->GetDevice(), hwnd);
//	if (!result)
//	{
//		MessageBox(hwnd, "Could not initialize the shadow shader object.","Error", MB_OK);
//		return false;
//	}
//
//	return true;
//}
//
//
//void GraphicsClass::Shutdown()
//{
//	//// Release the shadow shader object.
//	//if (m_ShadowShader)
//	//{
//	//	m_ShadowShader->Shutdown();
//	//	delete m_ShadowShader;
//	//	m_ShadowShader = 0;
//	//}
//
//	//// Release the depth shader object.
//	//if (m_DepthShader)
//	//{
//	//	m_DepthShader->Shutdown();
//	//	delete m_DepthShader;
//	//	m_DepthShader = 0;
//	//}
//
//	// Release the render to texture object.
//	if (m_RenderTexture)
//	{
//		m_RenderTexture->Shutdown();
//		delete m_RenderTexture;
//		m_RenderTexture = 0;
//	}
//
//	// Release the light object.
//	if (m_Light)
//	{
//		delete m_Light;
//		m_Light = 0;
//	}
//
//	// Release the ground model object.
//	if (m_GroundModel)
//	{
//		m_GroundModel->Shutdown();
//		delete m_GroundModel;
//		m_GroundModel = 0;
//	}
//
//	// Release the sphere model object.
//	if (m_SphereModel)
//	{
//		m_SphereModel->Shutdown();
//		delete m_SphereModel;
//		m_SphereModel = 0;
//	}
//
//	// Release the cube model object.
//	if (m_CubeModel)
//	{
//		m_CubeModel->Shutdown();
//		delete m_CubeModel;
//		m_CubeModel = 0;
//	}
//
//	// Release the camera object.
//	if (m_Camera)
//	{
//		delete m_Camera;
//		m_Camera = 0;
//	}
//
//	// Release the D3D object.
//	if (m_D3D)
//	{
//		m_D3D->Shutdown();
//		delete m_D3D;
//		m_D3D = 0;
//	}
//
//	return;
//}
//
//
//bool GraphicsClass::Frame(float posX, float posY, float posZ, float rotX, float rotY, float rotZ)
//{
//	bool result;
//	static float lightPositionX = -5.0f;
//
//
//	// Set the position of the camera.
//	m_Camera->SetPosition(posX, posY, posZ);
//	m_Camera->SetRotation(rotX, rotY, rotZ);
//
//	// Update the position of the light each frame.
//	lightPositionX += 0.05f;
//	if (lightPositionX > 5.0f)
//	{
//		lightPositionX = -5.0f;
//	}
//
//	// Update the position of the light.
//	m_Light->SetPosition(lightPositionX, 8.0f, -5.0f);
//
//	// Render the graphics scene.
//	result = Render();
//	if (!result)
//	{
//		return false;
//	}
//
//	return true;
//}
//
//
//bool GraphicsClass::RenderSceneToTexture()
//{
//	XMMATRIX worldMatrix, lightViewMatrix, lightProjectionMatrix, translateMatrix;
//	float posX, posY, posZ;
//	bool result;
//
//
//	// Set the render target to be the render to texture.
//	m_RenderTexture->SetRenderTarget(m_D3D->GetDeviceContext());
//
//	// Clear the render to texture.
//	m_RenderTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);
//
//	// Generate the light view matrix based on the light's position.
//	m_Light->GenerateViewMatrix();
//
//	// Get the world matrix from the d3d object.
//	m_D3D->GetWorldMatrix(worldMatrix);
//
//	// Get the view and orthographic matrices from the light object.
//	m_Light->GetViewMatrix(lightViewMatrix);
//	m_Light->GetProjectionMatrix(lightProjectionMatrix);
//
//	// Setup the translation matrix for the cube model.
//	m_CubeModel->GetPosition(posX, posY, posZ);
//	//D3DXMatrixTranslation(&worldMatrix, posX, posY, posZ);
//	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixTranslation(posX, posY, posZ));
//
//	// Render the cube model with the depth shader.
//	m_CubeModel->Render(m_D3D->GetDeviceContext());
//	result = m_DepthShader->Render(m_D3D->GetDeviceContext(), m_CubeModel->GetIndexCount(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
//	if (!result)
//	{
//		return false;
//	}
//
//	// Reset the world matrix.
//	m_D3D->GetWorldMatrix(worldMatrix);
//
//	// Setup the translation matrix for the sphere model.
//	m_SphereModel->GetPosition(posX, posY, posZ);
//	//D3DXMatrixTranslation(&worldMatrix, posX, posY, posZ);
//	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixTranslation(posX, posY, posZ));
//
//
//	// Render the sphere model with the depth shader.
//	m_SphereModel->Render(m_D3D->GetDeviceContext());
//	result = m_DepthShader->Render(m_D3D->GetDeviceContext(), m_SphereModel->GetIndexCount(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
//	if (!result)
//	{
//		return false;
//	}
//
//	// Reset the world matrix.
//	m_D3D->GetWorldMatrix(worldMatrix);
//
//	// Setup the translation matrix for the ground model.
//	m_GroundModel->GetPosition(posX, posY, posZ);
//	//D3DXMatrixTranslation(&worldMatrix, posX, posY, posZ);
//	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixTranslation(posX, posY, posZ));
//
//
//	// Render the ground model with the depth shader.
//	m_GroundModel->Render(m_D3D->GetDeviceContext());
//	result = m_DepthShader->Render(m_D3D->GetDeviceContext(), m_GroundModel->GetIndexCount(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
//	if (!result)
//	{
//		return false;
//	}
//
//	// Reset the render target back to the original back buffer and not the render to texture anymore.
//	m_D3D->SetBackBufferRenderTarget();
//
//	// Reset the viewport back to the original.
//	m_D3D->ResetViewport();
//
//	return true;
//}
//
//
//bool GraphicsClass::Render()
//{
//	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, translateMatrix;
//	XMMATRIX lightViewMatrix, lightProjectionMatrix;
//	bool result;
//	float posX, posY, posZ;
//
//
//	// First render the scene to a texture.
//	result = RenderSceneToTexture();
//	if (!result)
//	{
//		return false;
//	}
//
//	// Clear the buffers to begin the scene.
//	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
//
//	// Generate the view matrix based on the camera's position.
//	m_Camera->Render();
//
//	// Generate the light view matrix based on the light's position.
//	m_Light->GenerateViewMatrix();
//
//	// Get the world, view, and projection matrices from the camera and d3d objects.
//	m_Camera->GetViewMatrix(viewMatrix);
//	m_D3D->GetWorldMatrix(worldMatrix);
//	m_D3D->GetProjectionMatrix(projectionMatrix);
//
//	// Get the light's view and projection matrices from the light object.
//	m_Light->GetViewMatrix(lightViewMatrix);
//	m_Light->GetProjectionMatrix(lightProjectionMatrix);
//
//	// Setup the translation matrix for the cube model.
//	m_CubeModel->GetPosition(posX, posY, posZ);
//	//D3DXMatrixTranslation(&worldMatrix, posX, posY, posZ);
//	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixTranslation(posX, posY, posZ));
//
//
//	// Put the cube model vertex and index buffers on the graphics pipeline to prepare them for drawing.
//	m_CubeModel->Render(m_D3D->GetDeviceContext());
//
//	// Render the model using the shadow shader.
//	result = m_ShadowShader->Render(
//		m_D3D->GetDeviceContext(), m_CubeModel->GetIndexCount(),
//		worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix,
//		lightProjectionMatrix, m_CubeModel->GetTexture(), m_RenderTexture->GetShaderResourceView(), m_Light->GetPosition(),
//		m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
//	if (!result)
//	{
//		return false;
//	}
//
//	// Reset the world matrix.
//	m_D3D->GetWorldMatrix(worldMatrix);
//
//	// Setup the translation matrix for the sphere model.
//	m_SphereModel->GetPosition(posX, posY, posZ);
//	//D3DXMatrixTranslation(&worldMatrix, posX, posY, posZ);
//	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixTranslation(posX, posY, posZ));
//
//
//	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
//	m_SphereModel->Render(m_D3D->GetDeviceContext());
//	result = m_ShadowShader->Render(m_D3D->GetDeviceContext(), m_SphereModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix,
//		lightProjectionMatrix, m_SphereModel->GetTexture(), m_RenderTexture->GetShaderResourceView(), m_Light->GetPosition(),
//		m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
//	if (!result)
//	{
//		return false;
//	}
//
//	// Reset the world matrix.
//	m_D3D->GetWorldMatrix(worldMatrix);
//
//	// Setup the translation matrix for the ground model.
//	m_GroundModel->GetPosition(posX, posY, posZ);
//	//D3DXMatrixTranslation(&worldMatrix, posX, posY, posZ);
//	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixTranslation(posX, posY, posZ));
//
//
//	// Render the ground model using the shadow shader.
//	m_GroundModel->Render(m_D3D->GetDeviceContext());
//	result = m_ShadowShader->Render(
//		m_D3D->GetDeviceContext(),
//		m_GroundModel->GetIndexCount(),
//		worldMatrix, viewMatrix, projectionMatrix,
//		lightViewMatrix, lightProjectionMatrix,
//		m_GroundModel->GetTexture(), m_RenderTexture->GetShaderResourceView(), m_Light->GetPosition(),
//		m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
//	if (!result)
//	{
//		return false;
//	}
//
//	// Present the rendered scene to the screen.
//	m_D3D->EndScene();
//
//	return true;
//}

#pragma endregion