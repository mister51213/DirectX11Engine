#include "World.h"
#include "Input.h"
#include <DirectXMath.h>

using namespace DirectX;

World::World()
{
}

World::~World()
{
}

bool World::Initialize()
{
	// Create the position object.
	_CamPosition.reset(new PositionClass);

	return true;
}

void World::Tick(float deltaTime, Input* pInput)
{
	ProcessInput(deltaTime, pInput);
}

void World::ProcessInput(float deltaTime, Input* pInput)
{
	_CamPosition->SetFrameTime(deltaTime);

	// working old method
	//int mouseX = 0.f; int mouseY = 0.f;
	//pInput->GetMouseLocation(mouseX, mouseY);
	//_CamPosition->SetOrientation(XMFLOAT3((float)mouseY, (float)mouseX, 0.f));

	// Bitshifting method
	uint32_t x = pInput->GetMouseXY() >> 32;
	uint32_t y = pInput->GetMouseXY() & 0xffffffff;
	_CamPosition->SetOrientation(XMFLOAT3(
		static_cast<float>(*reinterpret_cast<int *>(&(y))), 
		static_cast<float>(*reinterpret_cast<int *>(&(x))), 
		0.f));

	_CamPosition->MoveForward(pInput->IsKeyDown(DIK_W));
	_CamPosition->MoveBack(pInput->IsKeyDown(DIK_S));
	_CamPosition->MoveLeft(pInput->IsKeyDown(DIK_A));
	_CamPosition->MoveRight(pInput->IsKeyDown(DIK_D));
}


