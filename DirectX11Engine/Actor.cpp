#include "Actor.h"

Actor::Actor()
	:
	Name("Actor")
{
	_scale = XMFLOAT3(1.f,1.f,1.f);
	_MovementComp.reset(new MovementComponent);
}

Actor::Actor(const string& name)
	:
	Name(name)
{
	_MovementComp.reset(new MovementComponent);
}

bool Actor::InitializeMovement(bool visible)
{
	bVisible = visible;

	_scale = XMFLOAT3(1.f, 1.f, 1.f);
	if (_MovementComp)
	{
		_MovementComp->Initialize();
	}
	else
	{
		ThrowRuntime("MovementComp not found!");
	}

	return true;
}

XMFLOAT3 Actor::GetPosition() const
{
	if (_MovementComp)
	{
		return _MovementComp->GetPosition();
	}
	else
	{
		ThrowRuntime("MovementComp not found!");
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
		ThrowRuntime("MovementComp not found!");
	}
}

XMFLOAT3 Actor::GetLookAt() const
{
	if (_MovementComp)
	{
		return _MovementComp->GetLookAt();
	}
	else
	{
		ThrowRuntime("MovementComp not found!");
	}
}

void Actor::SetPosition(const XMFLOAT3 & pos)
{
	if (_MovementComp)
	{
		_MovementComp->SetPosition(pos);
	}
	else
	{
		ThrowRuntime("MovementComp not found!");
	}
}

void Actor::SetOrientation(const XMFLOAT3 & or )
{
	if (_MovementComp)
	{
		_MovementComp->SetOrientation(or);
	}
	else
	{
		ThrowRuntime("MovementComp not found!");
	}
}

void Actor::SetLookAt(const XMFLOAT3 & lookat)
{
	if (_MovementComp)
	{
		_MovementComp->SetLookAt(lookat);
	}
	else
	{
		ThrowRuntime("MovementComp not found!");
	}
}