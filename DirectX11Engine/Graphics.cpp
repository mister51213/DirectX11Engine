////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphics.h"

using namespace VectorMath;

GraphicsClass::GraphicsClass()
{}

GraphicsClass::GraphicsClass(const GraphicsClass& other)
{}

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd, Scene* pScene)
{
	// D3D CLASS //
	_D3D.reset(new D3DClass);
	if (!_D3D) return false;

	_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);

	// SHADERS //
	_ShaderManager.reset(new ShaderManagerClass);
	_ShaderManager->Initialize(_D3D->GetDevice(), hwnd);

	// CAMERA //
	_Camera.reset(new Camera);
	if (!_Camera)return false;

	//_Camera->SetPosition(pScene->GetCamera()->GetPosition().x, pScene->GetCamera()->GetPosition().y, pScene->GetCamera()->GetPosition().z);
	_Camera->SetPosition(0.0f, 0.0f, -10.0f);
	_Camera->RenderBaseViewMatrix();
	_Camera->UpdateViewPoint();
	//_Camera->SetRotation(pScene->GetCamera()->GetOrientation().x, pScene->GetCamera()->GetOrientation().y, pScene->GetCamera()->GetOrientation().z);

#pragma region MAIN CODE

	// SHADOW RENDER TEXTURES //
	for (int i = 0; i < NUM_SHADOWS; ++i)
	{
		_RenderTextures.push_back(unique_ptr<RenderTextureClass>());
		_RenderTextures[i].reset(new RenderTextureClass);

		if (_RenderTextures[i])
		{
			_RenderTextures[i]->Initialize(_D3D->GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SCREEN_DEPTH, SCREEN_NEAR), "render to texture";
		}
	}

	// WATER RENDER TEXTURES
	//_RefractionTexture.reset(new RenderTextureClass); // TEMP TEST
	//_RefractionTexture->Initialize(_D3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR), "render to texture"; // TEMP TEST
	//_ReflectionTexture.reset(new RenderTextureClass);
	//_ReflectionTexture->Initialize(_D3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR), "render to texture";

	///////////////////////////////////
	// DEFAULT SCENE BACKGROUND
	///////////////////////////////////	
	// EARTH
	_Earth.reset(new Model(XMFLOAT3(250, 400, 250), XMFLOAT3(180, 0, 0), XMFLOAT3(0, -300, 0), "Earth Outer"));
	//_Earth.reset(new Model());
	vector<string> earthtex = { "../DirectX11Engine/data/fire2.dds", "../DirectX11Engine/data/skydome_alpha3.jpg" };
	_Earth->Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(), "../DirectX11Engine/data/skydome_lowpoly.txt",
		earthtex, EShaderType::ETEXTURE);

	// EARTH INNER
	_EarthInner.reset(new Model(.75f * _Earth->GetScale(), _Earth->GetOrientation(), XMFLOAT3(0, -200, 0), "Earth Inner"));
	vector<string> earthtex2 = { "../DirectX11Engine/data/fire.dds", "../DirectX11Engine/data/skydome_alpha5.jpg" };
	_EarthInner->Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(), "../DirectX11Engine/data/skydome_lowpoly.txt",
		earthtex2, EShaderType::ETEXTURE);

	// SKY
	_Sky.reset(new Model(XMFLOAT3(100, 150, 100), XMFLOAT3(0, 0, 0), XMFLOAT3(100, 300, 100), "Sky Outer"));
	vector<string> skytex = { "../DirectX11Engine/data/galaxyblue.jpg", "../DirectX11Engine/data/skydome_alpha2.jpg" };
	_Sky->Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(), "../DirectX11Engine/data/skydome_lowpoly.txt",
		skytex, EShaderType::ETEXTURE);

	// SKY INNNER
	_SkyInner.reset(new Model(.7f * _Sky->GetScale(), XMFLOAT3(0, 0, 0), XMFLOAT3(0, 150, 0), "Sky Inner"));
	vector<string> skytexInner = { "../DirectX11Engine/data/galaxypurple.jpg", "../DirectX11Engine/data/skydome_alpha4.jpg" };
	_SkyInner->Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(), "../DirectX11Engine/data/skydome_lowpoly.txt",
		skytexInner, EShaderType::ETEXTURE);

	///////////////////////////////////
	// DEFAULT SCENE MODELS
	///////////////////////////////////	

	// DEFAULT TEX //
	vector<string> texNames = { "wall01.dds", "marble.png", "metal001.dds", "wall01.dds", "metal001.dds", "metal001.dds", "metal001.dds", "metal001.dds", "metal001.dds" };
	vector<string> meshNames = { "sphere.txt", "cube2.txt", "sphere.txt" };
	vector<string> defaultTex{
		"../DirectX11Engine/data/metal001.dds",
		"../DirectX11Engine/data/dirt.dds",
		"../DirectX11Engine/data/light.dds",
		"../DirectX11Engine/data/alpha.dds",
		"../DirectX11Engine/data/bumpMap.dds", // normal map
		"../DirectX11Engine/data/specMap.dds",
		"../DirectX11Engine/data/specMap.dds",
		"../DirectX11Engine/data/specMap.dds",
		"../DirectX11Engine/data/specMap.dds"
	};

	int i = 0;
	for (map<string, unique_ptr<Actor>>::const_iterator it = pScene->_Actors.begin(); it != pScene->_Actors.end(); ++it)
	{
		// SUPER HACK TO GET TEMPORARY WATER MODELS IN!!!!!!!! //
		if (it->first == "Wall" || it->first == "Bath" || it->first == "Water" || it->first == "Ground")
		{
			continue;
		}

		it->second->InstantiateModel(new Model(it->second->GetScale(), it->second->GetOrientation(), it->second->GetPosition(), it->first));

		vector<string> texArray(9, "../DirectX11Engine/data/" + texNames[i]);
		it->second->GetModel()->Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(), "../DirectX11Engine/data/" + meshNames[i],
			defaultTex, EShaderType::ELIGHT_SPECULAR);

		// Store the render texture in the texture view array of each model to make it accessible to the graphics pipeline
		for (int idx = 0; idx < _RenderTextures.size(); ++idx)
		{
			it->second->GetModel()->SetResourceView(6 + idx, _RenderTextures[idx]->GetShaderResourceView());
		}

		++i;
	}

	///////////////////////////////////
	// REFRACTION / REFLECTION MODELS
	///////////////////////////////////
	pScene->_Actors["Wall"]->InstantiateModel(new Model(pScene->_Actors["Wall"]->GetScale(), XMFLOAT3(), XMFLOAT3(), "Wall"));
	vector<string>wallTex{
		"../DirectX11Engine/data/wall.dds",
		"../DirectX11Engine/data/dirt.dds",
		"../DirectX11Engine/data/light.dds",
		"../DirectX11Engine/data/alpha.dds",
		"../DirectX11Engine/data/bumpMap.dds", // normal map
		"../DirectX11Engine/data/specMap.dds",
		"../DirectX11Engine/data/noise.png",
		"../DirectX11Engine/data/noise.png",
		"../DirectX11Engine/data/noise.png" };
	pScene->_Actors["Wall"]->GetModel()->Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(), "../DirectX11Engine/data/wall.txt", wallTex, EShaderType::ELIGHT_SPECULAR);
	
	// Set Wall Model Shadow textures
	for (int idx = 0; idx < _RenderTextures.size(); ++idx)
	{
		pScene->_Actors["Wall"]->GetModel()->SetResourceView(6 + idx, _RenderTextures[idx]->GetShaderResourceView());
	}
	pScene->_Actors["Bath"]->InstantiateModel(new Model(pScene->_Actors["Bath"]->GetScale(), XMFLOAT3(), XMFLOAT3(), "Bath"));
	vector<string>bathTex{
		"../DirectX11Engine/data/wall.dds",
		"../DirectX11Engine/data/dirt.dds",
		"../DirectX11Engine/data/light.dds",
		"../DirectX11Engine/data/alpha.dds",
		"../DirectX11Engine/data/nMap5.png", // normal map
		"../DirectX11Engine/data/specMap.dds",
		"../DirectX11Engine/data/noise.png",
		"../DirectX11Engine/data/noise.png",
		"../DirectX11Engine/data/noise.png" };
	pScene->_Actors["Bath"]->GetModel()->Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(), "../DirectX11Engine/data/bath.txt", bathTex, EShaderType::ELIGHT_SPECULAR);
	
	// Set Bath Model Shadow textures
	for (int idx = 0; idx < _RenderTextures.size(); ++idx)
	{
		pScene->_Actors["Bath"]->GetModel()->SetResourceView(6 + idx, _RenderTextures[idx]->GetShaderResourceView());
	}

	pScene->_Actors["Ground"]->InstantiateModel(new Model(pScene->_Actors["Ground"]->GetScale(), XMFLOAT3(), XMFLOAT3(), "Ground"));
	vector<string>groundTex{
		"../DirectX11Engine/data/snow.jpg",
		"../DirectX11Engine/data/snow.jpg",
		"../DirectX11Engine/data/light.dds",
		"../DirectX11Engine/data/noise.png",	// alpha map
		"../DirectX11Engine/data/nMap4.png",	// normal map
		"../DirectX11Engine/data/specMap.dds",
		"../DirectX11Engine/data/noise.png",
		"../DirectX11Engine/data/noise.png",
		"../DirectX11Engine/data/noise.png" };

	pScene->_Actors["Ground"]->GetModel()->Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(), "../DirectX11Engine/data/plane01.txt"/*"../DirectX11Engine/data/SnowTerrain_LowPoly.txt"*/, groundTex, EShaderType::ELIGHT_SPECULAR);
	pScene->_Actors["Ground"]->GetModel()->SetScale(XMFLOAT3(3, 3, 3));

	// Set Bath Model Shadow textures
	for (int idx = 0; idx < _RenderTextures.size(); ++idx)
	{
		pScene->_Actors["Ground"]->GetModel()->SetResourceView(6 + idx, _RenderTextures[idx]->GetShaderResourceView());
	}

	pScene->_Actors["Water"]->InstantiateModel(new Model(pScene->_Actors["Water"]->GetScale(), XMFLOAT3(), XMFLOAT3(), "Water"));
	//vector<string> waterTextures{ "../DirectX11Engine/data/water.dds", "../DirectX11Engine/data/water.dds", "../DirectX11Engine/data/water.dds" };
	vector<string> waterTextures{ "../DirectX11Engine/data/water.dds"}; // TEST
	
	pScene->_Actors["Water"]->GetModel()->Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(), "../DirectX11Engine/data/water.txt", waterTextures, EShaderType::EWATER);
	pScene->_Actors["Water"]->GetModel()->GetMaterial()->reflectRefractScale = 0.01f;
	pScene->_Actors["Water"]->GetModel()->GetMaterial()->waterHeight = _waterHeight;// *_waterSceneScale;
	pScene->_Actors["Water"]->GetModel()->GetMaterial()->bAnimated = true;
	//pScene->_Actors["Water"]->GetModel()->SetResourceView(0, _ReflectionTexture->GetShaderResourceView());
	//pScene->_Actors["Water"]->GetModel()->SetResourceView(1, _RefractionTexture->GetShaderResourceView());
	//_RefractionTexture.reset(new RenderTextureClass(_D3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR)); // TEMP TEST
	pScene->_Actors["Water"]->GetModel()->GetMaterial()->AddRenderTexture(_D3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR); // REFRACTION TEXTURE
	pScene->_Actors["Water"]->GetModel()->GetMaterial()->AddRenderTexture(_D3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR); // REFLECTION TEXTURE (CURRENTLY NOT USED!!!)

	///////////////////////////////////
	// INITIALIZE LIGHTS
	///////////////////////////////////
	for (int i = 0; i < pScene->_LightActors.size(); ++i)
	{
		_Lights.push_back(unique_ptr<LightClass>());
		_Lights[i].reset(new LightClass);
		if (!_Lights[i])return false;

		_Lights[i]->SetDiffuseColor(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
		_Lights[i]->SetLookAt(pScene->_LightActors[i]->GetLookAt());
		_Lights[i]->SetPosition(pScene->_LightActors[i]->GetPosition());
		_Lights[i]->GenerateProjectionMatrix(SCREEN_DEPTH, SCREEN_NEAR);
	}

	// Effects
	_sceneEffects.ambientColor = XMFLOAT4(.2, .2, .2, 1);
	_sceneEffects.fogEnd = 0;
	_sceneEffects.fogStart = 0;
	
	// UI
	InitializeUI(screenWidth, screenHeight);
	
#pragma endregion

#pragma region QUARANTINE SECTION
	/////////////// QUARANTINE SECTION ////////////////
	bool result;
	int downSampleWidth, downSampleHeight;
	
	// MODELS
	m_CubeModel.reset(new Model);
	m_CubeModel->Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(), "../DirectX11Engine/data/cube.txt", vector<string>(1, "../DirectX11Engine/data/wall01.dds"));
	m_CubeModel->SetPosition(XMFLOAT3(-2.0f, 2.0f, 0.0f));

	m_SphereModel.reset(new Model);
	m_SphereModel->Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(), "../DirectX11Engine/data/sphere.txt", vector<string>(1, "../DirectX11Engine/data/ice.dds"));
	m_SphereModel->SetPosition(XMFLOAT3(2.0f, 2.0f, 0.0f));

	m_GroundModel.reset(new Model);
	m_GroundModel->Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(), "../DirectX11Engine/data/plane01.txt", vector<string>(1, "../DirectX11Engine/data/metal001.dds"));
	m_GroundModel->SetPosition(XMFLOAT3(0.0f, 1.0f, 0.0f));

	// Create the light object.
	m_SoftLight.reset(new LightClass);
	if (!m_SoftLight)
	{
		return false;
	}

	// Initialize the light object.
	m_SoftLight->SetAmbientColor(XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f));
	m_SoftLight->SetDiffuseColor(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	m_SoftLight->SetLookAt(XMFLOAT3(0.00f, 0.000f, 0.000f));
	//m_SoftLight->SetLookAt(m_GroundModel->GetPosition() - XMFLOAT3(-5,8,0));
	m_SoftLight->GenerateProjectionMatrix(SCREEN_DEPTH, SCREEN_NEAR);

	// Create the render to texture object.
	m_RenderTexture.reset(new RenderTextureClass);
	if (!m_RenderTexture)
	{
		return false;
	}

	// Initialize the render to texture object.
	m_RenderTexture->Initialize(_D3D->GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SCREEN_DEPTH, SCREEN_NEAR);

	m_BlackWhiteRenderTexture.reset(new RenderTextureClass);
	m_BlackWhiteRenderTexture->Initialize(_D3D->GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SCREEN_DEPTH, SCREEN_NEAR);
	m_ShadowShader.reset(new ShadowShaderClass);
	m_ShadowShader->Initialize(_D3D->GetDevice(), hwnd);

	// Set the size to sample down to.
	downSampleWidth = SHADOWMAP_WIDTH / 2;
	downSampleHeight = SHADOWMAP_HEIGHT / 2;

	m_DownSampleTexure.reset(new RenderTextureClass);
	m_DownSampleTexure->Initialize(_D3D->GetDevice(), downSampleWidth, downSampleHeight, 100.0f, 1.0f);
	m_SmallWindow.reset(new OrthoWindowClass);
	result = m_SmallWindow->Initialize(_D3D->GetDevice(), downSampleWidth, downSampleHeight);
	m_HorizontalBlurTexture.reset(new RenderTextureClass);
	result = m_HorizontalBlurTexture->Initialize(_D3D->GetDevice(), downSampleWidth, downSampleHeight, SCREEN_DEPTH, 0.1f);
	m_HorizontalBlurShader.reset(new HorizontalBlurShaderClass);
	result = m_HorizontalBlurShader->Initialize(_D3D->GetDevice(), hwnd);
	m_VerticalBlurTexture.reset(new RenderTextureClass);
	result = m_VerticalBlurTexture->Initialize(_D3D->GetDevice(), downSampleWidth, downSampleHeight, SCREEN_DEPTH, 0.1f);
	m_VerticalBlurShader.reset(new VerticalBlurShaderClass);
	result = m_VerticalBlurShader->Initialize(_D3D->GetDevice(), hwnd);
	m_UpSampleTexure.reset(new RenderTextureClass);
	result = m_UpSampleTexure->Initialize(_D3D->GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SCREEN_DEPTH, 0.1f);
	m_FullScreenWindow.reset(new OrthoWindowClass);
	result = m_FullScreenWindow->Initialize(_D3D->GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT);
	m_SoftShadowShader.reset(new SoftShadowShaderClass);
	m_SoftShadowShader->Initialize(_D3D->GetDevice(), hwnd);
#pragma endregion

	return true;
}

