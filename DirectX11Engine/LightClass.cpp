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

void LightClass::SetAmbientColor(const XMFLOAT4& rgba/*float red, float green, float blue, float alpha*/)
{
	_ambientColor = rgba;//XMFLOAT4(red, green, blue, alpha);
}

//~~~~ Info stored in VS Buffers ~~~~
//void LightClass::SetPosition(const XMFLOAT3& pos/*float x, float y, float z*/)
//{
//	_position = pos;
//}

void LightClass::GenerateViewMatrix()
{
	_viewMatrix = DirectX::XMMatrixLookAtLH(XMLoadFloat3(&_position), XMLoadFloat3(&_lookAt), XMLoadFloat3(&XMFLOAT3(0,1,0)));
}

void LightClass::GenerateProjectionMatrix(float screenDepth, float screenNear)
{
	float fieldOfView = (float)XM_PI / 2.0f;
	float screenAspect = 1.0f;
	_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);
}

XMMATRIX LightClass::GetViewMatrix() const
{
	return _viewMatrix;
}

XMMATRIX LightClass::GetProjectionMatrix() const
{
	return _projectionMatrix;
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~ Info stored in PS Buffers ~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void LightClass::SetDirection(const XMFLOAT3& dir/*float x, float y, float z*/)
{
	_direction = dir;
}

void LightClass::SetSpecularColor(const XMFLOAT4& rgba/*float red, float green, float blue, float alpha*/)
{
	_specularColor = rgba;
}

void LightClass::SetSpecularPower(float power)
{
	_specularPower = power;
}

void LightClass::SetDiffuseColor(const XMFLOAT4& rgba/*float red, float green, float blue, float alpha*/)
{
	_diffuseColor = rgba;// XMFLOAT4(red, green, blue, alpha);
}

//~~~ To encapsulate later!!!!
void LightClass::SetLookAt(const XMFLOAT3& lookAt/*float x, float y, float z*/)
{
	_lookAt = lookAt;
}

XMFLOAT4 LightClass::GetAmbientColor() const 
{
	return _ambientColor;
}

XMFLOAT4 LightClass::GetDiffuseColor() const
{
	return _diffuseColor;
}

XMFLOAT3 LightClass::GetPosition() const
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