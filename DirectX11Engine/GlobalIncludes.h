#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define _XM_NO_INTRINSICS_ 

#include <Windows.h>
#include <stdlib.h>
#include <string>
#include <memory>
#include <sstream>
#include <wrl/client.h>
#include <directxmath.h>
#include "VectorMath.h"
#include <d3d11.h>
#include "comdef.h"

using namespace std;
using Microsoft::WRL::ComPtr;

/////////////	
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 10000.0f;
const float SCREEN_NEAR = 1.0f;
const int SHADOWMAP_WIDTH = 2048; //1024;// 4096;
const int SHADOWMAP_HEIGHT = 2048; //1024;// 4096;
const float SHADOWMAP_DEPTH = 50.0f;
const float SHADOWMAP_NEAR = 1.0f;
const int NUM_LIGHTS = 3;

#define VISIBLE true
#define INVISIBLE false

// PHYSICS
static bool bRigidBodyPhysicsEnabled = true;

struct PhysicsTransform
{
	XMFLOAT4 rotation;
	XMFLOAT3 position;

	PhysicsTransform()
	{
		rotation = XMFLOAT4(0, 0, 0, 1);
		position = XMFLOAT3(0, 0, 0);
	}
	PhysicsTransform(const XMFLOAT4& newRot, const XMFLOAT3& newPos)
	{
		rotation = newRot;
		position = newPos;
	}
};

class RuntimeException
{
public:
	RuntimeException() = default;
	RuntimeException(std::string Message, std::string Filename, const size_t LineNumber)
	{
		std::stringstream ss;

		ss << Message
			<< "\nLine number: "
			<< LineNumber
			<< "\nFilename: "
			<< CaptureFilename(Filename);
		error = ss.str();
	}

	RuntimeException(std::string Message)
	{
		error = Message;
	}

	std::string GetError()const
	{
		return error;
	}
	std::wstring GetWindowsError()const
	{
		return std::wstring(error.begin(), error.end());
	}

protected:
	std::string CaptureFilename(const std::string& Filename)
	{
		const auto beg = Filename.find_last_of('\\') + 1;
		return Filename.substr(beg);
	}

protected:
	std::string error;
};

class CompileShaderException : public RuntimeException
{
public:
	CompileShaderException(std::string ShaderFilename, std::string ShaderErrorMsg, std::string OutputLog)
		:
		RuntimeException("Error compiling " + ShaderFilename + ".  Error logged to " + OutputLog +
			"\nShader error message: \n" + ShaderErrorMsg)
	{}
};

class MissingShaderException :public RuntimeException
{
public:
	MissingShaderException(std::string ShaderFilename)
		:
		RuntimeException("Missing Shader File " + ShaderFilename)
	{}
};

class HResultException : public RuntimeException
{
public:
	HResultException(HRESULT HResult, std::string Filename, const size_t LineNumber);
};

#define ThrowRuntime( msg ) throw RuntimeException( (msg), __FILE__, __LINE__)
#define ThrowHResultIf( hr ) if( FAILED( (hr) ) ) throw HResultException( (hr), __FILE__, __LINE__)

template<class InString, class OutString>
auto ConvertTo(const InString text)->OutString
{
	return OutString(text.begin(), text.end());
}
