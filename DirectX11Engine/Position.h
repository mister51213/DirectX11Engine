////////////////////////////////////////////////////////////////////////////////
// Filename: positionclass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

//////////////
// INCLUDES //
//////////////
#include <math.h>
#include <DirectXMath.h>

using namespace DirectX;

////////////////////////////////////////////////////////////////////////////////
// Class name: PositionClass
////////////////////////////////////////////////////////////////////////////////
class PositionClass
{
public:
	PositionClass();
	PositionClass(const PositionClass&);
	~PositionClass();

	void SetFrameTime(float);
	float GetFrameTime(float);
	void GetRotation(float&) const;
	void GetOrientation(XMFLOAT3& destination) const;
	void GetPosition(XMFLOAT3& destination) const;
	void TurnLeft(bool);
	void TurnRight(bool);

	void SetMouseLocation(int X, int Y);

	void MoveForward(bool keydown);
	//void AccelerateInDirection(float x, float y, float z, float speed, float speedLimit);
	//void DecelerateInDirection(float x, float y, float z, float speed, float lowerLimit);
	void PositionClass::MoveInDirection(bool keydown, float& increment, float x, float y, float z);
	//void MoveInDirectionBack(bool keydown, float x, float y, float z);
	void MoveInDirectionRelative(const XMFLOAT3 & displacement);
	void MoveBack(bool keydown);
	void StrafeLeft(bool keydown);
	void StrafeRight(bool keydown);

private:
	float _frameTime;
	float _rotationY;
	XMFLOAT3 _position;
	XMFLOAT3 _orientation;
	float _leftTurnSpeed, _rightTurnSpeed;
	float _fwdIncrement;
	float _backIncrement;
	int mouseX, mouseY;
	float _moveSpeed = 0.0001f;
	float _brakeSpeed = 0.00007f;
	float _maxSpeed = 0.01f;
};