#ifndef PHYSICS_H
#define PHYSICS_H

#include "btBulletDynamicsCommon.h"
#include "Utility.h"
#include <map>

using namespace std;


/*
필요한 기능
1. 

*/
class GameObject;
class Transform;

class Physics
{
private:
	map<GameObject *, btCollisionObject*> collisionObjectsTable;
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btBroadphaseInterface* overlappingPairCache;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamicsWorld;

	void init();
	void release();
	bool exists(GameObject * other);



protected:

	Physics()
	{
		init();
	}
	virtual ~Physics()
	{
		release();
	}

public:

	friend class Locator;

	virtual void registerRigidBody(GameObject * other, Transform * transform, bool isSphereCollider = true);
	virtual void unregisterRigidBody(GameObject * other);
	virtual void addForce(GameObject * other, Vector3 & directionWithScalar);

	// 최신화시킨 위치를 받아오거나 / 사용자가 GameObject를 인위적으로 움직였을때 시스템에 접근해서 최신화시켜줘야 한다.
	// rigidbody를 가지고 있다면 이 함수를 통해서 계속해서 위치를 받아온다.
	virtual void setTransformFromSystem(Transform * output, GameObject * other);

	// 사용자가 GameObject를 인위적으로 움직였을때 최신화 시키기 위한 함수이다.
	virtual void setTransformToSystem(const Transform * input, GameObject * other);

	// 여기서 델타타임은 sec단위이다. // 16 milliSec = 0.016 sec
	virtual void update(float deltaTime);
	virtual void setGravity(GameObject * other, Vector3 & value);
	virtual void getGravity(Vector3 & output, GameObject * other);
};

class NullPhysics : public Physics
{
private:
	NullPhysics()
		: Physics() {}
	virtual ~NullPhysics() {}

public:
	friend class Locator;
	virtual void registerRigidBody(GameObject * other, Transform * transform, bool isSphereCollider = true) {}
	virtual void unregisterRigidBody(GameObject * other) {}
	virtual void addForce(GameObject * other, Vector3 & directionWithScalar) {}

	// 여기서 델타타임은 sec단위이다. // 16 milliSec = 0.016 sec
	virtual void update(float deltaTime) {}
	virtual void setGravity(GameObject * other, Vector3 & value) {}
	virtual void getGravity(Vector3 & output, GameObject * other) {}
};

class DebuggingPhysics : public Physics
{

private:
	Physics & wrapped;

	DebuggingPhysics(Physics & physcis)
		: wrapped(physcis) {}
	virtual ~DebuggingPhysics() {}

	void log(const char * message)
	{
		if (!message) return;
		cout << message << endl;
	}

public:
	friend class Locator;
	//  나중에 로그 찍는 것으로 바꿀예정

};

#endif