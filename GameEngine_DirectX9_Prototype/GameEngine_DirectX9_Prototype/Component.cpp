#include "Component.h"
#include "GameObject.h"

#include "Scene.h"
#include "Audio.h"
#include "Physics.h"
#include "FbxParser.h"
#include "gameui.h"

#include "FbxModelMesh.h"
#include "FbxModelSkeletonBones.h"
#include "FbxModelAnimations.h"

#include "ParticleSystem.h"

#include "Utility.h"
#include "Trace.h"


const D3DXVECTOR3 Transform::WorldRight{ 1,0,0 };
const D3DXVECTOR3 Transform::WorldUp{ 0,1,0 };
const D3DXVECTOR3 Transform::WorldForward{ 0, 0, 1 };
const D3DXMATRIX Transform::IdentityMatrix
{
	1,0,0,0,
	0,1,0,0,
	0,0,1,0,
	0,0,0,1
};
const float Transform::Pi = 3.14159f;


//void Transform::transformUpdate(bool dirty,  const D3DXMATRIX & parentPositionMatrix, const D3DXMATRIX & parentRotationMatrix)
//{
//	// 비트 연산으로 최종적으로 위에서 최신화 메시지를 받았거나, 
//	// 자체적으로 최신화하는 경우에 최신화 해주고 자식객체에게도 최신화를 전달해준다.
//	
//	dirty |= this->dirty;
//	if (dirty)
//	{
//		// 현재 변환을 부모 변환과 결합해서 계산한다.
//		// 로테이션행렬과 포지션 행렬을 따로 계산해주어야 한다.
//		//setRotationMatrix_DX(parentRotationMatrix);
//		//setPositionMatrix_DX(parentPositionMatrix);
//		// 최종 transformMatrix = 계산한 로테이션 * 포지션
//		setTransformMatrix_DX(parentPositionMatrix, parentRotationMatrix);
//		this->dirty = false;
//	}
//
//
//	vector<GameObject*> & children = gameObject->getChildren();
//
//
//	// 현재는 더티플래그가 꺼져있어도 중간 차일드에 켜져있을 가능성이 있기 때문에
//	// 모든 객체에 대해서 전달해준다.
//	for (auto it : children)
//	{
//		it->getTransform()->transformUpdate(dirty, getPositionMatrix_DX(),getRotationMatrix_DX());
//	}
//
//}


void Transform::transformUpdate(bool dirty, const D3DXMATRIX & parentTransformMatrix, bool toPhysicsSystem)
{
	// 비트 연산으로 최종적으로 위에서 최신화 메시지를 받았거나, 
	// 자체적으로 최신화하는 경우에 최신화 해주고 자식객체에게도 최신화를 전달해준다.

	dirty |= this->dirty;
	if (dirty)
	{
		// 현재 변환을 부모 변환과 결합해서 계산한다.
		// 로테이션행렬과 포지션 행렬을 따로 계산해주어야 한다.
		//setRotationMatrix_DX(parentRotationMatrix);
		//setPositionMatrix_DX(parentPositionMatrix);
		// 최종 transformMatrix = 계산한 로테이션 * 포지션
		setTransformMatrix(parentTransformMatrix);

		// 값이 변경되었을때 리지드바디가 있다면 물리에서도 transform값을 변경해준다.
		if (gameObject->getRigidBody() && toPhysicsSystem)
			gameObject->getPhysics().setTransformToSystem(gameObject);


		this->dirty = false;
	}


	vector<GameObject*> & children = gameObject->getChildren();


	// 현재는 더티플래그가 꺼져있어도 중간 차일드에 켜져있을 가능성이 있기 때문에
	// 모든 객체에 대해서 전달해준다.
	for (auto it : children)
	{
		it->getTransform()->transformUpdate(dirty, getTransformMatrix());
	}
}

void Transform::rootTransformUpdate(bool toPhysicsSystem)
{
	Transform * rootTransform = gameObject->getRoot()->getTransform();
	// 부모객체 기준으로 TransformMatrix를 최신화 시켜준다.
	rootTransform->transformUpdate(rootTransform->getDirty(), Transform::IdentityMatrix, toPhysicsSystem);
}

void Transform::setTransformMatrix(const D3DXMATRIX & parentTransformMatrix)
{
	// 먼저 자식의 부모좌표계 기준으로 로테이션과 포지션 행렬을 구해준다.
	calcRotationMatrix(); calcPositionMatrix();

	// 먼저 자식기준(월드 기준 X / 부모를 월드로 생각했을때 기준이다) trasnformMatrix를 구하고 
	// rotationMatrix * positionMatrix
	// 최종적으로 위에서 부터 최신화된 부모의 transform행렬을 곱해준다.
	// 따라서 최종 행렬은
	localTransformMatrix = rotationMatrix * positionMatrix;
	transformMatrix = localTransformMatrix * parentTransformMatrix;
}

void Transform::calcRotationMatrix()
{
	D3DXMatrixIdentity(&rotationMatrix);
	//	D3DXMATRIX temp;
	//	D3DXMatrixIdentity(&rotationMatrix_DX);
	//
	//	D3DXMatrixRotationYawPitchRoll(&rotationMatrix_DX, rotation.getY(), rotation.getX(), rotation.getZ());
	//
	//	//D3DXMatrixMultiply(&rotationMatrix_DX, &rotationMatrix_DX, D3DXMatrixRotationX(&temp, rotation.getX()));
	//	//D3DXMatrixMultiply(&rotationMatrix_DX, &rotationMatrix_DX, D3DXMatrixRotationY(&temp, rotation.getY()));
	//	//D3DXMatrixMultiply(&rotationMatrix_DX, &rotationMatrix_DX, D3DXMatrixRotationZ(&temp, rotation.getZ()));
	//	D3DXMatrixMultiply(&rotationMatrix_DX, &rotationMatrix_DX, &parentRotationMatrix);

	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, DegreeToRadian(rotation.getY()), DegreeToRadian(rotation.getX()), DegreeToRadian(rotation.getZ()));
}

void Transform::calcPositionMatrix()
{
	D3DXMatrixIdentity(&positionMatrix);
	D3DXMatrixTranslation(&positionMatrix, position.getX(), position.getY(), position.getZ());
}

//void Transform::setTransformMatrix_DX(const D3DXMATRIX & parentPositionMatrix, const D3DXMATRIX & parentRotationMatrix)
//
//	{
//		// 중요 :: 부모에의해 부모의 로테이션만큼 더 돌게 되어있다. (부모 + 자식 로테이션) > 부모기준(forward up right) 자식 위치 + 부모위치
//		calcRotationMatrix_DX(parentRotationMatrix); calcPositionMatrix_DX(parentPositionMatrix);
//		transformMatrix_DX = rotationMatrix_DX* positionMatrix_DX;
//	}

//void Transform::calcRotationMatrix_DX(const D3DXMATRIX & parentRotationMatrix)
//{
//	D3DXMATRIX temp;
//	D3DXMatrixIdentity(&rotationMatrix_DX);
//
//	D3DXMatrixRotationYawPitchRoll(&rotationMatrix_DX, rotation.getY(), rotation.getX(), rotation.getZ());
//
//	//D3DXMatrixMultiply(&rotationMatrix_DX, &rotationMatrix_DX, D3DXMatrixRotationX(&temp, rotation.getX()));
//	//D3DXMatrixMultiply(&rotationMatrix_DX, &rotationMatrix_DX, D3DXMatrixRotationY(&temp, rotation.getY()));
//	//D3DXMatrixMultiply(&rotationMatrix_DX, &rotationMatrix_DX, D3DXMatrixRotationZ(&temp, rotation.getZ()));
//	D3DXMatrixMultiply(&rotationMatrix_DX, &rotationMatrix_DX, &parentRotationMatrix);
//}

//void Transform::calcPositionMatrix_DX(const D3DXMATRIX & parentPositionMatrix)
//{
//	Vector3 result{};
//	GameObject * parent = gameObject->getParent();
//	// 부모객체 기준으로 right up forward기준으로 계산해준다.
//	// 부모객체가 없는 루트 오브젝트인 경우 월드 right up forward를 사용한다.
//	if (!parent)
//		result = Vector3::Right * position.getX() + Vector3::Up * position.getY() + Vector3::Forward * position.getZ();
//	else
//		result = parent->getTransform()->getRight() * position.getX() + parent->getTransform()->getUp() * position.getY()
//		+ parent->getTransform()->getForward() * position.getZ();
//
//	D3DXMatrixIdentity(&positionMatrix_DX);
//
//	D3DXMatrixTranslation(&positionMatrix_DX, result.getX(), result.getY(), result.getZ());
//	// 마지막으로 부모객체의 위치만큼 이동시킨다.
//	D3DXMatrixMultiply(&positionMatrix_DX, &positionMatrix_DX, &parentPositionMatrix);
//}



void Transform::convertLocalPositionIfItIsChild(Vector3 & output, const Vector3 & input)
{
	// 부모객체가 없다면 그냥 바로 초기화
	if (!gameObject->getParent())
	{
		output = input;
		return;
	}

	// 부모객체가 있다면 
	/*
	1. 부모객체의 TransformMatrix의 역행렬을 곱해서 로컬좌표로 들어간다.
	2. 로컬좌표를 정해준다.
	*/
	D3DXMATRIX parentInverseTransformMatrix;
	gameObject->getParent()->getTransform()->getInverseTransformMatrix(parentInverseTransformMatrix);

	D3DXVECTOR3 inputLocalPos;
	Vector3::ToD3DXVECTOR3(inputLocalPos, { input.getX(), input.getY(), input.getZ() });

	D3DXVec3TransformCoord(&inputLocalPos, &inputLocalPos, &parentInverseTransformMatrix);
	output.setVector3(Vector3{ inputLocalPos.x,inputLocalPos.y, inputLocalPos.z });

}


void Transform::convertLocalRotationIfItIsChild(Vector3 & output, const Vector3 & input)
{
	// 부모객체가 없다면 그냥 바로 초기화
	if (!gameObject->getParent())
	{
		output = input;
		return;
	}

	// 부모객체가 있다면 
	/*
	1. 부모객체의 TransformMatrix의 역행렬을 곱해서 로컬좌표로 들어간다.
	2. 로컬좌표를 정해준다.
	*/
	D3DXMATRIX parentInverseTransformMatrix;
	gameObject->getParent()->getTransform()->getInverseTransformMatrix(parentInverseTransformMatrix);

	D3DXVECTOR3 inputLocalRotation;
	Vector3::ToD3DXVECTOR3(inputLocalRotation, { input.getX(), input.getY(), input.getZ() });

	D3DXVec3TransformCoord(&inputLocalRotation, &inputLocalRotation, &parentInverseTransformMatrix);
	output.setVector3(Vector3{ inputLocalRotation.x,inputLocalRotation.y, inputLocalRotation.z });
}



void Transform::setWorldPosition_physics(const Vector3 & other)
{
	dirty = true;
	// 내부적으로 부모객체가 없으면 바로 초기화 / 있으면 변환후 초기화해준다.
	convertLocalPositionIfItIsChild(position, other);

	// 새롭게 적용된 행렬을 최종 transformMatrix에 적용해준다.
	// 받아오는 것이기 때문에 다시 System에 최신화시키면 안된다.
	rootTransformUpdate(false);
}

// 변경예정/////////////////////////////////////////////////////////////// 임시
void Transform::setWorldRotation_physics(const Vector3 & other)
{
	dirty = true;
	convertLocalRotationIfItIsChild(rotation, other);


	setDirectionVectorWithRotation_DX();

	// 새롭게 적용된 행렬을 최종 transformMatrix에 적용해준다.
	// 받아오는 것이기 때문에 다시 System에 최신화시키면 안된다.
	rootTransformUpdate(false);

}
// 변경예정/////////////////////////////////////////////////////////////// 임시


// 변경예정/////////////////////////////////////////////////////////////// 임시
void Transform::setWorldScale_physics(const Vector3 & other)
{
	scale = other;
}
// 변경예정/////////////////////////////////////////////////////////////// 임시

void Transform::rotate(const Vector3 & other)
{
	setLocalRotation(Vector3(rotation.getX() + other.getX(), rotation.getY() + other.getY(), rotation.getZ() + other.getZ()));
	// 새롭게 적용된 행렬을 최종 transformMatrix에 적용해준다.
	rootTransformUpdate();
}

void Transform::setWorldPosition(const Vector3 & other)
{
	//if (gameObject->getTag() == "Enemy")
	//{
	//		Vector3 temp = transform->getWorldPosition();
	//		Trace::Write("TAG_DEBUG", "enemy position world", "");
	//		Trace::Write("TAG_DEBUG", "x", temp.getX());
	//		Trace::Write("TAG_DEBUG", "y", temp.getY());
	//		Trace::Write("TAG_DEBUG", "z", temp.getZ());
	//}
	dirty = true;
	// 내부적으로 부모객체가 없으면 바로 초기화 / 있으면 변환후 초기화해준다.
	convertLocalPositionIfItIsChild(position, other);

	// 새롭게 적용된 행렬을 최종 transformMatrix에 적용해준다.
	rootTransformUpdate();

	//// 값이 변경되었을때 리지드바디가 있다면 물리에서도 처리해준다.
	//if(gameObject->getRigidBody())
	//	gameObject->getPhysics().setTransformToSystem(gameObject);

}


void Transform::setWorldRotation(const Vector3 & other)
{
	dirty = true;

	convertLocalRotationIfItIsChild(rotation, other);


	setDirectionVectorWithRotation_DX();
	//// 값이 변경되었을때 리지드바디가 있다면 물리에서도 처리해준다.
	//if(gameObject->getRigidBody())
	//	gameObject->getPhysics().setTransformToSystem(gameObject);
	rootTransformUpdate();
}


void Transform::setWorldPosition(float x, float y, float z)
{
	dirty = true;
	// 내부적으로 부모객체가 없으면 바로 초기화 / 있으면 변환후 초기화해준다.
	convertLocalPositionIfItIsChild(position, Vector3{ x,y,z });

	// 새롭게 적용된 행렬을 최종 transformMatrix에 적용해준다.
	rootTransformUpdate();

	//// 값이 변경되었을때 리지드바디가 있다면 물리에서도 처리해준다.
	//if (gameObject->getRigidBody())
	//	gameObject->getPhysics().setTransformToSystem(gameObject);

}


void Transform::setWorldRotation(float x, float y, float z)
{
	dirty = true;

	convertLocalRotationIfItIsChild(rotation, Vector3{ x,y,z });

	setDirectionVectorWithRotation_DX();

	//// 값이 변경되었을때 리지드바디가 있다면 물리에서도 처리해준다.
	//if (gameObject->getRigidBody())
	//	gameObject->getPhysics().setTransformToSystem(gameObject);


	// 새롭게 적용된 행렬을 최종 transformMatrix에 적용해준다.
	rootTransformUpdate();
}


void Transform::setLocalPosition(const Vector3 & other)
{
	dirty = true;
	position = other;

	// 새롭게 적용된 행렬을 최종 transformMatrix에 적용해준다.
	rootTransformUpdate();
}


void Transform::setLocalRotation(const Vector3 & other)
{
	dirty = true;
	rotation.setX(constrainLessThan180(other.getX()));
	rotation.setY(constrainLessThan180(other.getY()));
	rotation.setZ(constrainLessThan180(other.getZ()));
	setDirectionVectorWithRotation_DX();

	// 새롭게 적용된 행렬을 최종 transformMatrix에 적용해준다.
	rootTransformUpdate();
}


void Transform::setLocalPosition(float x, float y, float z)
{
	dirty = true;

	position.setX(x);
	position.setY(y);
	position.setZ(z);


	// 새롭게 적용된 행렬을 최종 transformMatrix에 적용해준다.
	rootTransformUpdate();

	//// 값이 변경되었을때 리지드바디가 있다면 물리에서도 처리해준다.
	//if (gameObject->getRigidBody())
	//	gameObject->getPhysics().setTransformToSystem(gameObject);
}


void Transform::setLocalRotation(float x, float y, float z)
{
	dirty = true;
	rotation.setX(constrainLessThan180(x));
	rotation.setY(constrainLessThan180(y));
	rotation.setZ(constrainLessThan180(z));
	setDirectionVectorWithRotation_DX();

	// 새롭게 적용된 행렬을 최종 transformMatrix에 적용해준다.
	rootTransformUpdate();

	//// 값이 변경되었을때 리지드바디가 있다면 물리에서도 처리해준다.
	//if (gameObject->getRigidBody())
	//	gameObject->getPhysics().setTransformToSystem(gameObject);
}





void PlayerScript::update()
{
	Vector3 temp = transform->getWorldRotation();
	Trace::Write("TAG_DEBUG", "player", "");
	Trace::Write("TAG_DEBUG", "x", temp.getX());
	Trace::Write("TAG_DEBUG", "y", temp.getY());
	Trace::Write("TAG_DEBUG", "z", temp.getZ());


	//if (::GetAsyncKeyState('Q') & 0x8000f)
	//	transform->setRotation(transform->getRotation() + Vector3{ 0,-.05f,0 });
	//if (::GetAsyncKeyState('E') & 0x8000f)
	//	transform->setRotation(transform->getRotation() + Vector3{ 0,.05f,0 });

	//if (::GetAsyncKeyState('Z') & 0x8000f)
	//	transform->setRotation(transform->getRotation() + Vector3{ -.05f,0,0 });
	//if (::GetAsyncKeyState('C') & 0x8000f)
	//	transform->setRotation(transform->getRotation() + Vector3{ .05f,0,0 });

	if (InputManager::GetKeyStay(0x57))
	{
		gameObject->getAudio().playEffectSound("BS_Effect_FootStep.mp3", false);
		transform->setWorldPosition(transform->getWorldPosition() + FrameTime::GetDeltaTime()* .1f *(transform->getForward()));

	}
	if (InputManager::GetKeyStay(0x53))
	{
		gameObject->getAudio().playEffectSound("BS_Effect_FootStep.mp3", false);
		transform->setWorldPosition(transform->getWorldPosition() + FrameTime::GetDeltaTime()*.1f *(-1)*(transform->getForward()));
	}
	
	if (InputManager::GetKeyStay(0x44))
	{
		gameObject->getAudio().playEffectSound("BS_Effect_FootStep.mp3", false);
		transform->setWorldPosition(transform->getWorldPosition() + FrameTime::GetDeltaTime()*.1f * (transform->getRight()));
	}

	if (InputManager::GetKeyStay(0x41))
	{
		gameObject->getAudio().playEffectSound("BS_Effect_FootStep.mp3", false);
		transform->setWorldPosition(transform->getWorldPosition() + FrameTime::GetDeltaTime()*.1f * (-1)*(transform->getRight()));
	}


	if (InputManager::GetKeyUp(0x31))
	{
		gameObject->getAudio().playEffectSound("BS_Effect_Arrow.mp3", true);
		Vector3 direction = transform->getForward() + Vector3(0, 0.01, 0);
		GameObject * bullet = GameObject::Instantiate("bullet1", "Bullet");
		//bullet->addComponent<MeshRenderer>()->loadXFile("car.x");
		//FbxModelRenderer * bulletFbxModelRenderer = bullet->addComponent<FbxModelRenderer>();
		//bulletFbxModelRenderer->loadFbxFile("Arrow_Regular.fbx");
		//bulletFbxModelRenderer->setScale(Vector3(10, 10, 10));
		//bullet->getTransform()->setWorldRotation(Vector3(0, 180, 0));

		bullet->getTransform()->setWorldPosition(transform->getWorldPosition()+direction + Vector3(0,3.0f,0));

		bullet->addComponent<BulletScript>();
		RigidBody * bulletRigidBody = bullet->addComponent<RigidBody>();
		bulletRigidBody->setSphereCollider(1);
		bulletRigidBody->addForce(direction * shootingPower);

		shootingPower = 100.0f;


		//car1RigidBody->setGravity(Vector3(0, 0, 0));
		//car1->addComponent<BoxCollider>();
	}
	else if (InputManager::GetKeyStay(0x31))
	{
		if (shootingPower < 200.0f)
		{
			shootingPower += FrameTime::GetDeltaTime()*.1f * chargingSpeed;
		}
	}

	if (::GetAsyncKeyState('N') & 0x8000f)
			transform->setWorldRotation(transform->getWorldRotation() + FrameTime::GetDeltaTime() *.1f * Vector3{ 0,-1,0 });

	if (::GetAsyncKeyState('M') & 0x8000f)
			transform->setWorldRotation(transform->getWorldRotation() + FrameTime::GetDeltaTime() *.1f * Vector3{ 0,1,0 });
}

