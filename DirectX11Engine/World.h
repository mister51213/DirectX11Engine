#pragma once

#include <memory>
#include <vector>

#include "Position.h" 
#include "LightClass.h"

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
	vector<unique_ptr<PositionClass>> _ActorPositions;
	vector<unique_ptr<LightClass>> _Lights;
};