bool GraphicsClass::UpdateFrame(float frameTime, class Scene* pScene, int fps)
{
	// 1. Animate Materials @TODO - initialize models in initialize function
	for (map<string, unique_ptr<Actor>>::const_iterator it = pScene->_Actors.begin(); it != pScene->_Actors.end(); ++it)
	{
		if (it->second->GetModel())
		{
			it->second->GetModel()->GetMaterial()->Animate();
		}
	}

	// WATER DEMO
	if(pScene->_Actors["Water"]->GetModel())
	pScene->_Actors["Water"]->GetModel()->GetMaterial()->Animate();

	// 2. Update Camera
	XMFLOAT3 camPos = pScene->GetCamera()->GetPosition();
	XMFLOAT3 camRot = pScene->GetCamera()->GetOrientation();
	_Camera->SetPosition(camPos.x, camPos.y, camPos.z);
	_Camera->SetRotation(camRot.x, camRot.y, camRot.z);

	// 3. Update lights
	int i = 0;
	for (auto& light : _Lights)
	{
		light->SetPosition(pScene->_LightActors[i]->GetPosition());
		light->SetLookAt(pScene->_LightActors[i]->GetLookAt());
		light->SetDirection(pScene->_LightActors[i]->GetLookAt());
		++i;
	}
	
	// 4. Update ACTORS
	for (map<string, unique_ptr<Actor>>::const_iterator it = pScene->_Actors.begin(); it != pScene->_Actors.end(); ++it)
	{
		it->second->GetModel()->SetOrientation(it->second->GetOrientation());
		it->second->GetModel()->SetPosition(it->second->GetPosition());
		it->second->GetModel()->SetScale(it->second->GetScale());
	}

	// 4. Update UI
	UpdateFpsString(_D3D->GetDeviceContext(), fps);
	UpdatePositionStrings(_D3D->GetDeviceContext(), camPos.x, camPos.y, camPos.z, camRot.x, camRot.y, camRot.z);

	if (!bDrawAlternate)
	{
		// 5. Draw the Actual frame
		DrawFrame(pScene);
	}
	else
	{
		// SOFT SHADOW DEMO
		static float lightPositionX = -5.0f;
		lightPositionX += 0.05f;
		if (lightPositionX > 5.0f)
		{
			lightPositionX = -5.0f;
		}
		m_SoftLight->SetPosition(XMFLOAT3(lightPositionX, 8.0f, -5.0f));
		RenderShadowScene(pScene);
	}

	return true;
}

