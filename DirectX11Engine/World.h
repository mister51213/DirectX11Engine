#pragma once

#include <memory>
#include "Position.h" 

using namespace std;

class World
{
public:
	World();
	~World();
	bool Initialize();
	void Tick(float deltaTime, class Input* pInput);
	void ProcessInput(float deltaTime, Input * pInput);
	
	unique_ptr<PositionClass> _CamPosition;
};

