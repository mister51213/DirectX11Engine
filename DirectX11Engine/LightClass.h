////////////////////////////////////////////////////////////////////////////////
// Filename: lightclass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

//////////////
// INCLUDES //
//////////////
#include "Actor.h"
#include <directxmath.h> 
using namespace DirectX;

////////////////////////////////////////////////////////////////////////////////
// Class name: LightClass
////////////////////////////////////////////////////////////////////////////////
class LightClass : public Actor
{
public:
	LightClass();
	LightClass(const LightClass&);
	~LightClass();

	void SetAmbientColor(const XMFLOAT4&/*float, float, float, float*/);
	void SetDiffuseColor(const XMFLOAT4&/*float, float, float, float*/);
	void SetPosition(const XMFLOAT3&/*float x, float y, float z*/);
	void SetDirection(const XMFLOAT3&/*float, float, float*/);
	void SetSpecularColor(const XMFLOAT4&/*float, float, float, float*/);
	void SetSpecularPower(float);
	void SetLookAt(const XMFLOAT3&/*float, float, float*/);

	XMFLOAT4 GetAmbientColor() const;
	XMFLOAT4 GetDiffuseColor() const;

	XMFLOAT3 GetPosition() const;
	XMFLOAT3 GetDirection() const;
	XMFLOAT4 GetSpecularColor() const;
	float GetSpecularPower() const;

	void GenerateViewMatrix();
	void GenerateProjectionMatrix(float screenDepth, float screenNear);

	XMMATRIX GetViewMatrix() const;
	XMMATRIX GetProjectionMatrix() const;

private:
	XMFLOAT4 _ambientColor;
	XMFLOAT4 _diffuseColor;
	XMFLOAT3 _direction;
	//XMFLOAT4 _position;

	XMFLOAT3 _position;
	XMFLOAT4 _specularColor;
	float _specularPower;

	XMFLOAT3 _lookAt;
	XMMATRIX _viewMatrix;
	XMMATRIX _projectionMatrix;
};