bool GraphicsClass::DrawFrame(Scene* pScene)
{
	// UPDATE LIGHTS 	//@TODO // AWAY WITH THIS MADNESS! ~ properly loop / hold in the right containers
	XMMATRIX lightViewMatrix[NUM_LIGHTS], lightProjectionMatrix[NUM_LIGHTS];
	for (int i = 0; i < pScene->NUM_LIGHTS; ++i)
	{
		_Lights[i]->GenerateViewMatrix();
		lightViewMatrix[i] = _Lights[i]->GetViewMatrix();
		lightProjectionMatrix[i] = _Lights[i]->GetProjectionMatrix();
	}
	LightClass* lights[3] = { _Lights[0].get(), _Lights[1].get(), _Lights[2].get() };

	// DRAW TO RENDER TEXTURES
	RenderShadowsToTexture(pScene, lights);
	RenderWaterToTexture(pScene, lights);

	// DRAW MAIN SCENE
	_D3D->BeginScene(1.0f, 1.0f, 1.0f, 1.0f);

	XMMATRIX worldTransform;
	XMMATRIX viewMatrix = _Camera->GetViewMatrix();
	XMMATRIX projectionMatrix = _D3D->GetProjectionMatrix();
	MatrixBufferType transforms = { XMMatrixIdentity(), XMMatrixTranspose(_Camera->GetViewMatrix()), XMMatrixTranspose(_D3D->GetProjectionMatrix())};

	// Generate the view matrix based on the camera's position.
	_Camera->UpdateViewPoint();

	////////////////// RENDER ACTUAL SCENE  /////////////////////////

	//////// BACKGROUND /////////
	_D3D->EnableAlphaBlending();
	DrawModel(*_Earth, transforms/*, worldTransform, viewMatrix, projectionMatrix*/);
	DrawModel(*_EarthInner, transforms/*, worldTransform, viewMatrix, projectionMatrix*/);
	DrawModel(*_Sky, transforms/*, worldTransform, viewMatrix, projectionMatrix*/);
	DrawModel(*_SkyInner, transforms/*, worldTransform, viewMatrix, projectionMatrix*/);
	_D3D->DisableAlphaBlending();

	//////// OTHER OBJECTS /////////
	for (map<string, unique_ptr<Actor>>::const_iterator it = pScene->_Actors.begin(); it != pScene->_Actors.end(); ++it)
	{
		// SUPER HACK TO GET TEMPORARY WATER MODELS IN!!!!!!!! //
		if (it->first == "Water" || !it->second->GetModel()) continue;

		//_D3D->EnableAlphaBlending();
		DrawModel(*it->second->GetModel(), transforms,/*, worldTransform, viewMatrix, projectionMatrix, */lights);
		//_D3D->DisableAlphaBlending();
	}
	
	////////// RENDER WATER //////////////
	if (pScene->_Actors["Water"]->GetModel())
	{
		DrawModel(*pScene->_Actors["Water"]->GetModel(), transforms/*, worldTransform, viewMatrix, projectionMatrix*/, lights, EMATERIAL_DEFAULT, _Camera->GetReflectionViewMatrix());
	}
	
	///////////////// UI ///////////////////
	RenderText();

	// Present the rendered scene to the screen.
	_D3D->EndScene();

	return true;
}

