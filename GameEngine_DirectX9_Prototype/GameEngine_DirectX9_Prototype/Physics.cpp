#include "Physics.h"
#include "GameObject.h"

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
	dynamicsWorld->setGravity(btVector3(0, -10, 0));
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

	delete dynamicsWorld;
	delete solver;
	delete overlappingPairCache;
	delete dispatcher;
	delete collisionConfiguration;
}

bool Physics::exists(GameObject * other)
{
	if (!other) return false;
	// ���࿡ ��ϵǾ� ���� ���� ���ӿ�����Ʈ �϶� // �� ������ٵ� ��ϵǾ� ���� �ʴٸ�
	if (collisionObjectsTable.find(other) == collisionObjectsTable.end()) return false;
	return true;
}


void Physics::registerRigidBody(GameObject * other, Transform * transform, bool isSphereCollider)
{
	if (!exists(other)) return;

	// �ϴ� �ݶ��̴��� ������ �����ϱ�� �ߴ�.
	btCollisionShape* colShape = new btSphereShape(btScalar(1.));

	/// Create Dynamic Objects
	// ��ġ �ʱ�ȭ
	btTransform startTransform;
	startTransform.setIdentity();

	// ��ġ �ʱ�ȭ
	startTransform.setOrigin(btVector3(transform->getPosition().getX(), transform->getPosition().getY(), transform->getPosition().getZ()));
	// �����̼� �ʱ�ȭ
	//startTransform.set

	// �ϴ� �Ž��� ������ 1�� �ϱ�� �ߴ�.
	btScalar mass(1.f);

	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	// �߰��۾� // ����
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0, 0, 0);
	if (isDynamic)
		colShape->calculateLocalInertia(mass, localInertia);


	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
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
	}

}

void Physics::unregisterRigidBody(GameObject * other)
{
	if (!exists(other)) return;

	btCollisionObject * targetObj = nullptr;
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

		// ������ �����ش�.
		delete obj;
		// �� �����ٸ� �������´�.
		break;
	}

	// ����ص� ���̺����� �����ش�.
	// ���࿡ �ʿ��� ��ϵ� ��Ȳ�̶� ���װ� �߻����� �ʴ´�.
	collisionObjectsTable.erase(other);
}

void Physics::addForce(GameObject * other, Vector3 & directionWithScalar)
{
	if (!exists(other)) return;

	btCollisionObject * temp = collisionObjectsTable[other];
	btRigidBody* body = btRigidBody::upcast(temp);
	if (!body) return;
	body->activate(true);
	btVector3 tempVector{ directionWithScalar.getX(), directionWithScalar.getY(), directionWithScalar.getZ() };
	body->applyCentralImpulse(tempVector);
}

void Physics::setTransformFromSystem(Transform * output, GameObject * other)
{
	if (!exists(other)) return;
	if (!output) return;

	btCollisionObject * temp = collisionObjectsTable[other];
	btRigidBody * body = btRigidBody::upcast(temp);
	btTransform trans;
	if (body && body->getMotionState())
	{
		// rigidBody�� �ְ� ��������� ����� �� ������
		// transform�� rigidbody�� getMotionState > getWorldTransform�Լ��� ���ؼ� �޾ƿ´�.
		body->getMotionState()->getWorldTransform(trans);
	}
	else
	{
		// �ƴϸ� collisionshape�� ���ؼ� �޾ƿ´�.
		trans = temp->getWorldTransform();
	}

	// �ý������κ��� ���� �޾Ƽ� ��ġ �����̼� �ֽ�ȭ
	btVector3 & position = trans.getOrigin();

	
	output->setPosition(position.getX(), position.getY(), position.getZ());
	
	/*
	btQuaternion & quatern = trans.getRotation();
	// �������� ���Ϸ������� �����ϱ� ���� �۾��� ���ش�.
	output->setRotation();
	*/
}

void Physics::setTransformToSystem(const Transform * input, GameObject * other)
{
	if (!exists(other)) return;
	if (!input) return;

	btTransform trans;

	// �ý��ۿ� ���� ��ü���� transform���� �ý����� transform �ֽ�ȭ
	btCollisionObject * temp = collisionObjectsTable[other];

	trans.setOrigin(btVector3(input->getPosition().getX(), input->getPosition().getY(), input->getPosition().getZ()));
	
	/*
	btQuaternion quatern;
	// input->���⼭ �����̼ǰ��� �ʱ�ȭ���ش�.
	trans.setRotation(quatern);
	*/
	temp->setWorldTransform(trans);
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
