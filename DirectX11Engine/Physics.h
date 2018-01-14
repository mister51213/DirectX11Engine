#pragma once

#include "GlobalIncludes.h"
#include "btBulletDynamicsCommon.h"

using namespace VectorMath;

class Actor;
class btCollisionConfiguration;
class btCollisionDispatcher;
class btBroadphaseInterface;
class btSequentialImpulseConstraintSolver;
class btDynamicsWorld;
class btCollisionShape;
class btSoftBodySolver;
class btSoftBodySolverOutput;

class Physics
{
protected:
	std::vector<PhysicsTransform*> transforms;
	bool firstRunWorld;
	int registeredObjects;
private:
	btCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btBroadphaseInterface* overlappingPairCache;
	btSequentialImpulseConstraintSolver* solver;
	btDynamicsWorld* dynamicsWorld;
	btAlignedObjectArray<btCollisionShape*> collisionShapes;

	btSoftBodySolver* softBodySolver;
	btSoftBodySolverOutput* softBodySolverOutput;

	void DeleteActor(int id);
public:
	Physics();
	~Physics();

	bool Initialize();

	void Tick(float deltaTime);
	void FirstRunWorld() { firstRunWorld = true; }
	void NextRunWorld() { firstRunWorld = false; }

	int GetObjectCount() { return registeredObjects + 1; }

	void AddBox(const XMFLOAT3& sca, const XMFLOAT4& rot, const XMFLOAT3& pos, float newMass = 1, float newFriction = 1, float newRestitution = 1, float newDamping = 1, bool bKinematic = false);

	void AddSphere(float rad, const XMFLOAT3& pos, float newMass = 1, float newFriction = 1, float newRestitution = 1, float newDamping = 1, bool bKinematic = false);

	void AddCapsule(float rad, float hei, const XMFLOAT4& rot, const XMFLOAT3& pos, float newMass = 1, float newFriction = 1, float newRestitution = 1, float newDamping = 1, bool bKinematic = false);

	void AddTriangleMesh(const std::vector<XMFLOAT4>& vertices, const std::vector<unsigned int>& indices, const XMFLOAT3& sca, const XMFLOAT4& rot, const XMFLOAT3& pos, float newMass = 1, float newFriction = 1, float newRestitution = 1, float newDamping = 1, bool bKinematic = false);

	PhysicsTransform* GetPhysicsTransform(int index);

	void RegisterActor(Actor* object);
	void RemoveActor(Actor* object);

	void ClearWorld();
	void CleanUp();
};