#pragma region NEW LOGIC
void GraphicsClass::RenderShadowScene(Scene* pScene)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, translateMatrix;
	float posX, posY, posZ;

	RenderShadows(pScene);
	ID3D11ShaderResourceView* blurredShadows = ApplyBlur(m_BlackWhiteRenderTexture->GetShaderResourceView(), m_UpSampleTexure.get());

	// Clear the buffers to begin the scene.
	_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	_Camera->UpdateViewPoint();
	viewMatrix = _Camera->GetViewMatrix();

	worldMatrix = _D3D->GetWorldMatrix();
	projectionMatrix = _D3D->GetProjectionMatrix();
	XMFLOAT3 cubePos = m_CubeModel->GetPosition();
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixTranslation(cubePos.x, cubePos.y, cubePos.z));
	m_CubeModel->PutVerticesOnPipeline(_D3D->GetDeviceContext());
	m_SoftShadowShader->Render(_D3D->GetDeviceContext(), m_CubeModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_CubeModel->GetMaterial()->GetResourceArray()[0],blurredShadows,
		m_SoftLight->GetPosition(), _sceneEffects.ambientColor, m_SoftLight->GetDiffuseColor());
	m_CubeModel->Draw(_D3D->GetDeviceContext());

	// Reset the world matrix.
	worldMatrix = _D3D->GetWorldMatrix();
	XMFLOAT3 sphPos = m_SphereModel->GetPosition();
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixTranslation(sphPos.x, sphPos.y, sphPos.z));
	m_SphereModel->PutVerticesOnPipeline(_D3D->GetDeviceContext());
	m_SoftShadowShader->Render(_D3D->GetDeviceContext(), m_SphereModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_SphereModel->GetMaterial()->GetResourceArray()[0],blurredShadows, 
		m_SoftLight->GetPosition(), _sceneEffects.ambientColor, m_SoftLight->GetDiffuseColor());
	m_SphereModel->Draw(_D3D->GetDeviceContext());

	// Reset the world matrix.
	worldMatrix = _D3D->GetWorldMatrix();
	XMFLOAT3 gPos = m_GroundModel->GetPosition();
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixTranslation(gPos.x, gPos.y, gPos.z));
	m_GroundModel->PutVerticesOnPipeline(_D3D->GetDeviceContext());
	m_SoftShadowShader->Render(_D3D->GetDeviceContext(), m_GroundModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_GroundModel->GetMaterial()->GetResourceArray()[0], blurredShadows,
		m_SoftLight->GetPosition(), _sceneEffects.ambientColor, m_SoftLight->GetDiffuseColor());
	m_GroundModel->Draw(_D3D->GetDeviceContext());

	_D3D->EndScene();
}

