#include "Physics.h"
#include "GameObject.h"

Physics * Physics::currentPhysics = nullptr;

// 멤버함수를 포인터로 넘겨주기에는 포인터 형이 맞지 않는다 
// 멤버함수 포인터형 : void (Physics::*)(btDynamicsWorld * world, btScalar timeStep);
// 파라미터에서의 포인터형 : void (*btInternalTickCallback)(btDynamicsWorld *world, btScalar timeStep);
/*
2가지 방법
1. global로 함수를 friend선언해 만들어 넘겨준다.
2. static 멤버함수를 만들어서 넘겨준다.

결국 둘다 내부 변수에 접근하려면 static 멤버변수를 통해서 접근해야한다.
이 static멤버변수는 현재 Physics시스템을 가리키는 포인터이다.
*/
void collisionCallBack(btDynamicsWorld * world, btScalar timeStep)
{
	// collision / trigger 모두 이함수를 거침

	Physics * currentPhysics = Physics::currentPhysics;
	if (!world || !currentPhysics) return;

	int numManifolds = currentPhysics->dynamicsWorld->getDispatcher()->getNumManifolds();
	for (int i = 0; i < numManifolds; i++)
	{
		btPersistentManifold *contactManifold = currentPhysics->dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);

		const btCollisionObject *objA = contactManifold->getBody0();
		const btCollisionObject *objB = contactManifold->getBody1();

		// 두개의 충돌체를 기준으로 GameObject를 찾아준다.
		map<btCollisionObject *, GameObject *>::iterator gameObjA = currentPhysics->gameObjectsTable.find((btCollisionObject *)(objA));
		map<btCollisionObject *, GameObject *>::iterator gameObjB = currentPhysics->gameObjectsTable.find((btCollisionObject *)(objB));

		// 만약 하나라도 등록되어 있지 않다면 컨티뉴
		if (gameObjA == currentPhysics->gameObjectsTable.end() || gameObjB == currentPhysics->gameObjectsTable.end()) continue;

		btVector3 contactPoint{ 0,0,0 };
		int numContacts = contactManifold->getNumContacts();
		// 접촉지점 하나에 대해서 A B의 지점을 구하고 그 평균을 내서 콜리전 업데이트를 해준다.
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

		// 두 충돌체에 대해서 콜리전 업데이트를 해준다. // 크로스로 파라미터를 넘겨준다
		gameObjA->second->collisionUpdate(GameObjectWithCollision{ gameObjB->second, Vector3{ contactPoint.getX(),contactPoint.getY(),contactPoint.getZ() } });
		gameObjB->second->collisionUpdate(GameObjectWithCollision{ gameObjA->second, Vector3{ contactPoint.getX(),contactPoint.getY(),contactPoint.getZ() } });
	}
}


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
	dynamicsWorld->setGravity(btVector3(0, -20, 0));

	currentPhysics = this;
	// 콜백함수 등록
	// 현재 객체 기준으로 콜백함수를 등록해줘야 한다.
	dynamicsWorld->setInternalTickCallback(&(collisionCallBack));
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
	// 만약에 등록되어 있지 않은 게임오브젝트 일때 // 즉 리지드바디가 등록되어 있지 않다면
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

	// 게임오브젝트만 등록하는 경우 충돌체는 반지름 1인 구
	btCollisionShape* colShape = new btSphereShape(btScalar(1.));

	// 위치 초기화
	btTransform startTransform;
	startTransform.setIdentity();

	Transform * transform = other->getTransform();
	Vector3 & worldPos = transform->getWorldPosition();

	// 위치 초기화
	startTransform.setOrigin(btVector3(worldPos.getX(), worldPos.getY(), worldPos.getZ()));
	// 로테이션 초기화
	//startTransform.set

	// 게임오브젝트만 등록하는 경우 mass값은 무조건 1
	btScalar mass(1.f);

	// 추가작업 // 관성
	//bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0, 0, 0);
	//if (isDynamic)
	colShape->calculateLocalInertia(mass, localInertia);


	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
	setBaseConstructionInfo(rbInfo);

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
		gameObjectsTable[temp] = other;
	}

}

