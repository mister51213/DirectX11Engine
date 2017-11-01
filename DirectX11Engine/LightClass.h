////////////////////////////////////////////////////////////////////////////////
// Filename: lightclass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

//////////////
// INCLUDES //
//////////////
#include <directxmath.h>

using namespace DirectX;

////////////////////////////////////////////////////////////////////////////////
// Class name: LightClass
////////////////////////////////////////////////////////////////////////////////
class LightClass
{
public:
	LightClass();
	LightClass(const LightClass&);
	~LightClass();

	void SetAmbientColor(float, float, float, float);
	void SetDiffuseColor(float, float, float, float);
	void SetDirection(float, float, float);

	XMFLOAT4 GetAmbientColor() const;
	XMFLOAT4 GetDiffuseColor() const;
	XMFLOAT3 GetDirection() const;

private:
	XMFLOAT4 _ambientColor;
	XMFLOAT4 _diffuseColor;
	XMFLOAT3 _direction;
};