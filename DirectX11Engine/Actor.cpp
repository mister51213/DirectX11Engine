#include "Actor.h"



Actor::Actor()
{
	_MovementComp.reset(new MovementComponent);
}

bool Actor::InitializeMovement(bool visible)
{
	bVisible = visible;

	_MovementComp->Initialize();

	return true;
}

XMFLOAT3 Actor::
GetPosition() const
{
	if (_MovementComp)
	{
		return _MovementComp->GetPosition();
	}
	else
	{
		return XMFLOAT3();
	}
}

XMFLOAT3 Actor::GetOrientation() const
{
	if (_MovementComp)
	{
		return _MovementComp->GetOrientation();
	}
	else
	{
		return XMFLOAT3();
	}
}

void Actor::SetPosition(const XMFLOAT3 & pos)
{
	if (_MovementComp)
	{
		_MovementComp->SetPosition(pos);
	}
}

void Actor::SetOrientation(const XMFLOAT3 & or )
{
	if (_MovementComp)
	{
		_MovementComp->SetOrientation(or);
	}
}
