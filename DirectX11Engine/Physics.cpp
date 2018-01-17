#include "Physics.h"

#include "LinearMath/btHashMap.h"
#include "BulletSoftBody/btSoftRigidDynamicsWorld.h"
#include "BulletSoftBody/btSoftBodyHelpers.h"

#include "BulletSoftBody/btSoftBodySolvers.h"
#include "BulletSoftBody/btDefaultSoftBodySolver.h"
#include "BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h"

#include "Actor.h"
#include "Model.h"

using namespace std;

Physics::Physics()
{
	registeredObjects = -1;

	///-----initialization_start-----
	//softBodySolver = new btDefaultSoftBodySolver();
	softBodySolver = NULL;

	///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	//collisionConfiguration = new btDefaultCollisionConfiguration();
	collisionConfiguration = new btSoftBodyRigidBodyCollisionConfiguration();

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	dispatcher = new	btCollisionDispatcher(collisionConfiguration);

	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	overlappingPairCache = new btDbvtBroadphase();

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	solver = new btSequentialImpulseConstraintSolver;

	//dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,overlappingPairCache,solver,collisionConfiguration);
	dynamicsWorld = new btSoftRigidDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration, softBodySolver);


	dynamicsWorld->getSolverInfo().m_solverMode |= SOLVER_RANDMIZE_ORDER;
	dynamicsWorld->getDispatchInfo().m_enableSatConvex = true;
	dynamicsWorld->getSolverInfo().m_splitImpulse = true;

	dynamicsWorld->setGravity(btVector3(0, -11, 0));
	((btSoftRigidDynamicsWorld*)dynamicsWorld)->getWorldInfo().air_density = (btScalar)1.0;
	((btSoftRigidDynamicsWorld*)dynamicsWorld)->getWorldInfo().water_density = 0;
	((btSoftRigidDynamicsWorld*)dynamicsWorld)->getWorldInfo().water_offset = 0;
	((btSoftRigidDynamicsWorld*)dynamicsWorld)->getWorldInfo().water_normal = btVector3(0, 0, 0);
	((btSoftRigidDynamicsWorld*)dynamicsWorld)->getWorldInfo().m_gravity.setValue(0, -11, 0);
	((btSoftRigidDynamicsWorld*)dynamicsWorld)->getWorldInfo().m_sparsesdf.Initialize(); //???
}


Physics::~Physics()
{
	ClearWorld();

	//delete dynamics world
	delete dynamicsWorld;

	//delete solver
	delete solver;

	//delete broadphase
	delete overlappingPairCache;

	//delete dispatcher
	delete dispatcher;

	delete collisionConfiguration;

	//next line is optional: it will be cleared by the destructor when the array goes out of scope
	collisionShapes.clear();

	///-----cleanup_end-----
	CleanUp();
}

bool Physics::Initialize()
{
	ThrowRuntime("Currently placeholder code, not actually used! - Most init is now done in constructor");

	btBroadphaseInterface* broadphase = new btDbvtBroadphase();

	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);

	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;

	btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);

	dynamicsWorld->setGravity(btVector3(0, -10, 0));


	btCollisionShape* groundShape = //new btSphereShape(1);
		new btStaticPlaneShape(btVector3(0, 1, 0), 1);

	btCollisionShape* fallShape = new btSphereShape(1);


	btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -1, 0)));
	btRigidBody::btRigidBodyConstructionInfo
		groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
	btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
	dynamicsWorld->addRigidBody(groundRigidBody);

	btDefaultMotionState* fallMotionState =
		new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 50, 0)));
	btScalar mass = 1;
	btVector3 fallInertia(0, 0, 0);
	fallShape->calculateLocalInertia(mass, fallInertia);
	btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, fallMotionState, fallShape, fallInertia);
	btRigidBody* fallRigidBody = new btRigidBody(fallRigidBodyCI);
	dynamicsWorld->addRigidBody(fallRigidBody);

	return true;
}

void Physics::Tick(float deltaTime)
{
	if (bRigidBodyPhysicsEnabled)
	{
		//dynamicsWorld->stepSimulation(deltaTime, 6);
	}
}

