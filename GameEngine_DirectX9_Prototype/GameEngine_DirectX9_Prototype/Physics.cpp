#include "Physics.h"
#include "GameObject.h"

Physics * Physics::currentPhysics = nullptr;

// ����Լ��� �����ͷ� �Ѱ��ֱ⿡�� ������ ���� ���� �ʴ´� 
// ����Լ� �������� : void (Physics::*)(btDynamicsWorld * world, btScalar timeStep);
// �Ķ���Ϳ����� �������� : void (*btInternalTickCallback)(btDynamicsWorld *world, btScalar timeStep);
/*
2���� ���
1. global�� �Լ��� friend������ ����� �Ѱ��ش�.
2. static ����Լ��� ���� �Ѱ��ش�.

�ᱹ �Ѵ� ���� ������ �����Ϸ��� static ��������� ���ؼ� �����ؾ��Ѵ�.
�� static��������� ���� Physics�ý����� ����Ű�� �������̴�.
*/
void collisionCallBack(btDynamicsWorld * world, btScalar timeStep)
{
	// collision / trigger ��� ���Լ��� ��ħ

	Physics * currentPhysics = Physics::currentPhysics;
	if (!world || !currentPhysics) return;

	int numManifolds = currentPhysics->dynamicsWorld->getDispatcher()->getNumManifolds();
	for (int i = 0; i < numManifolds; i++)
	{
		btPersistentManifold *contactManifold = currentPhysics->dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);

		const btCollisionObject *objA = contactManifold->getBody0();
		const btCollisionObject *objB = contactManifold->getBody1();

		// �ΰ��� �浹ü�� �������� GameObject�� ã���ش�.
		map<btCollisionObject *, GameObject *>::iterator gameObjA = currentPhysics->gameObjectsTable.find((btCollisionObject *)(objA));
		map<btCollisionObject *, GameObject *>::iterator gameObjB = currentPhysics->gameObjectsTable.find((btCollisionObject *)(objB));

		// ���� �ϳ��� ��ϵǾ� ���� �ʴٸ� ��Ƽ��
		if (gameObjA == currentPhysics->gameObjectsTable.end() || gameObjB == currentPhysics->gameObjectsTable.end()) continue;

		btVector3 contactPoint{ 0,0,0 };
		int numContacts = contactManifold->getNumContacts();
		// �������� �ϳ��� ���ؼ� A B�� ������ ���ϰ� �� ����� ���� �ݸ��� ������Ʈ�� ���ش�.
		if (numContacts > 0)
		{
			for (int j = 0; j < 1/*numContacts*/; j++)
			{
				btManifoldPoint& pt = contactManifold->getContactPoint(j);
				const btVector3& ptA = pt.getPositionWorldOnA();
				const btVector3& ptB = pt.getPositionWorldOnB();
				//const btVector3& normalOnB = pt.m_normalWorldOnB;

				contactPoint = (ptA + ptB) / 2;
			}
		}

		// �� �浹ü�� ���ؼ� �ݸ��� ������Ʈ�� ���ش�. // ũ�ν��� �Ķ���͸� �Ѱ��ش�
		gameObjA->second->collisionUpdate(GameObjectWithCollision{ gameObjB->second, Vector3{ contactPoint.getX(),contactPoint.getY(),contactPoint.getZ() } });
		gameObjB->second->collisionUpdate(GameObjectWithCollision{ gameObjA->second, Vector3{ contactPoint.getX(),contactPoint.getY(),contactPoint.getZ() } });
	}
}