void PlayerScript::start()
{
	//animationFSM = gameObject->getComponent<AnimationFSM>();
	//AudioSource * temp = gameObject->getAudio().getAudioSource("BS_BackGround_1.mp3");
	//if (temp)
	//{
	//	temp->play(false);
	//	temp->setVolume(7.0f);
	//}
}

void PlayerScript::onCollisionStay(GameObjectWithCollision & other)
{
	
}

void BasicEnemyScript::fixedUpdate()
{
	if (target)
	{
		Trace::Write("TAG_DEBUG", "current Target", target->getTag());
	}

}
void BasicEnemyScript::update()
{
	basicEnemyBT->tick();

	//Vector3 temp = transform->getWorldRotation();
	//Trace::Write("TAG_DEBUG", "enemy rotation", "");
	//Trace::Write("TAG_DEBUG", "x", temp.getX());
	//Trace::Write("TAG_DEBUG", "y", temp.getY());
	//Trace::Write("TAG_DEBUG", "z", temp.getZ());

	//temp = transform->getWorldPosition();
	//Trace::Write("TAG_DEBUG", "enemy position", "");
	//Trace::Write("TAG_DEBUG", "x", temp.getX());
	//Trace::Write("TAG_DEBUG", "y", temp.getY());
	//Trace::Write("TAG_DEBUG", "z", temp.getZ());

}

void BasicEnemyScript::start()
{
	// Unity에서 start함수가 바로 호출되지 않는 이유를 알 것 같다. // 여기서 구현한 start함수는 awake함수랑 같다고 보면 된다.
	// 만약 자식객체인 경우 현재 좌표가 로컬좌표계가 되어서 부모객체의 Transform 행렬을 추가적으로 곱해서 최종 World좌표를 구해야하는데
	// 그 최종 계산은 게임루프 한곳에서 실행된다. 그렇게되면 여기서는 World기준 최종 위치를 알 수 없다..
	// 일단 여기서 TransformUpdate를 한번 돌려서 모든 객체에 대해서 TransformMatrix를 구해주기로한다 // 나중에 start함수 호출시점 변경할 예정
	transform->rootTransformUpdate();

	// 최종 TransformMatrix에서의 월드좌표를 뽑아내서 반환한 것으로 초기화
	startPoint = transform->getWorldPosition();

	basicEnemyAnimationFSM = gameObject->getComponent<BasicEnemyAnimationFSM>();


	basicEnemyBT = new BT::Root();
	basicEnemyBT->openBranch
	(
		(new BT::Selector())->openBranch
		(
			(new BT::Suquence())->openBranch
			(
				// 타겟검사
				new BT::Condition<BasicEnemyScript, &BasicEnemyScript::isTargeting>(this),
				(new BT::Selector())->openBranch
				(
					// 복귀할 위치라면 // 일정 범위를 벗어났다면
					// 범위 이상나가면 다시 타겟을 nullptr로 바꿔준다.
					(new BT::If<BasicEnemyScript, &BasicEnemyScript::isOutOfRange>(this))->openBranch
					(
						new BT::Action<BasicEnemyScript, &BasicEnemyScript::resetTarget>(this),
						BTNodeBranchEnd
					),

					// Range1일때 // 공격
					(new BT::If<BasicEnemyScript, &BasicEnemyScript::isInRange1>(this))->openBranch
					(
						new BT::Action<BasicEnemyScript, &BasicEnemyScript::attackTarget>(this),
						BTNodeBranchEnd
					),

					// Range2일때 // 대쉬
					(new BT::If<BasicEnemyScript, &BasicEnemyScript::isInRange2>(this))->openBranch
					(
						new BT::Action<BasicEnemyScript, &BasicEnemyScript::dashToTarget>(this),
						BTNodeBranchEnd
					),

					// Range3일때 // 추격
					(new BT::If<BasicEnemyScript, &BasicEnemyScript::isInRange3>(this))->openBranch
					(
						new BT::Action<BasicEnemyScript, &BasicEnemyScript::chaseTarget>(this),
						BTNodeBranchEnd
					),


					BTNodeBranchEnd
				),
				BTNodeBranchEnd
			),

			// 복귀검사 // 스타트 포인트와 비슷해질때까지 복귀
			(new BT::If<BasicEnemyScript, &BasicEnemyScript::haveToReturn>(this))->openBranch
			(
				new BT::Action<BasicEnemyScript, &BasicEnemyScript::returnToStartPoint>(this),
				BTNodeBranchEnd
			),

			// 대기할때 할일
			(new BT::If<BasicEnemyScript, &BasicEnemyScript::isNotTargeting>(this))->openBranch
			(
				new BT::Action<BasicEnemyScript, &BasicEnemyScript::idle>(this),
				BTNodeBranchEnd
			),
			BTNodeBranchEnd
		),
		BTNodeBranchEnd
	);

}

void BasicEnemyScript::onDestroy()
{
	delete basicEnemyBT;
}

void BasicEnemyScript::onCollisionStay(GameObjectWithCollision & other)
{
	if (other.gameObject->getTag() == "Player")
	{
		DamageableScript * damageableScript = other.gameObject->getComponent<DamageableScript>();
		if (damageableScript)
			damageableScript->getDamaged(power);
	}

}

float BasicEnemyScript::getTargetDistance()
{
	if (target == nullptr)
		return INFINITE;

	//// 자식 객체일 수도 있으니 최종 Transformation행렬을 받아서 계산한다. // 위치만 추출한다.
	//const D3DXMATRIX & transformMatrix = transform->getTransformMatrix();
	//const D3DXMATRIX & targetTransformMatrix = transform->getTransformMatrix();

	//Vector3 targetPos{ transformMatrix._41,transformMatrix._42,transformMatrix._43 };
	//Vector3 pos{ targetTransformMatrix._41,  targetTransformMatrix._42,  targetTransformMatrix._43 };
	Vector3 pos{ transform->getWorldPosition() };
	Vector3 targetPos{ target->getTransform()->getWorldPosition() };
	return Vector3::Distance(targetPos, pos);
}

bool BasicEnemyScript::isOutOfRange()
{

	// 타겟과 시작점과의 거리가 300이상이면 
	return Vector3::Distance(target->getTransform()->getWorldPosition(), startPoint)  > 200.0f;
}

bool BasicEnemyScript::isInRange()
{
	return Vector3::Distance(transform->getWorldPosition(), startPoint) <= 160.0f;
}

bool BasicEnemyScript::isInRange1()
{
	return getTargetDistance() <= 5.0f;
}

bool BasicEnemyScript::isInRange2()
{
	return getTargetDistance() <= 30.0f;
}

bool BasicEnemyScript::isInRange3()
{
	return getTargetDistance() <= 200.0f;
}

bool BasicEnemyScript::isTargeting()
{
	return target != nullptr;
}

bool BasicEnemyScript::isNotTargeting()
{
	return target == nullptr;
}

bool BasicEnemyScript::haveToReturn()
{
	return (Vector3::Distance(transform->getWorldPosition(), startPoint) > 10);
}

void BasicEnemyScript::returnToStartPoint()
{
	Trace::Write("TAG_DEBUG", "return");
	
	if (basicEnemyAnimationFSM)
	{
		basicEnemyAnimationFSM->setBool("isIdle", false);
		basicEnemyAnimationFSM->setBool("isRunning", true);
		basicEnemyAnimationFSM->setBool("isAttacking", false);
	}

	// 월드 좌표를 받아주고
	Vector3 & currentWorldPos = transform->getWorldPosition();

	// 방향을 구하고 정규화
	Vector3 direction = startPoint - currentWorldPos;
	Vector3::Normalized(direction, direction);

	// 이동한다
	transform->setWorldPosition(transform->getWorldPosition() + FrameTime::GetDeltaTime() * .1f * speed * direction);

	//lookAtTarget(direction);
}

void BasicEnemyScript::idle()
{
	Trace::Write("TAG_DEBUG", "idle");
	if (basicEnemyAnimationFSM)
	{
		basicEnemyAnimationFSM->setBool("isIdle", true);
		basicEnemyAnimationFSM->setBool("isRunning", false);
		basicEnemyAnimationFSM->setBool("isAttacking", false);
	}
}

void BasicEnemyScript::resetTarget()
{
	gameObject->getAudio().playBackGroundSound("BS_BackGround_MainScene.mp3");
	target = nullptr;
}

void BasicEnemyScript::attackTarget()
{
	Trace::Write("TAG_DEBUG", "attack");
	if (!target) return;

	if (basicEnemyAnimationFSM)
	{
		basicEnemyAnimationFSM->setBool("isIdle", false);
		basicEnemyAnimationFSM->setBool("isRunning", false);
		basicEnemyAnimationFSM->setBool("isAttacking", true);
	}

	// 월드 좌표를 받아주고
	Vector3 & currentWorldPos = transform->getWorldPosition();

	// 방향을 구하고 정규화
	Vector3 direction = target->getTransform()->getWorldPosition() - currentWorldPos;
	Vector3::Normalized(direction, direction);

	lookAtTarget(direction);
}

void BasicEnemyScript::dashToTarget()
{
	Trace::Write("TAG_DEBUG", "dash");

	if (!target) return;

	if (basicEnemyAnimationFSM)
	{
		basicEnemyAnimationFSM->setBool("isIdle", false);
		basicEnemyAnimationFSM->setBool("isRunning", true);
		basicEnemyAnimationFSM->setBool("isAttacking", false);
	}
	// 월드 좌표를 받아주고
	Vector3 & currentWorldPos = transform->getWorldPosition();

	// 방향을 구하고 정규화
	Vector3 direction = target->getTransform()->getWorldPosition() -currentWorldPos;
	Vector3::Normalized(direction, direction);

	// 이동한다
	transform->setWorldPosition(transform->getWorldPosition() + FrameTime::GetDeltaTime() * .1f * dashSpeed * direction);

	lookAtTarget(direction);
}

void BasicEnemyScript::chaseTarget()
{
	Trace::Write("TAG_DEBUG", "chase");
	if (!target) return;

	if (basicEnemyAnimationFSM)
	{
		basicEnemyAnimationFSM->setBool("isIdle", false);
		basicEnemyAnimationFSM->setBool("isRunning", true);
		basicEnemyAnimationFSM->setBool("isAttacking", false);
	}
	// 월드 좌표를 받아주고
	Vector3 & currentWorldPos = transform->getWorldPosition();

	// 방향을 구하고 정규화
	Vector3 direction = target->getTransform()->getWorldPosition() - currentWorldPos;
	Vector3::Normalized(direction, direction);

	// 이동한다
	transform->setWorldPosition(transform->getWorldPosition() + FrameTime::GetDeltaTime() * .1f * speed * direction);

	lookAtTarget(direction);
}



void BasicEnemyScript::lookAtTarget(const Vector3 & targetDirection)
{
	// 타겟방향쪽을 기준으로 enemy의 로컬좌표계로 들어간다.
	// 구하고자 하는 점을 Terrain의 로컬좌표계로 변환한다.
	// 로컬좌표계로 변환하기 위해서는 Terrain의 transform행렬의 역행렬을 곱해주면 된다.
	D3DXMATRIX inverseTransformMatrix;
	D3DXVECTOR3 targetDirectionInEnemyLocal;

	if (gameObject->getParent() == nullptr)
	{
		D3DXMatrixIdentity(&inverseTransformMatrix);
	}
	else
	{
		if (gameObject->getParent()->getTransform()->getInverseTransformMatrix(inverseTransformMatrix) == false) return;
	}
	//if (transform->getInverseTransformMatrix(inverseTransformMatrix) == false) return;

	Vector3::ToD3DXVECTOR3(targetDirectionInEnemyLocal, targetDirection);
	//D3DXVec3Normalize(&targetDirectionInEnemyLocal, &targetDirectionInEnemyLocal);

	D3DXVec3TransformNormal(&targetDirectionInEnemyLocal, &targetDirectionInEnemyLocal, &inverseTransformMatrix);
	D3DXVec3Normalize(&targetDirectionInEnemyLocal, &targetDirectionInEnemyLocal);
	//Vector3::ToVector3(targetDirection, targetDirectionInEnemyLocal);
	// 현재 로컬좌표계로들어왔다.
	// 포워드(0,0,1)와 계산해서 로테이션을 정해준다.
	targetDirectionInEnemyLocal.y = 0;
	float angle = (float)acos(D3DXVec3Dot(&D3DXVECTOR3{ 0,0,1 }, &targetDirectionInEnemyLocal) / (1.0f * 1.0f));
	if (targetDirectionInEnemyLocal.x < 0)
		angle = -angle;

	transform->setWorldRotation(Vector3(transform->getLocalRotation().getX(), Transform::RadianToDegree(angle), transform->getLocalRotation().getZ()));

}





void BasicEnemySearchRangeScript::start()
{
	// 필요한 것들을 초기화시켜준다.
	GameObject * parent = gameObject->getParent();
	if (parent != nullptr)
	{
		BasicEnemyScript * basicEnemyScript = parent->getComponent<BasicEnemyScript>();
		if (basicEnemyScript)
		{
			this->basicEnemyScript = basicEnemyScript;
		}
	}
}

void BasicEnemySearchRangeScript::onCollisionStay(GameObjectWithCollision & other)
{
	if (basicEnemyScript == nullptr) return;

	// 들어온 오브젝트 태그가 플레이어일때
	if (other.gameObject->getTag() == "Player")
	{
		// 타겟이 없을때
		if (basicEnemyScript->isNotTargeting())
		{
			if (gameObject->getParent()->getTag() == "Enemy")
			{
				gameObject->getAudio().playBackGroundSound("BS_BackGround_Monster.mp3");
			}
			else // if(gameObject->getTag() == "EnemyBoss")
			{
				gameObject->getAudio().playBackGroundSound("BS_BackGround_BossMonster.mp3");
			}
			basicEnemyScript->setTarget(other.gameObject);
		}
	}
}

void DamageableScript::start()
{
	currentHp = maxHp;
}
void DamageableScript::setMaxHp(float maxHp)
{
	if (maxHp < 0)
		maxHp = 10.0f;
	this->maxHp = maxHp;
	currentHp = maxHp;

	if (gameObject->getTag() == "Player")
	{
		gameObject->getGameUI().setCurrentPlayerHpFactor(currentHp / maxHp);
	}
}
void DamageableScript::setCurrentHp(float hp)
{
	if (hp < 0)
		hp = 0;
	if (hp > maxHp)
		hp = maxHp;

	currentHp = hp;

	if (gameObject->getTag() == "Player")
	{
		gameObject->getGameUI().setCurrentPlayerHpFactor(currentHp / maxHp);
	}
}

void DamageableScript::getDamaged(float power)
{
	if (power < 0)
		power = 0;

	currentHp -= power;

	if (gameObject->getTag() == "Player")
	{
		gameObject->getGameUI().setCurrentPlayerHpFactor(currentHp / maxHp);
	}
	if (currentHp <= 0)
	{
		if (gameObject->getTag() == "Player")
		{
			Locator::provideScene(Locator::SystemType::RELEASETYPE, Locator::SceneType::END);
			gameObject->getGameUI().setEndSceneUI();
			// 리셋
			return;
		}
		currentHp = 0.0f;
		GameObject::Destroy(gameObject);
	}


}
void MoveScript_C::start()
{

}

void MoveScript_C::update()
{
	//Vector3 temp = transform->getWorldRotation();
	//Trace::Write("TAG_DEBUG", "x", temp.getX());
	//Trace::Write("TAG_DEBUG", "y", temp.getY());
	//Trace::Write("TAG_DEBUG", "z", temp.getZ());

	if (::GetAsyncKeyState('K') & 0x8000f)
	{
		transform->setWorldRotation(transform->getWorldRotation() + Vector3{ 0,-.05f,0 });
		//AudioSource * temp = gameObject->getAudio().getAudioSource("BS_Effect_1.mp3");
		//if (temp)
		//{
		//	//temp->play(false);
		//	//temp->setVolume(2.0f);
		//}
	}

	if (::GetAsyncKeyState('L') & 0x8000f)
	{
		transform->setWorldRotation(transform->getWorldRotation() + Vector3{ 0,.05f,0 });
		//AudioSource * temp = gameObject->getAudio().getAudioSource("BS_Effect_1.mp3");
		//if (temp)
		//{
		//	temp->play(false);
		//	temp->setVolume(2.0f);
		//}
	}



	if (::GetAsyncKeyState('P') & 0x8000f)
		GameObject::Destroy(gameObject);
}



void MainCamera::start()
{
	mainObjTag = Scene::MainObjTag::MOT_MAINCAMERA;
	gameObject->getScene().registerMainObject(gameObject, (Scene::MainObjTag)mainObjTag);
}

void MainCamera::update()
{
	// 메인카메라에서 오디오를 최신화 시킨다.
	setViewSpace();

	if (::GetAsyncKeyState('E') & 0x8000f)
		transform->setLocalRotation(transform->getLocalRotation() + Vector3{ 0,1.005f,0 });
	if (::GetAsyncKeyState('Q') & 0x8000f)
		transform->setLocalRotation(transform->getLocalRotation() + Vector3{ 0,-1.005f,0 });

	if (::GetAsyncKeyState('Z') & 0x8000f)
	{
		//transform->setRotation(transform->getRotation() + Vector3{ 0,.05f,.05f });
		transform->setLocalRotation(transform->getLocalRotation() + Vector3{ 1.05f,0,0 });
	}

	if (::GetAsyncKeyState('C') & 0x8000f)
	{
		//transform->setRotation(transform->getRotation() + Vector3{ 0,-.05f,-.05f });
		transform->setLocalRotation(transform->getLocalRotation() + Vector3{ -1.05f,0,0 });
	}

	//if (::GetAsyncKeyState('W') & 0x8000f)
	//	transform->setLocalPosition(transform->getLocalPosition() + 2*(transform->getForward()));
	//if (::GetAsyncKeyState('S') & 0x8000f)
	//	transform->setLocalPosition(transform->getLocalPosition() + 2*(-1)*(transform->getForward()));

	//if (::GetAsyncKeyState('D') & 0x8000f)
	//	transform->setLocalPosition(transform->getLocalPosition() + 2*(transform->getRight()));
	//if (::GetAsyncKeyState('A') & 0x8000f)
	//	transform->setLocalPosition(transform->getLocalPosition() + 2*(-1)*(transform->getRight()));

}

MainCamera::MainCamera(GameObject * go, Transform * tf)
	: Component(go, tf)
{
	start();
}

MainCamera::~MainCamera()
{
	gameObject->getScene().unregisterMainObject(gameObject, (Scene::MainObjTag)mainObjTag);
}

