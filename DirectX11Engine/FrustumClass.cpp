#include "FrustumClass.h"

FrustumClass::FrustumClass()
{}

FrustumClass::FrustumClass(const FrustumClass& other)
{}

FrustumClass::~FrustumClass()
{}

void FrustumClass::ConstructFrustum(float screenDepth, XMMATRIX projectionMatrix, XMMATRIX viewMatrix)
{
	float zMinimum, r;
	XMMATRIX matrix;
	
	// Calculate the minimum Z distance in the frustum.
	float* element43 = &(projectionMatrix.r[3].m128_f32[2]);
	float* element33 = &(projectionMatrix.r[2].m128_f32[2]);

	//zMinimum = -projectionMatrix._43 / projectionMatrix._33;
	zMinimum = -*element43 / *element33;
	r = screenDepth / (screenDepth - zMinimum);
	*element33 = r; //projectionMatrix._33 = r;
	*element43 = -r * zMinimum; //projectionMatrix._43 = -r * zMinimum;

	// Create the frustum matrix from the view matrix and updated projection matrix.
	matrix = XMMatrixMultiply(viewMatrix, projectionMatrix);
	XMFLOAT4X4 tempMat;
	XMStoreFloat4x4(&tempMat, matrix); //XMLoadFloat4x4

	// Calculate near plane of frustum.
	_planes[0].x = tempMat._14 + tempMat._13;
	_planes[0].y = tempMat._24 + tempMat._23;
	_planes[0].z = tempMat._34 + tempMat._33;
	_planes[0].w = tempMat._44 + tempMat._43;
	XMStoreFloat4(&_planes[0], XMPlaneNormalize(XMLoadFloat4(&_planes[0])));

	// Calculate far plane of frustum.
	_planes[1].x = tempMat._14 - tempMat._13;
	_planes[1].y = tempMat._24 - tempMat._23;
	_planes[1].z = tempMat._34 - tempMat._33;
	_planes[1].w = tempMat._44 - tempMat._43;
	XMStoreFloat4(&_planes[1], XMPlaneNormalize(XMLoadFloat4(&_planes[1])));

	// Calculate left plane of frustum.
	_planes[2].x = tempMat._14 + tempMat._11;
	_planes[2].y = tempMat._24 + tempMat._21;
	_planes[2].z = tempMat._34 + tempMat._31;
	_planes[2].w = tempMat._44 + tempMat._41;
	XMStoreFloat4(&_planes[2], XMPlaneNormalize(XMLoadFloat4(&_planes[2])));

	// Calculate right plane of frustum.
	_planes[3].x = tempMat._14 - tempMat._11;
	_planes[3].y = tempMat._24 - tempMat._21;
	_planes[3].z = tempMat._34 - tempMat._31;
	_planes[3].w = tempMat._44 - tempMat._41;
	XMStoreFloat4(&_planes[3], XMPlaneNormalize(XMLoadFloat4(&_planes[3])));

	// Calculate top plane of frustum.
	_planes[4].x = tempMat._14 - tempMat._12;
	_planes[4].y = tempMat._24 - tempMat._22;
	_planes[4].z = tempMat._34 - tempMat._32;
	_planes[4].w = tempMat._44 - tempMat._42;
	XMStoreFloat4(&_planes[4], XMPlaneNormalize(XMLoadFloat4(&_planes[4])));

	// Calculate bottom plane of frustum.
	_planes[5].x = tempMat._14 + tempMat._12;
	_planes[5].y = tempMat._24 + tempMat._22;
	_planes[5].z = tempMat._34 + tempMat._32;
	_planes[5].w = tempMat._44 + tempMat._42;
	XMStoreFloat4(&_planes[5], XMPlaneNormalize(XMLoadFloat4(&_planes[5])));
}

bool FrustumClass::CheckPoint(float x, float y, float z)
{
	// Check if the point is inside all six planes of the view frustum.
	for (int i = 0; i<6; i++)
	{
		if (XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&_planes[i]), XMLoadFloat3(&XMFLOAT3(x, y, z)))) < 0.0f)
		{
			return false;
		}
	}

	return true;
}

