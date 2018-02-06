////////////////////////////////////////////////////////////////////////////////
// Filename: positionclass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

//////////////
// INCLUDES //
//////////////
#include <math.h>
#include <DirectXMath.h>
#include "Physics.h"

using namespace DirectX;

////////////////////////////////////////////////////////////////////////////////
// Class name: MovementComponent
////////////////////////////////////////////////////////////////////////////////
class MovementComponent
{
public:
	MovementComponent();
	MovementComponent(const MovementComponent&);
	~MovementComponent();

	bool Initialize();

	void SetFrameTime(float);
	float GetFrameTime(float);
	XMFLOAT3 GetOrientation() const;
	XMFLOAT3 GetPosition() const;
	XMFLOAT3 GetLookAt() const;
	void SetPosition(const XMFLOAT3& pos);
	void SetOrientation(const XMFLOAT3 & newOrientation);
	void SetLookAt(const XMFLOAT3 & newLookat);
	void TurnLeft(bool);
	void TurnRight(bool);

	void SetMouseLocation(int X, int Y);

	void MoveForward(bool keydown);
	void MovementComponent::MoveInDirection(bool keydown, float& increment, float x, float y, float z);
	void MoveInDirectionRelative(const XMFLOAT3 & displacement);
	void MoveBack(bool keydown);
	void MoveLeft(bool keydown);
	void MoveRight(bool keydown);

	///////////// PHYSICS BASED ///////////////////
	void ApplyForce(float deltaTime);

private:
	float _frameTime;
	float _rotationY;
	XMFLOAT3 _position;
	XMFLOAT3 _orientation;
	XMFLOAT3 _lookat;
	float _leftTurnSpeed, _rightTurnSpeed;
	float _fwdIncrement;
	float _backIncrement;
	float _leftIncrement;
	float _rightIncrement; //@TODO: store these values in one vector

	// FOR CONTROLLABLE, NON-PHYSICS BASED ACTORS
	//const float _moveSpeed = 0.0002f;
	//const float _moveSpeed = 0.07f;
	const float _moveSpeed = 0.1f;
	//const float _brakeSpeed = 0.13f;
	const float _brakeSpeed = 0.2f;
	//const float _maxSpeed = 0.7f;
	const float _maxSpeed = 1.f;

	int mouseX, mouseY;
};