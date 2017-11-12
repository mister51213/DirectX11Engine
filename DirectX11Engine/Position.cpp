////////////////////////////////////////////////////////////////////////////////
// Filename: positionclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Position.h"

PositionClass::PositionClass()
	:
	_frameTime(0.f),
	_rotationY(0.f),
	_leftTurnSpeed(0.f),
	_rightTurnSpeed(0.f)
{
	_orientation = XMFLOAT3(0.f, 0.f, 0.f);
}

PositionClass::PositionClass(const PositionClass& other)
{
}

PositionClass::~PositionClass()
{
}

void PositionClass::SetFrameTime(float time)
{
	_frameTime = time;
	return;
}

float PositionClass::GetFrameTime(float)
{
	return _frameTime;
}

void PositionClass::GetRotation(float& y) const
{
	y = _rotationY;
	return;
}

void PositionClass::GetOrientation(XMFLOAT3& destination) const
{
	destination = _orientation;
	return;
}


void PositionClass::TurnLeft(bool keydown)
{
	// If the key is pressed increase the speed at which the camera turns left.  If not slow down the turn speed.
	if (keydown)
	{
		_leftTurnSpeed += _frameTime * 0.01f;

		if (_leftTurnSpeed > (_frameTime * 0.15f))
		{
			_leftTurnSpeed = _frameTime * 0.15f;
		}
	}
	else
	{
		_leftTurnSpeed -= _frameTime* 0.005f;

		if (_leftTurnSpeed < 0.0f)
		{
			_leftTurnSpeed = 0.0f;
		}
	}

	// Update the rotation using the turning speed.
	//_rotationY -= _leftTurnSpeed;
	//if (_rotationY < 0.0f)
	//{
	//	_rotationY += 360.0f;
	//}

	//@CUSTOM
	_orientation.y -= _leftTurnSpeed;
	if (_orientation.y < 0.0f)
	{
		_orientation.y += 360.0f;
	}

	return;
}

void PositionClass::TurnRight(bool keydown)
{
	// If the key is pressed increase the speed at which the camera turns right.  If not slow down the turn speed.
	if (keydown)
	{
		_rightTurnSpeed += _frameTime * 0.01f;

		if (_rightTurnSpeed > (_frameTime * 0.15f))
		{
			_rightTurnSpeed = _frameTime * 0.15f;
		}
	}
	else
	{
		_rightTurnSpeed -= _frameTime* 0.005f;

		if (_rightTurnSpeed < 0.0f)
		{
			_rightTurnSpeed = 0.0f;
		}
	}

	// Update the rotation using the turning speed.
	//_rotationY += _rightTurnSpeed;
	//if (_rotationY > 360.0f)
	//{
	//	_rotationY -= 360.0f;
	//}

	//@CUSTOM
	if (_orientation.y  > 360.0f)
	{
		_orientation.y -= 360.0f;
	}

	return;
}

void PositionClass::SetMouseLocation(int X, int Y)
{
	mouseX = X;
	mouseY = Y;
}

void PositionClass::MoveForward(bool keydown)
{}

void PositionClass::MoveBack(bool keydown)
{
}

void PositionClass::StrafeLeft(bool keydown)
{
}

void PositionClass::StrafeRight(bool keydown)
{
}
