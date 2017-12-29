#include "Scene.h"
#include "Input.h"
#include "Graphics.h"
#include <DirectXMath.h>

using namespace DirectX;

Scene::Scene()
{
}

Scene::~Scene()
{}

bool Scene::Initialize()
{
	_Camera.reset(new Actor);
	_Camera->InitializeMovement(false);
	_Camera->GetMovementComponent()->SetPosition(XMFLOAT3(0.f,5.f,-12.f));

	/////// INIT ACTORS //////////////
	vector<XMFLOAT3> positions = { XMFLOAT3(-2.0f, 1.f, 0.0f), XMFLOAT3(2.0f, 1.f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.f, 2.0f) };

	for (int i = 0; i < _numActors; ++i)
	{
		unique_ptr<Actor> pActor = std::make_unique<Actor>("Actor"+ to_string(i + 1));
		pActor->InitializeMovement(true); 
		pActor->SetPosition(positions[i]);
		
		_Actors.emplace(pActor->Name, std::move(pActor));
	}
	
	///// INIT LIGHTS //////
	for (int i = 0; i < NUM_LIGHTS; ++i)
	{
		//_Lights.push_back(unique_ptr<LightClass>());
		//_Lights[i].reset(new LightClass);
		//if (!_Lights[i])return false;

		//_Lights[i]->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
		//_Lights[i]->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
		//_Lights[i]->SetLookAt(0.0f, 0.0f, 0.0f);
		//_Lights[i]->GenerateProjectionMatrix(SCREEN_DEPTH, SCREEN_NEAR);

		_LightActors.push_back(unique_ptr<Actor>());
		_LightActors[i].reset(new Actor("Light" + to_string(i + 1)));
		_LightActors[i]->InitializeMovement(true);
		_LightActors[i]->SetLookAt(XMFLOAT3(0.0f, 0.0f, 0.0f));
	}
	
	//_Lights[1]->SetPosition(-5.f, 8.0f, -5.f);
	_LightActors[0]->SetPosition(XMFLOAT3 (5.f, 8.0f, -5.f));
	_LightActors[1]->SetPosition(XMFLOAT3(-5.f, 8.0f, -5.f));

	///// WATER DEMO SETUP //////
	// Overwrite the last 4 actors in the array with custom appearance (initialization by hand)
	//int indexToStopAt = _Actors.size() - 4;
	//for (int i = _Actors.size() - 1; i >= indexToStopAt; --i)
	//{
	//	_Actors[i]->bCustomAppearance = true;
	//}

	//_Actors[_Actors.size() - 4]->SetPosition(XMFLOAT3(0.0f, 1.0f, 0.0f));
	//_Actors[_Actors.size() - 3]->SetPosition(XMFLOAT3(0.0f, 6.0f, 8.0f));
	//_Actors[_Actors.size() - 2]->SetPosition(XMFLOAT3(0.0f, 2.0f, 0.0f));
	//_Actors[_Actors.size() - 1]->SetPosition(XMFLOAT3(0.0f, 2.75, 0.0f));

	return true;
}

void Scene::Tick(float deltaTime, Input* pInput)
{
	ProcessInput(deltaTime, pInput);

	UpdateActors(deltaTime);

	// Update Lights
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
	//_Lights[0]->SetPosition(cos(_lightPositionX)*5.f, 8.0f, sin(_lightPositionZ)*5.f);
	_LightActors[0]->SetPosition(XMFLOAT3(cos(_lightPositionX)*5.f, 8.0f, sin(_lightPositionZ)*5.f));
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


	////////// MOVE LIGHTS (TEST) ////////////
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
