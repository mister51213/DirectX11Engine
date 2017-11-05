#pragma once

///////////////////////////
// PREPROCESSOR DIRECTIVES /
///////////////////////////

#define WIN32_LEAN_AND_MEAN

//////////////
// INCLUDES //
//////////////
#include <windows.h>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Input.h"
#include "Graphics.h"
#include "FpsClass.h"
#include "CpuClass.h"
#include "TimerClass.h"
#include "Position.h"


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
	bool Frame();
	//bool ProcessInput(); //@custom
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

private:
	LPCWSTR _applicationName;
	HINSTANCE _hinstance;
	HWND _hwnd;

	Input* _Input;
	Graphics* _Graphics;

	FpsClass* _Fps;
	CpuClass* _Cpu;
	TimerClass* _Timer;
	PositionClass* _Position;
};

/////////////////////////
// FUNCTION PROTOTYPES //
/////////////////////////
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

/////////////
// GLOBALS //
/////////////
static System* ApplicationHandle = 0;