ID3D11ShaderResourceView* GraphicsClass::ApplyBlur(ID3D11ShaderResourceView* viewToBlur, RenderTextureClass* outputRenderTarget)
{
	XMMATRIX worldMatrix = _D3D->GetWorldMatrix();
	XMMATRIX baseViewMatrix, orthoMatrix;

	_Camera->UpdateViewPoint();
	_D3D->TurnZBufferOff();
	_Camera->GetBaseViewMatrix(baseViewMatrix);

	// Tex 1
	m_DownSampleTexure->SetRenderTarget(_D3D->GetDeviceContext());
	m_DownSampleTexure->ClearRenderTarget(_D3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f); // REALLY UNNECESSARY???
	m_DownSampleTexure->GetOrthoMatrix(orthoMatrix);
	m_SmallWindow->PutVerticesOnPipeline(_D3D->GetDeviceContext());
	_ShaderManager->_TextureShader->Render(_D3D->GetDeviceContext(), m_SmallWindow->GetIndexCount(),
		MatrixBufferType(XMMatrixTranspose(worldMatrix), XMMatrixTranspose(baseViewMatrix), XMMatrixTranspose(orthoMatrix)),
		&viewToBlur, 
		vector<ComPtr<ID3D11ShaderResourceView>>(1, ComPtr<ID3D11ShaderResourceView>(viewToBlur)));
	m_SmallWindow->Draw(_D3D->GetDeviceContext());

	// Tex 2
	m_HorizontalBlurTexture->SetRenderTarget(_D3D->GetDeviceContext());
	m_HorizontalBlurShader->Render(_D3D->GetDeviceContext(), m_SmallWindow->GetIndexCount(), worldMatrix, baseViewMatrix, orthoMatrix,
		m_DownSampleTexure->GetShaderResourceView(), (float)(SHADOWMAP_WIDTH * 0.5f));
	m_SmallWindow->Draw(_D3D->GetDeviceContext());

	// Tex 3
	m_VerticalBlurTexture->SetRenderTarget(_D3D->GetDeviceContext());
	m_VerticalBlurShader->Render(_D3D->GetDeviceContext(), m_SmallWindow->GetIndexCount(), worldMatrix, baseViewMatrix, orthoMatrix,
		m_HorizontalBlurTexture->GetShaderResourceView(), (float)(SHADOWMAP_HEIGHT * 0.5f));
	m_SmallWindow->Draw(_D3D->GetDeviceContext());

	// Tex 4
	outputRenderTarget->SetRenderTarget(_D3D->GetDeviceContext());
	outputRenderTarget->GetOrthoMatrix(orthoMatrix);
	ID3D11ShaderResourceView* tempView = m_VerticalBlurTexture->GetShaderResourceView();
	m_FullScreenWindow->PutVerticesOnPipeline(_D3D->GetDeviceContext());
	_ShaderManager->_TextureShader->Render(_D3D->GetDeviceContext(), m_FullScreenWindow->GetIndexCount(), 
		MatrixBufferType(XMMatrixTranspose(worldMatrix), XMMatrixTranspose(baseViewMatrix), XMMatrixTranspose(orthoMatrix)),
		&tempView,
		vector<ComPtr<ID3D11ShaderResourceView>>(1, ComPtr<ID3D11ShaderResourceView>(m_VerticalBlurTexture->GetShaderResourceView())));
	m_FullScreenWindow->Draw(_D3D->GetDeviceContext());

	_D3D->TurnZBufferOn();
	_D3D->SetBackBufferRenderTarget();
	_D3D->ResetViewport();

	return outputRenderTarget->GetShaderResourceView();
}

