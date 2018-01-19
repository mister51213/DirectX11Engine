#pragma once

#include <memory>
#include <stdlib.h>
#include <vector>
#include <map>
#include <string>

#include "Input.h"
#include "Actor.h"
#include "Position.h" 
#include "LightClass.h"
#include "GlobalIncludes.h"

using namespace std;

class Scene
{
public:
	Scene();
	//~Scene();
	bool Initialize();
	void InitializeLights(map<string, unique_ptr<Actor>>& actors);
	void InitializeActors();
	void Tick(float deltaTime, Input* pInput);
	void ProcessInput(float deltaTime, Input* pInput);
	void UpdateActors(float deltaTime);
	
	inline Actor* GetCamera() const { return _Camera.get(); }

	unique_ptr<Actor> _Camera;
	map<string, unique_ptr<Actor>> _Actors;
	vector<unique_ptr<Actor>> _LightActors;

	/////////// Light Animation /////////////
	float _lightPositionX = 1.f;
	float _lightPositionZ = 1.f;
	float _lightPosIncrement = -0.008f;

	float _lightPositionX2 = 5.f;
	float _increment2 = -0.08f;
	
	const int NUM_LIGHTS = 3;

	//////// WATER CONSTANTS /////////
	float _waterHeight = 2.75f;
};

