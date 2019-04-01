#ifndef PHYSICS_H
#define PHYSICS_H

#include "btBulletDynamicsCommon.h"
#include "Utility.h"
#include "Trace.h"
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
	// 두개의 테이블을 선택한 이유
	/*
	1. 등록하거나 해제할때 GameObject * 기준으로 작업
	2. 콜백함수 호출시 btCollisionObject * 기준으로 작업
	*/
	static Physics * currentPhysics;

	map<GameObject *, btCollisionObject*> collisionObjectsTable;
	map<btCollisionObject *, GameObject *> gameObjectsTable;

	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btBroadphaseInterface* overlappingPairCache;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamicsWorld;

	void init();
	void release();
	bool exists(GameObject * other) const;
	void setBaseConstructionInfo(btRigidBody::btRigidBodyConstructionInfo & info) const;
	static void collisionCallBack(btDynamicsWorld *world, btScalar timeStep);


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

	virtual void registerRigidBody(GameObject * other);
	virtual void registerRigidBody(GameObject * other, btCollisionShape * shape, float mass = 1.0f);
	virtual void unregisterRigidBody(GameObject * other);
	virtual void addForce(GameObject * other,const Vector3 & directionWithScalar);

	// 최신화시킨 위치를 받아오거나 / 사용자가 GameObject를 인위적으로 움직였을때 시스템에 접근해서 최신화시켜줘야 한다.
	// rigidbody를 가지고 있다면 이 함수를 통해서 계속해서 위치를 받아온다.
	virtual void setTransformFromSystem(GameObject * other);

	// 사용자가 GameObject를 인위적으로 움직였을때 최신화 시키기 위한 함수이다.
	virtual void setTransformToSystem(GameObject * other);

	// 여기서 델타타임은 sec단위이다. // 16 milliSec = 0.016 sec
	virtual void update(float deltaTime);
	virtual void setGravity(GameObject * other, Vector3 & value);
	virtual void getGravity(Vector3 & output, GameObject * other);

	virtual void setMass(GameObject * other, float mass);
	virtual float getMass(GameObject * other);

	virtual void turnOnStaticFlag(GameObject * other);
	virtual void turnOffStaticFlag(GameObject * other, float mass);

	virtual void turnOnIsTriggerFlag(GameObject * other);
	virtual void turnOffIsTriggerFlag(GameObject * other);

	virtual void setBoxCollider(GameObject * other, const Vector3 & size);
	virtual void setSphereCollider(GameObject * other, float radius);

	
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
	virtual void turnOnStaticFlag(GameObject * other) {}
	virtual void turnOffStaticFlag(GameObject * other){}

	virtual void turnOnIsTriggerFlag(GameObject * other) {}
	virtual void turnOffIsTriggerFlag(GameObject * other) {}
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

	Physics * getWrappedPhysics() { return &wrapped; }
public:
	friend class Locator;
	//  나중에 로그 찍는 것으로 바꿀예정

};

#endif