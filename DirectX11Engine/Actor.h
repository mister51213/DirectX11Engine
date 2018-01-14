#pragma once

#include "GfxUtil.h"
#include "Position.h"
#include "Model.h"
#include <string>

//class Model;

using namespace GfxUtil;

class Actor
{
public:
	Actor();
	Actor::Actor(const string& name);

	bool InitializeMovement(bool visible);

	/////////// Accessor functions ///////////////
	XMFLOAT3 GetPosition()const;
	XMFLOAT3 GetOrientation()const;
	XMFLOAT3 GetScale() const;

	void SetScale(XMFLOAT3 scale);
	
	XMFLOAT3 GetLookAt() const;

	void SetPosition(const XMFLOAT3& pos);
	void SetOrientation(const XMFLOAT3& or);

	void SetLookAt(const XMFLOAT3 & lookat);

	MovementComponent* GetMovementComponent();

	void InstantiateModel(Model* model);
	//inline void SetModel(unique_ptr<Model> model){_Model = model;}

	Model* GetModel() const;

	string Name;
	bool bCustomAppearance = false;
	bool bVisible = true;
	
	//PHYSICS @TODO - move into movement component?
	int physicsObjectID;
	bool bRigidBody, bKinematic;
	string collisionShape, physicsType;
	float mass, friction, restitution, damping;

private:
	unique_ptr<MovementComponent> _MovementComp;
	unique_ptr<Model> _Model;
	//shared_ptr<Model> _Model;
	XMFLOAT3 _scale;
};

