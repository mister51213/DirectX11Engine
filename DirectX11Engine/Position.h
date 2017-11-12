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
	void TurnLeft(bool);
	void TurnRight(bool);

	void SetMouseLocation(int X, int Y);

	void MoveForward(bool keydown);
	void MoveBack(bool keydown);
	void StrafeLeft(bool keydown);
	void StrafeRight(bool keydown);

private:
	float _frameTime;
	float _rotationY;
	XMFLOAT3 _position;
	XMFLOAT3 _orientation;
	float _leftTurnSpeed, _rightTurnSpeed;
	int mouseX, mouseY;
};