////////////////////////////////////////////////////////////////////////////////
// Filename: positionclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "VectorMath.h"
#include "Position.h"

using namespace VectorMath;

PositionClass::PositionClass()
	:
	_frameTime(0.f),
	_rotationY(0.f),
	_leftTurnSpeed(0.f),
	_rightTurnSpeed(0.f),
	_fwdIncrement(0.f),
	_backIncrement(0.f),
	_leftIncrement(0.f),
	_rightIncrement(0.f)
{
	_orientation = XMFLOAT3(0.f, 0.f, 0.f);
	_position = XMFLOAT3(0.f, 0.f, 0.f);
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

void PositionClass::GetPosition(XMFLOAT3& destination) const
{
	destination = _position;
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

	//@CUSTOM
	_orientation.y += _rightTurnSpeed;
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
{
	MoveInDirection(keydown, _fwdIncrement, 0.f, 0.f, 1.f);
}

void PositionClass::MoveBack(bool keydown)
{
	MoveInDirection(keydown, _backIncrement, 0.f, 0.f, -1.f);
}

void PositionClass::MoveLeft(bool keydown)
{
	MoveInDirection(keydown, _leftIncrement, -1.f, 0.f, 0.f);
}

void PositionClass::MoveRight(bool keydown)
{
	MoveInDirection(keydown, _rightIncrement, 1.f, 0.f, 0.f);
}

void PositionClass::MoveInDirection(bool keydown, float& increment, float x, float y, float z)
{
	if (keydown)
	{
		increment += _frameTime * _moveSpeed;

		if (increment > (_frameTime * _maxSpeed))
		{
			increment = _frameTime * _maxSpeed;
		}
	}
	else
	{
		increment -= _frameTime * _brakeSpeed;

		if (increment < 0.0f)
		{
			increment = 0.0f;
		}
	}

	//@STUDY Command pattern
	//MoveInDirectionRelative(XMFLOAT3(0.0f, 0.0f, increment));
	MoveInDirectionRelative(XMFLOAT3(x,y,z)*increment);

}

//void PositionClass::MoveInDirectionBack(bool keydown, float x, float y, float z)
//{
//	if (keydown)
//	{
//		_backIncrement += _frameTime * _moveSpeed;
//
//		if (_backIncrement > (_frameTime * _maxSpeed))
//		{
//			_backIncrement = _frameTime * _maxSpeed;
//		}
//	}
//	else
//	{
//		_backIncrement -= _frameTime * _brakeSpeed;
//
//		if (_backIncrement < 0.0f)
//		{
//			_backIncrement = 0.0f;
//		}
//	}
//
//	//@STUDY Command pattern
//	MoveInDirectionRelative(XMFLOAT3(0.0f, 0.0f, -_backIncrement));
//	//MoveInDirectionRelative(XMFLOAT3(x,y,z)*_walkIncrement);
//
//}


void PositionClass::MoveInDirectionRelative(const XMFLOAT3& displacement)
{
	XMVECTOR displacementLocal = XMLoadFloat3(&displacement);

	XMMATRIX localRotMatrix =
		DirectX::XMMatrixRotationRollPitchYaw(
			_orientation.x*0.0174532925f,
			_orientation.y*0.0174532925f,
			_orientation.z*0.0174532925f);

	XMVECTOR worldDisplacement =
		XMVector3TransformCoord(displacementLocal, localRotMatrix);

	// set these world values
	XMFLOAT3 worldDispFloat3;
	XMStoreFloat3(&worldDispFloat3, worldDisplacement);

	_position.x += worldDispFloat3.x;
	_position.y += worldDispFloat3.y;
	_position.z += worldDispFloat3.z;
}


void PositionClass::StrafeLeft(bool keydown)
{
}

void PositionClass::StrafeRight(bool keydown)
{
}