PhysicsTransform* Physics::GetPhysicsTransform(int index)
{

	btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[index];
	btTransform trans;

	btRigidBody* body = btRigidBody::upcast(obj);
	if (body && body->getMotionState())
	{
		body->getMotionState()->getWorldTransform(trans);
	}

	btSoftBody* softBody = btSoftBody::upcast(obj);
	if (softBody)
	{
		trans = softBody->getWorldTransform();
	}

	btQuaternion rot = trans.getRotation();
	btVector3 pos = trans.getOrigin();
	transforms[index]->rotation = XMFLOAT4(rot.getX(), rot.getY(), rot.getZ(), rot.getW());
	transforms[index]->position = XMFLOAT3(pos.getX(), pos.getY(), pos.getZ());
	return transforms[index];
}

void Physics::RegisterActor(Actor* actor)
{
	if (bRigidBodyPhysicsEnabled && actor->bRigidBody && actor->GetModel())
	{
		XMFLOAT4 R;
		XMFLOAT3 S, T;
		//actor->ApplyTransform();
		//actor->AttachTo(object->GetRoot());

		S = actor->GetScale();
		T = actor->GetPosition();
		R = EulerToQuat(actor->GetOrientation());

		if (!actor->collisionShape.compare("BOX")) {
			AddBox(
				S, R, T, 
				actor->mass, actor->friction, actor->restitution, 
				actor->damping, actor->bKinematic);
			actor->physicsObjectID = ++registeredObjects;
		}
		if (!actor->collisionShape.compare("SPHERE")) {
			AddSphere(
				S.x, T
				, actor->mass, actor->friction, actor->restitution
				, actor->damping, actor->bKinematic
			);
			actor->physicsObjectID = ++registeredObjects;
		}
		if (!actor->collisionShape.compare("CAPSULE")) {
			AddCapsule(
				S.x, S.y, R, T
				, actor->mass, actor->friction, actor->restitution
				, actor->damping, actor->bKinematic
			);
			actor->physicsObjectID = ++registeredObjects;
		}
		if (!actor->collisionShape.compare("MESH")) {
			//vector<XMFLOAT4> pos_stream(actor->GetModel()->_Vertices.size());
			//for (size_t i = 0; i < actor->mesh->vertices_POS.size(); ++i)
			//{
			//	pos_stream[i].x = actor->mesh->vertices_POS[i].pos.x;
			//	pos_stream[i].y = actor->mesh->vertices_POS[i].pos.y;
			//	pos_stream[i].z = actor->mesh->vertices_POS[i].pos.z;
			//	pos_stream[i].w = 1;
			//}

			AddTriangleMesh(
				actor->GetModel()->_Vertices, actor->GetModel()->_Indices,
				S, R, T
				, actor->mass, actor->friction, actor->restitution
				, actor->damping, actor->bKinematic
			);
			actor->physicsObjectID = ++registeredObjects;
		}
	}

	actor->physicsObjectID = ++registeredObjects;
}

void Physics::DeleteActor(int id)
{
	btCollisionObject*	obj = dynamicsWorld->getCollisionObjectArray()[id];
	btRigidBody*		body = btRigidBody::upcast(obj);

	if (body && body->getMotionState())
	{
		delete body->getMotionState();
	}
	while (dynamicsWorld->getNumConstraints())
	{
		btTypedConstraint*	pc = dynamicsWorld->getConstraint(0);
		dynamicsWorld->removeConstraint(pc);
		delete pc;
	}

	btSoftBody* softBody = btSoftBody::upcast(obj);
	if (softBody)
	{
		((btSoftRigidDynamicsWorld*)dynamicsWorld)->removeSoftBody(softBody);
	}
	else
	{
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body)
			dynamicsWorld->removeRigidBody(body);
		else
			dynamicsWorld->removeCollisionObject(obj);
	}
	delete obj;

	registeredObjects--;
}

void Physics::RemoveActor(Actor* actor)
{
	if (actor == nullptr || actor->physicsObjectID < 0)
	{
		return;
	}

	DeleteActor(actor->physicsObjectID);
	actor->physicsObjectID = -1;
}

