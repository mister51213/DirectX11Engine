////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphics.h"

using namespace VectorMath;

GraphicsClass::GraphicsClass()
{}

GraphicsClass::GraphicsClass(const GraphicsClass& other)
{}

GraphicsClass::~GraphicsClass()
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

	_Camera->SetPosition(pScene->GetCamera()->GetPosition().x, pScene->GetCamera()->GetPosition().y, pScene->GetCamera()->GetPosition().z);
	_Camera->UpdateViewPoint();
	//_Camera->SetRotation(pScene->GetCamera()->GetOrientation().x, pScene->GetCamera()->GetOrientation().y, pScene->GetCamera()->GetOrientation().z);

	// SHADOW RENDER TEXTURES //
	for (int i = 0; i < NUM_RENDER_TEXTURES; ++i)
	{
		_RenderTextures.push_back(unique_ptr<RenderTextureClass>());
		_RenderTextures[i].reset(new RenderTextureClass);

		if (_RenderTextures[i])
		{
			_RenderTextures[i]->Initialize(_D3D->GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SCREEN_DEPTH, SCREEN_NEAR), "render to texture";
		}
	}

	// WATER RENDER TEXTURES
	_RefractionTexture.reset(new RenderTextureClass);
	_RefractionTexture->Initialize(_D3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR), "render to texture";
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
	//_EarthInner.reset(new Model());
	vector<string> earthtex2 = { "../DirectX11Engine/data/fire.dds", "../DirectX11Engine/data/skydome_alpha5.jpg" };
	_EarthInner->Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(), "../DirectX11Engine/data/skydome_lowpoly.txt",
		earthtex2, EShaderType::ETEXTURE);

	// SKY
	_Sky.reset(new Model(XMFLOAT3(100, 150, 100), XMFLOAT3(0, 0, 0), XMFLOAT3(100, 300, 100), "Sky Outer"));
	//_Sky.reset(new Model());
	vector<string> skytex = { "../DirectX11Engine/data/galaxyblue.jpg", "../DirectX11Engine/data/skydome_alpha2.jpg" };
	_Sky->Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(), "../DirectX11Engine/data/skydome_lowpoly.txt",
		skytex, EShaderType::ETEXTURE);

	// SKY INNNER
	_SkyInner.reset(new Model(.7f * _Sky->GetScale(), XMFLOAT3(0, 0, 0), XMFLOAT3(0, 150, 0), "Sky Inner"));
	//_SkyInner.reset(new Model());
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
			it->second->GetModel()->SetResourceView(6+idx, _RenderTextures[idx]->GetShaderResourceView());
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
	vector<string> waterTextures{ "../DirectX11Engine/data/water.dds", "../DirectX11Engine/data/water.dds", "../DirectX11Engine/data/water.dds" };
	
	pScene->_Actors["Water"]->GetModel()->Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(), "../DirectX11Engine/data/water.txt", waterTextures, EShaderType::EWATER);
	pScene->_Actors["Water"]->GetModel()->GetMaterial()->reflectRefractScale = 0.01f;
	pScene->_Actors["Water"]->GetModel()->GetMaterial()->waterHeight = _waterHeight;// *_waterSceneScale;
	pScene->_Actors["Water"]->GetModel()->GetMaterial()->bAnimated = true;
	//pScene->_Actors["Water"]->GetModel()->SetResourceView(0, _ReflectionTexture->GetShaderResourceView());
	pScene->_Actors["Water"]->GetModel()->SetResourceView(1, _RefractionTexture->GetShaderResourceView());

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

	// 5. Draw the Actual frame
	Render(pScene);

	return true;
}