void MainCamera::setViewSpace()
{
	D3DXMATRIX v;
	D3DXMATRIX temp;
	D3DXMatrixIdentity(&v);

	transform->getInverseTransformMatrix(v);
	//v = transform->getTransformMatrix();
	//float t;
	//D3DXMatrixInverse(&v, &t, &v);
	/*
	// 위치 반대로 계산
	D3DXMatrixMultiply(&v, &v, D3DXMatrixTranslation(&temp, -transform->getPosition().getX(),
		-transform->getPosition().getY(), -transform->getPosition().getZ()));

	// 로테이션 반대로 계산
	//D3DXMatrixMultiply(&v, &v, D3DXMatrixRotationX(&temp, -transform->getRotation().getX()));
	//D3DXMatrixMultiply(&v, &v, D3DXMatrixRotationY(&temp, -transform->getRotation().getY()));
	//D3DXMatrixMultiply(&v, &v, D3DXMatrixRotationZ(&temp, -transform->getRotation().getZ()));
	D3DXMatrixRotationYawPitchRoll(&temp,transform->DegreeToRadian(-transform->getRotation().getY()), 
		transform->DegreeToRadian(-transform->getRotation().getX()), transform->DegreeToRadian(-transform->getRotation().getZ()));
	D3DXMatrixMultiply(&v, &v, &temp);
	*/

	gameObject->getDevice().SetTransform(D3DTS_VIEW, &v);
}

void MainCamera::getViewMatrix(D3DXMATRIX* v)
{
	if (!v) return;


	//D3DXMATRIX temp;
	//D3DXMatrixIdentity(v);


	//// 위치 반대로 계산
	//D3DXMatrixMultiply(v, v, D3DXMatrixTranslation(&temp, -transform->getPosition().getX(),
	//	-transform->getPosition().getY(), -transform->getPosition().getZ()));

	//// 로테이션 반대로 계산
	//D3DXMatrixMultiply(v, v, D3DXMatrixRotationX(&temp, -transform->getRotation().getX()));
	//D3DXMatrixMultiply(v, v, D3DXMatrixRotationY(&temp, -transform->getRotation().getY()));
	//D3DXMatrixMultiply(v, v, D3DXMatrixRotationZ(&temp, -transform->getRotation().getZ()));

	//-------------------------------------------------------------------------------------------------

	// Keep camera's axes orthogonal to eachother
	
	////월드 좌표계 기준으로 한다면 별로 필요없을 것 같다.
	//D3DXVECTOR3 & pos = (transform->getPosition()).toD3DXVECTOR3();

	//D3DXVec3Normalize(&_look, &_look);

	//D3DXVec3Cross(&_up, &_look, &_right);
	//D3DXVec3Normalize(&_up, &_up);

	//D3DXVec3Cross(&_right, &_up, &_look);
	//D3DXVec3Normalize(&_right, &_right);

	//// Build the view matrix:
	//float x = -D3DXVec3Dot(&_right, &pos);
	//float y = -D3DXVec3Dot(&_up, &pos);
	//float z = -D3DXVec3Dot(&_look, &pos);

	//(*V)(0, 0) = _right.x; (*V)(0, 1) = _up.x; (*V)(0, 2) = _look.x; (*V)(0, 3) = 0.0f;
	//(*V)(1, 0) = _right.y; (*V)(1, 1) = _up.y; (*V)(1, 2) = _look.y; (*V)(1, 3) = 0.0f;
	//(*V)(2, 0) = _right.z; (*V)(2, 1) = _up.z; (*V)(2, 2) = _look.z; (*V)(2, 3) = 0.0f;
	//(*V)(3, 0) = x;        (*V)(3, 1) = y;     (*V)(3, 2) = z;       (*V)(3, 3) = 1.0f;
	

}

MeshRenderer::MeshRenderer(GameObject * go, Transform * tf)
	: Component(go, tf), mesh(nullptr)
{
	device = &(gameObject->getDevice());
}

void MeshRenderer::loadXFile(const string & fileName)
{
	this->fileName = fileName;

	HRESULT hr = 0;

	ID3DXBuffer* adjBuffer = 0;
	ID3DXBuffer* mtrlBuffer = 0;
	unsigned long numMtrls = 0;

	// X파일을 읽는다.
	hr = D3DXLoadMeshFromX(
		this->fileName.c_str(),
		D3DXMESH_MANAGED,
		device,
		&adjBuffer,
		&mtrlBuffer,
		0,
		&numMtrls,
		&mesh);

	if (FAILED(hr))
	{
		::MessageBox(0, "GenMesh - FAILED", 0, 0);
		return;
	}

	// 재질이 있을때 내부적으로 처리
	setMtrlsAndTextures(mtrlBuffer, numMtrls);

	// 사용되었던 재질버퍼를 비워준다.
	if (mtrlBuffer)
		mtrlBuffer->Release();


	optimizeMesh(adjBuffer);

	// 사용완료한 인접버퍼 릴리즈
	if (adjBuffer)
		adjBuffer->Release();
}

void MeshRenderer::render()
{
	if (!mesh) return;

	// 현재 transform행렬로 적용시킨다.
	device->SetTransform(D3DTS_WORLD, &transform->getTransformMatrix());

	// 파일이 로드되어서 값이 있는 경우만 그려준다.
	for (int i = 0; i < mtrls.size(); ++i)
	{
		device->SetMaterial(&mtrls[i]);
		device->SetTexture(0, textures[i]);
		mesh->DrawSubset(i);
	}

}

void MeshRenderer::optimizeMesh(ID3DXBuffer * adjBuffer, unsigned long optimizeFlag)

{
	HRESULT hr;
	hr = mesh->OptimizeInplace(
		optimizeFlag,
		(unsigned long*)adjBuffer->GetBufferPointer(),
		0, 0, 0);

	if (FAILED(hr))
		::MessageBox(0, "OptimizeMesh - FAILED", 0, 0);

}


void MeshRenderer::setMtrlsAndTextures(ID3DXBuffer * mtrlBuffer, unsigned long numMtrls)
{
	if (mtrlBuffer == 0 || numMtrls == 0) return;

	// D3DXMATERAL 형식으로 읽으려면 타입캐스팅이 필요하다.
	D3DXMATERIAL* pMtrls = (D3DXMATERIAL*)mtrlBuffer->GetBufferPointer();

	for (int i = 0; i < numMtrls; i++)
	{
		// 로드될때 ambient값을 가지지 않으므로 이를 지정한다.
		pMtrls[i].MatD3D.Ambient = pMtrls[i].MatD3D.Diffuse;

		mtrls.push_back(pMtrls[i].MatD3D);

		if (pMtrls[i].pTextureFilename != 0)
		{
			// 파일이름이 정상적으로 들어가 있을때만 그 파일명으로 파일을 열어서 텍스처를 로드한다.
			IDirect3DTexture9* tex = 0;
			D3DXCreateTextureFromFile(
				device,
				pMtrls[i].pTextureFilename,
				&tex);

			// 텍스처가 있을때 그 텍스처를 넣어주고
			textures.push_back(tex);
		}
		else
		{
			// 없을때도 서브셋과 같은 인덱스 번호를 맞춰주기 위해서 널값을 넣어준다.
			textures.push_back(0);
		}
	}

}


const float BoxCollider::DefaultSize = 2.5f;

void RigidBody::start()
{
	gameObject->getPhysics().registerRigidBody(gameObject);
	gameObject->setRigidBody(this);
}

void RigidBody::fixedUpdate()
{
	transform->setDirty(true);
	gameObject->getPhysics().setTransformFromSystem(gameObject);
	transform->rootTransformUpdate(false);

	// 실시간으로 바로 업데이트하기 때문에 더티를 먼저켜고 업데이트한다.
	// 물리업데이트가 되었기 때문에 더티플래그를 켜준다.
	//transform->setDirty(true);

}

void RigidBody::update()
{
	if (colliderInfo.type == ColliderType::NONE) return;

	Vector3 worldPos = transform->getWorldPosition();
	// 콜라이더를 그려준다.
	switch (colliderInfo.type)
	{

	case ColliderType::BOX:
	/*
	월드 좌표계기준으로 그려줘야한다
	*/
		Gizmos::DrawBox({ worldPos.getX(), worldPos.getY(), worldPos.getZ() },	//({ transform->getPosition().getX(), transform->getPosition().getY(), transform->getPosition().getZ() },
		{ colliderInfo.size.getX(),colliderInfo.size.getY(),colliderInfo.size.getZ() });
		break;

	case ColliderType::SPHERE:
		Gizmos::DrawBox({ worldPos.getX(), worldPos.getY(), worldPos.getZ() },	//({ transform->getPosition().getX(), transform->getPosition().getY(), transform->getPosition().getZ() },
		{ colliderInfo.size.getX(),colliderInfo.size.getY(),colliderInfo.size.getZ() });
		break;

	}

}

void RigidBody::onDestroy()
{
	gameObject->getPhysics().unregisterRigidBody(gameObject);
	gameObject->setRigidBody(nullptr);
}

void RigidBody::turnOnStaticFlag()
{
	gameObject->getPhysics().turnOnStaticFlag(gameObject);
}

void RigidBody::turnOffStaticFlag(float mass)
{
	gameObject->getPhysics().turnOffStaticFlag(gameObject, mass);
}

void RigidBody::turnOnIsTriggerFlag()
{
	gameObject->getPhysics().turnOnIsTriggerFlag(gameObject);
}

void RigidBody::turnOffIsTriggerFlag()
{
	gameObject->getPhysics().turnOffIsTriggerFlag(gameObject);
}

void RigidBody::setBoxCollider(const Vector3 & size)
{
	gameObject->getPhysics().setBoxCollider(gameObject, size);
	// 내부적으로 가지고 있는 colliderInfo도 바꿔준다.
	colliderInfo.type = ColliderType::BOX;
	colliderInfo.size = size;
}

void RigidBody::setSphereCollider(float radius)
{
	gameObject->getPhysics().setSphereCollider(gameObject, radius);
	// 내부적으로 가지고 있는 colliderInfo도 바꿔준다.
	colliderInfo.type = ColliderType::SPHERE;
	colliderInfo.size = {radius,radius,radius};
}

//virtual void setGravity(GameObject * other, Vector3 & value);
//virtual void getGravity(Vector3 & output, GameObject * other);
void RigidBody::setGravity(Vector3 & value)
{
	gameObject->getPhysics().setGravity( gameObject,value);
}

void RigidBody::getGravity(Vector3 & output)
{
	gameObject->getPhysics().getGravity(output, gameObject);
}

void RigidBody::addForce(const Vector3 & directionWithScalar)
{
	gameObject->getPhysics().addForce(gameObject, directionWithScalar);
}

const unsigned long FbxModelRenderer::FbxModelVertex::DefaultFVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
const unsigned long FbxModelRenderer::DefaultOptimizeFlag = D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_COMPACT | D3DXMESHOPT_VERTEXCACHE;
const string FbxModelRenderer::filePathToLoadFbxMeshFiles = "../Fbx/Models/";
const string FbxModelRenderer::filePathToLoadFbxAnimationFiles = "../Fbx/Animations/";
const string FbxModelRenderer::filePathToLoadFbxTextureFiles = "../Fbx/Textures/";

void FbxModelRenderer::render()
{
	if (meshesForShading.size() ==0) return;

	int meshCount = meshesForShading.size();
	int fbxCount = fbxMeshes.size();


	map<MeshForShading *, vector<D3DMATERIAL9>>::iterator tempMaterialForMesh;
	map<MeshForShading *, vector<IDirect3DTexture9*>>::iterator tempTexturesForMesh;
	D3DMATERIAL9 * material = nullptr;
	IDirect3DBaseTexture9 * texture = nullptr;

	for(int i =0; i< meshesForShading.size(); ++i)
	{
		if (!meshesForShading[i]) continue;
		//animation-------------------------------------------------------------
		// 본들이 있을때			
		// 애니메이션 매니저가있을때
		// 애니메이션이 출력되어야 할때
		// 순차적으로 처리하기 때문에 널포인터가 getIsAnimating을 호출할 일은 없다.
		// 처리하다가 false판명시 바로 빠져나감
		if (skeletonBones && animations && animations->getIsAnimating())
		{
			animations->update();
			// 애니메이션을위한 본들의 계산(최종매트릭스 계산)
			processSkeletonBonesAnimation(animations);

			// 본들의 가중치를 합친 최종 버텍스 메트릭스 계산
			fbxMeshes[i]->processVertexAnimationMatrix(skeletonBones);


			// FbxModelMesh(mesh와 인덱스 1:1 매칭)에 접근해서(vectorMyVertex)
			// 렌더링할 mesh의 버텍스 버퍼를 열어서 최종적인 위치 값들을 다시 넣어준다.
			//processVerticesWithAnimation(meshes[i], fbxMeshes[i]);
			

			// 애니메이션이 있을때 애니메이션 메트릭스가 계속 변하기 때문에 최신화시켜준다.



			// FbxModelMesh의 각 버텍스에서 MyFbxVertex->getPositionWithAnimation해서 초기화 
			//  position * animationmatrix // 	
			// 내부적으로 가중치 벡터를 가진 점에 대해서 내부 애니메이션 매트릭스 계산


			// 만약에 애니메이션이 멈춘다면 모든 메쉬들을 processVertice함수로 // 초기값으로 지정해준다.
		}

		processVertices(meshesForShading[i], fbxMeshes[i]);


		
		//material, texture-----------------------------------------------------
		// 일단 맵에 등록되어있는지 확인한다.
		tempMaterialForMesh = materialsTable.find(meshesForShading[i]);
		tempTexturesForMesh = texturesTable.find(meshesForShading[i]);

		material = nullptr;
		texture = nullptr;

		// material처리
		// 맵에 등록되어있다면 벡터에 값이 들어있는지 확인한다.
		if (tempMaterialForMesh != materialsTable.end())
		{
			// 리스트(vector)에 값이 들어가 있다면 첫번째 값으로 초기화 한다.
			if ((*tempMaterialForMesh).second.size() >= 1)
			{
				material = (&(*tempMaterialForMesh).second[0]);
			}
		}

		// texture처리
		// 맵에 등록되어있다면 벡터에 값이 들어있는지 확인한다.
		if (tempTexturesForMesh != texturesTable.end())
		{
			// 리스트(vector)에 값이 들어가 있다면 첫번째 값으로 초기화 한다.
			if ((*tempTexturesForMesh).second.size() >= 1)
			{
				texture = (*tempTexturesForMesh).second[0];
			}
		}




		// 쉐이더 지정 // 내부적으로 빛 계산도 다 해준다.
		device->SetVertexShader(fbxModelRendererWithAnimationShader);
		device->SetVertexDeclaration(declaration);


		// 소스 지정
		device->SetStreamSource(0, meshesForShading[i]->vb, 0, sizeof(FbxModelVertex));
		device->SetIndices(meshesForShading[i]->ib);

		D3DXMATRIX viewMatrix;
		D3DXMATRIX projectionMatrix;

		// 먼저 카메라 부터 최신화
		device->GetTransform(D3DTS_VIEW, &viewMatrix);
		device->GetTransform(D3DTS_PROJECTION, &projectionMatrix);


		//gameObject->getScene().getMainCamera()->getComponent<MainCamera>()->getViewMatrix(&viewMatrix);
		/*쉐이더 파라미터 초기화*/
		// worldViewProjection 초기화
		constTable->SetMatrix
		(
			device,
			worldViewProjectionMatrixHandle,
			&D3DXMATRIX(transform->getTransformMatrix() * viewMatrix * projectionMatrix)
		);

		constTable->SetVector
		(
			device,
			scaleFactorHandle,
			&D3DXVECTOR4(scaleFactor.getX(), scaleFactor.getY(), scaleFactor.getZ(),0)
		);

		// 텍스처 지정



		// rendering-----------------------------------------------------
		if(material)
			device->SetMaterial(material);
		if(texture)
			device->SetTexture(0, texture);
		
		device->DrawIndexedPrimitive(
			D3DPT_TRIANGLELIST, 0, 0, meshesForShading[i]->vertexCount, 0, meshesForShading[i]->triCount);


		device->SetVertexShader(NULL);
		device->SetVertexDeclaration(NULL);



	}

}

void FbxModelRenderer::start()
{
	device = &(gameObject->getDevice());

	HRESULT hr = 0;

	D3DVERTEXELEMENT9 decl[] =
	{
		/*
		나중에 셰이더 코드의 구조체
		struct VS_INPUT
		{
		vector positi : POSITION // POSITION0
		: NORMAL0
		: NORMAL1
		: NORMAL2
		}
		과 매핑된다.
		/*

		float position[3];
		float normal[3];
		float uv[2];
		float animationMatrix[4][4];

		*/
		// offsets in bytes
		{ 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0 },
		{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },

		// 나머지 매트릭스
		{ 0, 32, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 1 },
		{ 0, 48, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 2 },
		{ 0, 64, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 3 },
		{ 0, 80, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 4 },

		D3DDECL_END()
	};


	hr = device->CreateVertexDeclaration(decl, &declaration);
	if (FAILED(hr))
	{
		::MessageBox(0, "CreateVertexDeclaration() - FAILED", 0, 0);
		return;
	}


	// 쉐이더 컴파일 / 핸들 받아오기
	//------------------------------------------------------------------
	ID3DXBuffer* compiledCode = 0;
	ID3DXBuffer* errorBuffer = 0;

	hr = D3DXCompileShaderFromFile(
		"FbxModelRendererWithAnimation.vs",
		0,
		0,
		"Main", // entry point function name
		"vs_2_0",   //"vs_2_sw",//"vs_2_0",//"vs_1_1",

		D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY | D3DXSHADER_DEBUG,
		&compiledCode,
		&errorBuffer,
		&constTable);

	// output any error messages
	if (errorBuffer)
	{
		::MessageBox(0, (char*)errorBuffer->GetBufferPointer(), 0, 0);
		errorBuffer->Release();
	}

	if (FAILED(hr))
	{
		::MessageBox(0, "D3DXCompileShaderFromFile() - FAILED", 0, 0);
		return;
	}

	hr = device->CreateVertexShader(
		(DWORD*)compiledCode->GetBufferPointer(),
		&fbxModelRendererWithAnimationShader);

	if (FAILED(hr))
	{
		::MessageBox(0, "CreateVertexShader - FAILED", 0, 0);
		return;
	}

	if (compiledCode)
		compiledCode->Release();

	worldViewProjectionMatrixHandle = constTable->GetConstantByName(0, "WorldViewProjectionMatrix");
	scaleFactorHandle = constTable->GetConstantByName(0, "ScaleFactor");
}

