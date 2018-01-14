////////////////////////////////////////////
// Filename: VectorMath.h
// Contains useful functions and operator overloads for working with XMFLOAT3
///////////////////////////////////////////////
#pragma once

#include <Windows.h>
#define _XM_NO_INTRINSICS_ 
#include <directxmath.h>
#include <stdlib.h>
#include <vector>

using namespace DirectX;

namespace VectorMath
{
	inline DirectX::XMFLOAT3 operator + (const DirectX::XMFLOAT3 in1, const DirectX::XMFLOAT3 in2)
	{
		return XMFLOAT3(in1.x + in2.x,
						in1.y + in2.y,
						in1.z + in2.z);
	}

	inline DirectX::XMFLOAT3 operator - (const DirectX::XMFLOAT3 destination, const DirectX::XMFLOAT3 origin)
	{
		return XMFLOAT3(
			destination.x - origin.x,
			destination.y - origin.y,
			destination.z - origin.z);
	}

	inline void operator += (DirectX::XMFLOAT3& thisVector, const DirectX::XMFLOAT3 in)
	{
		thisVector.x += in.x;
		thisVector.y += in.y;
		thisVector.z += in.z;
	}

	inline DirectX::XMFLOAT3 operator * (const DirectX::XMFLOAT3 in1, const float in2)
	{
		return XMFLOAT3(in1.x* in2,	in1.y * in2, in1.z * in2);
	}

	inline DirectX::XMFLOAT3 operator * (const float in1, const DirectX::XMFLOAT3 in2)
	{
		return XMFLOAT3(in2.x* in1, in2.y * in1, in2.z * in1);
	}

	inline DirectX::XMFLOAT4 EulerToQuat(const XMFLOAT3& pitchYawRoll)
	{
		XMFLOAT4 outQuat;
		XMStoreFloat4(&outQuat, DirectX::XMQuaternionRotationRollPitchYaw(pitchYawRoll.x, pitchYawRoll.y, pitchYawRoll.z));
		// REFERENCE: XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
		return outQuat;
	}

	/* 2D Spline generator
	1. Pick 3 points for the curve to pass through
	2. Generate a matrix from the coefficients of the terms of the equation to find
	3. Solve for Ax = b:		(example equation): ax^2 + bx + c(1) = y
		> For this equation, if you plugin 3 different points (x,y) for x and y, you will get three different LINEAR equations.
		> solve the three equations for the three unknowns a, b, and c using matrix form Ax = b
		> Multiply b by the inverse of A to cancel out A on the left side and get the vector of coefficients x
		* The number of linear equations/unknowns will correspond to the degree of the curve (highest exponent)
	4. If there is no solution, pick the closest fitting curve instead
	5. Apply these coefficients to the base equation ax^2 + bx + c(1) = y to calculate each point of the spline
	6. Apply these 2D values to a 3D vector along a given plane to generate the spline	*/
	//vector<XMFLOAT3> GenerateSpline(vector<XMFLOAT2> points, float xIncrement = 1.f, float range = 100.f)
	//{
	//	// TODO: draw vectors from 1st to 2nd and 1st to 3rd points
	//	// subtract the projection of vector 1 onto vector 2, FROM vector 1,
	//	// to get the perpendicular component to vector 1
	//	// > translate along vector 2 for X value, and along perpVector1 for Y value
	//	
	//	XMFLOAT4X4 tmp;			// XMStoreFloat4x4(&tmp, A); // other way around
	//	tmp.m[0][0] = points[0].x*points[0].x;		tmp.m[1][0] = points[0].x;		tmp.m[2][0] = 1;
	//	tmp.m[0][1] = points[1].x*points[1].x;		tmp.m[1][1] = points[1].x;		tmp.m[2][1] = 1;
	//	tmp.m[0][2] = points[2].x*points[2].x;		tmp.m[1][2] = points[2].x;		tmp.m[2][2] = 1;
	//	tmp.m[0][3] = 0;							tmp.m[1][3] = 0;				tmp.m[2][3] = 1;

	//	XMMATRIX A = XMLoadFloat4x4(&tmp); // matrix to get inverse of 

	//	XMFLOAT3 b(points[0].y, points[1].y, points[2].y); // right hand side vector
	//	XMVECTOR bHolder = XMLoadFloat3(&b);
	//
	//	XMVECTOR determinantA = XMMatrixDeterminant(A);
	//	XMMATRIX AInverse = XMMatrixInverse(&determinantA, A);

	//	// multiply by the inverse of the coefficient matrix to find the vector of coefficients
	//	XMVECTOR x = XMVector4Transform(bHolder, AInverse);

	//	XMFLOAT3 resultVector;
	//	XMStoreFloat3(&resultVector, x);

	//	// apply the coefficients in the result vector to each point along the spline to get the result
	//	vector<XMFLOAT3> pointsOnSpline;

	//	for (float inX = 0.f; inX <= range; inX += xIncrement)
	//	{
	//		float outY = resultVector.x * inX*inX + resultVector.y * inX + resultVector.z;

	//		// make the spline going down the Z axis and "y" value pointing up in world
	//		pointsOnSpline.push_back(XMFLOAT3(0.f, outY, inX));
	//	}

	//	// Pack these into 3D points in local space (must later transform into world space)
	//	for (auto& point : pointsOnSpline)
	//	{
	//		XMVECTOR pointVec = XMLoadFloat3(&point);
	//	}

	//	return pointsOnSpline;
	//}
};