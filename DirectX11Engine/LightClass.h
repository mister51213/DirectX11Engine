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

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~~~~ Info stored in VS Buffers ~~~~
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	void GenerateViewMatrix();
	void GenerateProjectionMatrix(float screenDepth, float screenNear);
	void SetPosition(const XMFLOAT3&pos);

	XMMATRIX GetViewMatrix() const;

	XMMATRIX GetProjectionMatrix() const;

	XMFLOAT3 GetPosition() const;

	LightDataTemplate_VS* GetLightBufferVS() const;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~~~~ Info stored in PS Buffers ~~~~
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	void SetDirection(const XMFLOAT3& dir);
	//{ _direction = dir; };

	void SetSpecularPower(float power);
	//{ _specularPower = power; };

	void SetSpecularColor(const XMFLOAT4& rgba);
	//{ _specularColor = rgba; };

	void SetDiffuseColor(const XMFLOAT4& rgba);
	//{ _diffuseColor = rgba; };

	XMFLOAT3 GetDirection() const;
	//{ return _direction; };

	float GetSpecularPower() const;
	//{ return _specularPower; };

	XMFLOAT4 GetSpecularColor() const;
	//{ return _specularColor; };

	XMFLOAT4 GetDiffuseColor() const;
	//{ return _diffuseColor; };

	LightDataTemplate_PS* GetLightBufferPS() const;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~~~ To encapsulate later!!!!
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	void SetAmbientColor(const XMFLOAT4& rgba);
	//{ _ambientColor = rgba; };

	void SetLookAt(const XMFLOAT3& lookAt);
	//{ _lookAt = lookAt; };

	//XMFLOAT4 GetAmbientColor() const;
	//{ return _ambientColor; };

private:
	//XMFLOAT3 _position;// @TODO - stuff into light buffer instead
	//XMMATRIX _projectionMatrix;// @TODO - stuff into light buffer instead
	//XMMATRIX _viewMatrix; // @TODO - stuff into light buffer instead

	//XMFLOAT3 _direction;// @TODO - stuff into light buffer instead
	//float _specularPower;// @TODO - stuff into light buffer instead
	//XMFLOAT4 _specularColor;// @TODO - stuff into light buffer instead
	//XMFLOAT4 _diffuseColor;// @TODO - stuff into light buffer instead

	XMFLOAT4 _ambientColor;// @TODO - stuff into light buffer instead
	XMFLOAT3 _lookAt;// @TODO - stuff into light buffer instead

	unique_ptr<LightDataTemplate_VS> _lightBufferVS;
	unique_ptr<LightDataTemplate_PS> _lightBufferPS;
};