void GraphicsClass::RenderShadows(Scene* pScene)
{
	//ProjectDepthToTexture//ProjectDepthToTexture//ProjectDepthToTexture//ProjectDepthToTexture
	//ProjectDepthToTexture//ProjectDepthToTexture//ProjectDepthToTexture//ProjectDepthToTexture
	//ProjectDepthToTexture//ProjectDepthToTexture//ProjectDepthToTexture//ProjectDepthToTexture
	XMMATRIX worldMatrix, lightViewMatrix, lightProjectionMatrix, translateMatrix;

	// Set the render target to be the render to texture.
	m_RenderTexture->SetRenderTarget(_D3D->GetDeviceContext());
	m_RenderTexture->ClearRenderTarget(_D3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the light view matrix based on the light's position.
	m_SoftLight->GenerateViewMatrix();
	worldMatrix = _D3D->GetWorldMatrix();
	lightViewMatrix = m_SoftLight->GetViewMatrix();
	lightProjectionMatrix = m_SoftLight->GetProjectionMatrix();

	// Setup the translation matrix for the cube model.
	XMFLOAT3 cubePos = m_CubeModel->GetPosition();
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixTranslation(cubePos.x, cubePos.y, cubePos.z));
	MatrixBufferType cubeTransforms = { XMMatrixTranspose(worldMatrix), XMMatrixTranspose(lightViewMatrix), XMMatrixTranspose(lightProjectionMatrix) };
	m_CubeModel->PutVerticesOnPipeline(_D3D->GetDeviceContext());
	_ShaderManager->_DepthShader->Render(_D3D->GetDeviceContext(), m_CubeModel->GetIndexCount(), cubeTransforms);
	m_CubeModel->Draw(_D3D->GetDeviceContext());

	// Reset the world matrix.
	worldMatrix = _D3D->GetWorldMatrix();
	XMFLOAT3 sphPos = m_SphereModel->GetPosition();
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixTranslation(sphPos.x, sphPos.y, sphPos.z));
	MatrixBufferType sphTransforms = { XMMatrixTranspose(worldMatrix), XMMatrixTranspose(lightViewMatrix), XMMatrixTranspose(lightProjectionMatrix) };
	m_SphereModel->PutVerticesOnPipeline(_D3D->GetDeviceContext());
	_ShaderManager->_DepthShader->Render(_D3D->GetDeviceContext(), m_SphereModel->GetIndexCount(), sphTransforms);
	m_SphereModel->Draw(_D3D->GetDeviceContext());

	// Reset the world matrix.
	worldMatrix = _D3D->GetWorldMatrix();
	XMFLOAT3 gPos = m_GroundModel->GetPosition();
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixTranslation(gPos.x, gPos.y, gPos.z));
	MatrixBufferType gTransforms = { XMMatrixTranspose(worldMatrix), XMMatrixTranspose(lightViewMatrix), XMMatrixTranspose(lightProjectionMatrix) };
	m_GroundModel->PutVerticesOnPipeline(_D3D->GetDeviceContext());
	_ShaderManager->_DepthShader->Render(_D3D->GetDeviceContext(), m_GroundModel->GetIndexCount(), gTransforms);
	m_GroundModel->Draw(_D3D->GetDeviceContext());

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	_D3D->SetBackBufferRenderTarget();
	_D3D->ResetViewport();

	//RenderBlackAndWhiteShadows//RenderBlackAndWhiteShadows//RenderBlackAndWhiteShadows//RenderBlackAndWhiteShadows
	//RenderBlackAndWhiteShadows//RenderBlackAndWhiteShadows//RenderBlackAndWhiteShadows//RenderBlackAndWhiteShadows
	//RenderBlackAndWhiteShadows//RenderBlackAndWhiteShadows//RenderBlackAndWhiteShadows//RenderBlackAndWhiteShadows
	XMMATRIX viewMatrix, projectionMatrix;

	// Set the render target to be the render to texture.
	m_BlackWhiteRenderTexture->SetRenderTarget(_D3D->GetDeviceContext());
	m_BlackWhiteRenderTexture->ClearRenderTarget(_D3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);
	_Camera->UpdateViewPoint();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	viewMatrix = _Camera->GetViewMatrix();
	worldMatrix = _D3D->GetWorldMatrix();
	projectionMatrix = _D3D->GetProjectionMatrix();

	// Get the light's view and projection matrices from the light object.
	lightProjectionMatrix = m_SoftLight->GetProjectionMatrix();

	// Setup the translation matrix for the cube model.
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixTranslation(cubePos.x, cubePos.y, cubePos.z));
	m_CubeModel->PutVerticesOnPipeline(_D3D->GetDeviceContext());
	m_ShadowShader->Render(_D3D->GetDeviceContext(), m_CubeModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix,
		lightProjectionMatrix, m_RenderTexture->GetShaderResourceView(), m_SoftLight->GetPosition());
	m_CubeModel->Draw(_D3D->GetDeviceContext());

	// Reset the world matrix.
	worldMatrix =_D3D->GetWorldMatrix();
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixTranslation(sphPos.x, sphPos.y, sphPos.z));
	m_SphereModel->PutVerticesOnPipeline(_D3D->GetDeviceContext());
	m_ShadowShader->Render(_D3D->GetDeviceContext(), m_SphereModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix,
		lightProjectionMatrix, m_RenderTexture->GetShaderResourceView(), m_SoftLight->GetPosition());
	m_SphereModel->Draw(_D3D->GetDeviceContext());

	// Reset the world matrix.
	worldMatrix = _D3D->GetWorldMatrix();
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixTranslation(gPos.x, gPos.y, gPos.z));
	m_GroundModel->PutVerticesOnPipeline(_D3D->GetDeviceContext());
	m_ShadowShader->Render(_D3D->GetDeviceContext(), m_GroundModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix,
		lightProjectionMatrix, m_RenderTexture->GetShaderResourceView(), m_SoftLight->GetPosition());
	m_GroundModel->Draw(_D3D->GetDeviceContext());

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	_D3D->SetBackBufferRenderTarget();

	// Reset the viewport back to the original.
	_D3D->ResetViewport();
}
#pragma endregion

