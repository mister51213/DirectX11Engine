#pragma once

#define _XM_NO_INTRINSICS_ 

#include <Windows.h>
#include <stdlib.h>
#include <string>
#include <memory>

using namespace std;


#define CHECK(cond, msg)if (!(cond)){throw std::runtime_error( "Could not initialize the " + std::string((msg)) + " object. - line " + std::to_string( __LINE__ ) );}

/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 100.0f;
const float SCREEN_NEAR = 1.0f;
const int SHADOWMAP_WIDTH = 1024;
const int SHADOWMAP_HEIGHT = 1024;

// @TODO - USE THESE RIGHT BEFORE AL DIRECTX FUNCTIONS
// RESEARCH different HRESULTs S_OK
// WinErr.h
//#define CHECK_HRESULT(cond, msg)if (FAILED(cond)){throw std::runtime_error( "Could not initialize the " + std::string((msg)) + " object. - line " + std::to_string( __LINE__ ) );}

//static void CHECK(bool cond, string msg)
//{
//	if (!cond)
//	{
//		throw std::runtime_error("Could not initialize the " + msg + " object. - line " + std::to_string(__LINE__));
//	}
//}