void FbxModelRenderer::onDestroy()
{
	{
		// 텍스처 정보 삭제
		// 머티리얼 경우는 그냥 복사해서 해제했기 때문에 따로 신경안써도된다.
		for (auto it = texturesTable.begin(); it != texturesTable.end(); ++it)
		{
			// 맵을 돌면서 텍스처 벡터를 받아서 모두 릴리즈 시켜준다.
			// second값이 그자체의 값을 반환하는 것이기 때문에 따로 복사가 안된다. 
			// 만약 복사가 된다고 해도 포인터자체값을 통해서 릴리즈 하기 때문에 상관없긴하다.
			/*vector<IDirect3DTexture9*> & temp = */

			for (auto it2 = (*it).second.begin(); it2 != (*it).second.end(); ++it2)
			{
				if ((*it2) != nullptr)
					(*it2)->Release();
			}

		}

		// 메쉬 정보 삭제
		for (auto it = meshesForShading.begin(); it != meshesForShading.end(); ++it)
		{
			if ((*it) != nullptr)
				delete (*it);
			(*it) = nullptr;
		}
		meshesForShading.clear();

		// fbx 메쉬 정보 삭제
		for (auto it = fbxMeshes.begin(); it != fbxMeshes.end(); ++it)
		{
			if ((*it) != nullptr)
				delete (*it);
			(*it) = nullptr;
		}
		fbxMeshes.clear();

		// 스켈레톤 본 정보 삭제
		if (skeletonBones)
			delete skeletonBones;

		if (animations)
			delete animations;

		if (importer)
			importer->Destroy();

		if (scene)
			scene->Destroy();

		//d3d::Release<ID3DXMesh*>(mesh);

		// 텍스처만 할당된 값이고 재질은 복사된 값이다.
		//for (int i = 0; i < textures.size(); i++)
		//	d3d::Release<IDirect3DTexture9*>(textures[i]);
	}
}

FbxModelRenderer::FbxModelRenderer(GameObject * go, Transform * tf)
	: Component(go, tf), scene(nullptr), importer(nullptr), skeletonBones(nullptr), animations(nullptr),
	scaleFactor(1,1,1),
	// 쉐이더
	declaration(nullptr), fbxModelRendererWithAnimationShader(nullptr), constTable(nullptr), worldViewProjectionMatrixHandle(0)
{
	start();
}

void FbxModelRenderer::playWithFileName(const string & animationFileName)
{
	if (animations)
		animations->playWithFileName(animationFileName);
}

void FbxModelRenderer::stop()
{
	animations->stop();
}


void FbxModelRenderer::setScaleFactor(const Vector3 & scaleFactor)
{
	this->scaleFactor.setVector3(scaleFactor.getX(), scaleFactor.getX(), scaleFactor.getX());
}




void FbxModelRenderer::loadFbxFile(const string & fileName)
{
	gameObject->getFbxParser().loadSceneFromFbxFile(filePathToLoadFbxMeshFiles+fileName, &scene, &importer);

	if (scene == nullptr) return;

	FbxNode * rootNode = scene->GetRootNode();
	// 루트 노드를 받아온다.
	//FbxNode * rootNode = (*fbxInfo.getFbxScene())->GetRootNode();


	/*
	1. 스켈레톤본들 초기화
	2. 메쉬 초기화 // 초기화 과정에서 스켈레톤 본이 존재한다면 버텍스들의 본 가중치도 넣어준다.
	*/
	processAllNodesWithSameAtrributeTypeFromRoot(rootNode, FbxNodeAttribute::eSkeleton);
	processAllNodesWithSameAtrributeTypeFromRoot(rootNode, FbxNodeAttribute::eMesh);

	// 애니메이션이 들어있을 수도 있으니 애니메이션 생성
	processAnimation(fileName,scene, importer);

	//skeletonBones->printAllInfo();



	//getAllFbxModelMeshesFromRoot(rootNode);

	// 노드를 순회하면서 메쉬정보부터 초기화하고
	//fbxInfo.loadMeshFromNodes(rootNode);

	// 여기서 processControlPoints와 processVertices빼고 처리
	// mesh객체에 FbxModelMesh의 정보를 넣어서 처리
	for (auto it = fbxMeshes.begin(); it != fbxMeshes.end(); ++it)
	{
		/// 메쉬정보로부터 컨트롤 포인트를 초기화 한다.
		///*삭제예정*/(*it)->processControlPoints();
		/// 마지막으로 메쉬정보, 컨트롤 포인트로 버텍스 / 인덱스를 초기화 한다.
		///*삭제예정*/(*it)->processVertices();

		// 미리 초기화된 FbxMesh를 가지는 fbxModelMesh에서 부터 필요한 정보 추출 완료 // fbxModelMeshes에 담겨있음
		//---------------------------------------------------------------------------
		// 정보를 가지고 DirectX에서 지원하는 mesh형식으로 변환
		MeshForShading * meshForShading = new MeshForShading;
		//ID3DXMesh * mesh = nullptr;
		HRESULT hr = 0;
		meshForShading->indexCount = (*it)->getIndexCount();
		meshForShading->vertexCount = (*it)->getVertexCount();
		meshForShading->triCount = (*it)->getTriCount();

		hr = device->CreateVertexBuffer(
			meshForShading->vertexCount * sizeof(FbxModelVertex),
			D3DUSAGE_WRITEONLY,

			// FVF는 버텍스 정보 선언을 사용하기 때문에 사용하지 않는다.
			// 버텍스 정보 선언은 FVF보다 확장된 버전이라고 보면된다.
			0, // using vertex declaration
			D3DPOOL_MANAGED,
			&(meshForShading->vb),
			0);

		if (FAILED(hr))
		{
			if (meshForShading->vb)
				meshForShading->vb->Release();
			if (meshForShading->ib)
				meshForShading->ib->Release();

			meshForShading->vb = nullptr;
			meshForShading->ib = nullptr;

			meshesForShading.push_back(meshForShading);
			continue;
		}

		// 32비트 이므로 나중에 읽을 때 DWORD로 읽어야 한다.
		device->CreateIndexBuffer(
			// 2개의 삼각형을 이루기 위한 인덱스의 갯수는 6개이다.
			meshForShading->indexCount * sizeof(DWORD), // 2 triangles per edge
			D3DUSAGE_WRITEONLY,
			D3DFMT_INDEX32,
			D3DPOOL_MANAGED,
			&(meshForShading->ib),
			0);

		if (FAILED(hr))
		{
			if (meshForShading->vb)
				meshForShading->vb->Release();
			if (meshForShading->ib)
				meshForShading->ib->Release();

			meshForShading->vb = nullptr;
			meshForShading->ib = nullptr;

			meshesForShading.push_back(meshForShading);
			continue;
		}



		// mesh내부의 버텍스 정보와 인덱스 정보를 초기화 시켜준다.
		processVertices(meshForShading, (*it));
		processIndices(meshForShading, (*it));
		processTextures(meshForShading, (*it));

		//processSubsets(meshForShading);
		//optimizeMesh(meshForShading);

		meshesForShading.push_back(meshForShading);
	}

}

void FbxModelRenderer::loadFbxFileForAnimation(const string & fileName)
{
	FbxScene * _scene = nullptr;
	FbxImporter * _importer = nullptr;
	gameObject->getFbxParser().loadSceneFromFbxFile(filePathToLoadFbxAnimationFiles + fileName, &_scene, &_importer);

	if (!_scene || !_importer) return;

	processAnimation(fileName,_scene, _importer);

	_importer->Destroy();
	_scene->Destroy();

}

void FbxModelRenderer::processAllNodesWithSameAtrributeTypeFromRoot(FbxNode * node , FbxNodeAttribute::EType attributeType)
{

	FbxNodeAttribute * nodeAtrribute = node->GetNodeAttribute();

	// 모든 노드를 돌면서 Mesh 속성이 붙은것들을 추출한다.
	// 조건문은 앞에서부터 검사하다가 없으면 바로 false반환하기 때문에 이런식으로 코딩이 가능하다
	if (nodeAtrribute && nodeAtrribute->GetAttributeType() == attributeType)
	{
		switch (attributeType)
		{
			case FbxNodeAttribute::eMesh:
			{
				FbxModelMesh * temp = new FbxModelMesh();
				// 변환작업을 해준다. // 컨트롤포인트로 이루어진 모든 것들을 삼각형2개로 나누어준다.
				gameObject->getFbxParser().convertGeometryInfo(&nodeAtrribute);


				// 노드를 초기화시켜주고
				// 나중에 노드를 받아서 사용해야한다 // processVertices에서
				temp->setNode(node);


				// 내부적으로 프로세스해준다 
				// 컨트롤포인트 정보받기 
				temp->processControlPoints();

				// 앞서 스켈레톤 초기화이후(본들의 정보가 버텍스 초기화시 필요) 버텍스값 초기화
				// 스켈레톤본들의 정보도 필요하기 때문에 넘겨준다 // 가중치 계산해야한다. 본의인덱스필요
				temp->processVertices(skeletonBones);
				fbxMeshes.push_back(temp);
			}
			break;

			case FbxNodeAttribute::eSkeleton:
			{
				FbxSkeleton * fbxSkeleton = node->GetSkeleton();
				if (!fbxSkeleton) break;

				// 최초로 스켈레톤객체를 받아왔을때 스켈레톤본 매니저가 생성된다.
				if (skeletonBones == nullptr) skeletonBones = new FbxModelSkeletonBones();

				// 루트본의 부모본은 인덱스가 -1이고 이름이 NONE이다
				int parentBoneIndex = -1;
				string parentBoneName = "NONE";
				FbxNode * parentNode = node->GetParent();
				if (parentNode)
				{
					// 부모노드의 스켈레톤이 존재할때만 처리해준다. 
					// 아닌경우는 부모노드의 스켈레톤 존재X = 부모본이 없음
					// 즉 초기 설정해뒀던 index : -1 / name : NONE 으로 결정된다.
					if (parentNode->GetSkeleton())
					{
						parentBoneName = parentNode->GetName();
						parentBoneIndex = skeletonBones->findBoneIndexWithBoneName(parentNode->GetName());
					}
				}
				int boneIndex = skeletonBones->getIndexOfBoneToInsert();
				string boneName = node->GetName();

				skeletonBones->AddBone(parentBoneIndex, parentBoneName, boneIndex, boneName);
			}
			break;
		}

	}

	// 자식들을 돌면서 나머지 메쉬를 찾는다.
	const int childCount = node->GetChildCount();
	for (unsigned int i = 0; i < childCount; ++i)
	{
		processAllNodesWithSameAtrributeTypeFromRoot(node->GetChild(i), attributeType);
	}
}

void FbxModelRenderer::processAnimation(const string & animationFileName, FbxScene * _scene, FbxImporter * _importer)
{
	// 외부에서 파일을 받아오는 경우 먼저 씬과 임포터를 생성해서 넘겨주면 여기서 알아서 처리되도록 한다.

	if (!_scene || !_importer) return;

	FbxNode * rootNode = _scene->GetRootNode();
	if (!rootNode) return;

	if (!animations) animations = new FbxModelAnimations();

	// 1초에 몇프레임인지 알수 있다
	float frameRate = FbxTime::GetFrameRate(scene->GetGlobalSettings().GetTimeMode());

	FbxArray<FbxString *> animationArray;
	FbxDocument * document = dynamic_cast<FbxDocument *>(_scene);
	if (document)
		document->FillAnimStackNameArray(animationArray);

	// 애니메이션 만큼
	for (int i = 0; i < _importer->GetAnimStackCount(); ++i)
	{
		FbxTakeInfo * animationInfo = _importer->GetTakeInfo(i);
		if (!animationInfo) continue;

		// fbxstring형식이기 때문에 직접버퍼를 통해서 받아온다
		string animationName = animationInfo->mName.Buffer();

		FbxTimeSpan span = animationInfo->mLocalTimeSpan;

		// 초단위로 받아온다 // 시작시간과 끝시간
		// 프레임 레이트를 알고있기 때문에 몇프레임동안 이 애니메이션이 실행되는지 알 수 있다.
		double startTime = span.GetStart().GetSecondDouble();
		double stopTime = span.GetStop().GetSecondDouble();

		if (startTime < stopTime)
		{
			// 초 * 1초당 프레임수 = 전체 키프레임 수가 나온다.
			int keyFrames = (stopTime - startTime) * frameRate;
			FbxModelAnimation * animation = new FbxModelAnimation(animationFileName, animationName, keyFrames, frameRate);
			animations->addAnimation(animation);

			// 애니메이션을 추가했으면 각 본을 돌면서 추가한 애니메이션에 대한 키프레임 매트릭스를 모두 구해서 추가해준다.
			processKeyFramesOfAllNodes(rootNode, animationFileName, animationName, frameRate, startTime, stopTime);

		}
	}

	animationArray.Clear();

}