bool GraphicsClass::RenderShadowsToTexture(Scene* pScene, LightClass* lights[])
{
	XMMATRIX worldMatrix, lightViewMatrix, lightProjectionMatrix;
	float posX, posY, posZ;
	bool result;

	for (int i = 0; i < _RenderTextures.size()/*3*/; ++i)
	{
		_RenderTextures[i]->SetRenderTarget(_D3D->GetDeviceContext());
		_RenderTextures[i]->ClearRenderTarget(_D3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

		MatrixBufferType matBuffer(XMMatrixIdentity(), XMMatrixTranspose(_Lights[i]->GetViewMatrix()), XMMatrixTranspose(_Lights[i]->GetProjectionMatrix()));

		// Draw all actors in scene to this render texture
		for (map<string, unique_ptr<Actor>>::const_iterator it = pScene->_Actors.begin(); it != pScene->_Actors.end(); ++it)
		{
			// SUPER HACK TO GET TEMPORARY WATER MODELS IN!!!!!!!! //
			if (it->first == "Water" || !it->second->GetModel())
			{
				continue;
			}
			DrawModel(*it->second->GetModel(), matBuffer,/*, worldMatrix, _Lights[i]->GetViewMatrix(), _Lights[i]->GetProjectionMatrix(), */nullptr, EDEPTH);
		}
	}

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	_D3D->SetBackBufferRenderTarget();
	_D3D->ResetViewport();

	return true;
}

void GraphicsClass::DrawModel(Model& model, MatrixBufferType& transforms, /*XMMATRIX &worldTransform, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix, */LightClass * lights[], EShaderType shaderType, XMMATRIX reflectionMatrix)
{
	// NOTE - this transposes it BEFORE sending it in!!!!
	transforms.world = XMMatrixTranspose(_D3D->GetWorldMatrix()*ComputeWorldTransform(model.GetOrientation(), model.GetScale(), model.GetPosition()));
	model.PutVerticesOnPipeline(_D3D->GetDeviceContext());
	_ShaderManager->Render(_D3D->GetDeviceContext(), model.GetIndexCount(), transforms,	model.GetMaterial(), lights, _sceneEffects, _Camera->GetPosition(), shaderType, reflectionMatrix);
	model.Draw(_D3D->GetDeviceContext());
}

bool GraphicsClass::RenderWaterToTexture(Scene* pScene, LightClass* lights[])
{
	//////////////
	// REFRACTION
	//////////////
	if (!pScene->_Actors["Bath"]->GetModel()) return false;

	pScene->_Actors["Water"]->GetModel()->GetMaterial()->_RenderTextures[0]->SetRenderTarget(_D3D->GetDeviceContext());
	pScene->_Actors["Water"]->GetModel()->GetMaterial()->_RenderTextures[0]->ClearRenderTarget(_D3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	//_RefractionTexture->SetRenderTarget(_D3D->GetDeviceContext());
	//_RefractionTexture->ClearRenderTarget(_D3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);
	//_sceneEffects.clipPlane = XMFLOAT4(0.0f, -1.0f, 0.0f, _waterHeight);
	_sceneEffects.clipPlane = XMFLOAT4(0,0,0,0);

	// Get the world, view, and projection matrices from the camera and d3d objects.
	_Camera->UpdateViewPoint();

	XMMATRIX worldMatrix;
	XMMATRIX viewMatrix = _Camera->GetViewMatrix();
	XMMATRIX projectionMatrix = _D3D->GetProjectionMatrix();

	// NEW REFACTOR
	MatrixBufferType transforms = { XMMatrixIdentity(), XMMatrixTranspose(_Camera->GetViewMatrix()), XMMatrixTranspose(_D3D->GetProjectionMatrix()) };
	// NEW REFACTOR
	
	//////// BACKGROUND /////////
	DrawModel(*pScene->_Actors["Bath"]->GetModel(), transforms, /*worldMatrix, viewMatrix, projectionMatrix, */lights, EShaderType::ELIGHT_SPECULAR);
	// TODO: DRAW OTHER STUFF AND USE IT AS A REFRACTION GLOBE
	//_D3D->EnableAlphaBlending();
	//DrawModel(*_Earth, transforms/*, worldTransform, viewMatrix, projectionMatrix*/);
	//DrawModel(*_EarthInner, transforms/*, worldTransform, viewMatrix, projectionMatrix*/);
	//DrawModel(*_Sky, transforms/*, worldTransform, viewMatrix, projectionMatrix*/);
	//DrawModel(*_SkyInner, transforms/*, worldTransform, viewMatrix, projectionMatrix*/);
	//_D3D->DisableAlphaBlending();

#pragma region REFLECTION DEACTIVATED
	////////////////
	//// REFLECTION
	////////////////
	//if (!pScene->_Actors["Wall"]->GetModel()) return false;

	//XMMATRIX reflectionViewMatrix;
	//
	//// Set the render target to be the reflection render to texture.
	//_ReflectionTexture->SetRenderTarget(_D3D->GetDeviceContext());

	//// Clear the reflection render to texture.
	//_ReflectionTexture->ClearRenderTarget(_D3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	//// Use the camera to render the reflection and create a reflection view matrix.
	////_Camera->RenderReflection(2.75f);

	//// Get the camera reflection view matrix instead of the normal view matrix.
	//reflectionViewMatrix = _Camera->GetReflectionViewMatrix();

	//// Get the world and projection matrices from the d3d object.
	//projectionMatrix = _D3D->GetProjectionMatrix();
	//worldMatrix = _D3D->GetWorldMatrix();
	//worldMatrix *= ComputeWorldTransform(pScene->_Actors["Wall"]->GetOrientation(), pScene->_Actors["Wall"]->GetScale(), pScene->_Actors["Wall"]->GetPosition());
	//pScene->_Actors["Wall"]->GetModel()->Draw(_D3D->GetDeviceContext());

	//_ShaderManager->_WaterShader->Render(_D3D->GetDeviceContext(), pScene->_Actors["Wall"]->GetModel()->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, reflectionViewMatrix,
	//	pScene->_Actors["Wall"]->GetModel()->GetMaterial()->GetResourceArray(), pScene->_Actors["Wall"]->GetModel()->GetMaterial()->translation, pScene->_Actors["Wall"]->GetModel()->GetMaterial()->reflectRefractScale);

	// Reset the render target back to the original back buffer and not the render to texture anymore.
#pragma endregion

	_sceneEffects.clipPlane = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.f);

	_D3D->SetBackBufferRenderTarget();

	return true;
}


#pragma region UI

bool GraphicsClass::InitializeUI(int screenWidth, int screenHeight)
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
	_Font1->Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(), "../DirectX11Engine/data/font.txt",	"../DirectX11Engine/data/font.tga", 32.0f, 3);

	// Create the text object for the fps string.
	_FpsString.reset(new TextClass);
	if (!_FpsString)
	{
		return false;
	}

	// Initialize the fps text string.
	_FpsString->Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(), screenWidth, screenHeight, 16, false, _Font1.get(),
		"Fps: 0", 10, 50, 0.0f, 1.0f, 0.0f);

	// Initial the previous frame fps.
	_previousFps = -1;

	// Initialize the position text strings.
	vector<char*> labels = { "X: 0", "Y: 0", "Z: 0", "rX: 0", "rY: 0", "rZ: 0" };
	char offset = 0;
	for (int i = 0; i < 6; ++i)
	{
		_PositionStrings.push_back(unique_ptr<TextClass>(new TextClass()));
		if (!_PositionStrings[i]) return false;

		_PositionStrings[i]->Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(), screenWidth, screenHeight, 16, false, _Font1.get(),
			labels[i], 10, 310 + offset, 1.0f, 1.0f, 1.0f);
		
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

		_RenderCountStrings[i]->Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(), screenWidth, screenHeight, 32, false, _Font1.get(), renderLabels[i], 10, 260 + offset, 1.0f, 1.0f, 1.0f);

		offset += 20;
	}
	
	return true;
}