void Physics::init()
{
	// collision configuration :: �⺻���� �޸� ������ �Ѵ�.
	collisionConfiguration = new btDefaultCollisionConfiguration();

	// collision dispatcher :: �ٸ� ���� ���μ��̿��� �ٸ� dispatcher�� ����� �� �ִ�. // ��Ƽ������
	dispatcher = new btCollisionDispatcher(collisionConfiguration);

	// �Ϲ����� ������ broadphase
	overlappingPairCache = new btDbvtBroadphase();

	// solver :: �ٸ� ���� ���μ��̿��� �ٸ� solver�� ����� �� �ִ�. // ��Ƽ������
	solver = new btSequentialImpulseConstraintSolver;

	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

	// ���̳��� ���忡 �߷� �����Ѵ�.
	// y�������� -10��ŭ ����
	dynamicsWorld->setGravity(btVector3(0, -20, 0));

	currentPhysics = this;
	// �ݹ��Լ� ���
	// ���� ��ü �������� �ݹ��Լ��� �������� �Ѵ�.
	dynamicsWorld->setInternalTickCallback(&(collisionCallBack));
}

void Physics::release()
{
	for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; --i)
	{
		btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		// ������ٵ�� ��ǽ�����Ʈ�� ������ �����ش�.
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}

		// collision shape�� �����ش�.
		btCollisionShape * shape = obj->getCollisionShape();
		if (shape)
		{
			delete shape;
		}

		// dynamicworld ����Ʈ���� �����
		dynamicsWorld->removeCollisionObject(obj);

		// ������ �����ش�.
		delete obj;
	}

	// ��ü �迭���� ��� �����־����Ƿ� ���̺� Ŭ���� �����ش�.
	collisionObjectsTable.clear();
	gameObjectsTable.clear();

	delete dynamicsWorld;
	delete solver;
	delete overlappingPairCache;
	delete dispatcher;
	delete collisionConfiguration;
}

bool Physics::exists(GameObject * other) const
{
	if (!other) return false;
	// ���࿡ ��ϵǾ� ���� ���� ���ӿ�����Ʈ �϶� // �� ������ٵ� ��ϵǾ� ���� �ʴٸ�
	if (collisionObjectsTable.find(other) == collisionObjectsTable.end()) return false;
	return true;
}

void Physics::setBaseConstructionInfo(btRigidBody::btRigidBodyConstructionInfo & info) const
{
	info.m_friction = 1.0f;
	info.m_angularDamping = .7f;
	info.m_linearDamping = .7f;
}



void Physics::registerRigidBody(GameObject * other)
{
	if (exists(other)) return;

	// ���ӿ�����Ʈ�� ����ϴ� ��� �浹ü�� ������ 1�� ��
	btCollisionShape* colShape = new btSphereShape(btScalar(1.));

	// ��ġ �ʱ�ȭ
	btTransform startTransform;
	startTransform.setIdentity();

	Transform * transform = other->getTransform();
	Vector3 & worldPos = transform->getWorldPosition();

	// ��ġ �ʱ�ȭ
	startTransform.setOrigin(btVector3(worldPos.getX(), worldPos.getY(), worldPos.getZ()));
	// �����̼� �ʱ�ȭ
	//startTransform.set

	// ���ӿ�����Ʈ�� ����ϴ� ��� mass���� ������ 1
	btScalar mass(1.f);

	// �߰��۾� // ����
	//bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0, 0, 0);
	//if (isDynamic)
	colShape->calculateLocalInertia(mass, localInertia);


	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
	setBaseConstructionInfo(rbInfo);

	btRigidBody* body = new btRigidBody(rbInfo);

	// ���̳��� ���忡 �߰��Ѵ�.
	dynamicsWorld->addRigidBody(body);

	btCollisionObject * temp = nullptr;
	// ���࿡ �ݸ��� �������� �ִٴ� ���� �ݸ���������Ʈ�� �߰�(push_back)�Ǿ��ٴ� ���̱� ������
	// �ݸ��� ������Ʈ�迭�� ���� ������ ���Ұ� ���� �ٵ��� �ݸ��� ������Ʈ�̴�.
	// �ݸ��� �������� ���� ��� �׳� �ٵ� �߰��ȴ�.
	if (body->getCollisionShape())
	{
		// �Ǹ����� �ݸ��� ������Ʈ�� �޾ƿ��� ����Ѵ�.
		temp = dynamicsWorld->getCollisionObjectArray()[dynamicsWorld->getNumCollisionObjects() - 1];
		collisionObjectsTable[other] = temp;
		gameObjectsTable[temp] = other;
	}

}

