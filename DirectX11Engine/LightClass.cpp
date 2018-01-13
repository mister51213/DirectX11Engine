////////////////////////////////////////////////////////////////////////////////
// Filename: lightclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "lightclass.h"

LightClass::LightClass()
{
	_lightBufferVS.reset(new LightDataTemplate_VS);
	_lightBufferPS.reset(new LightDataTemplate_PS);
}

LightClass::LightClass(const LightClass& other)
{}

LightClass::~LightClass()
{}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~ Info stored in VS Buffers ~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void LightClass::SetPosition(const XMFLOAT3& pos)
{
	_lightBufferVS->position = pos;
}

void LightClass::GenerateViewMatrix()
{
	XMFLOAT3 up(0, 1, 0);
	_lightBufferVS->viewMatrix = DirectX::XMMatrixLookAtLH(XMLoadFloat3(&_lightBufferVS->position), XMLoadFloat3(&_lookAt), XMLoadFloat3(&up)); // TEST
}

void LightClass::GenerateProjectionMatrix(float screenDepth, float screenNear)
{
	//float fieldOfView = (float)XM_PI / 2.f;
	//float fieldOfView = (float)XM_PI / .3f; // wider fov to hid frustum overlap with surface
	float fieldOfView = (float)XM_PI / 1.2f; // wider fov to hid frustum overlap with surface
	float screenAspect = 1.0f;
	_lightBufferVS->projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);
}

XMMATRIX LightClass::GetViewMatrix() const
{
	return _lightBufferVS->viewMatrix;
}

XMMATRIX LightClass::GetProjectionMatrix() const
{
	return _lightBufferVS->projectionMatrix;
}

XMFLOAT3 LightClass::GetPosition() const
{
	if (_lightBufferVS)
	return _lightBufferVS->position;
}

LightDataTemplate_VS * LightClass::GetLightBufferVS() const
{
	return _lightBufferVS.get();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~ Info stored in PS Buffers ~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void LightClass::SetDirection(const XMFLOAT3& dir)
{
	_lightBufferPS->direction = dir;
}

void LightClass::SetSpecularPower(float power)
{
	_lightBufferPS->specularPower = power;
}

void LightClass::SetSpecularColor(const XMFLOAT4& rgba)
{
	_lightBufferPS->specularColor = rgba;
}

void LightClass::SetDiffuseColor(const XMFLOAT4& rgba)
{
	_lightBufferPS->diffuseColor = rgba;
}

XMFLOAT3 LightClass::GetDirection() const
{
	return _lightBufferPS->direction;
}

XMFLOAT4 LightClass::GetDiffuseColor() const
{
	return _lightBufferPS->diffuseColor;
}

LightDataTemplate_PS * LightClass::GetLightBufferPS() const
{
	return _lightBufferPS.get();
}

XMFLOAT4 LightClass::GetSpecularColor() const
{
	return _lightBufferPS->specularColor;
}

float LightClass::GetSpecularPower() const
{
	return _lightBufferPS->specularPower;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~ To encapsulate later!!!!
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void LightClass::SetAmbientColor(const XMFLOAT4& rgba/*float red, float green, float blue, float alpha*/)
{
	_ambientColor = rgba;//XMFLOAT4(red, green, blue, alpha);
}

void LightClass::SetLookAt(const XMFLOAT3& lookAt/*float x, float y, float z*/)
{
	_lookAt = lookAt;
}

//XMFLOAT4 LightClass::GetAmbientColor() const 
//{
//	return _ambientColor;
//}