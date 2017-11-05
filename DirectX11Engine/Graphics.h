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

	bool Frame(int mouseX, int mouseY, int fps, int cpu, float frameTime);

	inline Camera* GetCamera() { return _Camera; }

private:
	bool Render(float lightRotation); //@TODO: dont pass light rotation here

private:
	D3DClass* _D3D;
	Camera* _Camera;
	TextClass* _Text;
	Model* _Model;
	LightShaderClass* _LightShader;
	LightClass* _Light;

	float _modelRotation = 0.0f;
};