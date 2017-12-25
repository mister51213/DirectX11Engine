////////////////////////////////////////////////////////////////////////////////
// Filename: lightclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "lightclassALT.h"


lightclassALT::lightclassALT()
{
}


lightclassALT::lightclassALT(const lightclassALT& other)
{
}


lightclassALT::~lightclassALT()
{
}


void lightclassALT::SetAmbientColor(float red, float green, float blue, float alpha)
{
	m_ambientColor = XMFLOAT4(red, green, blue, alpha);
	return;
}


void lightclassALT::SetDiffuseColor(float red, float green, float blue, float alpha)
{
	m_diffuseColor = XMFLOAT4(red, green, blue, alpha);
	return;
}


void lightclassALT::SetPosition(float x, float y, float z)
{
	m_position = XMFLOAT3(x, y, z);
	return;
}


void lightclassALT::SetLookAt(float x, float y, float z)
{
	m_lookAt.x = x;
	m_lookAt.y = y;
	m_lookAt.z = z;
	return;
}


XMFLOAT4 lightclassALT::GetAmbientColor()
{
	return m_ambientColor;
}


XMFLOAT4 lightclassALT::GetDiffuseColor()
{
	return m_diffuseColor;
}


XMFLOAT3 lightclassALT::GetPosition()
{
	return m_position;
}


void lightclassALT::GenerateViewMatrix()
{
	XMFLOAT3 up;


	// Setup the vector that points upwards.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// Create the view matrix from the three vectors.
	//D3DXMatrixLookAtLH(&m_viewMatrix, &m_position, &m_lookAt, &up);
	m_viewMatrix = DirectX::XMMatrixLookAtLH(XMLoadFloat3(&m_position), XMLoadFloat3(&m_lookAt), XMLoadFloat3(&up));


	return;
}


void lightclassALT::GenerateProjectionMatrix(float screenDepth, float screenNear)
{
	float fieldOfView, screenAspect;


	// Setup field of view and screen aspect for a square light source.
	fieldOfView = (float)XM_PI / 2.0f;
	screenAspect = 1.0f;

	// Create the projection matrix for the light.
	//D3DXMatrixPerspectiveFovLH(&m_projectionMatrix, fieldOfView, screenAspect, screenNear, screenDepth);
	m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);


	return;
}


void lightclassALT::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
	return;
}


void lightclassALT::GetProjectionMatrix(XMMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
	return;
}