bool GraphicsClass::Render(Scene* pScene)
{
	// DRAW TO RENDER TEXTURES
	RenderShadowsToTexture(pScene);
	RenderWaterToTexture(pScene);

	// UPDATE LIGHTS
	XMMATRIX lightViewMatrix[NUM_LIGHTS], lightProjectionMatrix[NUM_LIGHTS];
	for (int i = 0; i < pScene->NUM_LIGHTS; ++i)
	{
		_Lights[i]->GenerateViewMatrix();
		lightViewMatrix[i] = _Lights[i]->GetViewMatrix();
		lightProjectionMatrix[i] = _Lights[i]->GetProjectionMatrix();
	}

	//@TODO // AWAY WITH THIS MADNESS! ~ properly loop / hold in the right containers
	LightClass* lights[3] = { _Lights[0].get(), _Lights[1].get(), _Lights[2].get() };

	// DRAW MAIN SCENE
	_D3D->BeginScene(1.0f, 1.0f, 1.0f, 1.0f);

	XMMATRIX worldTransform;
	XMMATRIX viewMatrix = _Camera->GetViewMatrix();
	XMMATRIX projectionMatrix = _D3D->GetProjectionMatrix();

	// Generate the view matrix based on the camera's position.
	_Camera->UpdateViewPoint();

	//////// BACKGROUND /////////
	_D3D->EnableAlphaBlending();

	// DRAW EARTH
	DrawModel(*_Earth, worldTransform, viewMatrix, projectionMatrix);
	//_Earth->PutVertsOnPipeline(_D3D->GetDeviceContext());
	//worldTransform = _D3D->GetWorldMatrix()*ComputeWorldTransform(_Earth->GetOrientation(), _Earth->GetScale(), _Earth->GetPosition());
	//_ShaderManager->_TextureShader->Render(_D3D->GetDeviceContext(), _Earth->GetIndexCount(), worldTransform, viewMatrix, projectionMatrix, _Earth->GetMaterial()->GetResourceArray());

	// DRAW EARTH INNER
	DrawModel(*_EarthInner, worldTransform, viewMatrix, projectionMatrix);
	//_EarthInner->PutVertsOnPipeline(_D3D->GetDeviceContext());
	//worldTransform = _D3D->GetWorldMatrix()*ComputeWorldTransform(_EarthInner->GetOrientation(), _EarthInner->GetScale(), _EarthInner->GetPosition());
	//_ShaderManager->_TextureShader->Render(_D3D->GetDeviceContext(), _EarthInner->GetIndexCount(), worldTransform, viewMatrix, projectionMatrix, _EarthInner->GetMaterial()->GetResourceArray());
	
	// DRAW SKY
	DrawModel(*_Sky, worldTransform, viewMatrix, projectionMatrix);
	//_Sky->PutVertsOnPipeline(_D3D->GetDeviceContext());
	//worldTransform = _D3D->GetWorldMatrix()*ComputeWorldTransform(_Sky->GetOrientation(), _Sky->GetScale(), _Sky->GetPosition());
	//_ShaderManager->_TextureShader->Render(_D3D->GetDeviceContext(), _Sky->GetIndexCount(), worldTransform, viewMatrix, projectionMatrix,	_Sky->GetMaterial()->GetResourceArray());
	
	// DRAW INNER SKY
	DrawModel(*_SkyInner, worldTransform, viewMatrix, projectionMatrix);
	//_SkyInner->PutVertsOnPipeline(_D3D->GetDeviceContext());
	//worldTransform = _D3D->GetWorldMatrix()*ComputeWorldTransform(_SkyInner->GetOrientation(), _SkyInner->GetScale(), _SkyInner->GetPosition());
	//_ShaderManager->_TextureShader->Render(_D3D->GetDeviceContext(), _SkyInner->GetIndexCount(), worldTransform, viewMatrix, projectionMatrix, _SkyInner->GetMaterial()->GetResourceArray());
	
	_D3D->DisableAlphaBlending();

	////////////////// RENDER ACTUAL SCENE  /////////////////////////
	for (map<string, unique_ptr<Actor>>::const_iterator it = pScene->_Actors.begin(); it != pScene->_Actors.end(); ++it)
	{
		// SUPER HACK TO GET TEMPORARY WATER MODELS IN!!!!!!!! //
		if (it->first == "Water" || !it->second->GetModel()) continue;

		//_D3D->EnableAlphaBlending();
		DrawModel(*it->second->GetModel(), worldTransform, viewMatrix, projectionMatrix, lights);
		//_D3D->DisableAlphaBlending();
	}
	
	//////////// RENDER WATER //////////////
	if (pScene->_Actors["Water"]->GetModel())
	{
		DrawModel(*pScene->_Actors["Water"]->GetModel(), worldTransform, viewMatrix, projectionMatrix, lights, EMATERIAL_DEFAULT, _Camera->GetReflectionViewMatrix());

		//pScene->_Actors["Water"]->GetModel()->PutVertsOnPipeline(_D3D->GetDeviceContext());
		//worldTransform = _D3D->GetWorldMatrix()*ComputeWorldTransform(pScene->_Actors["Water"]->GetOrientation(), pScene->_Actors["Water"]->GetScale(), pScene->_Actors["Water"]->GetPosition());
		//XMMATRIX reflectionMatrix = _Camera->GetReflectionViewMatrix();
		//_ShaderManager->_WaterShader->Render(
		//	_D3D->GetDeviceContext(), pScene->_Actors["Water"]->GetModel()->GetIndexCount(), worldTransform, viewMatrix,projectionMatrix, reflectionMatrix, pScene->_Actors["Water"]->GetModel()->GetMaterial()->GetTextureObject()->GetTextureArray(), pScene->_Actors["Water"]->GetModel()->GetMaterial()->translation, 0.01f);
	}
	
	///////////////// UI ///////////////////
	RenderText();

	// Present the rendered scene to the screen.
	_D3D->EndScene();

	return true;
}

