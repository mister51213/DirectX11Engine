#pragma once

#include "btBulletDynamicsCommon.h"

class Physics
{
public:
	Physics();
	~Physics();

	bool Initialize();

	void Tick(float deltaTime);

};

