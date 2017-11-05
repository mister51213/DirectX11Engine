#include "FrustumClass.h"

FrustumClass::FrustumClass()
{
}

FrustumClass::FrustumClass(const FrustumClass& other)
{
}

FrustumClass::~FrustumClass()
{
}

void FrustumClass::ConstructFrustum(float screenDepth, XMMATRIX projectionMatrix, XMMATRIX viewMatrix)
{
	float zMinimum, r;
	XMMATRIX matrix;
	
	// Calculate the minimum Z distance in the frustum.
	zMinimum = -projectionMatrix._43 / projectionMatrix._33;
	r = screenDepth / (screenDepth - zMinimum);
	projectionMatrix._33 = r;
	projectionMatrix._43 = -r * zMinimum;

	// Create the frustum matrix from the view matrix and updated projection matrix.
	matrix = XMMatrixMultiply(viewMatrix, projectionMatrix);

	// Calculate near plane of frustum.
	_planes[0].a = matrix._14 + matrix._13;
	_planes[0].b = matrix._24 + matrix._23;
	_planes[0].c = matrix._34 + matrix._33;
	_planes[0].d = matrix._44 + matrix._43;
	_planes[0] = XMPlaneNormalize(_planes[0]);

	// Calculate far plane of frustum.
	_planes[1].a = matrix._14 - matrix._13;
	_planes[1].b = matrix._24 - matrix._23;
	_planes[1].c = matrix._34 - matrix._33;
	_planes[1].d = matrix._44 - matrix._43;
	D3DXPlaneNormalize(&m_planes[1], &m_planes[1]);

	// Calculate left plane of frustum.
	_planes[2].a = matrix._14 + matrix._11;
	_planes[2].b = matrix._24 + matrix._21;
	_planes[2].c = matrix._34 + matrix._31;
	_planes[2].d = matrix._44 + matrix._41;
	D3DXPlaneNormalize(&m_planes[2], &m_planes[2]);

	// Calculate right plane of frustum.
	_planes[3].a = matrix._14 - matrix._11;
	_planes[3].b = matrix._24 - matrix._21;
	_planes[3].c = matrix._34 - matrix._31;
	_planes[3].d = matrix._44 - matrix._41;
	D3DXPlaneNormalize(&m_planes[3], &m_planes[3]);

	// Calculate top plane of frustum.
	_planes[4].a = matrix._14 - matrix._12;
	_planes[4].b = matrix._24 - matrix._22;
	_planes[4].c = matrix._34 - matrix._32;
	_planes[4].d = matrix._44 - matrix._42;
	D3DXPlaneNormalize(&m_planes[4], &m_planes[4]);

	// Calculate bottom plane of frustum.
	_planes[5].a = matrix._14 + matrix._12;
	_planes[5].b = matrix._24 + matrix._22;
	_planes[5].c = matrix._34 + matrix._32;
	_planes[5].d = matrix._44 + matrix._42;
	D3DXPlaneNormalize(&m_planes[5], &m_planes[5]);

	return;
}

bool FrustumClass::CheckPoint(float x, float y, float z)
{
	int i;


	// Check if the point is inside all six planes of the view frustum.
	for (i = 0; i<6; i++)
	{
		if (D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3(x, y, z)) < 0.0f)
		{
			return false;
		}
	}

	return true;
}
//CheckCube checks if any of the eight corner points of the cube are inside the viewing frustum.It only requires as input the center point of the cube and the radius, it uses those to calculate the 8 corner points of the cube.It then checks if any one of the corner points are inside all 6 planes of the viewing frustum.If it does find a point inside all six planes of the viewing frustum it returns true, otherwise it returns false.

