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
#include "Graphics.h"
#include "World.h"
#include "UI.h"

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
	LPCWSTR _applicationName;
	HINSTANCE _hinstance;
	HWND _hwnd;

	Input* _Input;
	unique_ptr<World> _World;
	Graphics* _Graphics;
	unique_ptr<UI> _UI;

	//FpsClass* _Fps;
	//CpuClass* _Cpu;
	TimerClass* _Timer;
	PositionClass* _CamPosition;
};

/////////////////////////
// FUNCTION PROTOTYPES //
/////////////////////////
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

/////////////
// GLOBALS //
/////////////
static System* ApplicationHandle = 0;