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

void LightClass::SetPosition(float x, float y, float z)
{
	_position = XMFLOAT4(x, y, z, 1.f);
	return;
}


void LightClass::SetDirection(float x, float y, float z)
{
	_direction = XMFLOAT3(x, y, z);
	return;
}

void LightClass::SetSpecularColor(float red, float green, float blue, float alpha)
{
	_specularColor = XMFLOAT4(red, green, blue, alpha);
	return;
}

void LightClass::SetSpecularPower(float power)
{
	_specularPower = power;
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

XMFLOAT4 LightClass::GetPosition() const
{
	return _position;
}

XMFLOAT3 LightClass::GetDirection() const
{
	return _direction;
}

XMFLOAT4 LightClass::GetSpecularColor() const
{
	return _specularColor;
}

float LightClass::GetSpecularPower() const
{
	return _specularPower;
}