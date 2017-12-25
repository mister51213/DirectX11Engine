#pragma once

#include "GfxUtil.h"
#include "Position.h" 
#include "Model.h"

using namespace GfxUtil;

class Actor
{
public:
	Actor();

	bool InitializeMovement(bool visible);

	/////////// Accessor functions ///////////////
	XMFLOAT3 GetPosition()const;
	XMFLOAT3 GetOrientation()const;

	void SetPosition(const XMFLOAT3& pos);
	void SetOrientation(const XMFLOAT3& or);

	inline MovementComponent* GetMovementComponent()
	{
		return _MovementComp.get();
	}

	inline void SetModel(Model* model){_Model.reset(model);}

	inline Model* GetModel() const { return _Model.get(); }



	bool bCustomAppearance = false;
	bool bVisible = true;

private:
	unique_ptr<MovementComponent> _MovementComp;
	unique_ptr<Model> _Model;
};

