#pragma once

#include <memory>
#include <stdlib.h>
#include <vector>
#include <map>
#include <string>

#include "Actor.h"
#include "Position.h" 
#include "LightClass.h"
#include "GlobalIncludes.h"

using namespace std;

class Scene
{
public:
	Scene();
	~Scene();
	bool Initialize();
	void Tick(float deltaTime, class Input* pInput);
	void ProcessInput(float deltaTime, Input * pInput);
	void UpdateActors(float deltaTime);
	
	inline Actor* GetCamera() const { return _Camera.get(); }

	int _numActors = 3;

	unique_ptr<Actor> _Camera;
	map<string, unique_ptr<Actor>> _Actors;
	vector<unique_ptr<Actor>> _LightActors;

	/////////// Light Animation /////////////
	float _lightPositionX = 1.f;
	float _lightPositionZ = 1.f;
	float _lightPosIncrement = -0.008f;

	const int NUM_LIGHTS = 3;
};

