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

	int mouseX = 0.f; int mouseY = 0.f;
	pInput->GetMouseLocation(mouseX, mouseY);

	//long int mouseY = pInput->GetMouseXY() & 0xFFFFFFFF;
	//long int mouseX = (pInput->GetMouseXY() >> 32) & 0xFFFFFFFF;

	_CamPosition->SetOrientation(XMFLOAT3((float)mouseY, (float)mouseX, 0.f));

	//@STUDY command vs observer pattern
	//_CamPosition->TurnLeft(pInput->IsLeftArrowPressed());
	//_CamPosition->TurnRight(pInput->IsRightArrowPressed());
	_CamPosition->MoveForward(pInput->IsKeyDown(DIK_W));
	_CamPosition->MoveBack(pInput->IsKeyDown(DIK_S));
	_CamPosition->MoveLeft(pInput->IsKeyDown(DIK_A));
	_CamPosition->MoveRight(pInput->IsKeyDown(DIK_D));










}


