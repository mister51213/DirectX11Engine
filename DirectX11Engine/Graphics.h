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

// SOFT SHADOW INTEGRATION

#include "QUARANTINE/shadowshaderclass.h"
#include "QUARANTINE/orthowindowclass.h"
#include "QUARANTINE/horizontalblurshaderclass.h"
#include "QUARANTINE/verticalblurshaderclass.h"
#include "QUARANTINE/softshadowshaderclass.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: GraphicsClass
////////////////////////////////////////////////////////////////////////////////
class GraphicsClass
{

friend class System;

public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	//~GraphicsClass();

	bool Initialize(int screenWidth, int screenHeight, HWND hwnd, Scene* pScene);
	bool UpdateFrame(float frameTime, class Scene* pScene, int fps);

#pragma region WATER
	float _waterHeight = 2.75f;
	float _waterTranslation = 0.0f; 
	//unique_ptr<RenderTextureClass> _RefractionTexture;
	unique_ptr<RenderTextureClass> _ReflectionTexture; // TODO: encapsulate in materials
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

	bool bDrawAlternate = false;
private:
	bool RenderWaterToTexture(Scene* pScene, LightClass* lights[], ID3D11ShaderResourceView* blurredShadows);
	bool RenderShadowsToTexture(Scene* pScene, LightClass* lights[]);
	bool DrawFrame(Scene* pScene);
	void DrawModel(Model& model, MatrixBufferType& transforms, LightClass * shadowLights[] = nullptr, EShaderType shaderType = EShaderType::EMATERIAL_DEFAULT, XMMATRIX reflectionMatrix = XMMatrixIdentity());

	unique_ptr<D3DClass> _D3D;
	unique_ptr<ShaderManagerClass> _ShaderManager;
	unique_ptr<Camera> _Camera;

	unique_ptr<Model> _Sky;
	unique_ptr<Model> _SkyInner;
	unique_ptr<Model> _Earth;
	unique_ptr<Model> _EarthInner;

	/////////// GLOBAL EFFECTS /////////////
	SceneEffects _sceneEffects;
	const int NUM_SHADOWS = 3; // FOR SHADOWING ONLY!!!

	// Render textures used for shadowing
	vector<unique_ptr<RenderTextureClass>> _DepthTextures;
	vector<unique_ptr<LightClass>> _Lights;

	// INTEGRATED SOFT SHADOWS //
	ID3D11ShaderResourceView* ApplyBlur(ID3D11ShaderResourceView* viewToBlur, RenderTextureClass* outputRenderTarget);
	
	unique_ptr<RenderTextureClass> _SceneShadows;
	unique_ptr<RenderTextureClass> _SceneShadowsBlurred;
	vector<ComPtr<ID3D11ShaderResourceView>> _DepthViews;
	unique_ptr<RenderTextureClass> _RenderTexture,_BlackWhiteRenderTexture,_DownSampleTexure;
	unique_ptr<RenderTextureClass> _HorizontalBlurTexture,_VerticalBlurTexture,_UpSampleTexure;
	unique_ptr<OrthoWindowClass> _SmallWindow, _FullScreenWindow;
	unique_ptr<HorizontalBlurShaderClass> _HorizontalBlurShader;
	unique_ptr<VerticalBlurShaderClass> _VerticalBlurShader;

	///////// QUARANTINE - TEMP SECTION FOR SOFT SHADOW INTEGRATION ////////////
	//unique_ptr<SoftShadowShaderClass> _SoftShadowShader;
	//unique_ptr<ShadowShaderClass> _ShadowShader;
	//unique_ptr<RenderTextureClass> m_RenderTexture, m_BlackWhiteRenderTexture, m_DownSampleTexure;
	//unique_ptr<RenderTextureClass> m_HorizontalBlurTexture, m_VerticalBlurTexture, m_UpSampleTexure;
	//void RenderShadowScene(Scene * pScene);
	//void RenderShadows(Scene* pScene);
	//unique_ptr<LightClass> m_SoftLight;
	//unique_ptr<Model> m_CubeModel,m_GroundModel,m_SphereModel;
	///////// QUARANTINE - TEMP SECTION FOR SOFT SHADOW INTEGRATION ////////////
};
