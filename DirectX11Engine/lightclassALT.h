////////////////////////////////////////////////////////////////////////////////
// Filename: lightclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _LIGHTCLASS_H_
#define _LIGHTCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <DirectXMath.h>
using namespace DirectX;

////////////////////////////////////////////////////////////////////////////////
// Class name: lightclassALT
////////////////////////////////////////////////////////////////////////////////
class lightclassALT
{
public:
	lightclassALT();
	lightclassALT(const lightclassALT&);
	~lightclassALT();

	void SetAmbientColor(float, float, float, float);
	void SetDiffuseColor(float, float, float, float);
	void SetPosition(float, float, float);
	void SetLookAt(float, float, float);

	XMFLOAT4 GetAmbientColor();
	XMFLOAT4 GetDiffuseColor();
	XMFLOAT3 GetPosition();

	void GenerateViewMatrix();
	void GenerateProjectionMatrix(float, float);

	void GetViewMatrix(XMMATRIX&);
	void GetProjectionMatrix(XMMATRIX&);

private:
	XMFLOAT4 m_ambientColor;
	XMFLOAT4 m_diffuseColor;
	XMFLOAT3 m_position;
	XMFLOAT3 m_lookAt;
	XMMATRIX m_viewMatrix;
	XMMATRIX m_projectionMatrix;
};

#endif