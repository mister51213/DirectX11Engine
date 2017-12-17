#pragma once

#include "GfxUtil.h"
#include "Position.h" 
#include "Model.h"

using namespace GfxUtil;

class Actor
{
public:
	Actor();
	~Actor();

	bool Initialize();

	inline MovementComponent* GetMovementComponent()
	{
		return _MovementComp.get();
	}

private:
	unique_ptr<MovementComponent> _MovementComp;
	unique_ptr<Model> _Model;

};

