////////////////////////////////////////////
// Filename: VectorMath.h
// Contains useful functions and operator overloads for working with XMFLOAT3
///////////////////////////////////////////////
#pragma once

#include <DirectXMath.h>
using namespace DirectX;

namespace VectorMath
{
	inline DirectX::XMFLOAT3 operator + (const DirectX::XMFLOAT3 in1, const DirectX::XMFLOAT3 in2)
	{
		return XMFLOAT3(in1.x + in2.x,
						in1.y + in2.y,
						in1.z + in2.z);
	}

	inline void operator += (DirectX::XMFLOAT3& thisVector, const DirectX::XMFLOAT3 in)
	{
		thisVector.x += in.x;
		thisVector.y += in.y;
		thisVector.z += in.z;
	}
};