void FbxModelRenderer::processKeyFramesOfAllNodes(FbxNode * node, const string & animationFileName, const string & animationName, float frameRate, float start, float stop)
{
	if (!node) return;

	FbxNodeAttribute * nodeAttribute = node->GetNodeAttribute();
	// 순차적으로 검사하다가 아니면 더이상검사 안하기 때문에 괜찮다.
	if (nodeAttribute && nodeAttribute->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	{
		FbxModelSkeletonBone * bone = nullptr;
		if (skeletonBones)
		{
			string boneName = node->GetName();
			bone = skeletonBones->findBoneWithBoneName(boneName);
		}

		// 본마다 각 프레임별로 각각의 로컬 트랜스폼을 가진다.
		// 외부 애니메이션 파일을 가져올때는 똑같은 프로세스를 거친다. 단 본의 이름이 같아야 한다.
		if (bone)
		{
			FbxModelAnimationKeyFrames * animationKeyFrames = new FbxModelAnimationKeyFrames(animationFileName, animationName);

			double time = 0;

			D3DXMATRIX localTransform;
			FbxMatrix fbxLocalTransform;

			while (time <= (double)stop)
			{
				FbxTime animationTime;
				animationTime.SetSecondDouble(time);

				fbxLocalTransform = node->EvaluateLocalTransform(animationTime);

				FbxDXConverter::ToD3DXMATRIX(localTransform, fbxLocalTransform);

				animationKeyFrames->addKeyFrame(localTransform);

				time += 1.0f / frameRate;
			}

			bone->addAnimationKeyFrames(animationKeyFrames);
		}
	}

	// 자식본들도 재귀적으로 처리해준다.
	for (int i = 0; i < node->GetChildCount(); ++i)
	{
		processKeyFramesOfAllNodes(node->GetChild(i), animationFileName, animationName, frameRate, start, stop);
	}
}

void FbxModelRenderer::processSkeletonBonesAnimation(FbxModelAnimations * animations)
{
	if (!animations) return;
	if (!animations->getIsAnimating()) return;
	
	unsigned int currentKeyFrame = animations->getCurrentKeyFrame();
	unsigned int nextKeyFrame = animations->getNextKeyFrame();
	float keyFrameFactor = animations->getKeyFrameFactor();

	FbxModelAnimation * animation = animations->getCurrentAnimation();
	const string & animationFileName = animation->getAnimationFileName();
	const string & animationName = animation->getAnimationName();


	skeletonBones->setAllBonesAnimationMatrix(animationFileName, animationName, currentKeyFrame, nextKeyFrame, keyFrameFactor);
}

//void FbxModelRenderer::processVertices(ID3DXMesh * mesh, FbxModelMesh * fbxModelMesh)
//{
//	if (!mesh || !fbxModelMesh) return;
//	// mesh에 넣어줄때 fbxVertex형식으로 넣어줄것이다.
//	FbxModelVertex * fbxV = nullptr;
//	MyFbxVertex * tVertex = nullptr;
//	
//	//int t1 = mesh->GetNumVertices();
//	//int t2 = fbxModelMesh->getVertexCount();
//
//	// 생성한 메쉬를 잠그고 그안에 버텍스 정보를 집어넣는다.
//	// 버텍스 정보는 포지션 / 노말 / UV값이다.
//	mesh->LockVertexBuffer(0, (void**)&fbxV);
//	for (int i = 0; i < fbxModelMesh->getVertexCount(); ++i)
//	{
//		tVertex = fbxModelMesh->getVertex(i);
//		
//		// 값이 있을때
//		if (tVertex)
//		{
//			const D3DXMATRIX & tempAnimationMatrix = tVertex->getAnimationMatrix();
//			// 각각의 정보를 초기화 시켜준다.
//			fbxV[i] = 
//			{
//				// Position
//				(float)tVertex->getPosition().mData[0]/10,(float)tVertex->getPosition().mData[1]/10, (float)tVertex->getPosition().mData[2]/10,
//				// Normal
//				(float)tVertex->getNormal().mData[0],(float)tVertex->getNormal().mData[1], (float)tVertex->getNormal().mData[2],
//				// UV
//				(float)tVertex->getUV().mData[0],(float)tVertex->getUV().mData[1],
//				// AnimationMatrix
//				tempAnimationMatrix._11,tempAnimationMatrix._12,tempAnimationMatrix._13,tempAnimationMatrix._14,
//				tempAnimationMatrix._21,tempAnimationMatrix._22,tempAnimationMatrix._23,tempAnimationMatrix._24,
//				tempAnimationMatrix._31,tempAnimationMatrix._32,tempAnimationMatrix._33,tempAnimationMatrix._34,
//				tempAnimationMatrix._41,tempAnimationMatrix._42,tempAnimationMatrix._43,tempAnimationMatrix._44
//			};
//			continue;
//		}
//		// 값이 없을때 // nullptr일때
//		fbxV[i] = 
//		{
//			// Position
//			0,0,0,
//			// Normal
//			0,0,0,
//			// UV
//			0,0,
//
//			// AnimationMatrix
//			1,0,0,0,
//			0,1,0,0,
//			0,0,1,0,
//			0,0,0,1
//		};
//	}
//
//	//t1 = mesh->GetNumVertices();
//	//t2 = fbxModelMesh->getVertexCount();
//
//	mesh->UnlockVertexBuffer();
//}

//void FbxModelRenderer::processVerticesWithAnimation(ID3DXMesh * mesh, FbxModelMesh * fbxModelMesh)
//{
//	if (!mesh || !fbxModelMesh) return;
//
//	FbxModelVertex * fbxV = nullptr;
//	MyFbxVertex * tVertex = nullptr;
//	D3DXVECTOR3 temp{};
//
//	mesh->LockVertexBuffer(0, (void**)&fbxV);
//	for (int i = 0; i < fbxModelMesh->getVertexCount(); ++i)
//	{
//		tVertex = fbxModelMesh->getVertex(i);
//		// 값이 있을때
//		if (tVertex)
//		{
//			temp = tVertex->getPositionWithAnimation();
//			// 내부적으로 애니메이션 행렬을 곱한 것들을 처리해준다.
//			// 애니메이션 행렬을 곱하기 전에 본들의 가중치에 따른 애니메이션 행렬을 내부적으로 계산해준다.
//			//fbxV[i][0] = (float)tVertex->getPositionWithAnimation().x / 10;
//			//fbxV[i][1] = (float)tVertex->getPositionWithAnimation().y / 10;
//			//fbxV[i][2] = (float)tVertex->getPositionWithAnimation().z / 10;
//			// 각각의 정보를 초기화 시켜준다.
//			fbxV[i] =
//			{
//				// Position
//				temp.x / 10, temp.y/ 10, temp.z / 10,
//				// Normal
//				(float)tVertex->getNormal().mData[0],(float)tVertex->getNormal().mData[1], (float)tVertex->getNormal().mData[2],
//				// UV
//				(float)tVertex->getUV().mData[0],(float)tVertex->getUV().mData[1]
//			};
//			continue;
//
//			// 노말값이나 UV값은 안바꿔도 될까..
//			// UV값은 어차피 같은 텍스처 좌표이기때문에 상관없을것같은데 노말값은 달라져야하는거아닐까?
//		}
//		// 값이 없을때 // 아무것도 해주지않는다.
//	}
//
//
//	mesh->UnlockVertexBuffer();
//}

void FbxModelRenderer::processVertices(MeshForShading * mesh, FbxModelMesh * fbxModelMesh)
{

	if (!mesh || !fbxModelMesh) return;
	// mesh에 넣어줄때 fbxVertex형식으로 넣어줄것이다.
	FbxModelVertex * fbxV = nullptr;
	MyFbxVertex * tVertex = nullptr;

	//int t1 = mesh->GetNumVertices();
	//int t2 = fbxModelMesh->getVertexCount();

	// 생성한 메쉬를 잠그고 그안에 버텍스 정보를 집어넣는다.
	// 버텍스 정보는 포지션 / 노말 / UV값이다.
	mesh->vb->Lock(0,0, (void**)&fbxV,0);
	for (int i = 0; i < fbxModelMesh->getVertexCount(); ++i)
	{
		tVertex = fbxModelMesh->getVertex(i);

		//*************** 무조건 매트릭스를 곱해서 최종 위치를 만들기 전까지 스케일을 변경하지 않는다.
		//*************** 무조건 매트릭스를 곱해서 최종 위치를 만들기 전까지 스케일을 변경하지 않는다.
		//*************** 무조건 매트릭스를 곱해서 최종 위치를 만들기 전까지 스케일을 변경하지 않는다.
		// 예를들어 스케일을 1/10으로 조정하고 애니메이션 행렬을 곱하는 것과 
		// 애니메이션행렬을 곱하고 1/10으로 조정하는 것은 다르다.

		// 값이 있을때
		if (tVertex)
		{
			//D3DXVECTOR3 temp = tVertex->getPositionWithAnimation();
			 const D3DXMATRIX & tempAnimationMatrix = tVertex->getAnimationMatrix();

			// 각각의 정보를 초기화 시켜준다.
			fbxV[i] =
			{
				// Position

				(float)tVertex->getPosition().mData[0] ,(float)tVertex->getPosition().mData[1], (float)tVertex->getPosition().mData[2],
				// Normal
				(float)tVertex->getNormal().mData[0],(float)tVertex->getNormal().mData[1], (float)tVertex->getNormal().mData[2],
				// UV
				(float)tVertex->getUV().mData[0],(float)tVertex->getUV().mData[1],
				// AnimationMatrix
				tempAnimationMatrix._11,tempAnimationMatrix._12,tempAnimationMatrix._13,tempAnimationMatrix._14,
				tempAnimationMatrix._21,tempAnimationMatrix._22,tempAnimationMatrix._23,tempAnimationMatrix._24,
				tempAnimationMatrix._31,tempAnimationMatrix._32,tempAnimationMatrix._33,tempAnimationMatrix._34,
				tempAnimationMatrix._41,tempAnimationMatrix._42,tempAnimationMatrix._43,tempAnimationMatrix._44
			};
			continue;
		}
		// 값이 없을때 // nullptr일때
		fbxV[i] =
		{
			// Position
			0,0,0,
			// Normal
			0,0,0,
			// UV
			0,0,

			// AnimationMatrix
			1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			0,0,0,1
		};
	}

	mesh->vb->Unlock();
}

void FbxModelRenderer::processIndices(MeshForShading * mesh, FbxModelMesh * fbxModelMesh)
{
	if (!mesh || !fbxModelMesh) return;

	// mesh에 넣어줄때 fbxVertex형식으로 넣어줄것이다.
	unsigned long * fbxI = nullptr;
	int tIndex = -1;

	//int t1 = mesh->GetNumFaces();
	//int t2 = fbxModelMesh->getIndexCount();
	//int t3 = fbxModelMesh->getVertexCount();
	//int t4 = fbxModelMesh->getTriCount();

	// 생성한 메쉬를 잠그고 그안에 버텍스 정보를 집어넣는다.
	// 버텍스 정보는 포지션 / 노말 / UV값이다.
	mesh->ib->Lock(0,0, (void**)&fbxI,0);
	for (int i = 0; i < fbxModelMesh->getIndexCount(); ++i)
	{

		tIndex = fbxModelMesh->getIndex(i);
		// 값이 있을때
		if (tIndex != -1)
		{
			// 각각의 정보를 초기화 시켜준다.
			fbxI[i] = tIndex;
			continue;
		}
		// 값이 없을때
		fbxI[i] = 0;
	}
	mesh->ib->Unlock();
}

void FbxModelRenderer::processTextures(MeshForShading * mesh, FbxModelMesh * fbxModelMesh)
{
	if (!mesh || !fbxModelMesh) return;
	FbxNode * fbxMeshNode = fbxModelMesh->getNode();
	if (!fbxMeshNode) return;


	map<MeshForShading *, vector<D3DMATERIAL9>>::iterator materials_it = materialsTable.find(mesh);
	map<MeshForShading *, vector<IDirect3DTexture9*>>::iterator textures_it = texturesTable.find(mesh);

	// 각각 등록이 안되어있으면 등록해준다.
	if (materials_it == materialsTable.end())
	{
		materialsTable[mesh] = vector<D3DMATERIAL9>{};
	}
	if (textures_it == texturesTable.end())
	{
		texturesTable[mesh] = vector<IDirect3DTexture9*>{};
	}

	// 없으면 자동으로 삽입하는 []연산자이지만
	// 있다면 그 value값을 반환한다 앞서 없을때 모두 등록을 해준상태이므로 []로 접근해도 새롭게 할당되는 일은 없다.
	// 레퍼런스 형으로 반환하기 때문에 레퍼런스로 받으면 참조해서 접근할 수 있다.
	vector<D3DMATERIAL9> & materials = materialsTable[mesh];
	vector<IDirect3DTexture9*> & textures = texturesTable[mesh];


	int materialCount = fbxMeshNode->GetSrcObjectCount<FbxSurfaceMaterial>();

	for (int index = 0; index < materialCount; index++)
	{
		FbxSurfaceMaterial* material = (FbxSurfaceMaterial*)fbxMeshNode->GetSrcObject<FbxSurfaceMaterial>(index);

		if (material != NULL)
		{
			//LOG(INFO) << "\nmaterial: " << material->GetNameOnly() << std::endl;
			string tempMatrial = material->GetNameOnly();
			// This only gets the material of type sDiffuse, you probably need to traverse all Standard Material Property by its name to get all possible textures.

			// material 초기화
			FbxProperty prop{};
			D3DXMATERIAL tempMtrl{};
			FbxDouble3 result(0, 0, 0);

			prop = material->FindProperty(FbxSurfaceMaterial::sAmbient);
			result = prop.Get<FbxDouble3>();
			D3DCOLORVALUE ambient = { result.mData[0],result.mData[1],result.mData[2] };

			prop = material->FindProperty(FbxSurfaceMaterial::sSpecular);
			result = prop.Get<FbxDouble3>();
			D3DCOLORVALUE specular = { result.mData[0],result.mData[1],result.mData[2] };

			// 텍스처는 디퓨즈만 사용할것이기 때문에 마지막으로 처리해주는 부분은 디퓨즈이다.
			// 이 prop을 이용해서 텍스처를 받아올것이다.
			prop = material->FindProperty(FbxSurfaceMaterial::sDiffuse);
			result = prop.Get<FbxDouble3>();
			D3DCOLORVALUE diffuse = { result.mData[0],result.mData[1],result.mData[2] };

			tempMtrl.MatD3D.Ambient = ambient;
			tempMtrl.MatD3D.Diffuse = diffuse;
			tempMtrl.MatD3D.Specular = specular;

			materials.push_back(tempMtrl.MatD3D);

			// Check if it's layeredtextures
			int layeredTextureCount = prop.GetSrcObjectCount<FbxLayeredTexture>();

			if (layeredTextureCount > 0)
			{
				for (int j = 0; j < layeredTextureCount; j++)
				{
					FbxLayeredTexture* layered_texture = FbxCast<FbxLayeredTexture>(prop.GetSrcObject<FbxLayeredTexture>(j));
					int lcount = layered_texture->GetSrcObjectCount<FbxTexture>();


					for (int k = 0; k < lcount; k++)
					{
						FbxFileTexture* texture = FbxCast<FbxFileTexture>(layered_texture->GetSrcObject<FbxFileTexture>(k));
						// Then, you can get all the properties of the texture, include its name
						string textureName = texture->GetFileName();

						// 처음에는 바로 파일 경로를 받아서 열어보고

						IDirect3DTexture9* tex = nullptr;
						D3DXCreateTextureFromFile(
							device,
							textureName.c_str(),
							&tex);

						// 텍스처가 있을때 그 텍스처를 넣어주고
						if (tex)
						{
							textures.push_back(tex);
							continue;
						}


						// 파일 경로로 접근했을때 없는 경우
						// 이미 지정해둔 경로를 뒤져서 확인해본다.

						// 먼저 파일이름을 받아오고 > 그다음 지정해둔 경로를 더하는 작업을 해준다.
						int index = textureName.find_last_of('\\');
						textureName = textureName.substr(index + 1);
						//textureName = texture->GetRelativeFileName();
						textureName = filePathToLoadFbxTextureFiles + textureName;

						D3DXCreateTextureFromFile(
							device,
							textureName.c_str(),
							&tex);

						textures.push_back(tex);

					}
				}
			}
			else
			{
				int textureCount = prop.GetSrcObjectCount<FbxFileTexture>();
				for (int j = 0; j < textureCount; j++)
				{
					FbxFileTexture* texture = FbxCast<FbxFileTexture>(prop.GetSrcObject<FbxFileTexture>(j));

					string textureName = texture->GetFileName();

					//FbxProperty p = texture->RootProperty.Find(/*"Filename"*/textureName.c_str());
					// 처음에는 바로 파일 경로를 받아서 열어보고

					IDirect3DTexture9* tex = nullptr;
					D3DXCreateTextureFromFile(
						device,
						textureName.c_str(),
						&tex);

					// 텍스처가 있을때 그 텍스처를 넣어주고
					if (tex)
					{
						textures.push_back(tex);
						continue;
					}


					// 파일 경로로 접근했을때 없는 경우
					// 이미 지정해둔 경로를 뒤져서 확인해본다.

					// 먼저 파일이름을 받아오고 > 그다음 지정해둔 경로를 더하는 작업을 해준다.
					int index = textureName.find_last_of("/\\");
					textureName = textureName.substr(index + 1);
					//textureName = texture->GetRelativeFileName();
					textureName = filePathToLoadFbxTextureFiles + textureName;

					Trace::Write("TAG_DEBUG", textureName);

					D3DXCreateTextureFromFile(
						device,
						textureName.c_str(),
						&tex);

					textures.push_back(tex);

				}
			}
		}

	}

}


//void FbxModelRenderer::processIndices(ID3DXMesh * mesh, FbxModelMesh * fbxModelMesh)
//{
//	if (!mesh || !fbxModelMesh) return;
//
//	// mesh에 넣어줄때 fbxVertex형식으로 넣어줄것이다.
//	unsigned short * fbxI = nullptr;
//	int tIndex = -1;
//
//	//int t1 = mesh->GetNumFaces();
//	//int t2 = fbxModelMesh->getIndexCount();
//	//int t3 = fbxModelMesh->getVertexCount();
//	//int t4 = fbxModelMesh->getTriCount();
//
//	// 생성한 메쉬를 잠그고 그안에 버텍스 정보를 집어넣는다.
//	// 버텍스 정보는 포지션 / 노말 / UV값이다.
//	mesh->LockIndexBuffer(0, (void**)&fbxI);
//	for (int i = 0; i < fbxModelMesh->getIndexCount(); ++i)
//	{
//
//		tIndex = fbxModelMesh->getIndex(i);
//		// 값이 있을때
//		if (tIndex != -1)
//		{
//			// 각각의 정보를 초기화 시켜준다.
//			fbxI[i] = tIndex;
//			continue;
//		}
//		// 값이 없을때
//		fbxI[i] = 0;
//	}
//	mesh->UnlockIndexBuffer();
//}

//if (mtrlBuffer == 0 || numMtrls == 0) return;
//
//// D3DXMATERAL 형식으로 읽으려면 타입캐스팅이 필요하다.
//D3DXMATERIAL* pMtrls = (D3DXMATERIAL*)mtrlBuffer->GetBufferPointer();
//
//for (int i = 0; i < numMtrls; i++)
//{
//	// 로드될때 ambient값을 가지지 않으므로 이를 지정한다.
//	pMtrls[i].MatD3D.Ambient = pMtrls[i].MatD3D.Diffuse;
//
//	mtrls.push_back(pMtrls[i].MatD3D);
//
//	if (pMtrls[i].pTextureFilename != 0)
//	{
//		// 파일이름이 정상적으로 들어가 있을때만 그 파일명으로 파일을 열어서 텍스처를 로드한다.
//		IDirect3DTexture9* tex = 0;
//		D3DXCreateTextureFromFile(
//			device,
//			pMtrls[i].pTextureFilename,
//			&tex);
//
//		// 텍스처가 있을때 그 텍스처를 넣어주고
//		textures.push_back(tex);
//	}
//	else
//	{
//		// 없을때도 서브셋과 같은 인덱스 번호를 맞춰주기 위해서 널값을 넣어준다.
//		textures.push_back(0);
//	}
//}

//void FbxModelRenderer::processTextures(ID3DXMesh * mesh, FbxModelMesh * fbxModelMesh)
//{
//	if (!mesh || !fbxModelMesh) return;
//	FbxNode * fbxMeshNode = fbxModelMesh->getNode();
//	if (!fbxMeshNode) return;
//
//
//	map<ID3DXMesh *, vector<D3DMATERIAL9>>::iterator materials_it = materialsTable.find(mesh);
//	map<ID3DXMesh *, vector<IDirect3DTexture9*>>::iterator textures_it = texturesTable.find(mesh);
//
//	// 각각 등록이 안되어있으면 등록해준다.
//	if (materials_it == materialsTable.end())
//	{
//		materialsTable[mesh] = vector<D3DMATERIAL9>{};
//	}
//	if (textures_it == texturesTable.end())
//	{
//		texturesTable[mesh] = vector<IDirect3DTexture9*>{};
//	}
//
//	// 없으면 자동으로 삽입하는 []연산자이지만
//	// 있다면 그 value값을 반환한다 앞서 없을때 모두 등록을 해준상태이므로 []로 접근해도 새롭게 할당되는 일은 없다.
//	// 레퍼런스 형으로 반환하기 때문에 레퍼런스로 받으면 참조해서 접근할 수 있다.
//	vector<D3DMATERIAL9> & materials = materialsTable[mesh];
//	vector<IDirect3DTexture9*> & textures = texturesTable[mesh];
//
//	//	if (pMtrls[i].pTextureFilename != 0)
//	//	{
//	//		// 파일이름이 정상적으로 들어가 있을때만 그 파일명으로 파일을 열어서 텍스처를 로드한다.
//	//		IDirect3DTexture9* tex = 0;
//	//		D3DXCreateTextureFromFile(
//	//			device,
//	//			pMtrls[i].pTextureFilename,
//	//			&tex);
//	//
//	//		// 텍스처가 있을때 그 텍스처를 넣어주고
//	//		textures.push_back(tex);
//	//	}
//	//	else
//	//	{
//	//		// 없을때도 서브셋과 같은 인덱스 번호를 맞춰주기 위해서 널값을 넣어준다.
//	//		textures.push_back(0);
//	//	}
//
//	int materialCount = fbxMeshNode->GetSrcObjectCount<FbxSurfaceMaterial>();
//
//	for (int index = 0; index < materialCount; index++)
//	{
//		FbxSurfaceMaterial* material = (FbxSurfaceMaterial*)fbxMeshNode->GetSrcObject<FbxSurfaceMaterial>(index);
//
//		if (material != NULL)
//		{
//			//LOG(INFO) << "\nmaterial: " << material->GetNameOnly() << std::endl;
//			string tempMatrial = material->GetNameOnly();
//			// This only gets the material of type sDiffuse, you probably need to traverse all Standard Material Property by its name to get all possible textures.
//
//			// material 초기화
//			FbxProperty prop{};
//			D3DXMATERIAL tempMtrl{};
//			FbxDouble3 result(0, 0, 0);
//
//			prop = material->FindProperty(FbxSurfaceMaterial::sAmbient);
//			result = prop.Get<FbxDouble3>();
//			D3DCOLORVALUE ambient = { result.mData[0],result.mData[1],result.mData[2] };
//
//			prop = material->FindProperty(FbxSurfaceMaterial::sSpecular);
//			result = prop.Get<FbxDouble3>();
//			D3DCOLORVALUE specular = { result.mData[0],result.mData[1],result.mData[2] };
//
//			// 텍스처는 디퓨즈만 사용할것이기 때문에 마지막으로 처리해주는 부분은 디퓨즈이다.
//			// 이 prop을 이용해서 텍스처를 받아올것이다.
//			prop = material->FindProperty(FbxSurfaceMaterial::sDiffuse);
//			result = prop.Get<FbxDouble3>();
//			D3DCOLORVALUE diffuse = { result.mData[0],result.mData[1],result.mData[2] };
//
//			tempMtrl.MatD3D.Ambient = ambient;
//			tempMtrl.MatD3D.Diffuse = diffuse;
//			tempMtrl.MatD3D.Specular = specular;
//
//			materials.push_back(tempMtrl.MatD3D);
//	
//			// Check if it's layeredtextures
//			int layeredTextureCount = prop.GetSrcObjectCount<FbxLayeredTexture>();
//
//			if (layeredTextureCount > 0)
//			{
//				for (int j = 0; j < layeredTextureCount; j++)
//				{
//					FbxLayeredTexture* layered_texture = FbxCast<FbxLayeredTexture>(prop.GetSrcObject<FbxLayeredTexture>(j));
//					int lcount = layered_texture->GetSrcObjectCount<FbxTexture>();
//
//					
//					for (int k = 0; k < lcount; k++)
//					{
//						FbxFileTexture* texture = FbxCast<FbxFileTexture>(layered_texture->GetSrcObject<FbxFileTexture>(k));
//						// Then, you can get all the properties of the texture, include its name
//						string textureName = texture->GetFileName();
//
//						// 처음에는 바로 파일 경로를 받아서 열어보고
//
//						IDirect3DTexture9* tex = nullptr;
//						D3DXCreateTextureFromFile(
//							device,
//							textureName.c_str(),
//							&tex);
//
//						// 텍스처가 있을때 그 텍스처를 넣어주고
//						if (tex)
//						{
//							textures.push_back(tex);
//							continue;
//						}
//
//
//						// 파일 경로로 접근했을때 없는 경우
//						// 이미 지정해둔 경로를 뒤져서 확인해본다.
//
//						// 먼저 파일이름을 받아오고 > 그다음 지정해둔 경로를 더하는 작업을 해준다.
//						int index = textureName.find_last_of('\\');
//						textureName = textureName.substr(index + 1);
//						//textureName = texture->GetRelativeFileName();
//						textureName = filePathToLoadFbxTextureFiles + textureName;
//
//						D3DXCreateTextureFromFile(
//							device,
//							textureName.c_str(),
//							&tex);
//
//						textures.push_back(tex);
//
//					}
//				}
//			}
//			else
//			{
//				int textureCount = prop.GetSrcObjectCount<FbxFileTexture>();
//				for (int j = 0; j < textureCount; j++)
//				{
//					FbxFileTexture* texture = FbxCast<FbxFileTexture>(prop.GetSrcObject<FbxFileTexture>(j));
//		
//					string textureName = texture->GetFileName();
//
//					//FbxProperty p = texture->RootProperty.Find(/*"Filename"*/textureName.c_str());
//					// 처음에는 바로 파일 경로를 받아서 열어보고
//
//					IDirect3DTexture9* tex = nullptr;
//					D3DXCreateTextureFromFile(
//						device,
//						textureName.c_str(),
//						&tex);
//
//					// 텍스처가 있을때 그 텍스처를 넣어주고
//					if (tex)
//					{
//						textures.push_back(tex);
//						continue;
//					}
//
//
//					// 파일 경로로 접근했을때 없는 경우
//					// 이미 지정해둔 경로를 뒤져서 확인해본다.
//
//					// 먼저 파일이름을 받아오고 > 그다음 지정해둔 경로를 더하는 작업을 해준다.
//					int index = textureName.find_last_of('\\');
//					textureName = textureName.substr(index + 1);
//					//textureName = texture->GetRelativeFileName();
//					textureName = filePathToLoadFbxTextureFiles + textureName;
//
//					D3DXCreateTextureFromFile(
//						device,
//						textureName.c_str(),
//						&tex);
//
//					textures.push_back(tex);
//
//				}
//			}
//		}
//
//	}
//}

//void FbxModelRenderer::processSubsets(ID3DXMesh * mesh)
//{
//	if (!mesh ) return;
//
//	unsigned long* attributeBuffer = nullptr;
//
//	// 다시 속성 데이터를 수정하기 위해서 버퍼를 잠궈준다.
//	mesh->LockAttributeBuffer(0, &attributeBuffer);
//
//	int ttt = mesh->GetNumFaces();
//
//	// 서브셋을 0번으로 지정해준다.
//	for (int i = 0; i < mesh->GetNumFaces(); ++i)
//		attributeBuffer[i] = 0;
//
//
//	// 설정이 완료되었으므로 다시 잠궈준다.
//	mesh->UnlockAttributeBuffer();
//}

//void FbxModelRenderer::optimizeMesh(ID3DXMesh * mesh)
//{
//	if (!mesh) return;
//
//	// 인접버퍼를 이용한 최적화 작업
//	vector<unsigned long> adjacencyBuffer(mesh->GetNumFaces() * 3);
//	// 메쉬의 인접정보를 받아온다.
//	mesh->GenerateAdjacency(0.0f, &adjacencyBuffer[0]);
//
//
//	int faceCount1 = mesh->GetNumFaces();
//	int vertexCount1 = mesh->GetNumVertices();
//
//	HRESULT hr = 0;
//	// 그 인접 정보를 기반으로 최적화 작업을 시작한다.
//	hr = mesh->OptimizeInplace
//	(
//		// 속성으로(서브셋기준) 삼각형으로 정렬하고, 별도의 속성테이블을 생성
//		// GetAttributeTable함수를 이용해서 D3DXATTRIBUTERANGE구조체 배열을 받아올 수 있다.
//		// 내부 정보에는 서브셋ID / 각 면과 버텍스의 수 / 각 시작 지점이 들어있다.
//		// 속성 테이블의 각항목은 메쉬의 각 서브셋과 대응되며, 
//		// 서브셋의 기하정보들이 보관되는 버텍스/ 인덱스 버퍼내의 메모리 블록을 지정한다.
//		D3DXMESHOPT_ATTRSORT |
//		// 메쉬에서 이용하지 않는 인덱스와 버텍스를 제거한다.
//		D3DXMESHOPT_COMPACT |
//		// 버텍스 캐시의 히트율을 높인다.
//		D3DXMESHOPT_VERTEXCACHE,
//
//		// 최적화 되지않은 메쉬의 인접 배열 포인터
//		// 인접배열이 필요한이유?	// 최적화 목록에 인접정보가 필요한 부분이 있어서 필요할듯
//		&adjacencyBuffer[0],
//		// 최적화된 메쉬의 인접 배열 포인터
//		nullptr,
//		// 리맵정보 Face
//		nullptr,
//		// 리맵정보 Vertex
//		nullptr
//	);
//
//	int faceCount = mesh->GetNumFaces();
//	int vertexCount = mesh->GetNumVertices();
//}

void AnimationFSM::start()
{
	// 현재 컴포넌트가 있을때 초기화 된다.
	fbxModelRenderer = gameObject->getComponent<FbxModelRenderer>();

	// FbxModelRenderer의 컴포넌트가 있는 경우 애니메이션 매너저를 받아온다.
	if (fbxModelRenderer)
	{
		fbxModelAnimations = fbxModelRenderer->getAnimations();
	}

}

void AnimationFSM::update()
{
}

bool AnimationFSM::Transition::evaluate()
{
	if (!animationFSM) return false;

	// 각각의 애니메이션이 존재하는지 테이블을 통해서 확인
	if (animationFSM->stateTable.find(from) == animationFSM->stateTable.end()
		|| animationFSM->stateTable.find(to) == animationFSM->stateTable.end())
		return false;


	float value = 0.0f;
	float valueToCompare = 0.0f;
	bool result = false;

	// 타입에 따라 값을 받아온다.
	if (type == ValueType::BOOLTYPE)
	{
		// 비교할 주체가 되는 대상이 테이블에 등록되어있는지 확인해보고 값을 받아온다.
		auto it = animationFSM->boolTable.find(valueName);
		if (it == animationFSM->boolTable.end()) return false;

		value = it->second;
		valueToCompare = (float)bValueToCompare;
	}
	else //if (type == ValueType::FLOATTYPE)
	{
		// 비교할 주체가 되는 대상이 테이블에 등록되어있는지 확인해보고 값을 받아온다.
		auto it = animationFSM->floatTable.find(valueName);
		if (it == animationFSM->floatTable.end()) return false;

		value = it->second;
		valueToCompare = fValueToCompare;
	}

	// factor에 의해 비교연산자 결정
	if (factor == 0)
	{
		return value == valueToCompare;
	}
	else if (factor <0)
	{
		return value < valueToCompare;
	}
	else //if (factor > 0)
	{
		return value > valueToCompare;
	}

}

void AnimationFSM::onDestroy()
{

}

AnimationFSM::~AnimationFSM()
{
	for (auto it = transitionTable.begin(); it != transitionTable.end(); ++it)
	{

		if ((it->second.second) == nullptr) continue;

		delete (it->second.second);
		it->second.second = nullptr;
	}
	transitionTable.clear();
}

void AnimationFSM::registerAnimation(const string & animationFileName)
{
	if (!fbxModelRenderer || !fbxModelAnimations) return;

	fbxModelRenderer->loadFbxFileForAnimation(animationFileName);

	// 만약에 등록이 안되어있다면 stateTable에 등록한다.
	map<string, unsigned int>::iterator stateIt = stateTable.find(animationFileName);

	if (stateIt == stateTable.end())
	{
		// 인덱스는 등록한 순서대로 차례대로 해준다.
		stateTable[animationFileName] = stateTable.size();
	}
}

void AnimationFSM::setDefaultState(const string & animationFileName)
{
	if (!fbxModelRenderer || !fbxModelAnimations) return;

	map<string, unsigned int>::iterator stateIt = stateTable.find(animationFileName);

	// 만약에 등록이 안되어 있다면 리턴한다.
	if (stateIt == stateTable.end())
	{
		defaultState = "";
		return;
	}
	
	// 만약에 등록된 상황이라면 디폴트 상태를 정의해준다.
	defaultState = animationFileName;
	currentState = defaultState;
}

void AnimationFSM::registerFloat(const string & floatName)
{
	map<string, float>::iterator floatIt = floatTable.find(floatName);
	// 이미등록되어 있다면 리턴
	if (floatIt != floatTable.end())
		return;

	floatTable[floatName] = 0.0f;
}

void AnimationFSM::registerBool(const string & boolName)
{
	map<string, bool>::iterator boolIt = boolTable.find(boolName);
	// 이미등록되어 있다면 리턴
	if (boolIt != boolTable.end())
		return;

	boolTable[boolName] = false;
}

void AnimationFSM::setFloat(const string & floatName, float input)
{
	map<string, float>::iterator floatIt = floatTable.find(floatName);
	// 이미등록되어 있지 않다면 새롭게 등록해주고 input값으로 초기화 시켜준다.
	if (floatIt == floatTable.end())
	{
		floatTable[floatName] = input;
		return;
	}

	floatIt->second = input;
}

bool AnimationFSM::getFloat(float & output, const string & floatName)
{
	map<string, float>::iterator floatIt = floatTable.find(floatName);
	if (floatIt == floatTable.end())
		return false;

	output = floatIt->second;
	return true;
}

void AnimationFSM::setBool(const string & boolName, bool input)
{
	map<string, bool>::iterator boolIt = boolTable.find(boolName);
	// 이미등록되어 있지 않다면 새롭게 등록해주고 input값으로 초기화 시켜준다.
	if (boolIt == boolTable.end())
	{
		boolTable[boolName] = input;
		return;
	}
	
	boolIt->second = input;
}

bool AnimationFSM::getBool(bool & output, const string & boolName)
{
	map<string, bool>::iterator boolIt = boolTable.find(boolName);
	if (boolIt == boolTable.end())
		return false;

	output = boolIt->second;
	return true;
}

void AnimationFSM::makeTransition(const string & from, const string & to, const string & valueName, int factor, ValueType type, float valueToCompare)
{
	// 중복된 키값을 가질 수 있기 때문에 그냥 등록한다. 찾아보고 나온것들에 대해서 
	// 밸류값이 같은지 검사하고
	// 같다면 리턴하고 아니면 등록한다.
	// 원래는 밸류값이 같은지 확인하려고 했는데 밸류의 second값이 포인터라서 내부 파라미터들을 다 비교해야한다.
	pair <multimap<string, pair<string, Transition *>>::iterator, multimap<string, pair<string, Transition *>>::iterator> result;
	result = transitionTable.equal_range(from);

	Transition * transition = new Transition(this, from, to, valueName, factor, type, valueToCompare);

	for (multimap<string, pair<string, Transition *>>::iterator it = result.first; it != result.second; ++it)
	{
		if ((*transition) == (*(it->second.second)))
		{
			// 이미 동일한 transition이 등록되어 있으면 리턴해준다.
			delete transition;
			return;
		}
	}

	// 전이에 들어가는 애니메이션들이 등록되어 있지 않다면 리턴
	if (stateTable.find(from) == stateTable.end() || stateTable.find(to) == stateTable.end())
	{
		delete transition;
		return;
	}

	// 전이 객체가 등록되어야 할상황
	transitionTable.insert(pair<string, pair<string, Transition *>>({ from }, { to, transition }));
}

void AnimationFSM::updateAllTrasitions(const string & animationFileName)
{
	pair <multimap<string, pair<string, Transition *>>::iterator, multimap<string, pair<string, Transition *>>::iterator> result;
	result = transitionTable.equal_range(animationFileName);

	// 모든 전이상황들을 돌면서 확인해보고 
	// 만약 평가했을떄 true가 나오면 그 애니메이션을 현재애니메이션으로 설정한다.
	for (multimap<string, pair<string, Transition *>>::iterator it = result.first; it != result.second; ++it)
	{
		if (it->second.second->evaluate())
		{
			currentState = it->second.first;
			return;
		}
	}
}

void PlayerAnimationFSM::start()
{
	AnimationFSM::start();

	if (!fbxModelRenderer || !fbxModelAnimations) return;

	registerAnimation("standing run forward.fbx");
	registerAnimation("standing run back.fbx");
	registerAnimation("standing run right.fbx");
	registerAnimation("standing run left.fbx");
	registerAnimation("standing idle 01.fbx");
	registerAnimation("Standing Draw Arrow.fbx");
	setDefaultState("standing idle 01.fbx");

	// 플레이어 기준

	/*
	전이될 상황 등록
	*/
	string floatSpeed = "speed";
	string floatSideSpeed = "sideSpeed";
	// 변수 등록
	registerFloat("speed");
	setFloat("speed", 1.0f);

	registerFloat("sideSpeed");
	setFloat("sideSpeed", 1.0f);

	registerBool("isIdle");
	setBool("isIdle", true);

	registerBool("isShooting");
	setBool("isShooting", false);

	// forward > back / left / right / idle
	// float형으로 등록된 "speed"가 0보다 작을때 forward > back
	makeTransition("standing run forward.fbx", "standing run back.fbx", "speed", -1, AnimationFSM::ValueType::FLOATTYPE, 0);
	// float형으로 등록된 "sideSpeed"가 0보다 작을때 forward > left
	makeTransition("standing run forward.fbx", "standing run left.fbx", "sideSpeed", 1, AnimationFSM::ValueType::FLOATTYPE, 0);
	// float형으로 등록된 "sideSpeed"가 0보다 클때 forward > right
	makeTransition("standing run forward.fbx", "standing run right.fbx", "sideSpeed", -1, AnimationFSM::ValueType::FLOATTYPE, 0);
	// bool형으로 등록된 "isIdle"이 true 일때
	makeTransition("standing run forward.fbx", "standing idle 01.fbx", "isIdle", 0, AnimationFSM::ValueType::BOOLTYPE, true);

	// back > forward / left / right / idle
	// float형으로 등록된 "speed"가 0보다 클때 back > forward
	makeTransition("standing run back.fbx", "standing run forward.fbx", "speed", 1, AnimationFSM::ValueType::FLOATTYPE, 0);
	// float형으로 등록된 "sideSpeed"가 0보다 작을때 back > left
	makeTransition("standing run back.fbx", "standing run left.fbx", "sideSpeed", 1, AnimationFSM::ValueType::FLOATTYPE, 0);
	// float형으로 등록된 "speed"가 0보다 클때 back > right
	makeTransition("standing run back.fbx", "standing run right.fbx", "sideSpeed", -1, AnimationFSM::ValueType::FLOATTYPE, 0);
	// bool형으로 등록된 "isIdle"이 true 일때
	makeTransition("standing run back.fbx", "standing idle 01.fbx", "isIdle", 0, AnimationFSM::ValueType::BOOLTYPE, true);

	// left > forward / back / right / idle
	// float형으로 등록된 "speed"가 0보다 작을때 left > forward
	makeTransition("standing run left.fbx", "standing run forward.fbx", "speed", 1, AnimationFSM::ValueType::FLOATTYPE, 0);
	// float형으로 등록된 "speed"가 0보다 클때 left > back
	makeTransition("standing run left.fbx", "standing run back.fbx", "speed", -1, AnimationFSM::ValueType::FLOATTYPE, 0);
	// float형으로 등록된 "sideSpeed"가 0보다 클때 left > right
	makeTransition("standing run left.fbx", "standing run right.fbx", "sideSpeed", -1, AnimationFSM::ValueType::FLOATTYPE, 0);
	// bool형으로 등록된 "isIdle"이 true 일때
	makeTransition("standing run left.fbx", "standing idle 01.fbx", "isIdle", 0, AnimationFSM::ValueType::BOOLTYPE, true);


	// right > forward / back / left / idle
	// float형으로 등록된 "sideSpeed"가 0보다 작을때 right > left
	makeTransition("standing run right.fbx", "standing run left.fbx", "sideSpeed", 1, AnimationFSM::ValueType::FLOATTYPE, 0);
	// float형으로 등록된 "sideSpeed"가 0보다 클때 right > forward
	makeTransition("standing run right.fbx", "standing run forward.fbx", "speed", 1, AnimationFSM::ValueType::FLOATTYPE, 0);
	// float형으로 등록된 "speed"가 0보다 작을때 right > back
	makeTransition("standing run right.fbx", "standing run back.fbx", "speed", -1, AnimationFSM::ValueType::FLOATTYPE, 0);
	// bool형으로 등록된 "isIdle"이 true 일때
	makeTransition("standing run right.fbx", "standing idle 01.fbx", "isIdle", 0, AnimationFSM::ValueType::BOOLTYPE, true);

	// idle > forward / back / left / right / arrow
	// float형으로 등록된 "sideSpeed"가 0보다 작을때 idle > left
	makeTransition("standing idle 01.fbx", "standing run left.fbx", "sideSpeed", 1, AnimationFSM::ValueType::FLOATTYPE, 0);
	// float형으로 등록된 "sideSpeed"가 0보다 클때 left > right
	makeTransition("standing idle 01.fbx", "standing run right.fbx", "sideSpeed", -1, AnimationFSM::ValueType::FLOATTYPE, 0);
	// float형으로 등록된 "sideSpeed"가 0보다 클때 idle > forward
	makeTransition("standing idle 01.fbx", "standing run forward.fbx", "speed", 1, AnimationFSM::ValueType::FLOATTYPE, 0);
	// float형으로 등록된 "speed"가 0보다 작을때 idle > back
	makeTransition("standing idle 01.fbx", "standing run back.fbx", "speed", -1, AnimationFSM::ValueType::FLOATTYPE, 0);
	// bool형으로 등록된 "isShooting"가 0보다 작을때 idle > arrow
	makeTransition("standing idle 01.fbx", "Standing Draw Arrow.fbx", "isShooting", 0, AnimationFSM::ValueType::BOOLTYPE, true);

	// arrow > idel
	// float형으로 등록된 "speed"가 0보다 작을때 arrow > idle
	makeTransition("Standing Draw Arrow.fbx", "standing idle 01.fbx", "isShooting", 0, AnimationFSM::ValueType::BOOLTYPE, false);

}

void PlayerAnimationFSM::update()
{
	if (!fbxModelAnimations) return;

	auto it = stateTable.find(currentState);
	// 만약에 현재 상태를 테이블에서 찾지 못한다면 디폴트로 해준다.
	if (it == stateTable.end())
		currentState = defaultState;

	if (InputManager::GetKeyStay(0x57))
	{
		setFloat("speed", 1);
		setFloat("sideSpeed", 0);
		setBool("isIdle", false);
	}
	else if (InputManager::GetKeyStay(0x53))
	{
		setFloat("speed", -1);
		setFloat("sideSpeed", 0);
		setBool("isIdle", false);
	}
	else if (InputManager::GetKeyStay(0x44))
	{
		setFloat("sideSpeed", 1);
		setFloat("speed", 0);
		setBool("isIdle", false);
	}
	else if (InputManager::GetKeyStay(0x41))
	{
		setFloat("sideSpeed", -1);
		setFloat("speed", 0);
		setBool("isIdle", false);
	}
	else
	{
		setFloat("sideSpeed", 0);
		setFloat("speed", 0);
		setBool("isIdle", true);
	}


	if (InputManager::GetKeyDown(0x31))
	{
		setBool("isShooting", true);
	}
	else if (InputManager::GetKeyUp(0x31))
	{
		setBool("isShooting", false);
	}


	// 애니메이션을 플레이해주고 // 현재플레이중이면 계속플레이
	fbxModelAnimations->playWithFileName(currentState);

	updateAllTrasitions(currentState);
}

void BasicEnemyAnimationFSM::start()
{
	AnimationFSM::start();

	if (!fbxModelRenderer || !fbxModelAnimations) return;

	registerAnimation("Mutant Idle.fbx");
	registerAnimation("Mutant Swiping.fbx");
	registerAnimation("Mutant Run.fbx");
	setDefaultState("Mutant Idle.fbx");

	// 플레이어 기준

	/*
	전이될 상황 등록
	*/

	// 변수 등록
	registerBool("isIdle");
	setBool("isIdle", true);

	registerBool("isRunning");
	setBool("isRunning", false);

	registerBool("isAttacking");
	setBool("isAttacking", false);

	// idle > run / attack
	makeTransition("Mutant Idle.fbx", "Mutant Run.fbx", "isRunning", 0, AnimationFSM::ValueType::BOOLTYPE, true);
	makeTransition("Mutant Idle.fbx", "Mutant Swiping.fbx", "isAttacking", 0, AnimationFSM::ValueType::BOOLTYPE, true);

	// run > idle / attack
	makeTransition("Mutant Run.fbx", "Mutant Idle.fbx", "isIdle", 0, AnimationFSM::ValueType::BOOLTYPE, true);
	makeTransition("Mutant Run.fbx", "Mutant Swiping.fbx", "isAttacking", 0, AnimationFSM::ValueType::BOOLTYPE, true);

	// attack > idle / run
	makeTransition("Mutant Swiping.fbx", "Mutant Run.fbx", "isRunning", 0, AnimationFSM::ValueType::BOOLTYPE, true);
	makeTransition("Mutant Swiping.fbx", "Mutant Idle.fbx", "isIdle", 0, AnimationFSM::ValueType::BOOLTYPE, true);


}

void BasicEnemyAnimationFSM::update()
{
	if (!fbxModelAnimations) return;

	auto it = stateTable.find(currentState);
	// 만약에 현재 상태를 테이블에서 찾지 못한다면 디폴트로 해준다.
	if (it == stateTable.end())
		currentState = defaultState;

	// 애니메이션을 플레이해주고 // 현재플레이중이면 계속플레이
	fbxModelAnimations->playWithFileName(currentState);

	updateAllTrasitions(currentState);

}

void BulletScript::start()
{
	device = &(gameObject->getDevice());

	D3DXCreateSphere(device, 1.0f, 10, 10, &bulletMesh, 0);

	//HRESULT hr = 0;

	//// 파일을 로드시켜서 텍스처를 불러오는 방식 
	//hr = D3DXCreateTextureFromFile
	//(
	//	device,
	//	"../Fbx/Textures/fire4.jpg",
	//	&texture
	//);

	//if (FAILED(hr))
	//	texture = nullptr;

	// material 기본 설정
	// Set the RGBA for diffuse reflection.
	mtrl.Diffuse.r = 1.0f;
	mtrl.Diffuse.g = 0.0f;
	mtrl.Diffuse.b = 0.0f;
	mtrl.Diffuse.a = 1.0f;

	// Set the RGBA for ambient reflection.
	mtrl.Ambient.r = 1.0f;
	mtrl.Ambient.g = 0.0f;
	mtrl.Ambient.b = 0.0f;
	mtrl.Ambient.a = 1.0f;

	// Set the color and sharpness of specular highlights.
	mtrl.Specular.r = 100.0f;
	mtrl.Specular.g = .0f;
	mtrl.Specular.b = .0f;
	mtrl.Specular.a = 1.0f;
	mtrl.Power = 2000.0f;

	// Set the RGBA for emissive color.
	mtrl.Emissive.r = 1.0f;
	mtrl.Emissive.g = 0.0f;
	mtrl.Emissive.b = 0.0f;
	mtrl.Emissive.a = 1.0f;
}

void BulletScript::update()
{
	if (!bulletMesh) return;

	// 현재 transform행렬로 적용시킨다.
	device->SetTransform(D3DTS_WORLD, &transform->getTransformMatrix());

	//if (texture)
	//	device->SetTexture(0, texture);

	device->SetMaterial(&mtrl);

	bulletMesh->DrawSubset(0);
}

void BulletScript::onCollisionStay(GameObjectWithCollision & other)
{
	if (other.gameObject->getTag() == "Enemy" || other.gameObject->getTag() == "EnemyBoss")
	{
		// 몬스터에 따라 사운드 출력
		if (other.gameObject->getTag() == "Enemy")
		{
			gameObject->getAudio().playEffectSound("BS_Effect_MonsterDamaged.mp3", true);
		}
		else // if(other.gameObject->getTag() == "EnemyBoss")
		{
			gameObject->getAudio().playEffectSound("BS_Effect_BossMonsterDamaged.mp3", true);
		}


		// 데미지를 입힌다.
		DamageableScript * damageableScript = other.gameObject->getComponent<DamageableScript>();
		if (damageableScript)
			damageableScript->getDamaged(power);
		
		GameObject * bulletParticle = GameObject::Instantiate("bulletParticle", "BulletParticle");
		BulletParticle * particleSystem = bulletParticle->addComponent<BulletParticle>();

		D3DXVECTOR3 origin{};
		Vector3::ToD3DXVECTOR3(origin, transform->getWorldPosition());
		bulletParticle->getTransform()->setWorldPosition(transform->getWorldPosition());
		particleSystem->generateParticle(&gameObject->getDevice(), "../Fbx/Textures/flare.bmp", 2000, .9f, origin);
		GameObject::Destroy(gameObject);
	}
}

void BulletParticle::update()
{
	if (!fireExplosion)
	{
		GameObject::Destroy(this->gameObject);
		return;
	}

	if (lagTime >= fireExplosion->getDuration())
	{
		GameObject::Destroy(this->gameObject);
		return;
	}

	fireExplosion->update((float)(FrameTime::GetDeltaTime())/1000.0f);
	// 여기서는 따로 월드 지정을 안해줘도 쉐이더에서 알아서 포지션에 맞게 렌더링하기 때문에 괜찮다.
	// 그 이유는 처음 시작할때 시작점을 잘 지정했기 때문이다.
	fireExplosion->render();
	lagTime += (float)(FrameTime::GetDeltaTime()) / 1000.0f;
}

void BulletParticle::onDestroy()
{
	if(fireExplosion)
		delete fireExplosion;
}

void BulletParticle::generateParticle(IDirect3DDevice9 * device, const char * textureFileName, int numOfParticles, int particleSize, const D3DXVECTOR3 & origin)
{
	fireExplosion = new FireExplosion(device, textureFileName, numOfParticles, particleSize, origin);
}

void BulletParticle::setAmountFactor(float factor)
{
	if (fireExplosion)
		fireExplosion->setAmountFactor(factor);
}

void BulletParticle::setDuration(float duration)
{
	if (fireExplosion)
		fireExplosion->setDuration(duration);
}


const string Terrain::filePathToLoadTerrainFiles = "../Terrain/";
const string Terrain::filePathToLoadTerrainTextureFiles = "../Terrain/Texture/";


void Terrain::render()
{
	if (!mesh) return;

	// 현재 transform행렬로 적용시킨다.
	device->SetTransform(D3DTS_WORLD, &transform->getTransformMatrix());

	if(texture)
		device->SetTexture(0, texture);

	device->SetMaterial(&mtrl);

	mesh->DrawSubset(0);

	float height = 0.0f;
	getLocalHeight({ 0,0,0 }, &height);
	Gizmos::DrawLine({ 0,0,0 }, { 0,height,0 });

	return;

	//// 파일이 로드되어서 값이 있는 경우만 그려준다.
	//for (int i = 0; i < mtrls.size(); ++i)
	//{
	//	device->SetMaterial(&mtrls[i]);
	//	device->SetTexture(0, textures[i]);
	//	mesh->DrawSubset(i);
	//}
}

void Terrain::start()
{
	this->device = &(gameObject->getDevice());

	// material 기본 설정
	// Set the RGBA for diffuse reflection.
	mtrl.Diffuse.r = 0.0f;
	mtrl.Diffuse.g = 0.5f;
	mtrl.Diffuse.b = 0.5f;
	mtrl.Diffuse.a = 1.0f;

	// Set the RGBA for ambient reflection.
	mtrl.Ambient.r = 0.5f;
	mtrl.Ambient.g = 0.0f;
	mtrl.Ambient.b = 0.5f;
	mtrl.Ambient.a = 1.0f;

	// Set the color and sharpness of specular highlights.
	mtrl.Specular.r = .20f;
	mtrl.Specular.g = .30f;
	mtrl.Specular.b = .20f;
	mtrl.Specular.a = 1.0f;
	mtrl.Power = 50.0f;

	// Set the RGBA for emissive color.
	mtrl.Emissive.r = 0.0f;
	mtrl.Emissive.g = 0.0f;
	mtrl.Emissive.b = 0.0f;
	mtrl.Emissive.a = 0.0f;
}

const unsigned long Terrain::TerrainVertex::DefaultFVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;


void Terrain::loadRawFile(const string & fileName, unsigned int verticesPerRow, unsigned int verticesPerColumn, float distanceOfVertices, float heightValue)
{
	if (!device) return;

	// 외부에서 받아온 값들을 복사해준다.
	this->fileName = fileName;
	this->verticesPerRow = verticesPerRow;
	this->verticesPerColumn = verticesPerColumn;
	this->distanceOfVertices = distanceOfVertices;

	if (heightValue < 0) heightValue = 1.0f;
	this->heightValue = heightValue;

	// 나머지 값들도 초기화시켜준다.
	heightMapSize = verticesPerRow * verticesPerColumn;
	rectsPerRow = verticesPerRow - 1;
	rectsPerColumn = verticesPerColumn - 1;
	width = rectsPerRow * distanceOfVertices;
	depth = rectsPerColumn * distanceOfVertices;

	// 하이트맵 로드에 실패하면 리턴
	if (!loadHeightMap(fileName))return;

	// 메쉬 버퍼 생성에 실패하면 리턴
	if (!generateMeshBuffer()) return;

	// 버텍스와 인덱스 정보를 채워준다.
	processVertices();
	processIndices();
	processSubsets();
	//optimizeMesh();
}

bool Terrain::loadHeightMap(const string & fileName)
{
	vector<BYTE> infoToRead(heightMapSize);

	ifstream iFStream((filePathToLoadTerrainFiles + fileName).c_str(), ios_base::binary);

	// 파일을 열 수 없을때 리턴 false
	if (iFStream.is_open() == false)
		return false;

	iFStream.read
	(
		(char*)&infoToRead[0],
		infoToRead.size()
	);

	iFStream.close();

	// 하이트맵을 크기를 다시 조정하고
	heightMap.resize(heightMapSize);

	// 읽은 데이터를 다시 저장한다. // 이때 heightValue을 적용시켜준다.
	for (int i = 0; i < infoToRead.size(); i++)
		heightMap[i] = (float)infoToRead[i] * (float)heightValue;

	return true;
}

bool Terrain::generateMeshBuffer()
{
	HRESULT hr = 0;


	hr = D3DXCreateMeshFVF(
		// 메쉬가 가질 면의 개수 // 총사각형 갯수 * 2
		// 삼각형의 개수/3 = 인덱스의 수
		rectsPerRow * rectsPerColumn * 2,

		// 메쉬가 가질 버텍스의 수
		heightMapSize,

		// 메쉬를 만드는데 이용될 하나 이상의 플래그
		// 여기서는 메쉬는 관리 메모리 풀내에 보관되도록 하였다.
		D3DXMESH_MANAGED,// | 
//		D3DXMESH_32BIT,

		// 복제된 메쉬를 만드는데 이용될 포맷
		Terrain::TerrainVertex::DefaultFVF,
		device,
		// 복제된 메쉬가 담길 포인터
		&mesh);

	if (FAILED(hr))
	{
		mesh = nullptr;
		return false;
	}
	return true;
}

void Terrain::processVertices()
{
	if (!mesh) return;
	// mesh에 넣어줄때 fbxVertex형식으로 넣어줄것이다.
	TerrainVertex * terrainV = nullptr;

	// 생성한 메쉬를 잠그고 그안에 버텍스 정보를 집어넣는다.
	// 버텍스 정보는 포지션 / 노말 / UV값이다.
	mesh->LockVertexBuffer(0, (void**)&terrainV);

	if (!terrainV) return;

	float distanceForU = 1.0f / (float)verticesPerRow;
	float distanceForV = 1.0f / (float)verticesPerColumn;

	float x = 0;
	float z = 0;
	float u = 0;
	float v = 0;
	Vector3 normal{ 0,0,0 };
	Vector3 a{ 0,0,0 };
	Vector3 b{ 0,0,0 };
	Vector3 c{ 0,0,0 };

	Vector3 vector1{ 0,0,0 };
	Vector3 vector2{ 0,0,0 };

	// 맵사이즈 // 버텍스 총 개수(행 버텍스 개수 * 열 버텍스 개수) // 만큼 순회하면서 값들을 채워준다.
	for (int i = 0; i < verticesPerColumn; ++i)
	{
		// 여기서 각각 -width/2 와 depth/2을 해주는 이유는 원점을 기준으로 정렬을 시켜주기 위함이다.
		for (int j = 0; j < verticesPerRow; ++j)
		{
			// position 계산
			x = j * distanceOfVertices - width/2;

			// 파일의 xz좌표와 directx 의 xz좌표에서 z축의 부호가 서로 반대이기 때문에 이렇게 처리해준다.
			// 이렇게 처리를 안하면 읽은 하이트 맵과 반대로 출력된다. 
			// 또 파일 기준으로 삼각형으로 그려주기 때문에 삼각형이 뒷면처리 되어 컬링되는 현상이 발생한다.
			z = -i * distanceOfVertices + depth/2;

			u = (float)j / 2.0f;// *distanceForU;
			v = (float)i / 2.0f;// *distanceForV;

			//Trace::Write("TAG_DEBUG", "index", i * verticesPerRow + j);
			//Trace::Write("TAG_DEBUG", "x", x);
			//Trace::Write("TAG_DEBUG", "y", heightMap[i]);
			//Trace::Write("TAG_DEBUG", "z", z);
			//Trace::Write("TAG_DEBUG", "");
			//Trace::Write("TAG_DEBUG", "index", i * verticesPerRow + j);
			//Trace::Write("TAG_DEBUG", "u", u);
			//Trace::Write("TAG_DEBUG", "v", v);

			// 노멀벡터를 구해준다. // 윗삼각형 기준으로 ac X ab 를 하면된다.
			/*
			  a	      b
				*	*
				*	*
			  c       d

			a기준으로 순회하는데 abc / cbd 삼각형 2개를 설정해주면 된다.

			각각의 버텍스의 인덱스는
			a = i * verticesPerRow + j
			b = i * verticesPerRow + j +1
			c = (i+1) * verticesPerRow + j
			d = (i+1) * verticesPerRow + j + 1
			*/
			if (i < rectsPerColumn && j < rectsPerRow)
			{
				a = { x, heightMap[i * verticesPerRow + j] ,z };
				b = { x + distanceOfVertices, heightMap[i * verticesPerRow + j + 1] ,z };
				c = { x, heightMap[(i + 1) * verticesPerRow + j] ,z - distanceOfVertices };

			}
			else if(i < rectsPerColumn && j >= rectsPerRow)
			{

				// 맨오른쪽 열일때 더이상 오른쪽의 인덱스가 없다.
				a = { x, heightMap[i * verticesPerRow + j] ,z };
				b = { x, heightMap[(i + 1) * verticesPerRow + j] ,z - distanceOfVertices };
				c = { x - distanceOfVertices, heightMap[(i + 1) * verticesPerRow + j - 1] ,z - distanceOfVertices };

			}
			else if (i >= rectsPerColumn && j < rectsPerRow)
			{
				// 맨아래 행일때 더이상 아래의 인덱스가 없다.
				a = { x, heightMap[i * verticesPerRow + j] ,z };
				b = { x + distanceOfVertices, heightMap[(i - 1) * verticesPerRow + j + 1] ,z + distanceOfVertices };
				c = { x + distanceOfVertices, heightMap[i * verticesPerRow + j + 1] ,z };

			}
			else // if(i >= rectsPerColumn && j < rectsPerRow) // 맨오른쪽 맨아래 인덱스일때
			{
				a = { x, heightMap[i * verticesPerRow + j] ,z };
				b = { x - distanceOfVertices, heightMap[i * verticesPerRow + j - 1] ,z };
				c = { x, heightMap[(i - 1) * verticesPerRow + j] ,z + distanceOfVertices };
			}

			// ab // b - a
			vector1 = b - c;
			// ac // c - a
			vector2 = c - a;

			Vector3::Cross(normal, vector1, vector2);


			Trace::Write("TAG_INFO1", "x", x);
			Trace::Write("TAG_INFO1", "y", heightMap[i * verticesPerRow + j]);
			Trace::Write("TAG_INFO1", "z", z);

			// 인덱스를 제대로 설정해주지 않아서 제대로 렌더링이되지 않는 버그가 발생했다.
			// 주의해서 인덱스를 넣어주자
			terrainV[i * verticesPerRow + j] =
			{
				// Position
				x, heightMap[i * verticesPerRow + j], z,
				// Normal // 일단 항상 up으로 설정
				normal.getX(), normal.getY(), normal.getZ(),
				// UV
				u,v
			};

		}

	}
	mesh->UnlockVertexBuffer();

}

void Terrain::processIndices()
{
	if (!mesh) return;

	unsigned short * terrainI = nullptr;

	// 생성한 메쉬를 잠그고 그안에 버텍스 정보를 집어넣는다.
	// 버텍스 정보는 포지션 / 노말 / UV값이다.
	mesh->LockIndexBuffer(0, (void**)&terrainI);

	unsigned int indexCounter = 0;

	if (!terrainI) return;

	for (int i = 0; i < rectsPerColumn; ++i)
	{
		for (int j = 0; j < rectsPerRow; ++j)
		{
			/*
			  a	      b
				*	*
				*	*
			  c       d

			   a기준으로 순회하는데 abc / cbd 삼각형 2개를 설정해주면 된다.

			   각각의 버텍스의 인덱스는
			   a = i * verticesPerRow + j
			   b = i * verticesPerRow + j +1
			   c = (i+1) * verticesPerRow + j
			   d = (i+1) * verticesPerRow + j + 1

			*/
			int a = i * verticesPerRow + j;
			int b = i * verticesPerRow + j + 1;
			int c = (i + 1) * verticesPerRow + j;
			int d = (i + 1) * verticesPerRow + j + 1;

			// abc
			terrainI[indexCounter++] = a;//i * verticesPerRow + j;
			terrainI[indexCounter++] = b;//i * verticesPerRow + j + 1;
			terrainI[indexCounter++] = c;// (i + 1) * verticesPerRow + j;

			// cbd
			terrainI[indexCounter++] = c;// (i + 1) * verticesPerRow + j;
			terrainI[indexCounter++] = b;// i * verticesPerRow + j + 1;
			terrainI[indexCounter++] = d;// (i + 1) * verticesPerRow + j + 1;
		}


	}
	mesh->UnlockIndexBuffer();
}


void Terrain::processSubsets()
{
	if (!mesh) return;

	unsigned long* attributeBuffer = nullptr;

	// 다시 속성 데이터를 수정하기 위해서 버퍼를 잠궈준다.
	mesh->LockAttributeBuffer(0, &attributeBuffer);

	int ttt = mesh->GetNumFaces();

	// 서브셋을 0번으로 지정해준다.
	for (int i = 0; i < mesh->GetNumFaces(); ++i)
		attributeBuffer[i] = 0;


	// 설정이 완료되었으므로 다시 잠궈준다.
	mesh->UnlockAttributeBuffer();
}

void Terrain::optimizeMesh()
{
	if (!mesh) return;

	// 인접버퍼를 이용한 최적화 작업
	vector<unsigned long> adjacencyBuffer(mesh->GetNumFaces() * 3);
	// 메쉬의 인접정보를 받아온다.
	mesh->GenerateAdjacency(0.0f, &adjacencyBuffer[0]);


	int faceCount1 = mesh->GetNumFaces();
	int vertexCount1 = mesh->GetNumVertices();

	HRESULT hr = 0;
	// 그 인접 정보를 기반으로 최적화 작업을 시작한다.
	hr = mesh->OptimizeInplace
	(
		// 속성으로(서브셋기준) 삼각형으로 정렬하고, 별도의 속성테이블을 생성
		// GetAttributeTable함수를 이용해서 D3DXATTRIBUTERANGE구조체 배열을 받아올 수 있다.
		// 내부 정보에는 서브셋ID / 각 면과 버텍스의 수 / 각 시작 지점이 들어있다.
		// 속성 테이블의 각항목은 메쉬의 각 서브셋과 대응되며, 
		// 서브셋의 기하정보들이 보관되는 버텍스/ 인덱스 버퍼내의 메모리 블록을 지정한다.
		D3DXMESHOPT_ATTRSORT |
		// 메쉬에서 이용하지 않는 인덱스와 버텍스를 제거한다.
		D3DXMESHOPT_COMPACT |
		// 버텍스 캐시의 히트율을 높인다.
		D3DXMESHOPT_VERTEXCACHE,

		// 최적화 되지않은 메쉬의 인접 배열 포인터
		// 인접배열이 필요한이유?	// 최적화 목록에 인접정보가 필요한 부분이 있어서 필요할듯
		&adjacencyBuffer[0],
		// 최적화된 메쉬의 인접 배열 포인터
		nullptr,
		// 리맵정보 Face
		nullptr,
		// 리맵정보 Vertex
		nullptr
	);

	int faceCount = mesh->GetNumFaces();
	int vertexCount = mesh->GetNumVertices();
}

void Terrain::loadTextureFromFile(const string & fileName)
{
	HRESULT hr = 0;

	// 파일을 로드시켜서 텍스처를 불러오는 방식 
	hr = D3DXCreateTextureFromFile
	(
		device,
		(filePathToLoadTerrainTextureFiles + fileName).c_str(),
		&texture
	);

	if (FAILED(hr))
		texture = nullptr;
}

bool Terrain::getHeight(const Vector3 & position, float * output)
{
	if (!output) return false;

	// 구하고자 하는 점을 Terrain의 로컬좌표계로 변환한다.
	// 로컬좌표계로 변환하기 위해서는 Terrain의 transform행렬의 역행렬을 곱해주면 된다.
	D3DXMATRIX terrainInverseTransformMatrix;
	D3DXVECTOR3 positionInTerrainLocal;


	if (gameObject->getParent() == nullptr)
	{
		D3DXMatrixIdentity(&terrainInverseTransformMatrix);
	}
	else
	{
		if (gameObject->getParent()->getTransform()->getInverseTransformMatrix(terrainInverseTransformMatrix) == false) return false;
	}
	//if(transform->getInverseTransformMatrix(terrainInverseTransformMatrix) == false) return false;
	//float determinant = 0.0f;
	//terrainInverseTransformMatrix = transform->getTransformMatrix();
	//D3DXMatrixInverse(&terrainInverseTransformMatrix, &determinant, &terrainInverseTransformMatrix);
	//// 만약 역행렬이 존재하지 않으면 리턴
	//if (determinant == 0.0f) return false;

	Vector3::ToD3DXVECTOR3(positionInTerrainLocal, position);

	D3DXVec3TransformCoord(&positionInTerrainLocal, &positionInTerrainLocal, &terrainInverseTransformMatrix);

	// 이제부터 터레인의 로컬좌표계 기준으로 계산하면 된다. // 위치를 로컬좌표계로 끌어왔다.
	// 이렇게 로컬좌표계로 변경하는 이유는 터레인이 로테이션됐을경우 1칸의 계산이 어렵기 때문이다.

	if (!getLocalHeight({ positionInTerrainLocal.x,positionInTerrainLocal.y,positionInTerrainLocal.z }, output))
		return false;

	return true;
}

bool Terrain::getLocalHeight(const Vector3 & position, float * output)
{
	if (!output) return false;

	float startX = -width / 2;
	float startZ = depth / 2;
	float endX = width / 2;
	float endZ = -depth / 2;

	// 만약에 벗어난다면 리턴
	if (startX >= position.getX() || startZ <= position.getZ()
		|| endX <= position.getX() || endZ >= position.getZ())
		return false;

	// 시작점으로 부터 얼마나 떨어져 있는지 구한다.
	float z = (startZ - position.getZ());
	float x = (position.getX() - startX);
	// 인덱스의 값을 구해본다.
	int columnIndex = z / distanceOfVertices;
	int rowIndex = x / distanceOfVertices;

	// 그 인덱스 기준으로 얼마나 떨어져 있는지 구한다.
	// 인덱스 기준 좌표로 초기화 시켜준다.
	// distanceOfVertices인 정사각형이다. // 시작점 왼쪽위
	z -= columnIndex * distanceOfVertices;
	x -= rowIndex * distanceOfVertices;

	/*
	   a	  b
		* -- *
		l  / l
		* __ *
	   c      d

	a기준으로 순회하는데 abc / cbd 삼각형 2개를 설정해주면 된다.
	*/

	float aHeight = heightMap[columnIndex * verticesPerRow + rowIndex];
	float bHeight = heightMap[columnIndex * verticesPerRow + rowIndex + 1];
	float cHeight = heightMap[(columnIndex + 1) * verticesPerRow + rowIndex];
	float dHeight = heightMap[(columnIndex + 1) * verticesPerRow + rowIndex + 1];

	float resultHeight = 0.0f;

	// 어디로 치우쳐있는지에 대한 값을 구해준다 // 0 ~ 1사이 // 0 : x기준 맨왼쪽 z기준 맨위, 1 : x기준 맨오른쪽 z기준 맨아래
	float zFactor = z / distanceOfVertices;
	float xFactor = x / distanceOfVertices;

	if (x + z < distanceOfVertices)
	{
		// 윗쪽 삼각형인 경우
		// A Vector3 + (ab Vector3 * xFactor + ac Vector3 * zFactor) 의 y값
		// 그냥 y값중심으로만 계산해도 된다. 
		resultHeight = aHeight + ((bHeight - aHeight) * xFactor + (cHeight - aHeight) * zFactor);
	}
	else
	{
		// 아래쪽 삼각형인경우
		// D Vector3 + (dc Vector3 * (1 - xFactor) + db Vector3 * (1 - zFactor)) 의 y값
		// 그냥 y값중심으로만 계산해도 된다. 
		resultHeight = dHeight + ((cHeight - dHeight) * (1 - xFactor) + (bHeight - dHeight) * (1 - zFactor));
	}

	// 여기서 현재 터레인의 높이만큼 더해준다.
	*output = resultHeight + transform->getWorldPosition().getY();
	return true;
}

void MoveOnTerrainScript::update()
{
	if (terrain)
	{
		//if (gameObject->getTag() == "Enemy")
		//{
		//	Vector3 temp = transform->getWorldPosition();
		//	Trace::Write("TAG_DEBUG", "enemy position move on", "");
		//	Trace::Write("TAG_DEBUG", "x", temp.getX());
		//	Trace::Write("TAG_DEBUG", "y", temp.getY());
		//	Trace::Write("TAG_DEBUG", "z", temp.getZ());
		//}



		float height = 0.0f;
		if (terrain->getHeight(transform->getWorldPosition(), &height))
		{
			transform->setWorldPosition(transform->getWorldPosition().getX(), height + objectHeight, transform->getWorldPosition().getZ());
		}

		//if (gameObject->getTag() == "Enemy")
		//{
		//	Vector3 temp = transform->getWorldPosition();
		//	Trace::Write("TAG_DEBUG", "enemy position move on", "");
		//	Trace::Write("TAG_DEBUG", "x", temp.getX());
		//	Trace::Write("TAG_DEBUG", "y", temp.getY());
		//	Trace::Write("TAG_DEBUG", "z", temp.getZ());
		//}

		//float height = 0.0f;
		//if (terrain->getHeight(transform->getPosition(), &height))
		//{
		//	if (transform->getPosition().getY() < height)
		//		transform->setPosition(transform->getPosition().getX(), height, transform->getPosition().getZ());
		//}
	}
}

const string BillBoard::filePathToLoadBillBoardTextureFiles = "../BillBoard/";
const unsigned long BillBoard::BillBoardVertex::DefaultFVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;


void BillBoard::render()
{
	if (!mesh) return;

	device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	//device->SetRenderState(D3DRS_ZWRITEENABLE, false);

	// Color = TexelColor x SourceBlend + CurrentPixelColor x DestBlend
	// 여기서 소스알파값이 0이면 뒷배경이 1이 적용되어서 뒷배경이 반영되고
	// 소스 알파값이 .5이면 뒷배경이 .5로 적용되어서 .5만큼 반영된다.
	device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	//device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR);
	//device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR);


	// 현재 transform행렬로 적용시킨다.
	device->SetTransform(D3DTS_WORLD, &transform->getTransformMatrix());

	if (texture)
		device->SetTexture(0, texture);

	device->SetMaterial(&mtrl);

	mesh->DrawSubset(0);


	device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	//device->SetRenderState(D3DRS_ZWRITEENABLE, true);
}

void BillBoard::rotateIfMainCameraMoves()
{
	// 만약 이전 카메라 포지션과 변함 없다면 리턴
	//D3DXMATRIX mainCameraTransformMatrix = mainCamera->getTransform()->getTransformMatrix();
	// 이런작업을 해주는 이유는 메인카메라가 어느 오브젝트의 하위 오브젝트일 가능성이 있기 때문이다.
	// 하위오브젝트로 포함되는 경우 position은 로컬좌표이기 때문에 transformMatrix을 받아와서 글로벌 위치를 구해준다.
	//Vector3 mainCameraPosition{ mainCameraTransformMatrix._41,mainCameraTransformMatrix._42,mainCameraTransformMatrix._43 };

	Vector3 mainCameraPosition{ mainCamera->getTransform()->getWorldPosition() };

	if (mainCameraPosition == currentMainCameraPosition) return;

	// 카메라의 위치가 변경되었다면 위치 최신화를 해준다.
	currentMainCameraPosition = mainCameraPosition;

	// 현재물체에서 카메라방향으로의 벡터를 구해준다.
	Vector3 direction = currentMainCameraPosition - transform->getWorldPosition();

	// y값은 적용을 안해주기 때문에 빼준다. // y축으로만 로테이션 해야한다.
	direction.setY(0.0f);
	
	// 정규화해준다.
	Vector3::Normalized(direction, direction);


	// 초기 위치 forward  = (0,0, -1) dot direction을 해주어서 로테이션을 구해준다.
	// 그 사이각을 구하기 위해서 각각의 크기(둘다 정규화된 벡터이므로 1)로 내적값을 나눠주고 
	// 최종적으로 cos의 역함수를 적용해준다.
	float angle =  (float)acos(Vector3::Dot({ 0,0,-1 }, direction)/ (1.0f * 1.0f) );

	// 여기서 주의할점은 항상 사이각은 양수가 나온다는 점이다.
	/*
	양수각 음수각 판별법
	1. 여기서 판별하기 위해서 좌표평면에 한벡터 기준으로 정렬시키고 (여기서는 World back 라는 기준벡터를 사용했기 때문에 따로 필요 없음)
	   그 벡터 기준으로 음수각인지 판별한다. 
	   // 여기서는 한벡터가 back벡터이므로 x값이 + 일때 음수각이다. // right벡터라면 y값이 -일때 양수각 +일때 음수각이다.

	2. 두 벡터를 외적해서 방향이 어디인지 판별해본다. 
	   빌보드에서는 빌보드방향 X 카메라까지방향 를 했을때 World up 이 나온다면 양수각이고
	   빌보드방향 X 카메라까지방향 를 했을때 World down이 나온다면 음수각이다.
	
	*/


	// 좀더 명확한 표현 필요할듯 나중에 수정할 예정
	// 만약 음수일 조건이면 각을 음수로 바꿔준다.
	if (direction.getX() > 0)
		angle = -angle;
	
	
	// Y기준으로 // 시계방향 + / 반시계 - 
	// 만약에 빌보드의 로테이션가 카메라방향보다 더 시계방향쪽으로 돌아있다면 - 을 해주어야한다.
	// 즉 빌보드의 현재각도가 카메라방향 벡터 각보다 크다면 각을 음수로 바꿔준다.
	//if (transform->getRotation().getY() > mainCamera->getTransform()->getRotation().getY())
	//	angle = -angle;

	transform->setWorldRotation({ 0, Transform::RadianToDegree(angle),0 });
	
	// 노멀벡터를 새로 계산하기 위해서 버텍스들을 다시 초기화시킨다.
	// 현재 로테이션이 바뀌었기 때문에 노멀벡터도 바꿔줘야한다. // 항상 카메라에서 봤을때 잘보이게 하기 위해서
	processVertices();
	//processIndices();
}

void BillBoard::start()
{
	mainCamera = gameObject->getScene().getMainCamera();
	this->device = &(gameObject->getDevice());

	// material 기본 설정
	// Set the RGBA for diffuse reflection.
	mtrl.Diffuse.r = .50f;
	mtrl.Diffuse.g = .50f;
	mtrl.Diffuse.b = .50f;
	mtrl.Diffuse.a = 1.0f;

	// Set the RGBA for ambient reflection.
	mtrl.Ambient.r = 0.3f;
	mtrl.Ambient.g = 0.5f;
	mtrl.Ambient.b = 0.3f;
	mtrl.Ambient.a = 1.0f;

	// Set the color and sharpness of specular highlights.
	mtrl.Specular.r = 0.30f;
	mtrl.Specular.g = 0.50f;
	mtrl.Specular.b = 0.30f;
	mtrl.Specular.a = 1.0f;
	mtrl.Power = 20.0f;

	// Set the RGBA for emissive color.
	mtrl.Emissive.r = 0.0f;
	mtrl.Emissive.g = 0.0f;
	mtrl.Emissive.b = 0.0f;
	mtrl.Emissive.a = 0.0f;

}

void BillBoard::update()
{
	// 만약 메인 카메라 or device 가 없으면 리턴한다.
	if (!mainCamera || !device) return;

	rotateIfMainCameraMoves();
	render();
}

void BillBoard::loadTextureFromFile(const string & fileName)
{
	HRESULT hr = 0;

	// 파일을 로드시켜서 텍스처를 불러오는 방식 
	hr = D3DXCreateTextureFromFile
	(
		device,
		(filePathToLoadBillBoardTextureFiles + fileName).c_str(),
		&texture
	);

	if (FAILED(hr))
		texture = nullptr;

}

void BillBoard::generateBillBoard()
{
	// 메쉬가 있거나 디바이스가 없으면 리턴
	if (!device || mesh) return;

	// 생성실패시 리턴
	if (!generateMeshBuffer()) return;
	processVertices();
	processIndices();
	processSubsets();
	//optimizeMesh();
}

bool BillBoard::generateMeshBuffer()
{
	HRESULT hr = 0;

	hr = D3DXCreateMeshFVF(
		// 삼각형 2개
		2,
		// 버텍스 2개
		4,
		// 메쉬를 만드는데 이용될 하나 이상의 플래그
		// 여기서는 메쉬는 관리 메모리 풀내에 보관되도록 하였다.
		D3DXMESH_MANAGED,
		// 복제된 메쉬를 만드는데 이용될 포맷
		BillBoard::BillBoardVertex::DefaultFVF,
		device,
		// 복제된 메쉬가 담길 포인터
		&mesh);

	if (FAILED(hr))
	{
		mesh = nullptr;
		return false;
	}
	return true;
}

void BillBoard::processVertices()
{
	if (!mesh) return;
	// mesh에 넣어줄때 fbxVertex형식으로 넣어줄것이다.
	BillBoardVertex * billBoardV = nullptr;

	// 생성한 메쉬를 잠그고 그안에 버텍스 정보를 집어넣는다.
	// 버텍스 정보는 포지션 / 노말 / UV값이다.
	mesh->LockVertexBuffer(0, (void**)&billBoardV);

	if (!billBoardV) return;

	/*
	  a	      b
		*	*
		*	*
	  c       d

	a : 0 / b : 1 / c : 2 / d : 3
	// z값은 모두 0이다.
	*/

	Vector3 normal =(-1) * transform->getForward();

	float halfWidth = width / 2;
	float halfHeight = height / 2;
	// 여기서는 빌보드 좌표계 기준으로 결정해준다. 
	// 나중에 transformMatrix에 의해 위치와 로테이션이 결정된다.
	// Normal // 항상 back으로 설정 z가 0인 xy평면 기준 // 로테이션 기준 항상 로컬 back으로 변경했다. // 저방식대로하면 다른 방향에서 어두워짐
	billBoardV[0] =
	{
		// Position
		-halfWidth, halfHeight, 0,
		normal.getX(), normal.getY(), normal.getZ(),
		// UV
		0,0
	};
	billBoardV[1] =
	{
		// Position
		halfWidth, halfHeight, 0,
		normal.getX(), normal.getY(), normal.getZ(),
		// UV
		1,0
	};
	billBoardV[2] =
	{
		// Position
		-halfWidth, -halfHeight, 0,
		normal.getX(), normal.getY(), normal.getZ(),
		// UV
		0,1
	};
	billBoardV[3] =
	{
		// Position
		halfWidth, -halfHeight, 0,
		normal.getX(), normal.getY(), normal.getZ(),
		// UV
		1,1
	};

	mesh->UnlockVertexBuffer();

}

void BillBoard::processIndices()
{
	if (!mesh) return;

	unsigned short * billBoardI = nullptr;

	// 생성한 메쉬를 잠그고 그안에 버텍스 정보를 집어넣는다.
	// 버텍스 정보는 포지션 / 노말 / UV값이다.
	mesh->LockIndexBuffer(0, (void**)&billBoardI);

	if (!billBoardI) return;

	billBoardI[0] = 0;
	billBoardI[1] = 1;
	billBoardI[2] = 2;

	billBoardI[3] = 1;
	billBoardI[4] = 3;
	billBoardI[5] = 2;

	mesh->UnlockIndexBuffer();
}


void BillBoard::processSubsets()
{
	if (!mesh) return;

	unsigned long* attributeBuffer = nullptr;

	// 다시 속성 데이터를 수정하기 위해서 버퍼를 잠궈준다.
	mesh->LockAttributeBuffer(0, &attributeBuffer);

	// 서브셋을 0번으로 지정해준다.
	for (int i = 0; i < mesh->GetNumFaces(); ++i)
		attributeBuffer[i] = 0;

	// 설정이 완료되었으므로 다시 잠궈준다.
	mesh->UnlockAttributeBuffer();
}

void BillBoard::optimizeMesh()
{
	if (!mesh) return;

	// 인접버퍼를 이용한 최적화 작업
	vector<unsigned long> adjacencyBuffer(mesh->GetNumFaces() * 3);
	// 메쉬의 인접정보를 받아온다.
	mesh->GenerateAdjacency(0.0f, &adjacencyBuffer[0]);


	HRESULT hr = 0;
	// 그 인접 정보를 기반으로 최적화 작업을 시작한다.
	hr = mesh->OptimizeInplace
	(
		// 속성으로(서브셋기준) 삼각형으로 정렬하고, 별도의 속성테이블을 생성
		// GetAttributeTable함수를 이용해서 D3DXATTRIBUTERANGE구조체 배열을 받아올 수 있다.
		// 내부 정보에는 서브셋ID / 각 면과 버텍스의 수 / 각 시작 지점이 들어있다.
		// 속성 테이블의 각항목은 메쉬의 각 서브셋과 대응되며, 
		// 서브셋의 기하정보들이 보관되는 버텍스/ 인덱스 버퍼내의 메모리 블록을 지정한다.
		D3DXMESHOPT_ATTRSORT |
		// 메쉬에서 이용하지 않는 인덱스와 버텍스를 제거한다.
		D3DXMESHOPT_COMPACT |
		// 버텍스 캐시의 히트율을 높인다.
		D3DXMESHOPT_VERTEXCACHE,

		// 최적화 되지않은 메쉬의 인접 배열 포인터
		// 인접배열이 필요한이유?	// 최적화 목록에 인접정보가 필요한 부분이 있어서 필요할듯
		&adjacencyBuffer[0],
		// 최적화된 메쉬의 인접 배열 포인터
		nullptr,
		// 리맵정보 Face
		nullptr,
		// 리맵정보 Vertex
		nullptr
	);
}