//CheckCube checks if any of the eight corner points of the cube are inside the viewing frustum.It only requires as input the center point of the cube and the radius, it uses those to calculate the 8 corner points of the cube.It then checks if any one of the corner points are inside all 6 planes of the viewing frustum.If it does find a point inside all six planes of the viewing frustum it returns true, otherwise it returns false.
bool FrustumClass::CheckCube(float xCenter, float yCenter, float zCenter, float radius)
{	
	// Check if any one point of the cube is in the view frustum.
	for (int i = 0; i < 6; i++)
	{
		if (XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&_planes[i]), XMLoadFloat3(&XMFLOAT3((xCenter - radius), (yCenter - radius), (zCenter - radius)))))>= 0.0f)
		{
			continue;
		}
		if (XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&_planes[i]), XMLoadFloat3(&XMFLOAT3((xCenter + radius), (yCenter - radius), (zCenter - radius)))))>= 0.0f)
		{																																			 
			continue;																																 
		}																																			 
		if (XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&_planes[i]), XMLoadFloat3(&XMFLOAT3((xCenter - radius), (yCenter + radius), (zCenter - radius)))))>= 0.0f)
		{																																			 
			continue;																																 
		}																																			 
		if (XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&_planes[i]), XMLoadFloat3(&XMFLOAT3((xCenter + radius), (yCenter + radius), (zCenter - radius)))))>= 0.0f)
		{																																			
			continue;																																
		}																																			
		if (XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&_planes[i]), XMLoadFloat3(&XMFLOAT3((xCenter - radius), (yCenter - radius), (zCenter + radius)))))>= 0.0f)
		{																																			
			continue;																																
		}																																			
		if (XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&_planes[i]), XMLoadFloat3(&XMFLOAT3((xCenter + radius), (yCenter - radius), (zCenter + radius)))))>= 0.0f)
		{																																			
			continue;																																
		}																																			
		if (XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&_planes[i]), XMLoadFloat3(&XMFLOAT3((xCenter - radius), (yCenter + radius), (zCenter + radius)))))>= 0.0f)
		{																																			
			continue;																																
		}																																			
		if (XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&_planes[i]), XMLoadFloat3(&XMFLOAT3((xCenter + radius), (yCenter + radius), (zCenter + radius)))))>= 0.0f)
		{
			continue;
		}

		//@OLD
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
		if (XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&_planes[i]), XMLoadFloat3(&XMFLOAT3(xCenter, yCenter, zCenter)))) < -radius)
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
		if (XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&_planes[i]), XMLoadFloat3(&XMFLOAT3((xCenter - xSize), (yCenter - ySize), (zCenter - zSize))))) >= 0.0f)
		{
			continue;
		}
		if (XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&_planes[i]), XMLoadFloat3(&XMFLOAT3((xCenter + xSize), (yCenter - ySize), (zCenter - zSize))))) >= 0.0f)
		{
			continue;
		}
		if (XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&_planes[i]), XMLoadFloat3(&XMFLOAT3((xCenter - xSize), (yCenter + ySize), (zCenter - zSize))))) >= 0.0f)
		{
			continue;
		}
		if (XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&_planes[i]), XMLoadFloat3(&XMFLOAT3((xCenter - xSize), (yCenter - ySize), (zCenter + zSize))))) >= 0.0f)
		{
			continue;
		}
		if (XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&_planes[i]), XMLoadFloat3(&XMFLOAT3((xCenter + xSize), (yCenter + ySize), (zCenter - zSize))))) >= 0.0f)
		{
			continue;
		}
		if (XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&_planes[i]), XMLoadFloat3(&XMFLOAT3((xCenter + xSize), (yCenter - ySize), (zCenter + zSize))))) >= 0.0f)
		{
			continue;
		}
		if (XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&_planes[i]), XMLoadFloat3(&XMFLOAT3((xCenter - xSize), (yCenter + ySize), (zCenter + zSize))))) >= 0.0f)
		{
			continue;
		}
		if (XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&_planes[i]), XMLoadFloat3(&XMFLOAT3((xCenter + xSize), (yCenter + ySize), (zCenter + zSize))))) >= 0.0f)
		{
			continue;
		}
		//////////////////////////////////////////////////////
		//if (XMPlaneDotCoord(_planes[i], &D3DXVECTOR3((xCenter - xSize), (yCenter - ySize), (zCenter - zSize))) >= 0.0f)
		//{
		//	continue;
		//}
		//if (XMPlaneDotCoord(_planes[i], &D3DXVECTOR3((xCenter + xSize), (yCenter - ySize), (zCenter - zSize))) >= 0.0f)
		//{
		//	continue;
		//}
		//if (XMPlaneDotCoord(_planes[i], &D3DXVECTOR3((xCenter - xSize), (yCenter + ySize), (zCenter - zSize))) >= 0.0f)
		//{
		//	continue;
		//}
		//if (XMPlaneDotCoord(_planes[i], &D3DXVECTOR3((xCenter - xSize), (yCenter - ySize), (zCenter + zSize))) >= 0.0f)
		//{
		//	continue;
		//}
		//if (XMPlaneDotCoord(_planes[i], &D3DXVECTOR3((xCenter + xSize), (yCenter + ySize), (zCenter - zSize))) >= 0.0f)
		//{
		//	continue;
		//}
		//if (XMPlaneDotCoord(_planes[i], &D3DXVECTOR3((xCenter + xSize), (yCenter - ySize), (zCenter + zSize))) >= 0.0f)
		//{
		//	continue;
		//}
		//if (XMPlaneDotCoord(_planes[i], &D3DXVECTOR3((xCenter - xSize), (yCenter + ySize), (zCenter + zSize))) >= 0.0f)
		//{
		//	continue;
		//}
		//if (XMPlaneDotCoord(_planes[i], &D3DXVECTOR3((xCenter + xSize), (yCenter + ySize), (zCenter + zSize))) >= 0.0f)
		//{
		//	continue;
		//}

		return false;
	}

	return true;
}