void Physics::registerRigidBody(GameObject * other, btCollisionShape * shape, float mass)
{
	if (exists(other)) return; if (!shape) return;

	// ��ġ �ʱ�ȭ
	btTransform startTransform;
	startTransform.setIdentity();

	Transform * transform = other->getTransform();
	Vector3 & worldPos = transform->getWorldPosition();

	// ��ġ �ʱ�ȭ
	startTransform.setOrigin(btVector3(worldPos.getX(), worldPos.getY(), worldPos.getZ()));
	// �����̼� �ʱ�ȭ
	//startTransform.set

	if (mass < 0)
		mass = 1.0f;

	// �߰��۾� // ����
	btVector3 localInertia(0, 0, 0);
	//if (isDynamic)
	shape->calculateLocalInertia(mass, localInertia);


	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, shape, localInertia);
	setBaseConstructionInfo(rbInfo);

	btRigidBody* body = new btRigidBody(rbInfo);

	// ���̳��� ���忡 �߰��Ѵ�.
	dynamicsWorld->addRigidBody(body);

	btCollisionObject * temp = nullptr;
	if (body->getCollisionShape())
	{
		temp = dynamicsWorld->getCollisionObjectArray()[dynamicsWorld->getNumCollisionObjects() - 1];
		collisionObjectsTable[other] = temp;
		gameObjectsTable[temp] = other;
	}

}

void Physics::unregisterRigidBody(GameObject * other)
{
	// �������� ������ ����
	if (!exists(other)) return;

	// �����ϸ� �� �����ϴ� ��(collisionObject)�� �޾Ƽ� ���̳��Ϳ��忡�� �����ش�.
	btCollisionObject * targetObj = collisionObjectsTable.find(other)->second;

	// ��ȸ�ϸ鼭 �������� ã���ش�.
	for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; --i)
	{
		btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];

		// ã�� ����� �ƴ϶�� ��� ��ȸ�Ѵ�.
		if (obj != targetObj) continue;

		btRigidBody* body = btRigidBody::upcast(obj);
		// ������ٵ�� ��ǽ�����Ʈ�� ������ �����ش�.
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}

		// collision shape�� �����ش�.
		btCollisionShape * shape = obj->getCollisionShape();
		if (shape)
		{
			delete shape;
		}

		// dynamicworld ����Ʈ���� �����
		dynamicsWorld->removeCollisionObject(obj);

		// ���ӿ�����Ʈ �ʿ����� �����ְ�
		gameObjectsTable.erase(obj);

		// ������ �����ش�.
		delete obj;
		// �� �����ٸ� �������´�.
		break;
	}

	// ����ص� ���̺����� �����ش�.
	// ���࿡ �ʿ��� ��ϵ� ��Ȳ�̶� ���װ� �߻����� �ʴ´�.
	collisionObjectsTable.erase(other);
}

void Physics::addForce(GameObject * other,const Vector3 & directionWithScalar)
{
	if (!exists(other)) return;

	btCollisionObject * temp = collisionObjectsTable[other];
	btRigidBody* body = btRigidBody::upcast(temp);
	if (!body) return;
	body->activate(true);
	btVector3 tempVector{ directionWithScalar.getX(), directionWithScalar.getY(), directionWithScalar.getZ() };
	body->applyCentralImpulse(tempVector);
}

