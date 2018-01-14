#pragma once

#include "GfxUtil.h"
#include "GlobalIncludes.h"

#include "d3dclass.h"
#include "shadermanagerclass.h"

#include "Scene.h"
#include "camera.h"
#include "lightclass.h"
#include "model.h"
#include "rendertextureclass.h"

#include "textclass.h"
//#include "frustumclass.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: GraphicsClass
////////////////////////////////////////////////////////////////////////////////
class GraphicsClass
{

friend class System;

public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int screenWidth, int screenHeight, HWND hwnd, Scene* pScene);
	bool UpdateFrame(float frameTime, class Scene* pScene, int fps);

#pragma region WATER
	float _waterHeight = 2.75f;
	float _waterTranslation = 0.0f; // TODO: encapsulate in materials
	//unique_ptr<RenderTextureClass> _RefractionTexture;
	unique_ptr<RenderTextureClass> _ReflectionTexture;
#pragma endregion

#pragma region UI
	bool InitializeUI(int screenWidth, int screenHeight);
	bool UpdateRenderCounts(ID3D11DeviceContext*, int, int, int);
	bool UpdateFpsString(ID3D11DeviceContext*, int);
	bool UpdatePositionStrings(ID3D11DeviceContext*, float, float, float, float, float, float);
	void RenderText();
	unique_ptr<TextClass> _Text; //@CUSTOM - now have multiple text classes holding different info
	unique_ptr<FontClass> _Font1;
	
	unique_ptr<TextClass> _FpsString, _VideoStrings;
	vector<unique_ptr<TextClass>> _RenderCountStrings;
	vector<unique_ptr<TextClass>> _PositionStrings;
	int _previousFps;
	int _previousPosition[6];
	float textureTranslation = 0.f;
#pragma endregion

private:
	bool RenderWaterToTexture(Scene* pScene, LightClass* lights[]);
	bool RenderShadowsToTexture(Scene* pScene, LightClass* lights[]);
	bool Render(Scene* pScene);

	void DrawModel(Model& model, MatrixBufferType& transforms, XMMATRIX &worldTransform, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix, LightClass * shadowLights[] = nullptr, EShaderType shaderType = EShaderType::EMATERIAL_DEFAULT, XMMATRIX reflectionMatrix = XMMatrixIdentity());

	unique_ptr<D3DClass> _D3D;
	unique_ptr<ShaderManagerClass> _ShaderManager;
	unique_ptr<Camera> _Camera;

	unique_ptr<Model> _Sky;
	unique_ptr<Model> _SkyInner;
	unique_ptr<Model> _Earth;
	unique_ptr<Model> _EarthInner;
	vector<unique_ptr<RenderTextureClass>> _RenderTextures;
	vector<unique_ptr<LightClass>>_Lights;

	/////////// GLOBAL EFFECTS /////////////
	SceneEffects _sceneEffects;

	const int NUM_RENDER_TEXTURES = 3; // FOR SHADOWING ONLY!!!
};
