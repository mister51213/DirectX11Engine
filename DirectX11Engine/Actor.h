#pragma once

#include "GfxUtil.h"
#include "Position.h" 
#include "Model.h"
#include <string>

using namespace GfxUtil;

class Actor
{
public:
	Actor();
	Actor::Actor(const string& name);

	bool InitializeMovement(bool visible);

	/////////// Accessor functions ///////////////
	XMFLOAT3 GetPosition()const;
	XMFLOAT3 GetOrientation()const;
	inline XMFLOAT3 GetScale() 
	{ 
		//if (_Model)
		//{
		//	return _Model->GetScale();
		//}
		//else
		{
			return _scale;
		}
	}
	inline void SetScale(XMFLOAT3 scale) 
	{
		_scale = scale;
		if (_Model)
		{
			_Model->SetScale(scale);
		}
		//else
		//{
		//	ThrowRuntime("Model not found!");
		//}
	}
	XMFLOAT3 GetLookAt() const;

	void SetPosition(const XMFLOAT3& pos);
	void SetOrientation(const XMFLOAT3& or);

	void SetLookAt(const XMFLOAT3 & lookat);

	inline MovementComponent* GetMovementComponent()
	{
		return _MovementComp.get();
	}

	inline void InstantiateModel(Model* model) { _Model.reset(model); }
	//inline void SetModel(unique_ptr<Model> model){_Model = model;}

	inline Model* GetModel() const { return _Model.get(); }


	string Name;
	bool bCustomAppearance = false;
	bool bVisible = true;

private:
	unique_ptr<MovementComponent> _MovementComp;
	unique_ptr<Model> _Model;
	//shared_ptr<Model> _Model;
	XMFLOAT3 _scale;

};

