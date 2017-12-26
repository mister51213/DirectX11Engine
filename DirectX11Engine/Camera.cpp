#include "Camera.h"

Camera::Camera()
{
	_positionX = 0.0f;
	_positionY = 0.0f;
	_positionZ = 0.0f;

	_rotationX = 0.0f;
	_rotationY = 0.0f;
	_rotationZ = 0.0f;

	_bFirstRenderPass = true;
}

Camera::Camera(const Camera& other)
{
}

Camera::~Camera()
{
}

void Camera::Tick()
{

}

void Camera::SetPosition(float x, float y, float z)
{
	_positionX = x;
	_positionY = y;
	_positionZ = z;
	return;
}

void Camera::MoveInDirection(XMFLOAT3 displacement)
{
	_positionX += displacement.x;
	_positionY += displacement.y;
	_positionZ += displacement.z;
}

void Camera::MoveInDirectionRelative(XMFLOAT3 displacement)
{
	XMVECTOR posCamSpace = XMLoadFloat3(&displacement);

	XMMATRIX camRotMatrix =
		DirectX::XMMatrixRotationRollPitchYaw(
			_rotationX*0.0174532925f,
			_rotationY*0.0174532925f,
			_rotationZ*0.0174532925f);

	XMVECTOR worldDisplacement =
		XMVector3TransformCoord(posCamSpace, camRotMatrix);

	// set these world values
	XMFLOAT3 worldDispFloat3;
	XMStoreFloat3(&worldDispFloat3, worldDisplacement);

	_positionX += worldDispFloat3.x;
	_positionY += worldDispFloat3.y;
	_positionZ += worldDispFloat3.z;
}

void Camera::SetRotation(float x, float y, float z)
{
	_rotationX = x;
	_rotationY = y;
	_rotationZ = z;
	return;
}

void Camera::RotateInDirection(const XMFLOAT3& offset)
{
	_rotationX += offset.x;
	_rotationY += offset.y;
	_rotationZ += offset.z;
}

XMFLOAT3 Camera::GetPosition()
{
	return XMFLOAT3(_positionX, _positionY, _positionZ);
}

XMFLOAT3 Camera::GetRotation()
{
	return XMFLOAT3(_rotationX, _rotationY, _rotationZ);
}

void Camera::UpdateViewPoint()
{
	XMFLOAT3 up, position, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;

	// Setup the vector that points upwards.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// Load it into a XMVECTOR structure.
	upVector = XMLoadFloat3(&up);

	// Setup the position of the camera in the world.
	position.x = _positionX;
	position.y = _positionY;
	position.z = _positionZ;

	// Load it into a XMVECTOR structure.
	positionVector = XMLoadFloat3(&position);

	// Setup where the camera is looking by default.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	// Load it into a XMVECTOR structure.
	lookAtVector = XMLoadFloat3(&lookAt);

	// convert yaw (Y axis), pitch (X axis), and roll (Z axis) to radians.
	pitch = _rotationX * 0.0174532925f;
	yaw = _rotationY * 0.0174532925f;
	roll = _rotationZ * 0.0174532925f;

	// Create the rotation matrix from the yaw, pitch, and roll values.
	rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll); //@TODO reverse order
	//rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(yaw, pitch, roll); //@TODO reverse order

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	// Translate the rotated camera position to the location of the viewer.
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	// Finally create the view matrix from the three updated vectors.
	_viewMatrix = DirectX::XMMatrixLookAtLH(positionVector, lookAtVector, upVector);

	if(_bFirstRenderPass)
	m_baseViewMatrix = _viewMatrix;
	_bFirstRenderPass = false;

	return;
}

void Camera::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = _viewMatrix;
	return;
}

void Camera::GetBaseViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_baseViewMatrix;
	return;
}

void Camera::RenderReflection(float height)
{
	XMFLOAT3 up, position, lookAt;
	float radians;


	// Setup the vector that points upwards.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// Setup the position of the camera in the world.
	// For planar reflection invert the Y position of the camera.
	position.x =_positionX;
	position.y = -_positionY + (height * 2.0f);
	position.z = _positionZ;

	// Calculate the rotation in radians.
	radians = _rotationY * 0.0174532925f;

	// Setup where the camera is looking.
	lookAt.x = sinf(radians) + _positionX;
	lookAt.y = position.y;
	lookAt.z = cosf(radians) + _positionZ;

	// Create the view matrix from the three vectors.
	_reflectionViewMatrix = XMMatrixLookAtLH(XMLoadFloat3(&position), XMLoadFloat3(&lookAt), XMLoadFloat3(&up));
	
	return;
}

XMMATRIX Camera::GetReflectionViewMatrix()
{
	return _reflectionViewMatrix;
}

