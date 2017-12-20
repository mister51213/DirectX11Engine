#pragma once

#define _XM_NO_INTRINSICS_ 

#include <Windows.h>
#include <stdlib.h>
#include <string>
#include <memory>

using namespace std;

//#define CHECK(cond, msg) if (!cond) {throw std::runtime_error("Could not initialize the " + msg + " object. - line " + std::to_string(__LINE__));}

static void CHECK(bool cond, string msg)
{
	if (!cond)
	{
		throw std::runtime_error("Could not initialize the " + msg + " object. - line " + std::to_string(__LINE__));
	}
}
