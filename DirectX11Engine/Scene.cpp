#include "Scene.h"
#include "Input.h"
#include <DirectXMath.h>

using namespace DirectX;

Scene::Scene()
{
}

Scene::~Scene()
{
}

bool Scene::Initialize()
{
	_Camera.reset(new Actor);
	_Camera->Initialize(false);

	for (int i = 0; i < _numActors; ++i)
	{
		_Actors.push_back(unique_ptr<Actor>());
		_Actors[i].reset(new Actor);
		_Actors[i]->Initialize(true);
	}

	_Actors[0]->GetMovementComponent()->SetPosition(XMFLOAT3(0.0f, 1.0f, 0.0f));
	_Actors[1]->GetMovementComponent()->SetPosition(XMFLOAT3(0.0f, 6.0f, 8.0f));
	_Actors[2]->GetMovementComponent()->SetPosition(XMFLOAT3(0.0f, 2.0f, 0.0f));
	_Actors[3]->GetMovementComponent()->SetPosition(XMFLOAT3(0.0f, 2.75, 0.0f));

	return true;
}

void Scene::Tick(float deltaTime, Input* pInput)
{
	ProcessInput(deltaTime, pInput);

	UpdateActors(deltaTime);

}

void Scene::ProcessInput(float deltaTime, Input* pInput)
{

	_Camera->GetMovementComponent()->SetFrameTime(deltaTime);

	// working old method
	//int mouseX = 0.f; int mouseY = 0.f;
	//pInput->GetMouseLocation(mouseX, mouseY);
	//_Camera->SetOrientation(XMFLOAT3((float)mouseY, (float)mouseX, 0.f));

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
}

void Scene::UpdateActors(float deltaTime)
{


}

Actor ** Scene::GetActors() const
{
	vector<Actor*> actors;

	for (auto& actor : _Actors)
	{
		actors.push_back(actor.get());
	}

	return actors.data();
}