void Physics::registerRigidBody(GameObject * other, btCollisionShape * shape, float mass)
{
	if (exists(other)) return; if (!shape) return;

	// 위치 초기화
	btTransform startTransform;
	startTransform.setIdentity();

	Transform * transform = other->getTransform();
	Vector3 & worldPos = transform->getWorldPosition();

	// 위치 초기화
	startTransform.setOrigin(btVector3(worldPos.getX(), worldPos.getY(), worldPos.getZ()));
	// 로테이션 초기화
	//startTransform.set

	if (mass < 0)
		mass = 1.0f;

	// 추가작업 // 관성
	btVector3 localInertia(0, 0, 0);
	//if (isDynamic)
	shape->calculateLocalInertia(mass, localInertia);


	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, shape, localInertia);
	setBaseConstructionInfo(rbInfo);

	btRigidBody* body = new btRigidBody(rbInfo);

	// 다이나믹 월드에 추가한다.
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
	// 존재하지 않으면 리턴
	if (!exists(other)) return;

	// 존재하면 그 존재하는 것(collisionObject)를 받아서 다이나믹월드에서 지워준다.
	btCollisionObject * targetObj = collisionObjectsTable.find(other)->second;

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

		// 게임오브젝트 맵에서도 지워주고
		gameObjectsTable.erase(obj);

		// 실제로 지워준다.
		delete obj;
		// 다 지웠다면 빠져나온다.
		break;
	}

	// 등록해둔 테이블에서도 지워준다.
	// 만약에 맵에만 등록된 상황이라도 버그가 발생하지 않는다.
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
	대표 예제와는 다르게 body를 통해서 transform을 받아오면 오차가 생긴다.
	collisionObject를 통해서 받아오니 해결되었다.
	*/
	//if (body && body->getMotionState())
	//{
	//	// rigidBody가 있고 보간기능을 사용할 수 있으면
	//	// transform을 rigidbody의 getMotionState > getWorldTransform함수를 통해서 받아온다.
	//	body->getMotionState()->getWorldTransform(trans);
	//}
	//else
	{
		// 아니면 collisionshape을 통해서 받아온다.
		trans = temp->getWorldTransform();
	}

	// 시스템으로부터 값을 받아서 위치 로테이션 최신화
	btVector3 & position = trans.getOrigin();

	Transform * transform = other->getTransform();
	transform->setWorldPosition_physics(Vector3(position.getX(), position.getY(), position.getZ()));
	
	/*
	btQuaternion & quatern = trans.getRotation();
	// 여러가지 오일러각으로 변경하기 위한 작업을 해준다.
	output->setRotation();
	*/
}

void Physics::setTransformToSystem(GameObject * other)
{
	if (!exists(other)) return;


	// 시스템에 들어온 자체정의 transform으로 시스템의 transform 최신화
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
	// input->여기서 로테이션값을 초기화해준다.
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
	// mass에 따라 새롭게 계산해서 넣어준다.
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
	// 스태틱플래그를 켜고 중력을 0으로 만든다.
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
	// 스태틱플래그를 끄고 중력을 되돌린다.
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
	// 기존의 mass값은 똑같이 유지된다.
	float mass = body->getInvMass();

	btCollisionShape* shape = new btBoxShape(btVector3(size.getX(), size.getY(), size.getZ()));
	// 아예등록해제해주고
	unregisterRigidBody(other);

	// 다시 등록해준다
	registerRigidBody(other, shape, mass);
}

void Physics::setSphereCollider(GameObject * other, float radius)
{
	if (!exists(other)) return;

	btCollisionObject * temp = collisionObjectsTable[other];
	btRigidBody * body = btRigidBody::upcast(temp);
	// 기존의 mass값은 똑같이 유지된다.
	float mass = body->getInvMass();

	btCollisionShape* shape = new btSphereShape(radius);
	// 아예등록해제해주고
	unregisterRigidBody(other);

	// 다시 등록해준다
	registerRigidBody(other, shape, mass);
}