bool FrustumClass::CheckCube(float xCenter, float yCenter, float zCenter, float radius)
{
	int i;
	
	// Check if any one point of the cube is in the view frustum.
	for (i = 0; i < 6; i++)
	{
		//@TODO: is this the right value?
		if (XMPlaneDotCoord(_planes[i], XMLoadFloat3(&XMFLOAT3((xCenter - radius), (yCenter - radius), (zCenter - radius)))).m128_f32[0] >= 0.0f)
		{ continue; }		
		if (XMPlaneDotCoord(_planes[i], XMLoadFloat3(&XMFLOAT3((xCenter + radius), (yCenter - radius), (zCenter - radius)))).m128_f32[0] >= 0.0f)
		{continue;}		
		if (XMPlaneDotCoord(_planes[i], XMLoadFloat3(&XMFLOAT3((xCenter - radius), (yCenter + radius), (zCenter - radius)))).m128_f32[0] >= 0.0f)
		{continue;}
		if (XMPlaneDotCoord(_planes[i], XMLoadFloat3(&XMFLOAT3((xCenter + radius), (yCenter + radius), (zCenter - radius)))).m128_f32[0] >= 0.0f)
		{continue;}		
		if (XMPlaneDotCoord(_planes[i], XMLoadFloat3(&XMFLOAT3((xCenter - radius), (yCenter - radius), (zCenter + radius)))).m128_f32[0] >= 0.0f)
		{continue;}		
		if (XMPlaneDotCoord(_planes[i], XMLoadFloat3(&XMFLOAT3((xCenter + radius), (yCenter - radius), (zCenter + radius)))).m128_f32[0] >= 0.0f)
		{continue;}
		if (XMPlaneDotCoord(_planes[i], XMLoadFloat3(&XMFLOAT3((xCenter - radius), (yCenter + radius), (zCenter + radius)))).m128_f32[0] >= 0.0f)
		{continue;}
		if (XMPlaneDotCoord(_planes[i], XMLoadFloat3(&XMFLOAT3((xCenter + radius), (yCenter + radius), (zCenter + radius)))).m128_f32[0] >= 0.0f)
		{continue;}

		//if (D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((xCenter - radius), (yCenter - radius), (zCenter - radius))) >= 0.0f)
		//{
		//	continue;
		//}
		//if (D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((xCenter + radius), (yCenter - radius), (zCenter - radius))) >= 0.0f)
		//{
		//	continue;
		//}
		//if (D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((xCenter - radius), (yCenter + radius), (zCenter - radius))) >= 0.0f)
		//{
		//	continue;
		//}
		//if (D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((xCenter + radius), (yCenter + radius), (zCenter - radius))) >= 0.0f)
		//{
		//	continue;
		//}
		//if (D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((xCenter - radius), (yCenter - radius), (zCenter + radius))) >= 0.0f)
		//{
		//	continue;
		//}
		//if (D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((xCenter + radius), (yCenter - radius), (zCenter + radius))) >= 0.0f)
		//{
		//	continue;
		//}
		//if (D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((xCenter - radius), (yCenter + radius), (zCenter + radius))) >= 0.0f)
		//{
		//	continue;
		//}
		//if (D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((xCenter + radius), (yCenter + radius), (zCenter + radius))) >= 0.0f)
		//{
		//	continue;
		//}

		return false;
	}

	return true;
}
//CheckSphere checks if the radius of the sphere from the center point is inside all six planes of the viewing frustum.If it is outside any of them then the sphere cannot be seen and the function will return false.If it is inside all six the function returns true that the sphere can be seen.

bool FrustumClass::CheckSphere(float xCenter, float yCenter, float zCenter, float radius)
{
	int i;


	// Check if the radius of the sphere is inside the view frustum.
	for (i = 0; i<6; i++)
	{
		if (XMPlaneDotCoord(_planes[i], XMFLOAT3(xCenter, yCenter, zCenter)) < -radius)
		{
			return false;
		}
	}

	return true;
}
//CheckRectangle works the same as CheckCube except that that it takes as input the x radius, y radius, and z radius of the rectangle instead of just a single radius of a cube.It can then calculate the 8 corner points of the rectangle and do the frustum checks similar to the CheckCube function.

bool FrustumClass::CheckRectangle(float xCenter, float yCenter, float zCenter, float xSize, float ySize, float zSize)
{
	int i;


	// Check if any of the 6 planes of the rectangle are inside the view frustum.
	for (i = 0; i<6; i++)
	{
		if (XMPlaneDotCoord(_planes[i], &D3DXVECTOR3((xCenter - xSize), (yCenter - ySize), (zCenter - zSize))) >= 0.0f)
		{
			continue;
		}

		if (XMPlaneDotCoord(_planes[i], &D3DXVECTOR3((xCenter + xSize), (yCenter - ySize), (zCenter - zSize))) >= 0.0f)
		{
			continue;
		}

		if (XMPlaneDotCoord(_planes[i], &D3DXVECTOR3((xCenter - xSize), (yCenter + ySize), (zCenter - zSize))) >= 0.0f)
		{
			continue;
		}

		if (XMPlaneDotCoord(_planes[i], &D3DXVECTOR3((xCenter - xSize), (yCenter - ySize), (zCenter + zSize))) >= 0.0f)
		{
			continue;
		}

		if (XMPlaneDotCoord(_planes[i], &D3DXVECTOR3((xCenter + xSize), (yCenter + ySize), (zCenter - zSize))) >= 0.0f)
		{
			continue;
		}

		if (XMPlaneDotCoord(_planes[i], &D3DXVECTOR3((xCenter + xSize), (yCenter - ySize), (zCenter + zSize))) >= 0.0f)
		{
			continue;
		}

		if (XMPlaneDotCoord(_planes[i], &D3DXVECTOR3((xCenter - xSize), (yCenter + ySize), (zCenter + zSize))) >= 0.0f)
		{
			continue;
		}

		if (XMPlaneDotCoord(_planes[i], &D3DXVECTOR3((xCenter + xSize), (yCenter + ySize), (zCenter + zSize))) >= 0.0f)
		{
			continue;
		}

		return false;
	}

	return true;
}
