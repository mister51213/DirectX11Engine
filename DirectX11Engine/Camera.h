////////////////////////////////////////////////////////////////////////////////
// Filename: camera.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

//////////////
// INCLUDES //
//////////////
#include <directxmath.h>
using namespace DirectX;


////////////////////////////////////////////////////////////////////////////////
// Class name: Camera
////////////////////////////////////////////////////////////////////////////////
class Camera
{
public:
	Camera();
	Camera(const Camera&);
	~Camera();

	void Tick();

	void SetPosition(float, float, float);
	
	void MoveInDirection(XMFLOAT3 displacement);
	void MoveInDirectionRelative(XMFLOAT3 displacement);
	void SetRotation(float, float, float);
	void RotateInDirection(const XMFLOAT3& offset);
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();

	void UpdateViewFromPosition();
	void GetViewMatrix(XMMATRIX&);

	//void RenderBaseViewMatrix();
	void GetBaseViewMatrix(XMMATRIX&);

	void RenderReflection(float);
	XMMATRIX GetReflectionViewMatrix();
private:
	float _positionX, _positionY, _positionZ;
	float _rotationX, _rotationY, _rotationZ;
	XMMATRIX _viewMatrix, m_baseViewMatrix, _reflectionViewMatrix;
	bool _bFirstRenderPass;
};
