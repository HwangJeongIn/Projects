#ifndef PHYSICS_H
#define PHYSICS_H

#include "btBulletDynamicsCommon.h"
#include "Utility.h"
#include "Trace.h"
#include <map>

using namespace std;


/*
�ʿ��� ���
1. 

*/
class GameObject;
class Transform;

class Physics
{
private:
	// �ΰ��� ���̺��� ������ ����
	/*
	1. ����ϰų� �����Ҷ� GameObject * �������� �۾�
	2. �ݹ��Լ� ȣ��� btCollisionObject * �������� �۾�
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

	// �ֽ�ȭ��Ų ��ġ�� �޾ƿ��ų� / ����ڰ� GameObject�� ���������� ���������� �ý��ۿ� �����ؼ� �ֽ�ȭ������� �Ѵ�.
	// rigidbody�� ������ �ִٸ� �� �Լ��� ���ؼ� ����ؼ� ��ġ�� �޾ƿ´�.
	virtual void setTransformFromSystem(GameObject * other);

	// ����ڰ� GameObject�� ���������� ���������� �ֽ�ȭ ��Ű�� ���� �Լ��̴�.
	virtual void setTransformToSystem(GameObject * other);

	// ���⼭ ��ŸŸ���� sec�����̴�. // 16 milliSec = 0.016 sec
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

	// ���⼭ ��ŸŸ���� sec�����̴�. // 16 milliSec = 0.016 sec
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
	//  ���߿� �α� ��� ������ �ٲܿ���

};

#endif