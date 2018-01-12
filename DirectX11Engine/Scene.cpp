#include "Scene.h"
#include "Graphics.h"
#include <DirectXMath.h>

using namespace DirectX;
using namespace VectorMath;

Scene::Scene()
{
}

Scene::~Scene()
{}

bool Scene::Initialize()
{
	/////// INIT CAMERA ////////
	_Camera.reset(new Actor);
	_Camera->InitializeMovement(false);
	_Camera->GetMovementComponent()->SetPosition(XMFLOAT3(0.f, 5.f, -20.f));
	//_Camera->SetPosition(XMFLOAT3(0.f, 5.f, -20.f));
	//_Camera->SetOrientation(XMFLOAT3(-90, -90, 0));

	/////// INIT ACTORS ////////
	InitializeActors();

	/////// INIT LIGHTS ////////
	InitializeLights(_Actors);

	return true;
}

void Scene::InitializeActors()
{
	vector<string> actorNames = { // actors w same name must be added CONSECUTIVELY!!!!
		"Cube",
		"Sphere",
		//"Sphere",
		"Ground",
		"Wall",
		"Bath",
		"Water" };

	vector<XMFLOAT3> positions = {
		XMFLOAT3(-10.0f, 1.f, 0.0f), // cube
		XMFLOAT3(10.0f, 1.f, 0.0f), // sphere
		//XMFLOAT3(0.0f, 1.f, -5.0f), // sphere
		XMFLOAT3(0.0f, 0.0f, 0.0f), // ground
		XMFLOAT3(0.0f, 6.0f, 8.0f), // wall
		XMFLOAT3(0.0f, 2.0f, 0.0f),  // bath
		XMFLOAT3(0.0f, _waterHeight, 0.0f) }; //water

	positions.resize(actorNames.size()); // safety check

	// Instantiate actors and update their names if duplicates
	int duplicateIdx = 0;
	string DuplicateBaseName = actorNames[0];

	for (int i = 0; i < actorNames.size(); ++i)
	{
		if (i > 0)
		{
			// Decide whether to reset duplicate reference name
			if (actorNames[i - 1] == actorNames[i])
			{
				duplicateIdx = 0;
				DuplicateBaseName = actorNames[i];
			}

			// Check each name against the latest duplicate name and number it if it matches
			if (actorNames[i] == DuplicateBaseName)
			{
				if (actorNames[i - 1] == DuplicateBaseName)
				{
					actorNames[i - 1] += to_string(duplicateIdx);
				}

				duplicateIdx++;
				actorNames[i] += to_string(duplicateIdx);
			}
		}

		// Instantiate each actor with the updated name
		unique_ptr<Actor> pActor = std::make_unique<Actor>(actorNames[i]);
		pActor->InitializeMovement(VISIBLE);
		pActor->SetPosition(positions[i]);
		_Actors.emplace(pActor->Name, std::move(pActor));
	}

	// Custom tweaks on actors
	_Actors["Ground"]->SetScale(XMFLOAT3(3.f, 1.0f, 3.f));
}

void Scene::InitializeLights(map<string, unique_ptr<Actor>>& actors)
{
	for (int i = 0; i < NUM_LIGHTS; ++i)
	{
		_LightActors.push_back(unique_ptr<Actor>());
		_LightActors[i].reset(new Actor("Light" + to_string(i + 1)));
		_LightActors[i]->InitializeMovement(true);
		_LightActors[i]->SetLookAt(XMFLOAT3(0.0f, 0.0f, 0.0f));
	}

	_LightActors[0]->SetPosition(XMFLOAT3(20.f, 30.0f, 0.f));
	//_LightActors[0]->SetLookAt((-1.f*_LightActors[0]->GetPosition()));
	_LightActors[0]->SetLookAt((actors["Sphere"]->GetPosition() - _LightActors[0]->GetPosition()));

	_LightActors[1]->SetPosition(XMFLOAT3(-10.f, 30.0f, 0.f));
	//_LightActors[1]->SetLookAt((-1.f*_LightActors[1]->GetPosition()));
	_LightActors[1]->SetLookAt((actors["Sphere"]->GetPosition() - _LightActors[1]->GetPosition()));

	_LightActors[2]->SetPosition(XMFLOAT3(0.f, 30.0f,20.f));
	//_LightActors[2]->SetLookAt((-1.f*_LightActors[2]->GetPosition()));
	_LightActors[2]->SetLookAt((actors["Cube"]->GetPosition() - _LightActors[2]->GetPosition()));
}

void Scene::Tick(float deltaTime, Input* pInput)
{
	ProcessInput(deltaTime, pInput);

	UpdateActors(deltaTime);

	// Update Lights
	// light 1
	if (_lightPositionX > 1.0f)
	{
		_lightPositionX = -1.f;
	}
	if (_lightPositionZ > 1.0f)
	{
		_lightPositionZ = -1.f;
	}
	_lightPositionX += _lightPosIncrement;
	_lightPositionZ += _lightPosIncrement;
	_LightActors[0]->SetPosition(XMFLOAT3(cos(_lightPositionX)*20.f, 20.0f, sin(_lightPositionZ)*20.f));

	// light 2
	if (_lightPositionX2 > 20.0f || _lightPositionX2 < -20.0f)
	{
		_increment2 *= -1;
	}
	_lightPositionX2 += _increment2;

	_LightActors[1]->SetPosition(XMFLOAT3(_lightPositionX2, 20.0f, sin(_lightPositionZ)*-20.f));
}

void Scene::ProcessInput(float deltaTime, Input* pInput)
{
	_Camera->GetMovementComponent()->SetFrameTime(deltaTime);

	// working old method
	//int mouseX = 0.f; int mouseY = 0.f;
	//pInput->GetMouseLocation(mouseX, mouseY);
	//_Camera->SetOrientation(XMFLOAT3((float)mouseX, (float)mouseY, 0.f));

	// Bitshifting method
	uint32_t x = pInput->GetMouseXY() >> 32;
	uint32_t y = pInput->GetMouseXY() & 0xffffffff;
	_Camera->GetMovementComponent()->SetOrientation(XMFLOAT3(
		static_cast<float>(*reinterpret_cast<int *>(&(y))), 
		static_cast<float>(*reinterpret_cast<int *>(&(x))), 
		0.f));

	_Camera->GetMovementComponent()->MoveForward(pInput->IsKeyDown(DIK_W));
	_Camera->GetMovementComponent()->MoveBack(pInput->IsKeyDown(DIK_S));
	_Camera->GetMovementComponent()->MoveLeft(pInput->IsKeyDown(DIK_A));
	_Camera->GetMovementComponent()->MoveRight(pInput->IsKeyDown(DIK_D));

	////////// MOVE LIGHTS (@TEST) ////////////
	//for (int i = 0; i < NUM_LIGHTS; ++i)
	//{
	//	_LightActors[i]->GetMovementComponent()->SetFrameTime(deltaTime);
	//}
	//_LightActors[0]->GetMovementComponent()->MoveForward(pInput->IsKeyDown(DIK_UP));
	//_LightActors[0]->GetMovementComponent()->MoveBack(pInput->IsKeyDown(DIK_DOWN));
	//_LightActors[0]->GetMovementComponent()->MoveLeft(pInput->IsKeyDown(DIK_LEFT));
	//_LightActors[0]->GetMovementComponent()->MoveRight(pInput->IsKeyDown(DIK_RIGHT));
}

void Scene::UpdateActors(float deltaTime)
{}