void GraphicsClass::DrawModel(Model& model, DirectX::XMMATRIX &worldTransform, const DirectX::XMMATRIX &viewMatrix, const DirectX::XMMATRIX &projectionMatrix, LightClass * lights[], EShaderType shaderType, XMMATRIX reflectionMatrix)
{
	model.PutVertsOnPipeline(_D3D->GetDeviceContext());

	worldTransform = _D3D->GetWorldMatrix()*ComputeWorldTransform(model.GetOrientation(), model.GetScale(), model.GetPosition());

	_ShaderManager->Render(_D3D->GetDeviceContext(), model.GetIndexCount(), worldTransform, viewMatrix, projectionMatrix,
		model.GetMaterial(), lights, _sceneEffects, _Camera->GetPosition(), shaderType, reflectionMatrix);

	//_ShaderManager->_LightShader->Render(
	//	_D3D->GetDeviceContext(), model.GetIndexCount(), worldTransform, viewMatrix, projectionMatrix,
	//	model.GetMaterial()->GetResourceArray(),_sceneEffects.ambientColor, lights, _Camera->GetPosition(),
	//	_sceneEffects.fogStart, _sceneEffects.fogEnd, model.GetMaterial()->translation, model.GetMaterial()->transparency);

	//_D3D->GetDeviceContext()->DrawIndexed(model.GetIndexCount(), 0, 0);
}

bool GraphicsClass::RenderShadowsToTexture(Scene* pScene)
{
	XMMATRIX worldMatrix, lightViewMatrix, lightProjectionMatrix;
	float posX, posY, posZ;
	bool result;

	for (int i = 0; i < _RenderTextures.size()/*3*/; ++i)
	{
		_RenderTextures[i]->SetRenderTarget(_D3D->GetDeviceContext());
		_RenderTextures[i]->ClearRenderTarget(_D3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

		_Lights[i]->GenerateViewMatrix();
		lightViewMatrix = _Lights[i]->GetViewMatrix();
		lightProjectionMatrix = _Lights[i]->GetProjectionMatrix();

		// Draw all actors in scene to this render texture
		for (map<string, unique_ptr<Actor>>::const_iterator it = pScene->_Actors.begin(); it != pScene->_Actors.end(); ++it)
		{
			// SUPER HACK TO GET TEMPORARY WATER MODELS IN!!!!!!!! //
			if (it->first == "Water" || !it->second->GetModel())
			{
				continue;
			}

			DrawModel(*it->second->GetModel(), worldMatrix, lightViewMatrix, lightProjectionMatrix, nullptr, EDEPTH);

			// Reset the world matrix.
			//worldMatrix = _D3D->GetWorldMatrix();
			//worldMatrix *= ComputeWorldTransform(it->second->GetOrientation(), it->second->GetScale(), it->second->GetPosition());
			//it->second->GetModel()->PutVertsOnPipeline(_D3D->GetDeviceContext());
			//_ShaderManager->_DepthShader->Render(_D3D->GetDeviceContext(), it->second->GetModel()->GetIndexCount(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
		}
	}

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	_D3D->SetBackBufferRenderTarget();
	_D3D->ResetViewport();

	return true;
}

bool GraphicsClass::RenderWaterToTexture(Scene* pScene)
{
	//////////////
	// REFRACTION
	//////////////
	if (!pScene->_Actors["Bath"]->GetModel()) return false;

	_RefractionTexture->SetRenderTarget(_D3D->GetDeviceContext());
	_RefractionTexture->ClearRenderTarget(_D3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);
	_sceneEffects.clipPlane = XMFLOAT4(0.0f, -1.0f, 0.0f, _waterHeight);

	// Get the world, view, and projection matrices from the camera and d3d objects.
	_Camera->UpdateViewPoint();

	XMMATRIX worldMatrix;
	XMMATRIX viewMatrix = _Camera->GetViewMatrix();
	XMMATRIX projectionMatrix = _D3D->GetProjectionMatrix();
	
	XMMATRIX lightViewMatrix[3], lightProjectionMatrix[3];
	for (int i = 0; i < pScene->NUM_LIGHTS; ++i)
	{
		_Lights[i]->GenerateViewMatrix();
		lightViewMatrix[i] = _Lights[i]->GetViewMatrix();
		lightProjectionMatrix[i] = _Lights[i]->GetProjectionMatrix();
	}
	LightClass* lights[3] = { _Lights[0].get(), _Lights[1].get(), _Lights[2].get() };

	DrawModel(*pScene->_Actors["Bath"]->GetModel(), worldMatrix, viewMatrix, projectionMatrix, lights, EShaderType::ELIGHT_SPECULAR);
	
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
	//pScene->_Actors["Wall"]->GetModel()->PutVertsOnPipeline(_D3D->GetDeviceContext());

	//_ShaderManager->_WaterShader->Render(_D3D->GetDeviceContext(), pScene->_Actors["Wall"]->GetModel()->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, reflectionViewMatrix,
	//	pScene->_Actors["Wall"]->GetModel()->GetMaterial()->GetResourceArray(), pScene->_Actors["Wall"]->GetModel()->GetMaterial()->translation, pScene->_Actors["Wall"]->GetModel()->GetMaterial()->reflectRefractScale);

	// Reset the render target back to the original back buffer and not the render to texture anymore.

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
	_Font1->Initialize(_D3D->GetDevice(), _D3D->GetDeviceContext(), "../DirectX11Engine/data/font.txt",
		"../DirectX11Engine/data/font.tga", 32.0f, 3);

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

#pragma endregion UI
