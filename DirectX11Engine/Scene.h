#pragma once

#include <memory>
#include <vector>

#include "Actor.h"
#include "Position.h" 
#include "LightClass.h"

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
	
	Actor** GetActors() const;
	inline Actor* GetCamera() const { return _Camera.get(); }

	int _numActors = 4;

	unique_ptr<Actor> _Camera;
	vector<unique_ptr<Actor>> _Actors;
	vector<unique_ptr<Actor>> _LightActors;

	const int NUM_LIGHTS = 4;
};

