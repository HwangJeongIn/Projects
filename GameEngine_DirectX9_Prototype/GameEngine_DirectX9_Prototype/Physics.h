#ifndef PHYSICS_H
#define PHYSICS_H

#include "btBulletDynamicsCommon.h"
#include "Utility.h"
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

	// �ֽ�ȭ��Ų ��ġ�� �޾ƿ��ų� / ����ڰ� GameObject�� ���������� ���������� �ý��ۿ� �����ؼ� �ֽ�ȭ������� �Ѵ�.
	// rigidbody�� ������ �ִٸ� �� �Լ��� ���ؼ� ����ؼ� ��ġ�� �޾ƿ´�.
	virtual void setTransformFromSystem(Transform * output, GameObject * other);

	// ����ڰ� GameObject�� ���������� ���������� �ֽ�ȭ ��Ű�� ���� �Լ��̴�.
	virtual void setTransformToSystem(const Transform * input, GameObject * other);

	// ���⼭ ��ŸŸ���� sec�����̴�. // 16 milliSec = 0.016 sec
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

	// ���⼭ ��ŸŸ���� sec�����̴�. // 16 milliSec = 0.016 sec
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
	//  ���߿� �α� ��� ������ �ٲܿ���

};

#endif