void Physics::setTransformFromSystem(GameObject * other)
{
	if (!exists(other)) return;


	btCollisionObject * temp = collisionObjectsTable[other];
	btRigidBody * body = btRigidBody::upcast(temp);
	btTransform trans;
	trans.setIdentity();
	/*
	��ǥ �����ʹ� �ٸ��� body�� ���ؼ� transform�� �޾ƿ��� ������ �����.
	collisionObject�� ���ؼ� �޾ƿ��� �ذ�Ǿ���.
	*/
	//if (body && body->getMotionState())
	//{
	//	// rigidBody�� �ְ� ��������� ����� �� ������
	//	// transform�� rigidbody�� getMotionState > getWorldTransform�Լ��� ���ؼ� �޾ƿ´�.
	//	body->getMotionState()->getWorldTransform(trans);
	//}
	//else
	{
		// �ƴϸ� collisionshape�� ���ؼ� �޾ƿ´�.
		trans = temp->getWorldTransform();
	}

	// �ý������κ��� ���� �޾Ƽ� ��ġ �����̼� �ֽ�ȭ
	btVector3 & position = trans.getOrigin();

	Transform * transform = other->getTransform();
	transform->setWorldPosition_physics(Vector3(position.getX(), position.getY(), position.getZ()));
	
	/*
	btQuaternion & quatern = trans.getRotation();
	// �������� ���Ϸ������� �����ϱ� ���� �۾��� ���ش�.
	output->setRotation();
	*/
}

void Physics::setTransformToSystem(GameObject * other)
{
	if (!exists(other)) return;


	// �ý��ۿ� ���� ��ü���� transform���� �ý����� transform �ֽ�ȭ
	btCollisionObject * temp = collisionObjectsTable[other];


	Transform * transform = other->getTransform();
	Vector3 worldPos = transform->getWorldPosition();
	btTransform trans;
	trans.setIdentity();

	trans.setOrigin(btVector3(worldPos.getX(), worldPos.getY(), worldPos.getZ()));
	//Trace::Write("TAG_DEBUG","ToSystem X : ", transform->getPosition().getX());
	//Trace::Write("TAG_DEBUG", "ToSystem Y : ", transform->getPosition().getY());
	//Trace::Write("TAG_DEBUG", "ToSystem Z : ", transform->getPosition().getZ());

	/*
	btQuaternion quatern;
	// input->���⼭ �����̼ǰ��� �ʱ�ȭ���ش�.
	trans.setRotation(quatern);
	*/

	temp->setWorldTransform(trans);


	// test
	return;

	btRigidBody * body = btRigidBody::upcast(temp);
	btTransform ttt;
	ttt.setIdentity();
	body->getMotionState()->getWorldTransform(ttt);
	Trace::Write("TAG_DEBUG", "After ToSystem_Body X : ", ttt.getOrigin().getX());
	Trace::Write("TAG_DEBUG", "After ToSystem_Body Y : ", ttt.getOrigin().getY());
	Trace::Write("TAG_DEBUG", "After ToSystem_Body Z : ", ttt.getOrigin().getZ());



	ttt = temp->getWorldTransform();
	Trace::Write("TAG_DEBUG", "After ToSystem_ X : ", ttt.getOrigin().getX());
	Trace::Write("TAG_DEBUG", "After ToSystem_ Y : ", ttt.getOrigin().getY());
	Trace::Write("TAG_DEBUG", "After ToSystem_ Z : ", ttt.getOrigin().getZ());
}

void Physics::update(float deltaTime)
{
	dynamicsWorld->stepSimulation(deltaTime, 1 / deltaTime /*/ 60.f*/);
}

void Physics::setGravity(GameObject * other, Vector3 & value)
{
	if (!exists(other)) return;

	btCollisionObject * temp = collisionObjectsTable[other];
	btRigidBody * body = btRigidBody::upcast(temp);
	body->setGravity(btVector3{ value.getX(), value.getY(), value.getZ() });
}