void Physics::AddBox(const XMFLOAT3& sca, const XMFLOAT4& rot, const XMFLOAT3& pos,
	float newMass, float newFriction, float newRestitution, float newDamping, bool bKinematic)
{

	btCollisionShape* shape = new btBoxShape(btVector3(sca.x, sca.y, sca.z));
	shape->setMargin(btScalar(0.05));
	collisionShapes.push_back(shape);

	btTransform shapeTransform;
	shapeTransform.setIdentity();
	shapeTransform.setOrigin(btVector3(pos.x, pos.y, pos.z));

	shapeTransform.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));
	{
		btScalar mass(newMass);

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f && !bKinematic);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
		{
			shape->calculateLocalInertia(mass, localInertia);
		}
		else
		{
			mass = 0;
		}

		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(shapeTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, shape, localInertia);
		rbInfo.m_friction = newFriction;
		rbInfo.m_restitution = newRestitution;
		rbInfo.m_linearDamping = newDamping;
		rbInfo.m_angularDamping = newDamping;
		btRigidBody* body = new btRigidBody(rbInfo);
		if (bKinematic) body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		body->setActivationState(DISABLE_DEACTIVATION);

		//add the body to the dynamics world
		dynamicsWorld->addRigidBody(body);


		if (body->getMotionState())
		{
			btTransform trans;
			body->getMotionState()->getWorldTransform(trans);
			btQuaternion nRot = trans.getRotation();
			btVector3 nPos = trans.getOrigin();
			transforms.push_back(new PhysicsTransform(
				XMFLOAT4(nRot.getX(), nRot.getY(), nRot.getZ(), nRot.getW()), XMFLOAT3(nPos.getX(), nPos.getY(), nPos.getZ())));
		}
	}
}

void Physics::AddSphere(float rad, const XMFLOAT3& pos, float newMass, float newFriction, float newRestitution, float newDamping, bool bKinematic)
{
	btCollisionShape* shape = new btSphereShape(btScalar(rad));
	shape->setMargin(btScalar(0.05));
	collisionShapes.push_back(shape);

	btTransform shapeTransform;
	shapeTransform.setIdentity();
	shapeTransform.setOrigin(btVector3(pos.x, pos.y, pos.z));
	{
		btScalar mass(newMass);

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f && !bKinematic);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			shape->calculateLocalInertia(mass, localInertia);
		else
			mass = 0;

		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(shapeTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, shape, localInertia);
		rbInfo.m_friction = newFriction;
		rbInfo.m_restitution = newRestitution;
		rbInfo.m_linearDamping = newDamping;
		rbInfo.m_angularDamping = newDamping;
		btRigidBody* body = new btRigidBody(rbInfo);
		if (bKinematic) body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		body->setActivationState(DISABLE_DEACTIVATION);

		//add the body to the dynamics world
		dynamicsWorld->addRigidBody(body);


		if (body->getMotionState())
		{
			btTransform trans;
			body->getMotionState()->getWorldTransform(trans);
			btQuaternion nRot = trans.getRotation();
			btVector3 nPos = trans.getOrigin();
			transforms.push_back(new PhysicsTransform(
				XMFLOAT4(nRot.getX(), nRot.getY(), nRot.getZ(), nRot.getW()), XMFLOAT3(nPos.getX(), nPos.getY(), nPos.getZ()))
			);
		}
	}


}

void Physics::AddCapsule(float rad, float hei, const XMFLOAT4& rot, const XMFLOAT3& pos, float newMass, float newFriction, float newRestitution, float newDamping, bool bKinematic) 
{

	btCollisionShape* shape = new btCapsuleShape(btScalar(rad), btScalar(hei));
	shape->setMargin(btScalar(0.05));
	collisionShapes.push_back(shape);

	btTransform shapeTransform;
	shapeTransform.setIdentity();
	shapeTransform.setOrigin(btVector3(pos.x, pos.y, pos.z));
	shapeTransform.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));
	{
		btScalar mass(newMass);

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f && !bKinematic);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			shape->calculateLocalInertia(mass, localInertia);
		else
			mass = 0;

		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(shapeTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, shape, localInertia);
		rbInfo.m_friction = newFriction;
		rbInfo.m_restitution = newRestitution;
		rbInfo.m_linearDamping = newDamping;
		rbInfo.m_angularDamping = newDamping;
		btRigidBody* body = new btRigidBody(rbInfo);
		if (bKinematic) body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		body->setActivationState(DISABLE_DEACTIVATION);

		//add the body to the dynamics world
		dynamicsWorld->addRigidBody(body);


		if (body->getMotionState())
		{
			btTransform trans;
			body->getMotionState()->getWorldTransform(trans);
			btQuaternion nRot = trans.getRotation();
			btVector3 nPos = trans.getOrigin();
			transforms.push_back(new PhysicsTransform(
				XMFLOAT4(nRot.getX(), nRot.getY(), nRot.getZ(), nRot.getW()), XMFLOAT3(nPos.getX(), nPos.getY(), nPos.getZ()))
			);
		}
	}
}

