////////////////////////////////////////////////////////////////////////////////
// Filename: Input.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Input.h"

Input::Input()
{}

Input::Input(const Input& other)
{}

Input::~Input()
{}

bool Input::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	HRESULT result;

	// Store the screen size which will be used for positioning the mouse cursor.
	_screenWidth = screenWidth;
	_screenHeight = screenHeight;

	// Initialize the location of the mouse on the screen.
	_mouseX = 0;
	_mouseY = 0;

	// Initialize the main direct input interface.
	ThrowHResultIf(DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&_directInput, NULL));
	
	// Initialize the direct input interface for the keyboard.
	ThrowHResultIf(_directInput->CreateDevice(GUID_SysKeyboard, &_keyboard, NULL));

	// Set the data format.  In this case since it is a keyboard we can use the predefined data format.
	ThrowHResultIf(_keyboard->SetDataFormat(&c_dfDIKeyboard));

	// Set the cooperative level of the keyboard to not share with other programs.
	ThrowHResultIf(_keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE));

	// Now acquire the keyboard.
	ThrowHResultIf(_keyboard->Acquire());
	
	// Initialize the direct input interface for the mouse.
	ThrowHResultIf(_directInput->CreateDevice(GUID_SysMouse, &_mouse, NULL));

	// Set the data format for the mouse using the pre-defined mouse data format.
	ThrowHResultIf(_mouse->SetDataFormat(&c_dfDIMouse));

	//@SETTINGS
	// Set the cooperative level of the mouse to share with other programs.
	ThrowHResultIf(_mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE /*DISCL_NONEXCLUSIVE*/));

	// Acquire the mouse.
	ThrowHResultIf(_mouse->Acquire());

	return true;
}

void Input::Shutdown()
{
	// Release the mouse.
	if (_mouse)
	{
		_mouse->Unacquire();
	}

	// Release the keyboard.
	if (_keyboard)
	{
		_keyboard->Unacquire();
	}
}

bool Input::Tick()
{
	bool result;

	// Read the current state of the keyboard.
	result = ReadKeyboard();
	if (!result)
	{
		return false;
	}

	// Read the current state of the mouse.
	result = ReadMouse();
	if (!result)
	{
		return false;
	}

	// Process the changes in the mouse and keyboard.
	ProcessInput();

	return true;
}

bool Input::ReadKeyboard()
{
	HRESULT result;


	// Read the keyboard device.
	result = _keyboard->GetDeviceState(sizeof(_keyboardState), (LPVOID)&_keyboardState);
	if (FAILED(result))
	{
		// If the keyboard lost focus or was not acquired then try to get control back.
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			_keyboard->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}

bool Input::ReadMouse()
{
	HRESULT result;

	// Read the mouse device.
	result = _mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&_mouseState);
	if (FAILED(result))
	{
		// If the mouse lost focus or was not acquired then try to get control back.
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			_mouse->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}

void Input::ProcessInput()
{
	// Update the location of the mouse cursor based on the change of the mouse location during the frame.
	_mouseX += _mouseState.lX;
	_mouseY += _mouseState.lY;

	//@TODO - clamp angles not position   @STUDY
	// Ensure the mouse location doesn't exceed the screen width or height.
	//if (_mouseX < 0) { _mouseX = 0; }
	//if (_mouseY < 0) { _mouseY = 0; }

	//if (_mouseX > _screenWidth) { _mouseX = _screenWidth; }
	//if (_mouseY > _screenHeight) { _mouseY = _screenHeight; }

	return;
}

bool Input::IsEscapePressed()
{
	// Bitwise & on the keyboard state to check if the escape key is currently being pressed.
	if (_keyboardState[DIK_ESCAPE] & 0x80)
	{
		return true;
	}

	return false;
}

bool Input::IsLeftArrowPressed()
{
	if (_keyboardState[DIK_LEFT] & 0x80)
	{
		return true;
	}

	return false;
}


bool Input::IsRightArrowPressed()
{
	if (_keyboardState[DIK_RIGHT] & 0x80)
	{
		return true;
	}

	return false;
}

bool Input::IsKeyDown(int keyMacro)
{
	if (_keyboardState[keyMacro] & 0x80)
	{
		return true;
	}

	return false;
}

void Input::GetMouseLocation(int& mouseX, int& mouseY)
{
	mouseX = _mouseX;
	mouseY = _mouseY;
	return;
}

uint64_t Input::GetMouseXY()
{
	//return (__int64(_mouseX) << 32) | __int64(_mouseY);

	uint64_t wideX = static_cast<uint64_t>(*reinterpret_cast<uint32_t *>(&_mouseX));
	uint64_t wideY = static_cast<uint64_t>(*reinterpret_cast<uint32_t *>(&_mouseY));

	return ((wideX & 0xffffffff) << 32) | (wideY & 0xffffffff);
}
