////////////////////////////////////////////////////////////////////////////////
// Filename: positionclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "VectorMath.h"
#include "Position.h"

using namespace VectorMath;

MovementComponent::MovementComponent()
	:
	_frameTime(0.f),
	_rotationY(0.f),
	_leftTurnSpeed(0.f),
	_rightTurnSpeed(0.f),
	_fwdIncrement(0.f),
	_backIncrement(0.f),
	_leftIncrement(0.f),
	_rightIncrement(0.f),
	_orientation(XMFLOAT3(0.f, 0.f, 0.f)),
	_position(XMFLOAT3(0.f, 0.f, 0.f)),
	_lookat(XMFLOAT3(0.f, 0.f, 0.f))
{}

MovementComponent::MovementComponent(const MovementComponent& other)
{
}

MovementComponent::~MovementComponent()
{
}

bool MovementComponent::Initialize()
{
	return true;
}

void MovementComponent::SetFrameTime(float time)
{
	_frameTime = time;
	return;
}

float MovementComponent::GetFrameTime(float)
{
	return _frameTime;
}

XMFLOAT3 MovementComponent::GetOrientation() const
{
	return _orientation;
}

XMFLOAT3 MovementComponent::GetPosition() const
{
	return _position;
}

XMFLOAT3 MovementComponent::GetLookAt() const
{
	return _lookat;
}

void MovementComponent::SetPosition(const XMFLOAT3 & pos)
{
	_position = pos;
}

void MovementComponent::SetOrientation(const XMFLOAT3& newOrientation)
{
	_orientation = newOrientation;
	return;
}

void MovementComponent::SetLookAt(const XMFLOAT3 & newLookat)
{
	_lookat = newLookat;
}

void MovementComponent::TurnLeft(bool keydown)
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

void MovementComponent::TurnRight(bool keydown)
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

void MovementComponent::SetMouseLocation(int X, int Y)
{
	mouseX = X;
	mouseY = Y;
}

void MovementComponent::MoveForward(bool keydown)
{
	MoveInDirection(keydown, _fwdIncrement, 0.f, 0.f, 1.f);
}

void MovementComponent::MoveBack(bool keydown)
{
	MoveInDirection(keydown, _backIncrement, 0.f, 0.f, -1.f);
}

void MovementComponent::MoveLeft(bool keydown)
{
	MoveInDirection(keydown, _leftIncrement, -1.f, 0.f, 0.f);
}

void MovementComponent::MoveRight(bool keydown)
{
	MoveInDirection(keydown, _rightIncrement, 1.f, 0.f, 0.f);
}

void MovementComponent::ApplyForce(float deltaTime)
{
}

void MovementComponent::MoveInDirection(bool keydown, float& increment, float x, float y, float z)
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
	MoveInDirectionRelative(XMFLOAT3(x,y,z)*increment);
}

void MovementComponent::MoveInDirectionRelative(const XMFLOAT3& displacement)
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