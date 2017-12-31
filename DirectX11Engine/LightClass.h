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
	void SetPosition(const XMFLOAT3&pos);
	//{//_position = pos;
	//	_lightBufferVS->position = pos; // TEST
	//}
	//{_lightBufferVS.position = XMFLOAT4(pos.x, pos.y, pos.z, 1.f);}

	XMMATRIX GetViewMatrix() const;// { return _viewMatrix; }
	//{ return _lightBufferVS->viewMatrix; }

	XMMATRIX GetProjectionMatrix() const;// { return _projectionMatrix; }
		//{ return _lightBufferVS.projectionMatrix; }

	XMFLOAT3 GetPosition() const;//{ return _position; }
	//{
	//	if(_lightBufferVS)
	//	return _lightBufferVS->position;
	//}

	//~~~~ Info stored in PS Buffers ~~~~
	inline void SetDirection(const XMFLOAT3& dir) 
	{ _direction = dir; };
	inline void SetSpecularPower(float power) 
	{ _specularPower = power; };
	inline void SetSpecularColor(const XMFLOAT4& rgba) 
	{ _specularColor = rgba; };
	inline void SetDiffuseColor(const XMFLOAT4& rgba) 
	{ _diffuseColor = rgba; };

	inline XMFLOAT3 GetDirection() const 
	{ return _direction; };
	inline float GetSpecularPower() const 
	{ return _specularPower; };
	inline XMFLOAT4 GetSpecularColor() const 
	{ return _specularColor; };
	inline XMFLOAT4 GetDiffuseColor() const 
	{ return _diffuseColor; };

	//~~~ To encapsulate later!!!!
	inline void SetAmbientColor(const XMFLOAT4& rgba) 
	{ _ambientColor = rgba; };
	inline void SetLookAt(const XMFLOAT3& lookAt) 
	{ _lookAt = lookAt; };
	inline XMFLOAT4 GetAmbientColor() const 
	{ return _ambientColor; };

private:
	//XMFLOAT3 _position;// @TODO - stuff into light buffer instead
	//XMMATRIX _projectionMatrix;// @TODO - stuff into light buffer instead
	//XMMATRIX _viewMatrix; // @TODO - stuff into light buffer instead

	XMFLOAT3 _direction;// @TODO - stuff into light buffer instead
	float _specularPower;// @TODO - stuff into light buffer instead
	XMFLOAT4 _specularColor;// @TODO - stuff into light buffer instead
	XMFLOAT4 _diffuseColor;// @TODO - stuff into light buffer instead

	XMFLOAT4 _ambientColor;// @TODO - stuff into light buffer instead
	XMFLOAT3 _lookAt;// @TODO - stuff into light buffer instead

	LightBufferType_VS* _lightBufferVS;
	LightBufferType_PS _lightBufferPS;
};