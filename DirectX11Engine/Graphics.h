#pragma once

////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.h
////////////////////////////////////////////////////////////////////////////////

/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
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
	bool Frame(/*float rotation, int mouseX, int mouseY, int fps, int cpu,*/ float frameTime, int fps, float posX, float posY, float posZ,
		float rotX, float rotY, float rotZ);
	bool Render();

	inline Camera* GetCamera() { return _Camera; }

	bool UpdateRenderCounts(ID3D11DeviceContext*, int, int, int);

private:
	bool UpdateFpsString(ID3D11DeviceContext*, int);
	bool UpdatePositionStrings(ID3D11DeviceContext*, float, float, float, float, float, float);

private:
	D3DClass* _D3D;
	Camera* _Camera;
	TextClass* _Text; //@CUSTOM - now have multiple text classes holding different info
	FontClass* m_Font1;
	FontShaderClass* _FontShader;

	TextClass *m_FpsString, *m_VideoStrings, *m_PositionStrings;
	int m_previousFps;
	int m_previousPosition[6];
	TextClass* m_RenderCountStrings;


	Model* _Model;
	LightShaderClass* _LightShader;
	LightClass* _Light;
	ModelListClass* _ModelList;
	FrustumClass* _Frustum;

	float _modelRotation = 0.0f;
};