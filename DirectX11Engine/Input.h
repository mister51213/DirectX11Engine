#pragma once

#define DIRECTINPUT_VERSION 0x0800

/////////////
// LINKING //
/////////////
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

//////////////
// INCLUDES //
//////////////
#include <dinput.h>
#include <cstdint>
#include <wrl/client.h>
#include "GlobalIncludes.h"

using namespace Microsoft::WRL;

////////////////////////////////////////////////////////////////////////////////
// Class name: Input
////////////////////////////////////////////////////////////////////////////////
class Input
{
public:
	Input();
	Input(const Input&);
	~Input();

	bool Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight);
	void Shutdown();
	bool Tick();

	bool IsEscapePressed();
	bool IsLeftArrowPressed();
	bool IsKeyDown(int keyMacro);
	bool IsRightArrowPressed();
	void GetMouseLocation(int&, int&);

	uint64_t GetMouseXY();

private:
	bool ReadKeyboard();
	bool ReadMouse();
	void ProcessInput();

	ComPtr<IDirectInput8> _directInput;
	ComPtr<IDirectInputDevice8> _keyboard;
	ComPtr<IDirectInputDevice8> _mouse;

	unsigned char _keyboardState[256];
	DIMOUSESTATE _mouseState;

	int _screenWidth, _screenHeight;
	int _mouseX, _mouseY;
};

