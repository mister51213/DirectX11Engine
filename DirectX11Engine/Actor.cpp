#include "Actor.h"



Actor::Actor()
{
	_MovementComp.reset(new MovementComponent);
}

Actor::~Actor()
{
}

bool Actor::InitializeMovement(bool visible)
{
	bVisible = visible;

	_MovementComp->Initialize();

	return true;
}