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
//#include <windows.h>
#include "d3dclass.h"
#include "camera.h"
#include "textclass.h"
#include "model.h"
#include "LightShaderClass.h"
#include "LightClass.h"
#include "modellistclass.h"
#include "frustumclass.h"

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
	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame(float rotation, int mouseX, int mouseY, int fps, int cpu, float frameTime);
	bool Render();

	inline Camera* GetCamera() { return _Camera; }

private:

private:
	D3DClass* _D3D;
	Camera* _Camera;
	TextClass* _Text;
	Model* _Model;
	LightShaderClass* _LightShader;
	LightClass* _Light;
	ModelListClass* _ModelList;
	FrustumClass* _Frustum;

	float _modelRotation = 0.0f;
};