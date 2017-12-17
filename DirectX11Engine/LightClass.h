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

	void SetAmbientColor(float, float, float, float);
	void SetDiffuseColor(float, float, float, float);
	void SetDirection(float, float, float);
	void SetSpecularColor(float, float, float, float);
	void SetSpecularPower(float);

	XMFLOAT4 GetAmbientColor() const;
	XMFLOAT4 GetDiffuseColor() const;
	XMFLOAT3 GetDirection() const;
	XMFLOAT4 GetSpecularColor() const;
	float GetSpecularPower() const;

private:
	XMFLOAT4 _ambientColor;
	XMFLOAT4 _diffuseColor;
	XMFLOAT3 _direction;
	XMFLOAT4 _specularColor;
	float _specularPower;
};