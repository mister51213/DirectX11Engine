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

//////////////
// INCLUDES //
//////////////
#include "d3dclass.h"
#include "camera.h"
#include "textclass.h"
#include "model.h"
#include "LightClass.h"
#include "rendertextureclass.h"
#include "debugwindowclass.h"
#include "modellistclass.h"
#include "frustumclass.h"
#include "shadermanagerclass.h"
#include "Actor.h"
#include "Position.h"
#include "GfxUtil.h"
#include "Scene.h"

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
	bool InitializeLights();
	bool InitializeModels(const HWND &hwnd, int screenWidth, int screenHeight, vector<unique_ptr<Actor>>* sceneActors);
	bool InitializeUI(int screenWidth, int screenHeight);
	bool UpdateFrame(float frameTime, class Scene* scene, int fps, float posX, float posY, float posZ,
		float rotX, float rotY, float rotZ);

	bool DrawFrame(vector<unique_ptr<Actor>>* sceneActors, float frameTime);

	bool RenderRefractionToTexture();

	void RenderText();

	inline D3DClass* GetD3D()const { return _D3D.get();}
	inline Camera* GetCamera() const { return _Camera.get(); }
	bool UpdateRenderCounts(ID3D11DeviceContext*, int, int, int);

private:
	bool RenderReflectionToTexture();
	bool RenderScene(vector<unique_ptr<Actor>>* sceneActors, float frameTime);

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
	unique_ptr<ModelListClass>_ModelList;
	unique_ptr<FrustumClass> _Frustum;
	unique_ptr<RenderTextureClass> _RenderTexture;
	unique_ptr<DebugWindowClass> _DebugWindow;
	
	unique_ptr<ShaderManagerClass> _ShaderManager;

	/////////// WATER DEMO ////////////////
	unique_ptr<Model> _GroundModel, _WallModel, _BathModel, _WaterModel;
	unique_ptr<RenderTextureClass> _RefractionTexture, _ReflectionTexture;
	//vector<Model*> sceneModels;
	vector<unique_ptr<Actor>> sceneActors;

	/////////// GLOBAL EFFECTS /////////////
	SceneEffects _globalEffects;
};