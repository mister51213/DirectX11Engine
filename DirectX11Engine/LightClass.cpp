////////////////////////////////////////////////////////////////////////////////
// Filename: lightclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "lightclass.h"

LightClass::LightClass()
{
}

LightClass::LightClass(const LightClass& other)
{
}

LightClass::~LightClass()
{
}

void LightClass::SetAmbientColor(float red, float green, float blue, float alpha)
{
	_ambientColor = XMFLOAT4(red, green, blue, alpha);
	return;
}

void LightClass::SetDiffuseColor(float red, float green, float blue, float alpha)
{
	_diffuseColor = XMFLOAT4(red, green, blue, alpha);
	return;
}

void LightClass::SetDirection(float x, float y, float z)
{
	_direction = XMFLOAT3(x, y, z);
	return;
}

XMFLOAT4 LightClass::GetAmbientColor() const 
{
	return _ambientColor;
}

XMFLOAT4 LightClass::GetDiffuseColor() const
{
	return _diffuseColor;
}

XMFLOAT3 LightClass::GetDirection() const
{
	return _direction;
}