bool GraphicsClass::UpdateFpsString(ID3D11DeviceContext* deviceContext, int fps)
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
	_FpsString->UpdateSentence(deviceContext, _Font1.get(), finalString, 10, 50, red, green, blue);

	return true;
}

bool GraphicsClass::UpdatePositionStrings(ID3D11DeviceContext* deviceContext, float posX, float posY, float posZ,
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
		_PositionStrings[i]->UpdateSentence(deviceContext, _Font1.get(), finalString, 10, 100 + offset, 1.0f, 1.0f, 1.0f);

		offset += 20;
	}

	return true;
}

bool GraphicsClass::UpdateRenderCounts(ID3D11DeviceContext* deviceContext, int renderCount, int nodesDrawn, int nodesCulled)
{
	char tempString[32];
	char finalString[32];
	
	// Convert the render count integer to string format.
	_itoa_s(renderCount, tempString, 10);

	// Setup the render count string.
	strcpy_s(finalString, "Polys Drawn: ");
	strcat_s(finalString, tempString);

	// Update the sentence vertex buffer with the new string information.
	_RenderCountStrings[0]->UpdateSentence(deviceContext, _Font1.get(), finalString, 10, 260, 1.0f, 1.0f, 1.0f);

	// Convert the cells drawn integer to string format.
	_itoa_s(nodesDrawn, tempString, 10);

	// Setup the cells drawn string.
	strcpy_s(finalString, "Cells Drawn: ");
	strcat_s(finalString, tempString);

	// Update the sentence vertex buffer with the new string information.
	_RenderCountStrings[1]->UpdateSentence(deviceContext, _Font1.get(), finalString, 10, 280, 1.0f, 1.0f, 1.0f);

	// Convert the cells culled integer to string format.
	_itoa_s(nodesCulled, tempString, 10);

	// Setup the cells culled string.
	strcpy_s(finalString, "Cells Culled: ");
	strcat_s(finalString, tempString);

	// Update the sentence vertex buffer with the new string information.
	_RenderCountStrings[2]->UpdateSentence(deviceContext, _Font1.get(), finalString, 10, 300, 1.0f, 1.0f, 1.0f);

	return true;
}

void GraphicsClass::RenderText()
{
	_D3D->TurnZBufferOff();
	_D3D->EnableAlphaBlending();

	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	worldMatrix = _D3D->GetWorldMatrix();
	_Camera->GetBaseViewMatrix(baseViewMatrix);
	_D3D->GetOrthoMatrix(orthoMatrix);

	// NEW IMPLEMENTATION
	MatrixBufferType transforms = { XMMatrixTranspose(worldMatrix), XMMatrixTranspose(baseViewMatrix), XMMatrixTranspose(orthoMatrix) };
	// NEW IMPLEMENTATION

	_FpsString->Render(_D3D->GetDeviceContext(), _ShaderManager.get(), transforms,/*worldMatrix, baseViewMatrix, orthoMatrix, */_Font1->GetTexture(), _Font1->GetTextureObject()->_textureViews);
	for (int i = 0; i < 6; i++)
	{
		_PositionStrings[i]->Render(_D3D->GetDeviceContext(), _ShaderManager.get(), transforms, /*worldMatrix, baseViewMatrix, orthoMatrix,*/ _Font1->GetTexture(), _Font1->GetTextureObject()->_textureViews);
	}
	for (int i = 0; i < 3; i++)
	{
		_RenderCountStrings[i]->Render(_D3D->GetDeviceContext(), _ShaderManager.get(), transforms,/*worldMatrix, baseViewMatrix, orthoMatrix, */_Font1->GetTexture(), _Font1->GetTextureObject()->_textureViews);
	}

	_D3D->DisableAlphaBlending();
	_D3D->TurnZBufferOn(); // Turn the Z buffer back on now that all 2D rendering has completed.
}

#pragma endregion UI
