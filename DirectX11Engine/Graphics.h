#pragma once

////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.h
////////////////////////////////////////////////////////////////////////////////

/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = false;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

const int SHADOWMAP_WIDTH = 1024;
const int SHADOWMAP_HEIGHT = 1024;

//////////////
// INCLUDES //
//////////////
#include "d3dclass.h"
#include "camera.h"
#include "textclass.h"
#include "model.h"
#include "LightClass.h"
//#include "lightclassALT.h"
#include "rendertextureclass.h"
#include "debugwindowclass.h"
#include "modellistclass.h"
#include "frustumclass.h"
#include "shadermanagerclass.h"
#include "Actor.h"
#include "Position.h"
#include "GfxUtil.h"
#include "Scene.h"
#include "GlobalIncludes.h"

// Shadow debug
#include "shadowshaderclass.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: GraphicsClass
////////////////////////////////////////////////////////////////////////////////
class Graphics
{
public:
	Graphics();
	Graphics(const Graphics&);
	~Graphics();

	// Create the D3DClass object and call the D3DClass Initialize function.
	bool Initialize(int screenWidth, int screenHeight, HWND hwnd, Scene* scene);
	bool InitializeLights(Scene* pScene);
	bool InitializeModels(const HWND &hwnd, int screenWidth, int screenHeight, vector<unique_ptr<Actor>>& sceneActors);
	bool InitializeUI(int screenWidth, int screenHeight);
	bool UpdateFrame(float frameTime, class Scene* scene, int fps);

	bool DrawFrame(vector<unique_ptr<Actor>>& sceneActors, float frameTime);
	void Render();

	void RenderSceneToTexture();

	bool RenderRefractionToTexture(float surfaceHeight);

	void RenderText();

	inline D3DClass* GetD3D()const { return _D3D.get();}
	inline Camera* GetCamera() const { return _Camera.get(); }
	bool UpdateRenderCounts(ID3D11DeviceContext*, int, int, int);

private:
	bool RenderReflectionToTexture();
	bool RenderScene(vector<unique_ptr<Actor>>& sceneActors, float frameTime);

	bool UpdateFpsString(ID3D11DeviceContext*, int);
	bool UpdatePositionStrings(ID3D11DeviceContext*, float, float, float, float, float, float);

private:
	unique_ptr<D3DClass> _D3D;
	unique_ptr<Camera> _Camera;
	unique_ptr<TextClass> _Text; //@CUSTOM - now have multiple text classes holding different info
	unique_ptr<FontClass> _Font1;

	unique_ptr<TextClass> _FpsString, _VideoStrings;
	vector<unique_ptr<TextClass>> _RenderCountStrings;
	vector<unique_ptr<TextClass>> _PositionStrings;

	int _previousFps;
	int _previousPosition[6];
	float textureTranslation = 0.f;

	unique_ptr<LightClass> _Light;
	//unique_ptr<lightclassALT> _Light;
	vector<unique_ptr<LightClass>> _Lights;
	vector<LightClass*> _LightData;

	unique_ptr<ModelListClass>_ModelList;
	unique_ptr<FrustumClass> _Frustum;
	unique_ptr<RenderTextureClass> _RenderTexture;
	//unique_ptr<DebugWindowClass> _DebugWindow;
	
	unique_ptr<ShaderManagerClass> _ShaderManager;

	/////////// WATER DEMO ////////////////
	vector<unique_ptr<Model>> _DefaultModels;
	unique_ptr<Model> _GroundModel, _WallModel, _BathModel, _WaterModel;
	unique_ptr<RenderTextureClass> _RefractionTexture, _ReflectionTexture;

	////////// SHADOW DEMO ///////////////

	unique_ptr<Model> _CubeModel, _SphereModel, _ShadowGround;
	unique_ptr<RenderTextureClass> _ShadowMap;
	unique_ptr<ShadowShaderClass> _ShadowShader;

	/////////// GLOBAL EFFECTS /////////////
	SceneEffects _globalEffects;

	////////// GLOBAL LIGHTS ///////////
	const int NUM_LIGHTS = 4;
};

#pragma region UNIT_TESTING
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
/////////////// SHADOW TESTING /////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////////////
//// Filename: graphicsclass.h
//////////////////////////////////////////////////////////////////////////////////
//#ifndef _GRAPHICSCLASS_H_
//#define _GRAPHICSCLASS_H_
//
//
/////////////////////////
//// MY CLASS INCLUDES //
/////////////////////////
//#include "d3dclass.h"
//#include "cameraclass.h"
//#include "modelclass.h"
////#include "lightclass.h"
//#include "lightclassALT.h"
//#include "rendertextureclass.h"
////#include "depthshaderclass.h"
//#include "depthshaderclassALT.h"
//#include "shadowshaderclass.h"
//
//
///////////////
//// GLOBALS //
///////////////
//const bool FULL_SCREEN = true;
//const bool VSYNC_ENABLED = true;
//const float SCREEN_DEPTH = 100.0f;
//const float SCREEN_NEAR = 1.0f;
//const int SHADOWMAP_WIDTH = 1024;
//const int SHADOWMAP_HEIGHT = 1024;
//
//
//////////////////////////////////////////////////////////////////////////////////
//// Class name: GraphicsClass
//////////////////////////////////////////////////////////////////////////////////
//class GraphicsClass
//{
//public:
//	GraphicsClass();
//	GraphicsClass(const GraphicsClass&);
//	~GraphicsClass();
//
//	bool Initialize(int, int, HWND);
//	void Shutdown();
//	bool Frame(float, float, float, float, float, float);
//
//private:
//	bool RenderSceneToTexture();
//	bool Render();
//
//private:
//	D3DClass* m_D3D;
//	CameraClass* m_Camera;
//	ModelClass *m_CubeModel, *m_GroundModel, *m_SphereModel;
//	lightclassALT* m_Light;
//	RenderTextureClass* m_RenderTexture;
//	//DepthShaderClass* m_DepthShader;
//	depthShaderClassALT* m_DepthShader;
//	ShadowShaderClass* m_ShadowShader;
//};
//
//#endif


#pragma endregion