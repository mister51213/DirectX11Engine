///////////////////////////////////////////////////////////////////////////////
// Filename: fpsclass.cpp
///////////////////////////////////////////////////////////////////////////////
#include "fpsclass.h"

FpsClass::FpsClass()
	:
	_fps(0),
	_count(0),
	_startTime(0)
{}

void FpsClass::Initialize()
{
	_fps = 0;
	_count = 0;
	_startTime = timeGetTime();
	return;
}

void FpsClass::Frame()
{
	_count++;

	if (timeGetTime() >= (_startTime + 1000))
	{
		_fps = _count;
		_count = 0;

		_startTime = timeGetTime();
	}
}

int FpsClass::GetFps()
{
	return _fps;
}