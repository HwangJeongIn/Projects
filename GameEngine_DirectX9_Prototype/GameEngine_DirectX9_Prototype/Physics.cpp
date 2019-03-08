#include "Physics.h"
#include "GameObject.h"

void Physics::init()
{
	// collision configuration :: 기본적인 메모리 설정을 한다.
	collisionConfiguration = new btDefaultCollisionConfiguration();

	// collision dispatcher :: 다른 병렬 프로세싱에서 다른 dispatcher를 사용할 수 있다. // 멀티스레드
	dispatcher = new btCollisionDispatcher(collisionConfiguration);

	// 일반적인 목적의 broadphase
	overlappingPairCache = new btDbvtBroadphase();

	// solver :: 다른 병렬 프로세싱에서 다른 solver를 사용할 수 있다. // 멀티스레드
	solver = new btSequentialImpulseConstraintSolver;

	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

	// 다이나믹 월드에 중력 적용한다.
	// y방향으로 -10만큼 설정
	dynamicsWorld->setGravity(btVector3(0, -10, 0));
}

void Physics::release()
{
	for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; --i)
	{
		btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		// 리지드바디와 모션스테이트가 있을때 지워준다.
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}

		// collision shape를 지워준다.
		btCollisionShape * shape = obj->getCollisionShape();
		if (shape)
		{
			delete shape;
		}

		// dynamicworld 리스트에서 지우고
		dynamicsWorld->removeCollisionObject(obj);

		// 실제로 지워준다.
		delete obj;
	}

	// 전체 배열에서 모두 지워주었으므로 테이블도 클리어 시켜준다.
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
	// 만약에 등록되어 있지 않은 게임오브젝트 일때 // 즉 리지드바디가 등록되어 있지 않다면
	if (collisionObjectsTable.find(other) == collisionObjectsTable.end()) return false;
	return true;
}


void Physics::registerRigidBody(GameObject * other, Transform * transform, bool isSphereCollider)
{
	if (!exists(other)) return;

	// 일단 콜라이더는 무조건 삽입하기로 했다.
	btCollisionShape* colShape = new btSphereShape(btScalar(1.));

	/// Create Dynamic Objects
	// 위치 초기화
	btTransform startTransform;
	startTransform.setIdentity();

	// 위치 초기화
	startTransform.setOrigin(btVector3(transform->getPosition().getX(), transform->getPosition().getY(), transform->getPosition().getZ()));
	// 로테이션 초기화
	//startTransform.set

	// 일단 매스도 무조건 1로 하기로 했다.
	btScalar mass(1.f);

	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	// 추가작업 // 관성
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0, 0, 0);
	if (isDynamic)
		colShape->calculateLocalInertia(mass, localInertia);


	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);

	// 다이나믹 월드에 추가한다.
	dynamicsWorld->addRigidBody(body);

	btCollisionObject * temp = nullptr;
	// 만약에 콜리전 쉐이프가 있다는 것은 콜리전오브젝트에 추가(push_back)되었다는 것이기 때문에
	// 콜리전 오브젝트배열의 가장 마지막 원소가 현재 바디의 콜리전 오브젝트이다.
	// 콜리전 쉐이프가 없는 경우 그냥 바디만 추가된다.
	if (body->getCollisionShape())
	{
		// 맨마지막 콜리전 오브젝트를 받아오고 등록한다.
		temp = dynamicsWorld->getCollisionObjectArray()[dynamicsWorld->getNumCollisionObjects() - 1];
		collisionObjectsTable[other] = temp;
	}

}

void Physics::unregisterRigidBody(GameObject * other)
{
	if (!exists(other)) return;

	btCollisionObject * targetObj = nullptr;
	// 순회하면서 지울대상을 찾아준다.
	for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; --i)
	{
		btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];

		// 찾는 대상이 아니라면 계속 순회한다.
		if (obj != targetObj) continue;

		btRigidBody* body = btRigidBody::upcast(obj);
		// 리지드바디와 모션스테이트가 있을때 지워준다.
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}

		// collision shape를 지워준다.
		btCollisionShape * shape = obj->getCollisionShape();
		if (shape)
		{
			delete shape;
		}

		// dynamicworld 리스트에서 지우고
		dynamicsWorld->removeCollisionObject(obj);

		// 실제로 지워준다.
		delete obj;
		// 다 지웠다면 빠져나온다.
		break;
	}

	// 등록해둔 테이블에서도 지워준다.
	// 만약에 맵에만 등록된 상황이라도 버그가 발생하지 않는다.
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
		// rigidBody가 있고 보간기능을 사용할 수 있으면
		// transform을 rigidbody의 getMotionState > getWorldTransform함수를 통해서 받아온다.
		body->getMotionState()->getWorldTransform(trans);
	}
	else
	{
		// 아니면 collisionshape을 통해서 받아온다.
		trans = temp->getWorldTransform();
	}

	// 시스템으로부터 값을 받아서 위치 로테이션 최신화
	btVector3 & position = trans.getOrigin();

	
	output->setPosition(position.getX(), position.getY(), position.getZ());
	
	/*
	btQuaternion & quatern = trans.getRotation();
	// 여러가지 오일러각으로 변경하기 위한 작업을 해준다.
	output->setRotation();
	*/
}

void Physics::setTransformToSystem(const Transform * input, GameObject * other)
{
	if (!exists(other)) return;
	if (!input) return;

	btTransform trans;

	// 시스템에 들어온 자체정의 transform으로 시스템의 transform 최신화
	btCollisionObject * temp = collisionObjectsTable[other];

	trans.setOrigin(btVector3(input->getPosition().getX(), input->getPosition().getY(), input->getPosition().getZ()));
	
	/*
	btQuaternion quatern;
	// input->여기서 로테이션값을 초기화해준다.
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
