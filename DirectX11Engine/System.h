#pragma once

///////////////////////////
// PREPROCESSOR DIRECTIVES /
///////////////////////////

#define WIN32_LEAN_AND_MEAN

//////////////
// INCLUDES //
//////////////
#include <windows.h>
#include <memory>


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Input.h"
#include "Physics.h"
#include "Graphics.h"
#include "Scene.h"
#include "UI.h"
#include "GlobalIncludes.h"

///////////////////////
// Physics //
//////////////////////
#include "btBulletDynamicsCommon.h"

/////////////////////////
// Microsoft //
////////////////////

#include <wrl/client.h>

//#include "FpsClass.h" // put in UI
//#include "CpuClass.h" // put in UI

#include "TimerClass.h"

//#include "Position.h" // put in world

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// Class name: SystemClass
////////////////////////////////////////////////////////////////////////////////
class System
{
public:
	System();
	System(const System&);
	~System();

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Tick();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

private:
	LPCSTR _applicationName;
	HINSTANCE _hinstance;
	HWND _hwnd;

	unique_ptr<TimerClass> _Timer;
	unique_ptr<Input> _Input;
	unique_ptr<Physics> _Physics;
	unique_ptr<Scene> _Scene;
	//unique_ptr<Graphics> _Graphics;
	unique_ptr<GraphicsClass> _Graphics;
	unique_ptr<UI> _UI;
};

/////////////////////////
// FUNCTION PROTOTYPES //
/////////////////////////
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

/////////////
// GLOBALS //
/////////////
static System* ApplicationHandle = 0;