void Physics::getGravity(Vector3 & output, GameObject * other)
{
	if (!exists(other)) return;

	btCollisionObject * temp = collisionObjectsTable[other];
	btRigidBody * body = btRigidBody::upcast(temp);
	const btVector3 & tempVector3 = body->getGravity();
	output.setX(tempVector3.getX());
	output.setY(tempVector3.getY());
	output.setY(tempVector3.getZ());
}

void Physics::setMass(GameObject * other, float mass)
{
	if (!exists(other)) return;

	btCollisionObject * temp = collisionObjectsTable[other];
	btRigidBody * body = btRigidBody::upcast(temp);

	if (mass < 0)
		mass = 1.0f;

	btVector3 localInertia(0, 0, 0);
	temp->getCollisionShape()->calculateLocalInertia(mass, localInertia);
	// mass�� ���� ���Ӱ� ����ؼ� �־��ش�.
	body->setMassProps(mass, localInertia);
}

float Physics::getMass(GameObject * other)
{
	if (!exists(other)) return -1.0f;

	btCollisionObject * temp = collisionObjectsTable[other];
	btRigidBody * body = btRigidBody::upcast(temp);

	return body->getInvMass();
}


void Physics::turnOnStaticFlag(GameObject * other)
{
	if (!exists(other)) return;

	btCollisionObject * temp = collisionObjectsTable[other];
	btRigidBody * body = btRigidBody::upcast(temp);
	// turn on
	// ����ƽ�÷��׸� �Ѱ� �߷��� 0���� �����.
	body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
	body->setGravity(btVector3{ 0,0,0 });

	setMass(other, 0.0f);
}

void Physics::turnOffStaticFlag(GameObject * other, float mass)
{
	if (!exists(other)) return;

	btCollisionObject * temp = collisionObjectsTable[other];
	btRigidBody * body = btRigidBody::upcast(temp);
	// turn off
	// ����ƽ�÷��׸� ���� �߷��� �ǵ�����.
	body->setCollisionFlags(body->getCollisionFlags() & ~btCollisionObject::CF_STATIC_OBJECT);
	body->setGravity(btVector3{ 0,-10, 0 });

	setMass(other, mass);
}

void Physics::turnOnIsTriggerFlag(GameObject * other)
{
	if (!exists(other)) return;

	btCollisionObject * temp = collisionObjectsTable[other];
	btRigidBody * body = btRigidBody::upcast(temp);
	// turn on
	body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
}

void Physics::turnOffIsTriggerFlag(GameObject * other)
{
	if (!exists(other)) return;

	btCollisionObject * temp = collisionObjectsTable[other];
	btRigidBody * body = btRigidBody::upcast(temp);
	// turn off
	body->setCollisionFlags(body->getCollisionFlags() & ~btCollisionObject::CF_NO_CONTACT_RESPONSE);
}

void Physics::setBoxCollider(GameObject * other, const Vector3 & size)
{
	if (!exists(other)) return;

	btCollisionObject * temp = collisionObjectsTable[other];
	btRigidBody * body = btRigidBody::upcast(temp);
	// ������ mass���� �Ȱ��� �����ȴ�.
	float mass = body->getInvMass();

	btCollisionShape* shape = new btBoxShape(btVector3(size.getX(), size.getY(), size.getZ()));
	// �ƿ�����������ְ�
	unregisterRigidBody(other);

	// �ٽ� ������ش�
	registerRigidBody(other, shape, mass);
}

void Physics::setSphereCollider(GameObject * other, float radius)
{
	if (!exists(other)) return;

	btCollisionObject * temp = collisionObjectsTable[other];
	btRigidBody * body = btRigidBody::upcast(temp);
	// ������ mass���� �Ȱ��� �����ȴ�.
	float mass = body->getInvMass();

	btCollisionShape* shape = new btSphereShape(radius);
	// �ƿ�����������ְ�
	unregisterRigidBody(other);

	// �ٽ� ������ش�
	registerRigidBody(other, shape, mass);
}


