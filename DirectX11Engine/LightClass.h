////////////////////////////////////////////////////////////////////////////////
// Filename: lightclass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

//////////////
// INCLUDES //
//////////////
#include "Actor.h"
#include <directxmath.h> 
using namespace DirectX;

////////////////////////////////////////////////////////////////////////////////
// Class name: LightClass
////////////////////////////////////////////////////////////////////////////////
class LightClass : public Actor
{
public:
	LightClass();
	LightClass(const LightClass&);
	~LightClass();

	//~~~~ Info stored in VS Buffers ~~~~
	void GenerateViewMatrix();
	void GenerateProjectionMatrix(float screenDepth, float screenNear);
	inline void SetPosition(const XMFLOAT3&pos){_position = pos;}

	XMMATRIX GetViewMatrix() const;
	XMMATRIX GetProjectionMatrix() const;
	XMFLOAT3 GetPosition() const;

	//~~~~ Info stored in PS Buffers ~~~~
	void SetDirection(const XMFLOAT3&);
	void SetSpecularPower(float);
	void SetSpecularColor(const XMFLOAT4&);
	void SetDiffuseColor(const XMFLOAT4&);

	XMFLOAT3 GetDirection() const;
	float GetSpecularPower() const;
	XMFLOAT4 GetSpecularColor() const;
	XMFLOAT4 GetDiffuseColor() const;

	//~~~ To encapsulate later!!!!
	void SetAmbientColor(const XMFLOAT4&);
	XMFLOAT4 GetAmbientColor() const;
	void SetLookAt(const XMFLOAT3&);

private:
	XMFLOAT4 _ambientColor;// @TODO - stuff into light buffer instead
	XMFLOAT4 _diffuseColor;// @TODO - stuff into light buffer instead
	XMFLOAT3 _direction;// @TODO - stuff into light buffer instead

	XMFLOAT3 _position;// @TODO - stuff into light buffer instead
	XMFLOAT4 _specularColor;// @TODO - stuff into light buffer instead
	float _specularPower;// @TODO - stuff into light buffer instead

	XMFLOAT3 _lookAt;// @TODO - stuff into light buffer instead
	XMMATRIX _viewMatrix; // @TODO - stuff into light buffer instead
	XMMATRIX _projectionMatrix;// @TODO - stuff into light buffer instead

	LightBufferType_VS _lightBufferVS;
	LightBufferType_PS _lightBufferPS;
};