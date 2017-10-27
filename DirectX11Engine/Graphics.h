#pragma once

////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.h
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////
//#include <windows.h>
#include "d3dclass.h"
#include "camera.h"
#include "model.h"
#include "colorshaderclass.h"

/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

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
	bool Frame();

private:
	bool Render();

private:
	D3DClass* _D3D;
	Camera* _Camera;
	Model* _Model;
	ColorShaderClass* _ColorShader;
};