void Physics::AddTriangleMesh(const std::vector<XMFLOAT4>& vertices, const std::vector<unsigned int>& indices, const XMFLOAT3& sca, const XMFLOAT4& rot, const XMFLOAT3& pos, float newMass, float newFriction, float newRestitution, float newDamping, bool bKinematic) {

	int totalVerts = (int)vertices.size();
	int totalTriangles = (int)indices.size() / 3;

	btVector3* btVerts = new btVector3[totalVerts];
	for (unsigned int i = 0; i<vertices.size(); ++i)
		btVerts[i] = (btVector3(vertices[i].x, vertices[i].y, vertices[i].z));

	int* btInd = new int[indices.size()];
	for (unsigned int i = 0; i<indices.size(); ++i)
		btInd[i] = indices[i];

	int vertStride = sizeof(btVector3);
	int indexStride = 3 * sizeof(int);

	btTriangleIndexVertexArray* indexVertexArrays = new btTriangleIndexVertexArray(
		totalTriangles,
		btInd,
		indexStride,
		totalVerts,
		(btScalar*)&btVerts[0].x(),
		vertStride
	);

	bool useQuantizedAabbCompression = true;

	btCollisionShape* shape = new btBvhTriangleMeshShape(indexVertexArrays, useQuantizedAabbCompression);
	shape->setMargin(btScalar(0.05));
	shape->setLocalScaling(btVector3(sca.x, sca.y, sca.z));

	collisionShapes.push_back(shape);

	btTransform shapeTransform;
	shapeTransform.setIdentity();
	shapeTransform.setOrigin(btVector3(pos.x, pos.y, pos.z));
	shapeTransform.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));
	{
		btScalar mass(newMass);

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f && !bKinematic);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			shape->calculateLocalInertia(mass, localInertia);
		else
			mass = 0;

		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(shapeTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, shape, localInertia);
		rbInfo.m_friction = newFriction;
		rbInfo.m_restitution = newRestitution;
		rbInfo.m_linearDamping = newDamping;
		rbInfo.m_angularDamping = newDamping;
		btRigidBody* body = new btRigidBody(rbInfo);
		if (bKinematic) body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		body->setActivationState(DISABLE_DEACTIVATION);

		//add the body to the dynamics world
		dynamicsWorld->addRigidBody(body);

		if (body->getMotionState())
		{
			btTransform trans;
			body->getMotionState()->getWorldTransform(trans);
			btQuaternion nRot = trans.getRotation();
			btVector3 nPos = trans.getOrigin();
			transforms.push_back(new PhysicsTransform(
				XMFLOAT4(nRot.getX(), nRot.getY(), nRot.getZ(), nRot.getW()), XMFLOAT3(nPos.getX(), nPos.getY(), nPos.getZ())));
		}
	}
}

void Physics::ClearWorld()
{
	for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		DeleteActor(i);
	}

	//delete collision shapes
	for (int j = 0; j<collisionShapes.size(); j++)
	{
		btCollisionShape* shape = collisionShapes[j];
		collisionShapes[j] = 0;
		delete shape;
	}

	//delete transfom interface
	for (unsigned int i = 0; i<transforms.size(); ++i)
		delete transforms[i];
	transforms.clear();
	registeredObjects = -1;
}

void Physics::CleanUp()
{
	for (unsigned int i = 0; i < transforms.size(); ++i)
		delete transforms[i];
	transforms.clear();
}