////////////////////////////////////////////////////////////////////////////////
// Filename: lightclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "lightclass.h"

LightClass::LightClass()
{
	_lightBufferVS = new LightBufferType_VS;
	//_lightBufferVS = LightBufferType_VS();
	//_lightBufferPS = LightBufferType_PS();
}

LightClass::LightClass(const LightClass& other)
{
}

LightClass::~LightClass()
{
}

//~~~~ Info stored in VS Buffers ~~~~
void LightClass::SetPosition(const XMFLOAT3& pos/*float x, float y, float z*/)
{
	//_position = pos;
	_lightBufferVS->position = pos;
}

void LightClass::GenerateViewMatrix()
{
	XMFLOAT3 up(0, 1, 0);
	_lightBufferVS->viewMatrix = DirectX::XMMatrixLookAtLH(XMLoadFloat3(&_lightBufferVS->position), XMLoadFloat3(&_lookAt), XMLoadFloat3(&up)); // TEST

	//_viewMatrix = DirectX::XMMatrixLookAtLH(XMLoadFloat3(&_position), XMLoadFloat3(&_lookAt), XMLoadFloat3(&up));
}

void LightClass::GenerateProjectionMatrix(float screenDepth, float screenNear)
{
	float fieldOfView = (float)XM_PI / 2.0f;
	float screenAspect = 1.0f;
	//_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);
	_lightBufferVS->projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);
}

XMMATRIX LightClass::GetViewMatrix() const
{
	return _lightBufferVS->viewMatrix;
	//return _viewMatrix;
}

XMMATRIX LightClass::GetProjectionMatrix() const
{
	return _lightBufferVS->projectionMatrix;
	//return _projectionMatrix;
}

XMFLOAT3 LightClass::GetPosition() const
{
	//return _position;
if (_lightBufferVS)
return _lightBufferVS->position;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~ Info stored in PS Buffers ~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//void LightClass::SetDirection(const XMFLOAT3& dir/*float x, float y, float z*/)
//{
//	_direction = dir;
//}

//void LightClass::SetSpecularColor(const XMFLOAT4& rgba/*float red, float green, float blue, float alpha*/)
//{
//	_specularColor = rgba;
//}
//
//void LightClass::SetSpecularPower(float power)
//{
//	_specularPower = power;
//}

//void LightClass::SetDiffuseColor(const XMFLOAT4& rgba/*float red, float green, float blue, float alpha*/)
//{
//	_diffuseColor = rgba;// XMFLOAT4(red, green, blue, alpha);
//}

//XMFLOAT3 LightClass::GetDirection() const
//{
//	return _direction;
//}

//XMFLOAT4 LightClass::GetDiffuseColor() const
//{
//	return _diffuseColor;
//}

//XMFLOAT4 LightClass::GetSpecularColor() const
//{
//	return _specularColor;
//}

//float LightClass::GetSpecularPower() const
//{
//	return _specularPower;
//}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~ To encapsulate later!!!!
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//void LightClass::SetAmbientColor(const XMFLOAT4& rgba/*float red, float green, float blue, float alpha*/)
//{
//	_ambientColor = rgba;//XMFLOAT4(red, green, blue, alpha);
//}

//void LightClass::SetLookAt(const XMFLOAT3& lookAt/*float x, float y, float z*/)
//{
//	_lookAt = lookAt;
//}

//XMFLOAT4 LightClass::GetAmbientColor() const 
//{
//	return _ambientColor;
//}