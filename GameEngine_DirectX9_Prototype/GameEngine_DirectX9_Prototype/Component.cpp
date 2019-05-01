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
//	// ��Ʈ �������� ���������� ������ �ֽ�ȭ �޽����� �޾Ұų�, 
//	// ��ü������ �ֽ�ȭ�ϴ� ��쿡 �ֽ�ȭ ���ְ� �ڽİ�ü���Ե� �ֽ�ȭ�� �������ش�.
//	
//	dirty |= this->dirty;
//	if (dirty)
//	{
//		// ���� ��ȯ�� �θ� ��ȯ�� �����ؼ� ����Ѵ�.
//		// �����̼���İ� ������ ����� ���� ������־�� �Ѵ�.
//		//setRotationMatrix_DX(parentRotationMatrix);
//		//setPositionMatrix_DX(parentPositionMatrix);
//		// ���� transformMatrix = ����� �����̼� * ������
//		setTransformMatrix_DX(parentPositionMatrix, parentRotationMatrix);
//		this->dirty = false;
//	}
//
//
//	vector<GameObject*> & children = gameObject->getChildren();
//
//
//	// ����� ��Ƽ�÷��װ� �����־ �߰� ���ϵ忡 �������� ���ɼ��� �ֱ� ������
//	// ��� ��ü�� ���ؼ� �������ش�.
//	for (auto it : children)
//	{
//		it->getTransform()->transformUpdate(dirty, getPositionMatrix_DX(),getRotationMatrix_DX());
//	}
//
//}


void Transform::transformUpdate(bool dirty, const D3DXMATRIX & parentTransformMatrix, bool toPhysicsSystem)
{
	// ��Ʈ �������� ���������� ������ �ֽ�ȭ �޽����� �޾Ұų�, 
	// ��ü������ �ֽ�ȭ�ϴ� ��쿡 �ֽ�ȭ ���ְ� �ڽİ�ü���Ե� �ֽ�ȭ�� �������ش�.

	dirty |= this->dirty;
	if (dirty)
	{
		// ���� ��ȯ�� �θ� ��ȯ�� �����ؼ� ����Ѵ�.
		// �����̼���İ� ������ ����� ���� ������־�� �Ѵ�.
		//setRotationMatrix_DX(parentRotationMatrix);
		//setPositionMatrix_DX(parentPositionMatrix);
		// ���� transformMatrix = ����� �����̼� * ������
		setTransformMatrix(parentTransformMatrix);

		// ���� ����Ǿ����� ������ٵ� �ִٸ� ���������� transform���� �������ش�.
		if (gameObject->getRigidBody() && toPhysicsSystem)
			gameObject->getPhysics().setTransformToSystem(gameObject);


		this->dirty = false;
	}


	vector<GameObject*> & children = gameObject->getChildren();


	// ����� ��Ƽ�÷��װ� �����־ �߰� ���ϵ忡 �������� ���ɼ��� �ֱ� ������
	// ��� ��ü�� ���ؼ� �������ش�.
	for (auto it : children)
	{
		it->getTransform()->transformUpdate(dirty, getTransformMatrix());
	}
}

void Transform::rootTransformUpdate(bool toPhysicsSystem)
{
	Transform * rootTransform = gameObject->getRoot()->getTransform();
	// �θ�ü �������� TransformMatrix�� �ֽ�ȭ �����ش�.
	rootTransform->transformUpdate(rootTransform->getDirty(), Transform::IdentityMatrix, toPhysicsSystem);
}

void Transform::setTransformMatrix(const D3DXMATRIX & parentTransformMatrix)
{
	// ���� �ڽ��� �θ���ǥ�� �������� �����̼ǰ� ������ ����� �����ش�.
	calcRotationMatrix(); calcPositionMatrix();

	// ���� �ڽı���(���� ���� X / �θ� ����� ���������� �����̴�) trasnformMatrix�� ���ϰ� 
	// rotationMatrix * positionMatrix
	// ���������� ������ ���� �ֽ�ȭ�� �θ��� transform����� �����ش�.
	// ���� ���� �����
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
//		// �߿� :: �θ����� �θ��� �����̼Ǹ�ŭ �� ���� �Ǿ��ִ�. (�θ� + �ڽ� �����̼�) > �θ����(forward up right) �ڽ� ��ġ + �θ���ġ
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
//	// �θ�ü �������� right up forward�������� ������ش�.
//	// �θ�ü�� ���� ��Ʈ ������Ʈ�� ��� ���� right up forward�� ����Ѵ�.
//	if (!parent)
//		result = Vector3::Right * position.getX() + Vector3::Up * position.getY() + Vector3::Forward * position.getZ();
//	else
//		result = parent->getTransform()->getRight() * position.getX() + parent->getTransform()->getUp() * position.getY()
//		+ parent->getTransform()->getForward() * position.getZ();
//
//	D3DXMatrixIdentity(&positionMatrix_DX);
//
//	D3DXMatrixTranslation(&positionMatrix_DX, result.getX(), result.getY(), result.getZ());
//	// ���������� �θ�ü�� ��ġ��ŭ �̵���Ų��.
//	D3DXMatrixMultiply(&positionMatrix_DX, &positionMatrix_DX, &parentPositionMatrix);
//}



void Transform::convertLocalPositionIfItIsChild(Vector3 & output, const Vector3 & input)
{
	// �θ�ü�� ���ٸ� �׳� �ٷ� �ʱ�ȭ
	if (!gameObject->getParent())
	{
		output = input;
		return;
	}

	// �θ�ü�� �ִٸ� 
	/*
	1. �θ�ü�� TransformMatrix�� ������� ���ؼ� ������ǥ�� ����.
	2. ������ǥ�� �����ش�.
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
	// �θ�ü�� ���ٸ� �׳� �ٷ� �ʱ�ȭ
	if (!gameObject->getParent())
	{
		output = input;
		return;
	}

	// �θ�ü�� �ִٸ� 
	/*
	1. �θ�ü�� TransformMatrix�� ������� ���ؼ� ������ǥ�� ����.
	2. ������ǥ�� �����ش�.
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
	// ���������� �θ�ü�� ������ �ٷ� �ʱ�ȭ / ������ ��ȯ�� �ʱ�ȭ���ش�.
	convertLocalPositionIfItIsChild(position, other);

	// ���Ӱ� ����� ����� ���� transformMatrix�� �������ش�.
	// �޾ƿ��� ���̱� ������ �ٽ� System�� �ֽ�ȭ��Ű�� �ȵȴ�.
	rootTransformUpdate(false);
}

// ���濹��/////////////////////////////////////////////////////////////// �ӽ�
void Transform::setWorldRotation_physics(const Vector3 & other)
{
	dirty = true;
	convertLocalRotationIfItIsChild(rotation, other);


	setDirectionVectorWithRotation_DX();

	// ���Ӱ� ����� ����� ���� transformMatrix�� �������ش�.
	// �޾ƿ��� ���̱� ������ �ٽ� System�� �ֽ�ȭ��Ű�� �ȵȴ�.
	rootTransformUpdate(false);

}
// ���濹��/////////////////////////////////////////////////////////////// �ӽ�


// ���濹��/////////////////////////////////////////////////////////////// �ӽ�
void Transform::setWorldScale_physics(const Vector3 & other)
{
	scale = other;
}
// ���濹��/////////////////////////////////////////////////////////////// �ӽ�

void Transform::rotate(const Vector3 & other)
{
	setLocalRotation(Vector3(rotation.getX() + other.getX(), rotation.getY() + other.getY(), rotation.getZ() + other.getZ()));
	// ���Ӱ� ����� ����� ���� transformMatrix�� �������ش�.
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
	// ���������� �θ�ü�� ������ �ٷ� �ʱ�ȭ / ������ ��ȯ�� �ʱ�ȭ���ش�.
	convertLocalPositionIfItIsChild(position, other);

	// ���Ӱ� ����� ����� ���� transformMatrix�� �������ش�.
	rootTransformUpdate();

	//// ���� ����Ǿ����� ������ٵ� �ִٸ� ���������� ó�����ش�.
	//if(gameObject->getRigidBody())
	//	gameObject->getPhysics().setTransformToSystem(gameObject);

}


void Transform::setWorldRotation(const Vector3 & other)
{
	dirty = true;

	convertLocalRotationIfItIsChild(rotation, other);


	setDirectionVectorWithRotation_DX();
	//// ���� ����Ǿ����� ������ٵ� �ִٸ� ���������� ó�����ش�.
	//if(gameObject->getRigidBody())
	//	gameObject->getPhysics().setTransformToSystem(gameObject);
	rootTransformUpdate();
}


void Transform::setWorldPosition(float x, float y, float z)
{
	dirty = true;
	// ���������� �θ�ü�� ������ �ٷ� �ʱ�ȭ / ������ ��ȯ�� �ʱ�ȭ���ش�.
	convertLocalPositionIfItIsChild(position, Vector3{ x,y,z });

	// ���Ӱ� ����� ����� ���� transformMatrix�� �������ش�.
	rootTransformUpdate();

	//// ���� ����Ǿ����� ������ٵ� �ִٸ� ���������� ó�����ش�.
	//if (gameObject->getRigidBody())
	//	gameObject->getPhysics().setTransformToSystem(gameObject);

}


void Transform::setWorldRotation(float x, float y, float z)
{
	dirty = true;

	convertLocalRotationIfItIsChild(rotation, Vector3{ x,y,z });

	setDirectionVectorWithRotation_DX();

	//// ���� ����Ǿ����� ������ٵ� �ִٸ� ���������� ó�����ش�.
	//if (gameObject->getRigidBody())
	//	gameObject->getPhysics().setTransformToSystem(gameObject);


	// ���Ӱ� ����� ����� ���� transformMatrix�� �������ش�.
	rootTransformUpdate();
}


void Transform::setLocalPosition(const Vector3 & other)
{
	dirty = true;
	position = other;

	// ���Ӱ� ����� ����� ���� transformMatrix�� �������ش�.
	rootTransformUpdate();
}


void Transform::setLocalRotation(const Vector3 & other)
{
	dirty = true;
	rotation.setX(constrainLessThan180(other.getX()));
	rotation.setY(constrainLessThan180(other.getY()));
	rotation.setZ(constrainLessThan180(other.getZ()));
	setDirectionVectorWithRotation_DX();

	// ���Ӱ� ����� ����� ���� transformMatrix�� �������ش�.
	rootTransformUpdate();
}


void Transform::setLocalPosition(float x, float y, float z)
{
	dirty = true;

	position.setX(x);
	position.setY(y);
	position.setZ(z);


	// ���Ӱ� ����� ����� ���� transformMatrix�� �������ش�.
	rootTransformUpdate();

	//// ���� ����Ǿ����� ������ٵ� �ִٸ� ���������� ó�����ش�.
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

	// ���Ӱ� ����� ����� ���� transformMatrix�� �������ش�.
	rootTransformUpdate();

	//// ���� ����Ǿ����� ������ٵ� �ִٸ� ���������� ó�����ش�.
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
	// Unity���� start�Լ��� �ٷ� ȣ����� �ʴ� ������ �� �� ����. // ���⼭ ������ start�Լ��� awake�Լ��� ���ٰ� ���� �ȴ�.
	// ���� �ڽİ�ü�� ��� ���� ��ǥ�� ������ǥ�谡 �Ǿ �θ�ü�� Transform ����� �߰������� ���ؼ� ���� World��ǥ�� ���ؾ��ϴµ�
	// �� ���� ����� ���ӷ��� �Ѱ����� ����ȴ�. �׷��ԵǸ� ���⼭�� World���� ���� ��ġ�� �� �� ����..
	// �ϴ� ���⼭ TransformUpdate�� �ѹ� ������ ��� ��ü�� ���ؼ� TransformMatrix�� �����ֱ���Ѵ� // ���߿� start�Լ� ȣ����� ������ ����
	transform->rootTransformUpdate();

	// ���� TransformMatrix������ ������ǥ�� �̾Ƴ��� ��ȯ�� ������ �ʱ�ȭ
	startPoint = transform->getWorldPosition();

	basicEnemyAnimationFSM = gameObject->getComponent<BasicEnemyAnimationFSM>();


	basicEnemyBT = new BT::Root();
	basicEnemyBT->openBranch
	(
		(new BT::Selector())->openBranch
		(
			(new BT::Suquence())->openBranch
			(
				// Ÿ�ٰ˻�
				new BT::Condition<BasicEnemyScript, &BasicEnemyScript::isTargeting>(this),
				(new BT::Selector())->openBranch
				(
					// ������ ��ġ��� // ���� ������ ����ٸ�
					// ���� �̻󳪰��� �ٽ� Ÿ���� nullptr�� �ٲ��ش�.
					(new BT::If<BasicEnemyScript, &BasicEnemyScript::isOutOfRange>(this))->openBranch
					(
						new BT::Action<BasicEnemyScript, &BasicEnemyScript::resetTarget>(this),
						BTNodeBranchEnd
					),

					// Range1�϶� // ����
					(new BT::If<BasicEnemyScript, &BasicEnemyScript::isInRange1>(this))->openBranch
					(
						new BT::Action<BasicEnemyScript, &BasicEnemyScript::attackTarget>(this),
						BTNodeBranchEnd
					),

					// Range2�϶� // �뽬
					(new BT::If<BasicEnemyScript, &BasicEnemyScript::isInRange2>(this))->openBranch
					(
						new BT::Action<BasicEnemyScript, &BasicEnemyScript::dashToTarget>(this),
						BTNodeBranchEnd
					),

					// Range3�϶� // �߰�
					(new BT::If<BasicEnemyScript, &BasicEnemyScript::isInRange3>(this))->openBranch
					(
						new BT::Action<BasicEnemyScript, &BasicEnemyScript::chaseTarget>(this),
						BTNodeBranchEnd
					),


					BTNodeBranchEnd
				),
				BTNodeBranchEnd
			),

			// ���Ͱ˻� // ��ŸƮ ����Ʈ�� ������������� ����
			(new BT::If<BasicEnemyScript, &BasicEnemyScript::haveToReturn>(this))->openBranch
			(
				new BT::Action<BasicEnemyScript, &BasicEnemyScript::returnToStartPoint>(this),
				BTNodeBranchEnd
			),

			// ����Ҷ� ����
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

	//// �ڽ� ��ü�� ���� ������ ���� Transformation����� �޾Ƽ� ����Ѵ�. // ��ġ�� �����Ѵ�.
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

	// Ÿ�ٰ� ���������� �Ÿ��� 300�̻��̸� 
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

	// ���� ��ǥ�� �޾��ְ�
	Vector3 & currentWorldPos = transform->getWorldPosition();

	// ������ ���ϰ� ����ȭ
	Vector3 direction = startPoint - currentWorldPos;
	Vector3::Normalized(direction, direction);

	// �̵��Ѵ�
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

	// ���� ��ǥ�� �޾��ְ�
	Vector3 & currentWorldPos = transform->getWorldPosition();

	// ������ ���ϰ� ����ȭ
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
	// ���� ��ǥ�� �޾��ְ�
	Vector3 & currentWorldPos = transform->getWorldPosition();

	// ������ ���ϰ� ����ȭ
	Vector3 direction = target->getTransform()->getWorldPosition() -currentWorldPos;
	Vector3::Normalized(direction, direction);

	// �̵��Ѵ�
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
	// ���� ��ǥ�� �޾��ְ�
	Vector3 & currentWorldPos = transform->getWorldPosition();

	// ������ ���ϰ� ����ȭ
	Vector3 direction = target->getTransform()->getWorldPosition() - currentWorldPos;
	Vector3::Normalized(direction, direction);

	// �̵��Ѵ�
	transform->setWorldPosition(transform->getWorldPosition() + FrameTime::GetDeltaTime() * .1f * speed * direction);

	lookAtTarget(direction);
}



void BasicEnemyScript::lookAtTarget(const Vector3 & targetDirection)
{
	// Ÿ�ٹ������� �������� enemy�� ������ǥ��� ����.
	// ���ϰ��� �ϴ� ���� Terrain�� ������ǥ��� ��ȯ�Ѵ�.
	// ������ǥ��� ��ȯ�ϱ� ���ؼ��� Terrain�� transform����� ������� �����ָ� �ȴ�.
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
	// ���� ������ǥ��ε��Դ�.
	// ������(0,0,1)�� ����ؼ� �����̼��� �����ش�.
	targetDirectionInEnemyLocal.y = 0;
	float angle = (float)acos(D3DXVec3Dot(&D3DXVECTOR3{ 0,0,1 }, &targetDirectionInEnemyLocal) / (1.0f * 1.0f));
	if (targetDirectionInEnemyLocal.x < 0)
		angle = -angle;

	transform->setWorldRotation(Vector3(transform->getLocalRotation().getX(), Transform::RadianToDegree(angle), transform->getLocalRotation().getZ()));

}





void BasicEnemySearchRangeScript::start()
{
	// �ʿ��� �͵��� �ʱ�ȭ�����ش�.
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

	// ���� ������Ʈ �±װ� �÷��̾��϶�
	if (other.gameObject->getTag() == "Player")
	{
		// Ÿ���� ������
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
			// ����
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
	// ����ī�޶󿡼� ������� �ֽ�ȭ ��Ų��.
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
	// ��ġ �ݴ�� ���
	D3DXMatrixMultiply(&v, &v, D3DXMatrixTranslation(&temp, -transform->getPosition().getX(),
		-transform->getPosition().getY(), -transform->getPosition().getZ()));

	// �����̼� �ݴ�� ���
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


	//// ��ġ �ݴ�� ���
	//D3DXMatrixMultiply(v, v, D3DXMatrixTranslation(&temp, -transform->getPosition().getX(),
	//	-transform->getPosition().getY(), -transform->getPosition().getZ()));

	//// �����̼� �ݴ�� ���
	//D3DXMatrixMultiply(v, v, D3DXMatrixRotationX(&temp, -transform->getRotation().getX()));
	//D3DXMatrixMultiply(v, v, D3DXMatrixRotationY(&temp, -transform->getRotation().getY()));
	//D3DXMatrixMultiply(v, v, D3DXMatrixRotationZ(&temp, -transform->getRotation().getZ()));

	//-------------------------------------------------------------------------------------------------

	// Keep camera's axes orthogonal to eachother
	
	////���� ��ǥ�� �������� �Ѵٸ� ���� �ʿ���� �� ����.
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

	// X������ �д´�.
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

	// ������ ������ ���������� ó��
	setMtrlsAndTextures(mtrlBuffer, numMtrls);

	// ���Ǿ��� �������۸� ����ش�.
	if (mtrlBuffer)
		mtrlBuffer->Release();


	optimizeMesh(adjBuffer);

	// ���Ϸ��� �������� ������
	if (adjBuffer)
		adjBuffer->Release();
}

void MeshRenderer::render()
{
	if (!mesh) return;

	// ���� transform��ķ� �����Ų��.
	device->SetTransform(D3DTS_WORLD, &transform->getTransformMatrix());

	// ������ �ε�Ǿ ���� �ִ� ��츸 �׷��ش�.
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

	// D3DXMATERAL �������� �������� Ÿ��ĳ������ �ʿ��ϴ�.
	D3DXMATERIAL* pMtrls = (D3DXMATERIAL*)mtrlBuffer->GetBufferPointer();

	for (int i = 0; i < numMtrls; i++)
	{
		// �ε�ɶ� ambient���� ������ �����Ƿ� �̸� �����Ѵ�.
		pMtrls[i].MatD3D.Ambient = pMtrls[i].MatD3D.Diffuse;

		mtrls.push_back(pMtrls[i].MatD3D);

		if (pMtrls[i].pTextureFilename != 0)
		{
			// �����̸��� ���������� �� �������� �� ���ϸ����� ������ ��� �ؽ�ó�� �ε��Ѵ�.
			IDirect3DTexture9* tex = 0;
			D3DXCreateTextureFromFile(
				device,
				pMtrls[i].pTextureFilename,
				&tex);

			// �ؽ�ó�� ������ �� �ؽ�ó�� �־��ְ�
			textures.push_back(tex);
		}
		else
		{
			// �������� ����°� ���� �ε��� ��ȣ�� �����ֱ� ���ؼ� �ΰ��� �־��ش�.
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

	// �ǽð����� �ٷ� ������Ʈ�ϱ� ������ ��Ƽ�� �����Ѱ� ������Ʈ�Ѵ�.
	// ����������Ʈ�� �Ǿ��� ������ ��Ƽ�÷��׸� ���ش�.
	//transform->setDirty(true);

}

void RigidBody::update()
{
	if (colliderInfo.type == ColliderType::NONE) return;

	Vector3 worldPos = transform->getWorldPosition();
	// �ݶ��̴��� �׷��ش�.
	switch (colliderInfo.type)
	{

	case ColliderType::BOX:
	/*
	���� ��ǥ��������� �׷�����Ѵ�
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
	// ���������� ������ �ִ� colliderInfo�� �ٲ��ش�.
	colliderInfo.type = ColliderType::BOX;
	colliderInfo.size = size;
}

void RigidBody::setSphereCollider(float radius)
{
	gameObject->getPhysics().setSphereCollider(gameObject, radius);
	// ���������� ������ �ִ� colliderInfo�� �ٲ��ش�.
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
		// ������ ������			
		// �ִϸ��̼� �Ŵ�����������
		// �ִϸ��̼��� ��µǾ�� �Ҷ�
		// ���������� ó���ϱ� ������ �������Ͱ� getIsAnimating�� ȣ���� ���� ����.
		// ó���ϴٰ� false�Ǹ�� �ٷ� ��������
		if (skeletonBones && animations && animations->getIsAnimating())
		{
			animations->update();
			// �ִϸ��̼������� ������ ���(������Ʈ���� ���)
			processSkeletonBonesAnimation(animations);

			// ������ ����ġ�� ��ģ ���� ���ؽ� ��Ʈ���� ���
			fbxMeshes[i]->processVertexAnimationMatrix(skeletonBones);


			// FbxModelMesh(mesh�� �ε��� 1:1 ��Ī)�� �����ؼ�(vectorMyVertex)
			// �������� mesh�� ���ؽ� ���۸� ��� �������� ��ġ ������ �ٽ� �־��ش�.
			//processVerticesWithAnimation(meshes[i], fbxMeshes[i]);
			

			// �ִϸ��̼��� ������ �ִϸ��̼� ��Ʈ������ ��� ���ϱ� ������ �ֽ�ȭ�����ش�.



			// FbxModelMesh�� �� ���ؽ����� MyFbxVertex->getPositionWithAnimation�ؼ� �ʱ�ȭ 
			//  position * animationmatrix // 	
			// ���������� ����ġ ���͸� ���� ���� ���ؼ� ���� �ִϸ��̼� ��Ʈ���� ���


			// ���࿡ �ִϸ��̼��� ����ٸ� ��� �޽����� processVertice�Լ��� // �ʱⰪ���� �������ش�.
		}

		processVertices(meshesForShading[i], fbxMeshes[i]);


		
		//material, texture-----------------------------------------------------
		// �ϴ� �ʿ� ��ϵǾ��ִ��� Ȯ���Ѵ�.
		tempMaterialForMesh = materialsTable.find(meshesForShading[i]);
		tempTexturesForMesh = texturesTable.find(meshesForShading[i]);

		material = nullptr;
		texture = nullptr;

		// materialó��
		// �ʿ� ��ϵǾ��ִٸ� ���Ϳ� ���� ����ִ��� Ȯ���Ѵ�.
		if (tempMaterialForMesh != materialsTable.end())
		{
			// ����Ʈ(vector)�� ���� �� �ִٸ� ù��° ������ �ʱ�ȭ �Ѵ�.
			if ((*tempMaterialForMesh).second.size() >= 1)
			{
				material = (&(*tempMaterialForMesh).second[0]);
			}
		}

		// textureó��
		// �ʿ� ��ϵǾ��ִٸ� ���Ϳ� ���� ����ִ��� Ȯ���Ѵ�.
		if (tempTexturesForMesh != texturesTable.end())
		{
			// ����Ʈ(vector)�� ���� �� �ִٸ� ù��° ������ �ʱ�ȭ �Ѵ�.
			if ((*tempTexturesForMesh).second.size() >= 1)
			{
				texture = (*tempTexturesForMesh).second[0];
			}
		}




		// ���̴� ���� // ���������� �� ��굵 �� ���ش�.
		device->SetVertexShader(fbxModelRendererWithAnimationShader);
		device->SetVertexDeclaration(declaration);


		// �ҽ� ����
		device->SetStreamSource(0, meshesForShading[i]->vb, 0, sizeof(FbxModelVertex));
		device->SetIndices(meshesForShading[i]->ib);

		D3DXMATRIX viewMatrix;
		D3DXMATRIX projectionMatrix;

		// ���� ī�޶� ���� �ֽ�ȭ
		device->GetTransform(D3DTS_VIEW, &viewMatrix);
		device->GetTransform(D3DTS_PROJECTION, &projectionMatrix);


		//gameObject->getScene().getMainCamera()->getComponent<MainCamera>()->getViewMatrix(&viewMatrix);
		/*���̴� �Ķ���� �ʱ�ȭ*/
		// worldViewProjection �ʱ�ȭ
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

		// �ؽ�ó ����



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
		���߿� ���̴� �ڵ��� ����ü
		struct VS_INPUT
		{
		vector positi : POSITION // POSITION0
		: NORMAL0
		: NORMAL1
		: NORMAL2
		}
		�� ���εȴ�.
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

		// ������ ��Ʈ����
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


	// ���̴� ������ / �ڵ� �޾ƿ���
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
		// �ؽ�ó ���� ����
		// ��Ƽ���� ���� �׳� �����ؼ� �����߱� ������ ���� �Ű�Ƚᵵ�ȴ�.
		for (auto it = texturesTable.begin(); it != texturesTable.end(); ++it)
		{
			// ���� ���鼭 �ؽ�ó ���͸� �޾Ƽ� ��� ������ �����ش�.
			// second���� ����ü�� ���� ��ȯ�ϴ� ���̱� ������ ���� ���簡 �ȵȴ�. 
			// ���� ���簡 �ȴٰ� �ص� ��������ü���� ���ؼ� ������ �ϱ� ������ ��������ϴ�.
			/*vector<IDirect3DTexture9*> & temp = */

			for (auto it2 = (*it).second.begin(); it2 != (*it).second.end(); ++it2)
			{
				if ((*it2) != nullptr)
					(*it2)->Release();
			}

		}

		// �޽� ���� ����
		for (auto it = meshesForShading.begin(); it != meshesForShading.end(); ++it)
		{
			if ((*it) != nullptr)
				delete (*it);
			(*it) = nullptr;
		}
		meshesForShading.clear();

		// fbx �޽� ���� ����
		for (auto it = fbxMeshes.begin(); it != fbxMeshes.end(); ++it)
		{
			if ((*it) != nullptr)
				delete (*it);
			(*it) = nullptr;
		}
		fbxMeshes.clear();

		// ���̷��� �� ���� ����
		if (skeletonBones)
			delete skeletonBones;

		if (animations)
			delete animations;

		if (importer)
			importer->Destroy();

		if (scene)
			scene->Destroy();

		//d3d::Release<ID3DXMesh*>(mesh);

		// �ؽ�ó�� �Ҵ�� ���̰� ������ ����� ���̴�.
		//for (int i = 0; i < textures.size(); i++)
		//	d3d::Release<IDirect3DTexture9*>(textures[i]);
	}
}

FbxModelRenderer::FbxModelRenderer(GameObject * go, Transform * tf)
	: Component(go, tf), scene(nullptr), importer(nullptr), skeletonBones(nullptr), animations(nullptr),
	scaleFactor(1,1,1),
	// ���̴�
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
	// ��Ʈ ��带 �޾ƿ´�.
	//FbxNode * rootNode = (*fbxInfo.getFbxScene())->GetRootNode();


	/*
	1. ���̷��溻�� �ʱ�ȭ
	2. �޽� �ʱ�ȭ // �ʱ�ȭ �������� ���̷��� ���� �����Ѵٸ� ���ؽ����� �� ����ġ�� �־��ش�.
	*/
	processAllNodesWithSameAtrributeTypeFromRoot(rootNode, FbxNodeAttribute::eSkeleton);
	processAllNodesWithSameAtrributeTypeFromRoot(rootNode, FbxNodeAttribute::eMesh);

	// �ִϸ��̼��� ������� ���� ������ �ִϸ��̼� ����
	processAnimation(fileName,scene, importer);

	//skeletonBones->printAllInfo();



	//getAllFbxModelMeshesFromRoot(rootNode);

	// ��带 ��ȸ�ϸ鼭 �޽��������� �ʱ�ȭ�ϰ�
	//fbxInfo.loadMeshFromNodes(rootNode);

	// ���⼭ processControlPoints�� processVertices���� ó��
	// mesh��ü�� FbxModelMesh�� ������ �־ ó��
	for (auto it = fbxMeshes.begin(); it != fbxMeshes.end(); ++it)
	{
		/// �޽������κ��� ��Ʈ�� ����Ʈ�� �ʱ�ȭ �Ѵ�.
		///*��������*/(*it)->processControlPoints();
		/// ���������� �޽�����, ��Ʈ�� ����Ʈ�� ���ؽ� / �ε����� �ʱ�ȭ �Ѵ�.
		///*��������*/(*it)->processVertices();

		// �̸� �ʱ�ȭ�� FbxMesh�� ������ fbxModelMesh���� ���� �ʿ��� ���� ���� �Ϸ� // fbxModelMeshes�� �������
		//---------------------------------------------------------------------------
		// ������ ������ DirectX���� �����ϴ� mesh�������� ��ȯ
		MeshForShading * meshForShading = new MeshForShading;
		//ID3DXMesh * mesh = nullptr;
		HRESULT hr = 0;
		meshForShading->indexCount = (*it)->getIndexCount();
		meshForShading->vertexCount = (*it)->getVertexCount();
		meshForShading->triCount = (*it)->getTriCount();

		hr = device->CreateVertexBuffer(
			meshForShading->vertexCount * sizeof(FbxModelVertex),
			D3DUSAGE_WRITEONLY,

			// FVF�� ���ؽ� ���� ������ ����ϱ� ������ ������� �ʴ´�.
			// ���ؽ� ���� ������ FVF���� Ȯ��� �����̶�� ����ȴ�.
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

		// 32��Ʈ �̹Ƿ� ���߿� ���� �� DWORD�� �о�� �Ѵ�.
		device->CreateIndexBuffer(
			// 2���� �ﰢ���� �̷�� ���� �ε����� ������ 6���̴�.
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



		// mesh������ ���ؽ� ������ �ε��� ������ �ʱ�ȭ �����ش�.
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

	// ��� ��带 ���鼭 Mesh �Ӽ��� �����͵��� �����Ѵ�.
	// ���ǹ��� �տ������� �˻��ϴٰ� ������ �ٷ� false��ȯ�ϱ� ������ �̷������� �ڵ��� �����ϴ�
	if (nodeAtrribute && nodeAtrribute->GetAttributeType() == attributeType)
	{
		switch (attributeType)
		{
			case FbxNodeAttribute::eMesh:
			{
				FbxModelMesh * temp = new FbxModelMesh();
				// ��ȯ�۾��� ���ش�. // ��Ʈ������Ʈ�� �̷���� ��� �͵��� �ﰢ��2���� �������ش�.
				gameObject->getFbxParser().convertGeometryInfo(&nodeAtrribute);


				// ��带 �ʱ�ȭ�����ְ�
				// ���߿� ��带 �޾Ƽ� ����ؾ��Ѵ� // processVertices����
				temp->setNode(node);


				// ���������� ���μ������ش� 
				// ��Ʈ������Ʈ �����ޱ� 
				temp->processControlPoints();

				// �ռ� ���̷��� �ʱ�ȭ����(������ ������ ���ؽ� �ʱ�ȭ�� �ʿ�) ���ؽ��� �ʱ�ȭ
				// ���̷��溻���� ������ �ʿ��ϱ� ������ �Ѱ��ش� // ����ġ ����ؾ��Ѵ�. �����ε����ʿ�
				temp->processVertices(skeletonBones);
				fbxMeshes.push_back(temp);
			}
			break;

			case FbxNodeAttribute::eSkeleton:
			{
				FbxSkeleton * fbxSkeleton = node->GetSkeleton();
				if (!fbxSkeleton) break;

				// ���ʷ� ���̷��水ü�� �޾ƿ����� ���̷��溻 �Ŵ����� �����ȴ�.
				if (skeletonBones == nullptr) skeletonBones = new FbxModelSkeletonBones();

				// ��Ʈ���� �θ��� �ε����� -1�̰� �̸��� NONE�̴�
				int parentBoneIndex = -1;
				string parentBoneName = "NONE";
				FbxNode * parentNode = node->GetParent();
				if (parentNode)
				{
					// �θ����� ���̷����� �����Ҷ��� ó�����ش�. 
					// �ƴѰ��� �θ����� ���̷��� ����X = �θ��� ����
					// �� �ʱ� �����ص״� index : -1 / name : NONE ���� �����ȴ�.
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

	// �ڽĵ��� ���鼭 ������ �޽��� ã�´�.
	const int childCount = node->GetChildCount();
	for (unsigned int i = 0; i < childCount; ++i)
	{
		processAllNodesWithSameAtrributeTypeFromRoot(node->GetChild(i), attributeType);
	}
}

void FbxModelRenderer::processAnimation(const string & animationFileName, FbxScene * _scene, FbxImporter * _importer)
{
	// �ܺο��� ������ �޾ƿ��� ��� ���� ���� �����͸� �����ؼ� �Ѱ��ָ� ���⼭ �˾Ƽ� ó���ǵ��� �Ѵ�.

	if (!_scene || !_importer) return;

	FbxNode * rootNode = _scene->GetRootNode();
	if (!rootNode) return;

	if (!animations) animations = new FbxModelAnimations();

	// 1�ʿ� ������������ �˼� �ִ�
	float frameRate = FbxTime::GetFrameRate(scene->GetGlobalSettings().GetTimeMode());

	FbxArray<FbxString *> animationArray;
	FbxDocument * document = dynamic_cast<FbxDocument *>(_scene);
	if (document)
		document->FillAnimStackNameArray(animationArray);

	// �ִϸ��̼� ��ŭ
	for (int i = 0; i < _importer->GetAnimStackCount(); ++i)
	{
		FbxTakeInfo * animationInfo = _importer->GetTakeInfo(i);
		if (!animationInfo) continue;

		// fbxstring�����̱� ������ �������۸� ���ؼ� �޾ƿ´�
		string animationName = animationInfo->mName.Buffer();

		FbxTimeSpan span = animationInfo->mLocalTimeSpan;

		// �ʴ����� �޾ƿ´� // ���۽ð��� ���ð�
		// ������ ����Ʈ�� �˰��ֱ� ������ �������ӵ��� �� �ִϸ��̼��� ����Ǵ��� �� �� �ִ�.
		double startTime = span.GetStart().GetSecondDouble();
		double stopTime = span.GetStop().GetSecondDouble();

		if (startTime < stopTime)
		{
			// �� * 1�ʴ� �����Ӽ� = ��ü Ű������ ���� ���´�.
			int keyFrames = (stopTime - startTime) * frameRate;
			FbxModelAnimation * animation = new FbxModelAnimation(animationFileName, animationName, keyFrames, frameRate);
			animations->addAnimation(animation);

			// �ִϸ��̼��� �߰������� �� ���� ���鼭 �߰��� �ִϸ��̼ǿ� ���� Ű������ ��Ʈ������ ��� ���ؼ� �߰����ش�.
			processKeyFramesOfAllNodes(rootNode, animationFileName, animationName, frameRate, startTime, stopTime);

		}
	}

	animationArray.Clear();

}

void FbxModelRenderer::processKeyFramesOfAllNodes(FbxNode * node, const string & animationFileName, const string & animationName, float frameRate, float start, float stop)
{
	if (!node) return;

	FbxNodeAttribute * nodeAttribute = node->GetNodeAttribute();
	// ���������� �˻��ϴٰ� �ƴϸ� ���̻�˻� ���ϱ� ������ ������.
	if (nodeAttribute && nodeAttribute->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	{
		FbxModelSkeletonBone * bone = nullptr;
		if (skeletonBones)
		{
			string boneName = node->GetName();
			bone = skeletonBones->findBoneWithBoneName(boneName);
		}

		// ������ �� �����Ӻ��� ������ ���� Ʈ�������� ������.
		// �ܺ� �ִϸ��̼� ������ �����ö��� �Ȱ��� ���μ����� ��ģ��. �� ���� �̸��� ���ƾ� �Ѵ�.
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

	// �ڽĺ��鵵 ��������� ó�����ش�.
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
//	// mesh�� �־��ٶ� fbxVertex�������� �־��ٰ��̴�.
//	FbxModelVertex * fbxV = nullptr;
//	MyFbxVertex * tVertex = nullptr;
//	
//	//int t1 = mesh->GetNumVertices();
//	//int t2 = fbxModelMesh->getVertexCount();
//
//	// ������ �޽��� ��װ� �׾ȿ� ���ؽ� ������ ����ִ´�.
//	// ���ؽ� ������ ������ / �븻 / UV���̴�.
//	mesh->LockVertexBuffer(0, (void**)&fbxV);
//	for (int i = 0; i < fbxModelMesh->getVertexCount(); ++i)
//	{
//		tVertex = fbxModelMesh->getVertex(i);
//		
//		// ���� ������
//		if (tVertex)
//		{
//			const D3DXMATRIX & tempAnimationMatrix = tVertex->getAnimationMatrix();
//			// ������ ������ �ʱ�ȭ �����ش�.
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
//		// ���� ������ // nullptr�϶�
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
//		// ���� ������
//		if (tVertex)
//		{
//			temp = tVertex->getPositionWithAnimation();
//			// ���������� �ִϸ��̼� ����� ���� �͵��� ó�����ش�.
//			// �ִϸ��̼� ����� ���ϱ� ���� ������ ����ġ�� ���� �ִϸ��̼� ����� ���������� ������ش�.
//			//fbxV[i][0] = (float)tVertex->getPositionWithAnimation().x / 10;
//			//fbxV[i][1] = (float)tVertex->getPositionWithAnimation().y / 10;
//			//fbxV[i][2] = (float)tVertex->getPositionWithAnimation().z / 10;
//			// ������ ������ �ʱ�ȭ �����ش�.
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
//			// �븻���̳� UV���� �ȹٲ㵵 �ɱ�..
//			// UV���� ������ ���� �ؽ�ó ��ǥ�̱⶧���� ��������Ͱ����� �븻���� �޶������ϴ°žƴұ�?
//		}
//		// ���� ������ // �ƹ��͵� �������ʴ´�.
//	}
//
//
//	mesh->UnlockVertexBuffer();
//}

void FbxModelRenderer::processVertices(MeshForShading * mesh, FbxModelMesh * fbxModelMesh)
{

	if (!mesh || !fbxModelMesh) return;
	// mesh�� �־��ٶ� fbxVertex�������� �־��ٰ��̴�.
	FbxModelVertex * fbxV = nullptr;
	MyFbxVertex * tVertex = nullptr;

	//int t1 = mesh->GetNumVertices();
	//int t2 = fbxModelMesh->getVertexCount();

	// ������ �޽��� ��װ� �׾ȿ� ���ؽ� ������ ����ִ´�.
	// ���ؽ� ������ ������ / �븻 / UV���̴�.
	mesh->vb->Lock(0,0, (void**)&fbxV,0);
	for (int i = 0; i < fbxModelMesh->getVertexCount(); ++i)
	{
		tVertex = fbxModelMesh->getVertex(i);

		//*************** ������ ��Ʈ������ ���ؼ� ���� ��ġ�� ����� ������ �������� �������� �ʴ´�.
		//*************** ������ ��Ʈ������ ���ؼ� ���� ��ġ�� ����� ������ �������� �������� �ʴ´�.
		//*************** ������ ��Ʈ������ ���ؼ� ���� ��ġ�� ����� ������ �������� �������� �ʴ´�.
		// ������� �������� 1/10���� �����ϰ� �ִϸ��̼� ����� ���ϴ� �Ͱ� 
		// �ִϸ��̼������ ���ϰ� 1/10���� �����ϴ� ���� �ٸ���.

		// ���� ������
		if (tVertex)
		{
			//D3DXVECTOR3 temp = tVertex->getPositionWithAnimation();
			 const D3DXMATRIX & tempAnimationMatrix = tVertex->getAnimationMatrix();

			// ������ ������ �ʱ�ȭ �����ش�.
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
		// ���� ������ // nullptr�϶�
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

	// mesh�� �־��ٶ� fbxVertex�������� �־��ٰ��̴�.
	unsigned long * fbxI = nullptr;
	int tIndex = -1;

	//int t1 = mesh->GetNumFaces();
	//int t2 = fbxModelMesh->getIndexCount();
	//int t3 = fbxModelMesh->getVertexCount();
	//int t4 = fbxModelMesh->getTriCount();

	// ������ �޽��� ��װ� �׾ȿ� ���ؽ� ������ ����ִ´�.
	// ���ؽ� ������ ������ / �븻 / UV���̴�.
	mesh->ib->Lock(0,0, (void**)&fbxI,0);
	for (int i = 0; i < fbxModelMesh->getIndexCount(); ++i)
	{

		tIndex = fbxModelMesh->getIndex(i);
		// ���� ������
		if (tIndex != -1)
		{
			// ������ ������ �ʱ�ȭ �����ش�.
			fbxI[i] = tIndex;
			continue;
		}
		// ���� ������
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

	// ���� ����� �ȵǾ������� ������ش�.
	if (materials_it == materialsTable.end())
	{
		materialsTable[mesh] = vector<D3DMATERIAL9>{};
	}
	if (textures_it == texturesTable.end())
	{
		texturesTable[mesh] = vector<IDirect3DTexture9*>{};
	}

	// ������ �ڵ����� �����ϴ� []������������
	// �ִٸ� �� value���� ��ȯ�Ѵ� �ռ� ������ ��� ����� ���ػ����̹Ƿ� []�� �����ص� ���Ӱ� �Ҵ�Ǵ� ���� ����.
	// ���۷��� ������ ��ȯ�ϱ� ������ ���۷����� ������ �����ؼ� ������ �� �ִ�.
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

			// material �ʱ�ȭ
			FbxProperty prop{};
			D3DXMATERIAL tempMtrl{};
			FbxDouble3 result(0, 0, 0);

			prop = material->FindProperty(FbxSurfaceMaterial::sAmbient);
			result = prop.Get<FbxDouble3>();
			D3DCOLORVALUE ambient = { result.mData[0],result.mData[1],result.mData[2] };

			prop = material->FindProperty(FbxSurfaceMaterial::sSpecular);
			result = prop.Get<FbxDouble3>();
			D3DCOLORVALUE specular = { result.mData[0],result.mData[1],result.mData[2] };

			// �ؽ�ó�� ��ǻ� ����Ұ��̱� ������ ���������� ó�����ִ� �κ��� ��ǻ���̴�.
			// �� prop�� �̿��ؼ� �ؽ�ó�� �޾ƿð��̴�.
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

						// ó������ �ٷ� ���� ��θ� �޾Ƽ� �����

						IDirect3DTexture9* tex = nullptr;
						D3DXCreateTextureFromFile(
							device,
							textureName.c_str(),
							&tex);

						// �ؽ�ó�� ������ �� �ؽ�ó�� �־��ְ�
						if (tex)
						{
							textures.push_back(tex);
							continue;
						}


						// ���� ��η� ���������� ���� ���
						// �̹� �����ص� ��θ� ������ Ȯ���غ���.

						// ���� �����̸��� �޾ƿ��� > �״��� �����ص� ��θ� ���ϴ� �۾��� ���ش�.
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
					// ó������ �ٷ� ���� ��θ� �޾Ƽ� �����

					IDirect3DTexture9* tex = nullptr;
					D3DXCreateTextureFromFile(
						device,
						textureName.c_str(),
						&tex);

					// �ؽ�ó�� ������ �� �ؽ�ó�� �־��ְ�
					if (tex)
					{
						textures.push_back(tex);
						continue;
					}


					// ���� ��η� ���������� ���� ���
					// �̹� �����ص� ��θ� ������ Ȯ���غ���.

					// ���� �����̸��� �޾ƿ��� > �״��� �����ص� ��θ� ���ϴ� �۾��� ���ش�.
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
//	// mesh�� �־��ٶ� fbxVertex�������� �־��ٰ��̴�.
//	unsigned short * fbxI = nullptr;
//	int tIndex = -1;
//
//	//int t1 = mesh->GetNumFaces();
//	//int t2 = fbxModelMesh->getIndexCount();
//	//int t3 = fbxModelMesh->getVertexCount();
//	//int t4 = fbxModelMesh->getTriCount();
//
//	// ������ �޽��� ��װ� �׾ȿ� ���ؽ� ������ ����ִ´�.
//	// ���ؽ� ������ ������ / �븻 / UV���̴�.
//	mesh->LockIndexBuffer(0, (void**)&fbxI);
//	for (int i = 0; i < fbxModelMesh->getIndexCount(); ++i)
//	{
//
//		tIndex = fbxModelMesh->getIndex(i);
//		// ���� ������
//		if (tIndex != -1)
//		{
//			// ������ ������ �ʱ�ȭ �����ش�.
//			fbxI[i] = tIndex;
//			continue;
//		}
//		// ���� ������
//		fbxI[i] = 0;
//	}
//	mesh->UnlockIndexBuffer();
//}

//if (mtrlBuffer == 0 || numMtrls == 0) return;
//
//// D3DXMATERAL �������� �������� Ÿ��ĳ������ �ʿ��ϴ�.
//D3DXMATERIAL* pMtrls = (D3DXMATERIAL*)mtrlBuffer->GetBufferPointer();
//
//for (int i = 0; i < numMtrls; i++)
//{
//	// �ε�ɶ� ambient���� ������ �����Ƿ� �̸� �����Ѵ�.
//	pMtrls[i].MatD3D.Ambient = pMtrls[i].MatD3D.Diffuse;
//
//	mtrls.push_back(pMtrls[i].MatD3D);
//
//	if (pMtrls[i].pTextureFilename != 0)
//	{
//		// �����̸��� ���������� �� �������� �� ���ϸ����� ������ ��� �ؽ�ó�� �ε��Ѵ�.
//		IDirect3DTexture9* tex = 0;
//		D3DXCreateTextureFromFile(
//			device,
//			pMtrls[i].pTextureFilename,
//			&tex);
//
//		// �ؽ�ó�� ������ �� �ؽ�ó�� �־��ְ�
//		textures.push_back(tex);
//	}
//	else
//	{
//		// �������� ����°� ���� �ε��� ��ȣ�� �����ֱ� ���ؼ� �ΰ��� �־��ش�.
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
//	// ���� ����� �ȵǾ������� ������ش�.
//	if (materials_it == materialsTable.end())
//	{
//		materialsTable[mesh] = vector<D3DMATERIAL9>{};
//	}
//	if (textures_it == texturesTable.end())
//	{
//		texturesTable[mesh] = vector<IDirect3DTexture9*>{};
//	}
//
//	// ������ �ڵ����� �����ϴ� []������������
//	// �ִٸ� �� value���� ��ȯ�Ѵ� �ռ� ������ ��� ����� ���ػ����̹Ƿ� []�� �����ص� ���Ӱ� �Ҵ�Ǵ� ���� ����.
//	// ���۷��� ������ ��ȯ�ϱ� ������ ���۷����� ������ �����ؼ� ������ �� �ִ�.
//	vector<D3DMATERIAL9> & materials = materialsTable[mesh];
//	vector<IDirect3DTexture9*> & textures = texturesTable[mesh];
//
//	//	if (pMtrls[i].pTextureFilename != 0)
//	//	{
//	//		// �����̸��� ���������� �� �������� �� ���ϸ����� ������ ��� �ؽ�ó�� �ε��Ѵ�.
//	//		IDirect3DTexture9* tex = 0;
//	//		D3DXCreateTextureFromFile(
//	//			device,
//	//			pMtrls[i].pTextureFilename,
//	//			&tex);
//	//
//	//		// �ؽ�ó�� ������ �� �ؽ�ó�� �־��ְ�
//	//		textures.push_back(tex);
//	//	}
//	//	else
//	//	{
//	//		// �������� ����°� ���� �ε��� ��ȣ�� �����ֱ� ���ؼ� �ΰ��� �־��ش�.
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
//			// material �ʱ�ȭ
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
//			// �ؽ�ó�� ��ǻ� ����Ұ��̱� ������ ���������� ó�����ִ� �κ��� ��ǻ���̴�.
//			// �� prop�� �̿��ؼ� �ؽ�ó�� �޾ƿð��̴�.
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
//						// ó������ �ٷ� ���� ��θ� �޾Ƽ� �����
//
//						IDirect3DTexture9* tex = nullptr;
//						D3DXCreateTextureFromFile(
//							device,
//							textureName.c_str(),
//							&tex);
//
//						// �ؽ�ó�� ������ �� �ؽ�ó�� �־��ְ�
//						if (tex)
//						{
//							textures.push_back(tex);
//							continue;
//						}
//
//
//						// ���� ��η� ���������� ���� ���
//						// �̹� �����ص� ��θ� ������ Ȯ���غ���.
//
//						// ���� �����̸��� �޾ƿ��� > �״��� �����ص� ��θ� ���ϴ� �۾��� ���ش�.
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
//					// ó������ �ٷ� ���� ��θ� �޾Ƽ� �����
//
//					IDirect3DTexture9* tex = nullptr;
//					D3DXCreateTextureFromFile(
//						device,
//						textureName.c_str(),
//						&tex);
//
//					// �ؽ�ó�� ������ �� �ؽ�ó�� �־��ְ�
//					if (tex)
//					{
//						textures.push_back(tex);
//						continue;
//					}
//
//
//					// ���� ��η� ���������� ���� ���
//					// �̹� �����ص� ��θ� ������ Ȯ���غ���.
//
//					// ���� �����̸��� �޾ƿ��� > �״��� �����ص� ��θ� ���ϴ� �۾��� ���ش�.
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
//	// �ٽ� �Ӽ� �����͸� �����ϱ� ���ؼ� ���۸� ����ش�.
//	mesh->LockAttributeBuffer(0, &attributeBuffer);
//
//	int ttt = mesh->GetNumFaces();
//
//	// ������� 0������ �������ش�.
//	for (int i = 0; i < mesh->GetNumFaces(); ++i)
//		attributeBuffer[i] = 0;
//
//
//	// ������ �Ϸ�Ǿ����Ƿ� �ٽ� ����ش�.
//	mesh->UnlockAttributeBuffer();
//}

//void FbxModelRenderer::optimizeMesh(ID3DXMesh * mesh)
//{
//	if (!mesh) return;
//
//	// �������۸� �̿��� ����ȭ �۾�
//	vector<unsigned long> adjacencyBuffer(mesh->GetNumFaces() * 3);
//	// �޽��� ���������� �޾ƿ´�.
//	mesh->GenerateAdjacency(0.0f, &adjacencyBuffer[0]);
//
//
//	int faceCount1 = mesh->GetNumFaces();
//	int vertexCount1 = mesh->GetNumVertices();
//
//	HRESULT hr = 0;
//	// �� ���� ������ ������� ����ȭ �۾��� �����Ѵ�.
//	hr = mesh->OptimizeInplace
//	(
//		// �Ӽ�����(����±���) �ﰢ������ �����ϰ�, ������ �Ӽ����̺��� ����
//		// GetAttributeTable�Լ��� �̿��ؼ� D3DXATTRIBUTERANGE����ü �迭�� �޾ƿ� �� �ִ�.
//		// ���� �������� �����ID / �� ��� ���ؽ��� �� / �� ���� ������ ����ִ�.
//		// �Ӽ� ���̺��� ���׸��� �޽��� �� ����°� �����Ǹ�, 
//		// ������� ������������ �����Ǵ� ���ؽ�/ �ε��� ���۳��� �޸� ����� �����Ѵ�.
//		D3DXMESHOPT_ATTRSORT |
//		// �޽����� �̿����� �ʴ� �ε����� ���ؽ��� �����Ѵ�.
//		D3DXMESHOPT_COMPACT |
//		// ���ؽ� ĳ���� ��Ʈ���� ���δ�.
//		D3DXMESHOPT_VERTEXCACHE,
//
//		// ����ȭ �������� �޽��� ���� �迭 ������
//		// �����迭�� �ʿ�������?	// ����ȭ ��Ͽ� ���������� �ʿ��� �κ��� �־ �ʿ��ҵ�
//		&adjacencyBuffer[0],
//		// ����ȭ�� �޽��� ���� �迭 ������
//		nullptr,
//		// �������� Face
//		nullptr,
//		// �������� Vertex
//		nullptr
//	);
//
//	int faceCount = mesh->GetNumFaces();
//	int vertexCount = mesh->GetNumVertices();
//}

void AnimationFSM::start()
{
	// ���� ������Ʈ�� ������ �ʱ�ȭ �ȴ�.
	fbxModelRenderer = gameObject->getComponent<FbxModelRenderer>();

	// FbxModelRenderer�� ������Ʈ�� �ִ� ��� �ִϸ��̼� �ų����� �޾ƿ´�.
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

	// ������ �ִϸ��̼��� �����ϴ��� ���̺��� ���ؼ� Ȯ��
	if (animationFSM->stateTable.find(from) == animationFSM->stateTable.end()
		|| animationFSM->stateTable.find(to) == animationFSM->stateTable.end())
		return false;


	float value = 0.0f;
	float valueToCompare = 0.0f;
	bool result = false;

	// Ÿ�Կ� ���� ���� �޾ƿ´�.
	if (type == ValueType::BOOLTYPE)
	{
		// ���� ��ü�� �Ǵ� ����� ���̺� ��ϵǾ��ִ��� Ȯ���غ��� ���� �޾ƿ´�.
		auto it = animationFSM->boolTable.find(valueName);
		if (it == animationFSM->boolTable.end()) return false;

		value = it->second;
		valueToCompare = (float)bValueToCompare;
	}
	else //if (type == ValueType::FLOATTYPE)
	{
		// ���� ��ü�� �Ǵ� ����� ���̺� ��ϵǾ��ִ��� Ȯ���غ��� ���� �޾ƿ´�.
		auto it = animationFSM->floatTable.find(valueName);
		if (it == animationFSM->floatTable.end()) return false;

		value = it->second;
		valueToCompare = fValueToCompare;
	}

	// factor�� ���� �񱳿����� ����
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

	// ���࿡ ����� �ȵǾ��ִٸ� stateTable�� ����Ѵ�.
	map<string, unsigned int>::iterator stateIt = stateTable.find(animationFileName);

	if (stateIt == stateTable.end())
	{
		// �ε����� ����� ������� ���ʴ�� ���ش�.
		stateTable[animationFileName] = stateTable.size();
	}
}

void AnimationFSM::setDefaultState(const string & animationFileName)
{
	if (!fbxModelRenderer || !fbxModelAnimations) return;

	map<string, unsigned int>::iterator stateIt = stateTable.find(animationFileName);

	// ���࿡ ����� �ȵǾ� �ִٸ� �����Ѵ�.
	if (stateIt == stateTable.end())
	{
		defaultState = "";
		return;
	}
	
	// ���࿡ ��ϵ� ��Ȳ�̶�� ����Ʈ ���¸� �������ش�.
	defaultState = animationFileName;
	currentState = defaultState;
}

void AnimationFSM::registerFloat(const string & floatName)
{
	map<string, float>::iterator floatIt = floatTable.find(floatName);
	// �̵̹�ϵǾ� �ִٸ� ����
	if (floatIt != floatTable.end())
		return;

	floatTable[floatName] = 0.0f;
}

void AnimationFSM::registerBool(const string & boolName)
{
	map<string, bool>::iterator boolIt = boolTable.find(boolName);
	// �̵̹�ϵǾ� �ִٸ� ����
	if (boolIt != boolTable.end())
		return;

	boolTable[boolName] = false;
}

void AnimationFSM::setFloat(const string & floatName, float input)
{
	map<string, float>::iterator floatIt = floatTable.find(floatName);
	// �̵̹�ϵǾ� ���� �ʴٸ� ���Ӱ� ������ְ� input������ �ʱ�ȭ �����ش�.
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
	// �̵̹�ϵǾ� ���� �ʴٸ� ���Ӱ� ������ְ� input������ �ʱ�ȭ �����ش�.
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
	// �ߺ��� Ű���� ���� �� �ֱ� ������ �׳� ����Ѵ�. ã�ƺ��� ���°͵鿡 ���ؼ� 
	// ������� ������ �˻��ϰ�
	// ���ٸ� �����ϰ� �ƴϸ� ����Ѵ�.
	// ������ ������� ������ Ȯ���Ϸ��� �ߴµ� ����� second���� �����Ͷ� ���� �Ķ���͵��� �� ���ؾ��Ѵ�.
	pair <multimap<string, pair<string, Transition *>>::iterator, multimap<string, pair<string, Transition *>>::iterator> result;
	result = transitionTable.equal_range(from);

	Transition * transition = new Transition(this, from, to, valueName, factor, type, valueToCompare);

	for (multimap<string, pair<string, Transition *>>::iterator it = result.first; it != result.second; ++it)
	{
		if ((*transition) == (*(it->second.second)))
		{
			// �̹� ������ transition�� ��ϵǾ� ������ �������ش�.
			delete transition;
			return;
		}
	}

	// ���̿� ���� �ִϸ��̼ǵ��� ��ϵǾ� ���� �ʴٸ� ����
	if (stateTable.find(from) == stateTable.end() || stateTable.find(to) == stateTable.end())
	{
		delete transition;
		return;
	}

	// ���� ��ü�� ��ϵǾ�� �һ�Ȳ
	transitionTable.insert(pair<string, pair<string, Transition *>>({ from }, { to, transition }));
}

void AnimationFSM::updateAllTrasitions(const string & animationFileName)
{
	pair <multimap<string, pair<string, Transition *>>::iterator, multimap<string, pair<string, Transition *>>::iterator> result;
	result = transitionTable.equal_range(animationFileName);

	// ��� ���̻�Ȳ���� ���鼭 Ȯ���غ��� 
	// ���� �������� true�� ������ �� �ִϸ��̼��� ����ִϸ��̼����� �����Ѵ�.
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

	// �÷��̾� ����

	/*
	���̵� ��Ȳ ���
	*/
	string floatSpeed = "speed";
	string floatSideSpeed = "sideSpeed";
	// ���� ���
	registerFloat("speed");
	setFloat("speed", 1.0f);

	registerFloat("sideSpeed");
	setFloat("sideSpeed", 1.0f);

	registerBool("isIdle");
	setBool("isIdle", true);

	registerBool("isShooting");
	setBool("isShooting", false);

	// forward > back / left / right / idle
	// float������ ��ϵ� "speed"�� 0���� ������ forward > back
	makeTransition("standing run forward.fbx", "standing run back.fbx", "speed", -1, AnimationFSM::ValueType::FLOATTYPE, 0);
	// float������ ��ϵ� "sideSpeed"�� 0���� ������ forward > left
	makeTransition("standing run forward.fbx", "standing run left.fbx", "sideSpeed", 1, AnimationFSM::ValueType::FLOATTYPE, 0);
	// float������ ��ϵ� "sideSpeed"�� 0���� Ŭ�� forward > right
	makeTransition("standing run forward.fbx", "standing run right.fbx", "sideSpeed", -1, AnimationFSM::ValueType::FLOATTYPE, 0);
	// bool������ ��ϵ� "isIdle"�� true �϶�
	makeTransition("standing run forward.fbx", "standing idle 01.fbx", "isIdle", 0, AnimationFSM::ValueType::BOOLTYPE, true);

	// back > forward / left / right / idle
	// float������ ��ϵ� "speed"�� 0���� Ŭ�� back > forward
	makeTransition("standing run back.fbx", "standing run forward.fbx", "speed", 1, AnimationFSM::ValueType::FLOATTYPE, 0);
	// float������ ��ϵ� "sideSpeed"�� 0���� ������ back > left
	makeTransition("standing run back.fbx", "standing run left.fbx", "sideSpeed", 1, AnimationFSM::ValueType::FLOATTYPE, 0);
	// float������ ��ϵ� "speed"�� 0���� Ŭ�� back > right
	makeTransition("standing run back.fbx", "standing run right.fbx", "sideSpeed", -1, AnimationFSM::ValueType::FLOATTYPE, 0);
	// bool������ ��ϵ� "isIdle"�� true �϶�
	makeTransition("standing run back.fbx", "standing idle 01.fbx", "isIdle", 0, AnimationFSM::ValueType::BOOLTYPE, true);

	// left > forward / back / right / idle
	// float������ ��ϵ� "speed"�� 0���� ������ left > forward
	makeTransition("standing run left.fbx", "standing run forward.fbx", "speed", 1, AnimationFSM::ValueType::FLOATTYPE, 0);
	// float������ ��ϵ� "speed"�� 0���� Ŭ�� left > back
	makeTransition("standing run left.fbx", "standing run back.fbx", "speed", -1, AnimationFSM::ValueType::FLOATTYPE, 0);
	// float������ ��ϵ� "sideSpeed"�� 0���� Ŭ�� left > right
	makeTransition("standing run left.fbx", "standing run right.fbx", "sideSpeed", -1, AnimationFSM::ValueType::FLOATTYPE, 0);
	// bool������ ��ϵ� "isIdle"�� true �϶�
	makeTransition("standing run left.fbx", "standing idle 01.fbx", "isIdle", 0, AnimationFSM::ValueType::BOOLTYPE, true);


	// right > forward / back / left / idle
	// float������ ��ϵ� "sideSpeed"�� 0���� ������ right > left
	makeTransition("standing run right.fbx", "standing run left.fbx", "sideSpeed", 1, AnimationFSM::ValueType::FLOATTYPE, 0);
	// float������ ��ϵ� "sideSpeed"�� 0���� Ŭ�� right > forward
	makeTransition("standing run right.fbx", "standing run forward.fbx", "speed", 1, AnimationFSM::ValueType::FLOATTYPE, 0);
	// float������ ��ϵ� "speed"�� 0���� ������ right > back
	makeTransition("standing run right.fbx", "standing run back.fbx", "speed", -1, AnimationFSM::ValueType::FLOATTYPE, 0);
	// bool������ ��ϵ� "isIdle"�� true �϶�
	makeTransition("standing run right.fbx", "standing idle 01.fbx", "isIdle", 0, AnimationFSM::ValueType::BOOLTYPE, true);

	// idle > forward / back / left / right / arrow
	// float������ ��ϵ� "sideSpeed"�� 0���� ������ idle > left
	makeTransition("standing idle 01.fbx", "standing run left.fbx", "sideSpeed", 1, AnimationFSM::ValueType::FLOATTYPE, 0);
	// float������ ��ϵ� "sideSpeed"�� 0���� Ŭ�� left > right
	makeTransition("standing idle 01.fbx", "standing run right.fbx", "sideSpeed", -1, AnimationFSM::ValueType::FLOATTYPE, 0);
	// float������ ��ϵ� "sideSpeed"�� 0���� Ŭ�� idle > forward
	makeTransition("standing idle 01.fbx", "standing run forward.fbx", "speed", 1, AnimationFSM::ValueType::FLOATTYPE, 0);
	// float������ ��ϵ� "speed"�� 0���� ������ idle > back
	makeTransition("standing idle 01.fbx", "standing run back.fbx", "speed", -1, AnimationFSM::ValueType::FLOATTYPE, 0);
	// bool������ ��ϵ� "isShooting"�� 0���� ������ idle > arrow
	makeTransition("standing idle 01.fbx", "Standing Draw Arrow.fbx", "isShooting", 0, AnimationFSM::ValueType::BOOLTYPE, true);

	// arrow > idel
	// float������ ��ϵ� "speed"�� 0���� ������ arrow > idle
	makeTransition("Standing Draw Arrow.fbx", "standing idle 01.fbx", "isShooting", 0, AnimationFSM::ValueType::BOOLTYPE, false);

}

void PlayerAnimationFSM::update()
{
	if (!fbxModelAnimations) return;

	auto it = stateTable.find(currentState);
	// ���࿡ ���� ���¸� ���̺��� ã�� ���Ѵٸ� ����Ʈ�� ���ش�.
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


	// �ִϸ��̼��� �÷������ְ� // �����÷������̸� ����÷���
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

	// �÷��̾� ����

	/*
	���̵� ��Ȳ ���
	*/

	// ���� ���
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
	// ���࿡ ���� ���¸� ���̺��� ã�� ���Ѵٸ� ����Ʈ�� ���ش�.
	if (it == stateTable.end())
		currentState = defaultState;

	// �ִϸ��̼��� �÷������ְ� // �����÷������̸� ����÷���
	fbxModelAnimations->playWithFileName(currentState);

	updateAllTrasitions(currentState);

}

void BulletScript::start()
{
	device = &(gameObject->getDevice());

	D3DXCreateSphere(device, 1.0f, 10, 10, &bulletMesh, 0);

	//HRESULT hr = 0;

	//// ������ �ε���Ѽ� �ؽ�ó�� �ҷ����� ��� 
	//hr = D3DXCreateTextureFromFile
	//(
	//	device,
	//	"../Fbx/Textures/fire4.jpg",
	//	&texture
	//);

	//if (FAILED(hr))
	//	texture = nullptr;

	// material �⺻ ����
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

	// ���� transform��ķ� �����Ų��.
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
		// ���Ϳ� ���� ���� ���
		if (other.gameObject->getTag() == "Enemy")
		{
			gameObject->getAudio().playEffectSound("BS_Effect_MonsterDamaged.mp3", true);
		}
		else // if(other.gameObject->getTag() == "EnemyBoss")
		{
			gameObject->getAudio().playEffectSound("BS_Effect_BossMonsterDamaged.mp3", true);
		}


		// �������� ������.
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
	// ���⼭�� ���� ���� ������ �����൵ ���̴����� �˾Ƽ� �����ǿ� �°� �������ϱ� ������ ������.
	// �� ������ ó�� �����Ҷ� �������� �� �����߱� �����̴�.
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

	// ���� transform��ķ� �����Ų��.
	device->SetTransform(D3DTS_WORLD, &transform->getTransformMatrix());

	if(texture)
		device->SetTexture(0, texture);

	device->SetMaterial(&mtrl);

	mesh->DrawSubset(0);

	float height = 0.0f;
	getLocalHeight({ 0,0,0 }, &height);
	Gizmos::DrawLine({ 0,0,0 }, { 0,height,0 });

	return;

	//// ������ �ε�Ǿ ���� �ִ� ��츸 �׷��ش�.
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

	// material �⺻ ����
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

	// �ܺο��� �޾ƿ� ������ �������ش�.
	this->fileName = fileName;
	this->verticesPerRow = verticesPerRow;
	this->verticesPerColumn = verticesPerColumn;
	this->distanceOfVertices = distanceOfVertices;

	if (heightValue < 0) heightValue = 1.0f;
	this->heightValue = heightValue;

	// ������ ���鵵 �ʱ�ȭ�����ش�.
	heightMapSize = verticesPerRow * verticesPerColumn;
	rectsPerRow = verticesPerRow - 1;
	rectsPerColumn = verticesPerColumn - 1;
	width = rectsPerRow * distanceOfVertices;
	depth = rectsPerColumn * distanceOfVertices;

	// ����Ʈ�� �ε忡 �����ϸ� ����
	if (!loadHeightMap(fileName))return;

	// �޽� ���� ������ �����ϸ� ����
	if (!generateMeshBuffer()) return;

	// ���ؽ��� �ε��� ������ ä���ش�.
	processVertices();
	processIndices();
	processSubsets();
	//optimizeMesh();
}

bool Terrain::loadHeightMap(const string & fileName)
{
	vector<BYTE> infoToRead(heightMapSize);

	ifstream iFStream((filePathToLoadTerrainFiles + fileName).c_str(), ios_base::binary);

	// ������ �� �� ������ ���� false
	if (iFStream.is_open() == false)
		return false;

	iFStream.read
	(
		(char*)&infoToRead[0],
		infoToRead.size()
	);

	iFStream.close();

	// ����Ʈ���� ũ�⸦ �ٽ� �����ϰ�
	heightMap.resize(heightMapSize);

	// ���� �����͸� �ٽ� �����Ѵ�. // �̶� heightValue�� ��������ش�.
	for (int i = 0; i < infoToRead.size(); i++)
		heightMap[i] = (float)infoToRead[i] * (float)heightValue;

	return true;
}

bool Terrain::generateMeshBuffer()
{
	HRESULT hr = 0;


	hr = D3DXCreateMeshFVF(
		// �޽��� ���� ���� ���� // �ѻ簢�� ���� * 2
		// �ﰢ���� ����/3 = �ε����� ��
		rectsPerRow * rectsPerColumn * 2,

		// �޽��� ���� ���ؽ��� ��
		heightMapSize,

		// �޽��� ����µ� �̿�� �ϳ� �̻��� �÷���
		// ���⼭�� �޽��� ���� �޸� Ǯ���� �����ǵ��� �Ͽ���.
		D3DXMESH_MANAGED,// | 
//		D3DXMESH_32BIT,

		// ������ �޽��� ����µ� �̿�� ����
		Terrain::TerrainVertex::DefaultFVF,
		device,
		// ������ �޽��� ��� ������
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
	// mesh�� �־��ٶ� fbxVertex�������� �־��ٰ��̴�.
	TerrainVertex * terrainV = nullptr;

	// ������ �޽��� ��װ� �׾ȿ� ���ؽ� ������ ����ִ´�.
	// ���ؽ� ������ ������ / �븻 / UV���̴�.
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

	// �ʻ����� // ���ؽ� �� ����(�� ���ؽ� ���� * �� ���ؽ� ����) // ��ŭ ��ȸ�ϸ鼭 ������ ä���ش�.
	for (int i = 0; i < verticesPerColumn; ++i)
	{
		// ���⼭ ���� -width/2 �� depth/2�� ���ִ� ������ ������ �������� ������ �����ֱ� �����̴�.
		for (int j = 0; j < verticesPerRow; ++j)
		{
			// position ���
			x = j * distanceOfVertices - width/2;

			// ������ xz��ǥ�� directx �� xz��ǥ���� z���� ��ȣ�� ���� �ݴ��̱� ������ �̷��� ó�����ش�.
			// �̷��� ó���� ���ϸ� ���� ����Ʈ �ʰ� �ݴ�� ��µȴ�. 
			// �� ���� �������� �ﰢ������ �׷��ֱ� ������ �ﰢ���� �޸�ó�� �Ǿ� �ø��Ǵ� ������ �߻��Ѵ�.
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

			// ��ֺ��͸� �����ش�. // ���ﰢ�� �������� ac X ab �� �ϸ�ȴ�.
			/*
			  a	      b
				*	*
				*	*
			  c       d

			a�������� ��ȸ�ϴµ� abc / cbd �ﰢ�� 2���� �������ָ� �ȴ�.

			������ ���ؽ��� �ε�����
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

				// �ǿ����� ���϶� ���̻� �������� �ε����� ����.
				a = { x, heightMap[i * verticesPerRow + j] ,z };
				b = { x, heightMap[(i + 1) * verticesPerRow + j] ,z - distanceOfVertices };
				c = { x - distanceOfVertices, heightMap[(i + 1) * verticesPerRow + j - 1] ,z - distanceOfVertices };

			}
			else if (i >= rectsPerColumn && j < rectsPerRow)
			{
				// �ǾƷ� ���϶� ���̻� �Ʒ��� �ε����� ����.
				a = { x, heightMap[i * verticesPerRow + j] ,z };
				b = { x + distanceOfVertices, heightMap[(i - 1) * verticesPerRow + j + 1] ,z + distanceOfVertices };
				c = { x + distanceOfVertices, heightMap[i * verticesPerRow + j + 1] ,z };

			}
			else // if(i >= rectsPerColumn && j < rectsPerRow) // �ǿ����� �ǾƷ� �ε����϶�
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

			// �ε����� ����� ���������� �ʾƼ� ����� �������̵��� �ʴ� ���װ� �߻��ߴ�.
			// �����ؼ� �ε����� �־�����
			terrainV[i * verticesPerRow + j] =
			{
				// Position
				x, heightMap[i * verticesPerRow + j], z,
				// Normal // �ϴ� �׻� up���� ����
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

	// ������ �޽��� ��װ� �׾ȿ� ���ؽ� ������ ����ִ´�.
	// ���ؽ� ������ ������ / �븻 / UV���̴�.
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

			   a�������� ��ȸ�ϴµ� abc / cbd �ﰢ�� 2���� �������ָ� �ȴ�.

			   ������ ���ؽ��� �ε�����
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

	// �ٽ� �Ӽ� �����͸� �����ϱ� ���ؼ� ���۸� ����ش�.
	mesh->LockAttributeBuffer(0, &attributeBuffer);

	int ttt = mesh->GetNumFaces();

	// ������� 0������ �������ش�.
	for (int i = 0; i < mesh->GetNumFaces(); ++i)
		attributeBuffer[i] = 0;


	// ������ �Ϸ�Ǿ����Ƿ� �ٽ� ����ش�.
	mesh->UnlockAttributeBuffer();
}

void Terrain::optimizeMesh()
{
	if (!mesh) return;

	// �������۸� �̿��� ����ȭ �۾�
	vector<unsigned long> adjacencyBuffer(mesh->GetNumFaces() * 3);
	// �޽��� ���������� �޾ƿ´�.
	mesh->GenerateAdjacency(0.0f, &adjacencyBuffer[0]);


	int faceCount1 = mesh->GetNumFaces();
	int vertexCount1 = mesh->GetNumVertices();

	HRESULT hr = 0;
	// �� ���� ������ ������� ����ȭ �۾��� �����Ѵ�.
	hr = mesh->OptimizeInplace
	(
		// �Ӽ�����(����±���) �ﰢ������ �����ϰ�, ������ �Ӽ����̺��� ����
		// GetAttributeTable�Լ��� �̿��ؼ� D3DXATTRIBUTERANGE����ü �迭�� �޾ƿ� �� �ִ�.
		// ���� �������� �����ID / �� ��� ���ؽ��� �� / �� ���� ������ ����ִ�.
		// �Ӽ� ���̺��� ���׸��� �޽��� �� ����°� �����Ǹ�, 
		// ������� ������������ �����Ǵ� ���ؽ�/ �ε��� ���۳��� �޸� ����� �����Ѵ�.
		D3DXMESHOPT_ATTRSORT |
		// �޽����� �̿����� �ʴ� �ε����� ���ؽ��� �����Ѵ�.
		D3DXMESHOPT_COMPACT |
		// ���ؽ� ĳ���� ��Ʈ���� ���δ�.
		D3DXMESHOPT_VERTEXCACHE,

		// ����ȭ �������� �޽��� ���� �迭 ������
		// �����迭�� �ʿ�������?	// ����ȭ ��Ͽ� ���������� �ʿ��� �κ��� �־ �ʿ��ҵ�
		&adjacencyBuffer[0],
		// ����ȭ�� �޽��� ���� �迭 ������
		nullptr,
		// �������� Face
		nullptr,
		// �������� Vertex
		nullptr
	);

	int faceCount = mesh->GetNumFaces();
	int vertexCount = mesh->GetNumVertices();
}

void Terrain::loadTextureFromFile(const string & fileName)
{
	HRESULT hr = 0;

	// ������ �ε���Ѽ� �ؽ�ó�� �ҷ����� ��� 
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

	// ���ϰ��� �ϴ� ���� Terrain�� ������ǥ��� ��ȯ�Ѵ�.
	// ������ǥ��� ��ȯ�ϱ� ���ؼ��� Terrain�� transform����� ������� �����ָ� �ȴ�.
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
	//// ���� ������� �������� ������ ����
	//if (determinant == 0.0f) return false;

	Vector3::ToD3DXVECTOR3(positionInTerrainLocal, position);

	D3DXVec3TransformCoord(&positionInTerrainLocal, &positionInTerrainLocal, &terrainInverseTransformMatrix);

	// �������� �ͷ����� ������ǥ�� �������� ����ϸ� �ȴ�. // ��ġ�� ������ǥ��� ����Դ�.
	// �̷��� ������ǥ��� �����ϴ� ������ �ͷ����� �����̼ǵ������ 1ĭ�� ����� ��Ʊ� �����̴�.

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

	// ���࿡ ����ٸ� ����
	if (startX >= position.getX() || startZ <= position.getZ()
		|| endX <= position.getX() || endZ >= position.getZ())
		return false;

	// ���������� ���� �󸶳� ������ �ִ��� ���Ѵ�.
	float z = (startZ - position.getZ());
	float x = (position.getX() - startX);
	// �ε����� ���� ���غ���.
	int columnIndex = z / distanceOfVertices;
	int rowIndex = x / distanceOfVertices;

	// �� �ε��� �������� �󸶳� ������ �ִ��� ���Ѵ�.
	// �ε��� ���� ��ǥ�� �ʱ�ȭ �����ش�.
	// distanceOfVertices�� ���簢���̴�. // ������ ������
	z -= columnIndex * distanceOfVertices;
	x -= rowIndex * distanceOfVertices;

	/*
	   a	  b
		* -- *
		l  / l
		* __ *
	   c      d

	a�������� ��ȸ�ϴµ� abc / cbd �ﰢ�� 2���� �������ָ� �ȴ�.
	*/

	float aHeight = heightMap[columnIndex * verticesPerRow + rowIndex];
	float bHeight = heightMap[columnIndex * verticesPerRow + rowIndex + 1];
	float cHeight = heightMap[(columnIndex + 1) * verticesPerRow + rowIndex];
	float dHeight = heightMap[(columnIndex + 1) * verticesPerRow + rowIndex + 1];

	float resultHeight = 0.0f;

	// ���� ġ�����ִ����� ���� ���� �����ش� // 0 ~ 1���� // 0 : x���� �ǿ��� z���� ����, 1 : x���� �ǿ����� z���� �ǾƷ�
	float zFactor = z / distanceOfVertices;
	float xFactor = x / distanceOfVertices;

	if (x + z < distanceOfVertices)
	{
		// ���� �ﰢ���� ���
		// A Vector3 + (ab Vector3 * xFactor + ac Vector3 * zFactor) �� y��
		// �׳� y���߽����θ� ����ص� �ȴ�. 
		resultHeight = aHeight + ((bHeight - aHeight) * xFactor + (cHeight - aHeight) * zFactor);
	}
	else
	{
		// �Ʒ��� �ﰢ���ΰ��
		// D Vector3 + (dc Vector3 * (1 - xFactor) + db Vector3 * (1 - zFactor)) �� y��
		// �׳� y���߽����θ� ����ص� �ȴ�. 
		resultHeight = dHeight + ((cHeight - dHeight) * (1 - xFactor) + (bHeight - dHeight) * (1 - zFactor));
	}

	// ���⼭ ���� �ͷ����� ���̸�ŭ �����ش�.
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
	// ���⼭ �ҽ����İ��� 0�̸� �޹���� 1�� ����Ǿ �޹���� �ݿ��ǰ�
	// �ҽ� ���İ��� .5�̸� �޹���� .5�� ����Ǿ .5��ŭ �ݿ��ȴ�.
	device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	//device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR);
	//device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR);


	// ���� transform��ķ� �����Ų��.
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
	// ���� ���� ī�޶� �����ǰ� ���� ���ٸ� ����
	//D3DXMATRIX mainCameraTransformMatrix = mainCamera->getTransform()->getTransformMatrix();
	// �̷��۾��� ���ִ� ������ ����ī�޶� ��� ������Ʈ�� ���� ������Ʈ�� ���ɼ��� �ֱ� �����̴�.
	// ����������Ʈ�� ���ԵǴ� ��� position�� ������ǥ�̱� ������ transformMatrix�� �޾ƿͼ� �۷ι� ��ġ�� �����ش�.
	//Vector3 mainCameraPosition{ mainCameraTransformMatrix._41,mainCameraTransformMatrix._42,mainCameraTransformMatrix._43 };

	Vector3 mainCameraPosition{ mainCamera->getTransform()->getWorldPosition() };

	if (mainCameraPosition == currentMainCameraPosition) return;

	// ī�޶��� ��ġ�� ����Ǿ��ٸ� ��ġ �ֽ�ȭ�� ���ش�.
	currentMainCameraPosition = mainCameraPosition;

	// ���繰ü���� ī�޶���������� ���͸� �����ش�.
	Vector3 direction = currentMainCameraPosition - transform->getWorldPosition();

	// y���� ������ �����ֱ� ������ ���ش�. // y�����θ� �����̼� �ؾ��Ѵ�.
	direction.setY(0.0f);
	
	// ����ȭ���ش�.
	Vector3::Normalized(direction, direction);


	// �ʱ� ��ġ forward  = (0,0, -1) dot direction�� ���־ �����̼��� �����ش�.
	// �� ���̰��� ���ϱ� ���ؼ� ������ ũ��(�Ѵ� ����ȭ�� �����̹Ƿ� 1)�� �������� �����ְ� 
	// ���������� cos�� ���Լ��� �������ش�.
	float angle =  (float)acos(Vector3::Dot({ 0,0,-1 }, direction)/ (1.0f * 1.0f) );

	// ���⼭ ���������� �׻� ���̰��� ����� ���´ٴ� ���̴�.
	/*
	����� ������ �Ǻ���
	1. ���⼭ �Ǻ��ϱ� ���ؼ� ��ǥ��鿡 �Ѻ��� �������� ���Ľ�Ű�� (���⼭�� World back ��� ���غ��͸� ����߱� ������ ���� �ʿ� ����)
	   �� ���� �������� ���������� �Ǻ��Ѵ�. 
	   // ���⼭�� �Ѻ��Ͱ� back�����̹Ƿ� x���� + �϶� �������̴�. // right���Ͷ�� y���� -�϶� ����� +�϶� �������̴�.

	2. �� ���͸� �����ؼ� ������ ������� �Ǻ��غ���. 
	   �����忡���� ��������� X ī�޶�������� �� ������ World up �� ���´ٸ� ������̰�
	   ��������� X ī�޶�������� �� ������ World down�� ���´ٸ� �������̴�.
	
	*/


	// ���� ��Ȯ�� ǥ�� �ʿ��ҵ� ���߿� ������ ����
	// ���� ������ �����̸� ���� ������ �ٲ��ش�.
	if (direction.getX() > 0)
		angle = -angle;
	
	
	// Y�������� // �ð���� + / �ݽð� - 
	// ���࿡ �������� �����̼ǰ� ī�޶���⺸�� �� �ð���������� �����ִٸ� - �� ���־���Ѵ�.
	// �� �������� ���簢���� ī�޶���� ���� ������ ũ�ٸ� ���� ������ �ٲ��ش�.
	//if (transform->getRotation().getY() > mainCamera->getTransform()->getRotation().getY())
	//	angle = -angle;

	transform->setWorldRotation({ 0, Transform::RadianToDegree(angle),0 });
	
	// ��ֺ��͸� ���� ����ϱ� ���ؼ� ���ؽ����� �ٽ� �ʱ�ȭ��Ų��.
	// ���� �����̼��� �ٲ���� ������ ��ֺ��͵� �ٲ�����Ѵ�. // �׻� ī�޶󿡼� ������ �ߺ��̰� �ϱ� ���ؼ�
	processVertices();
	//processIndices();
}

void BillBoard::start()
{
	mainCamera = gameObject->getScene().getMainCamera();
	this->device = &(gameObject->getDevice());

	// material �⺻ ����
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
	// ���� ���� ī�޶� or device �� ������ �����Ѵ�.
	if (!mainCamera || !device) return;

	rotateIfMainCameraMoves();
	render();
}

void BillBoard::loadTextureFromFile(const string & fileName)
{
	HRESULT hr = 0;

	// ������ �ε���Ѽ� �ؽ�ó�� �ҷ����� ��� 
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
	// �޽��� �ְų� ����̽��� ������ ����
	if (!device || mesh) return;

	// �������н� ����
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
		// �ﰢ�� 2��
		2,
		// ���ؽ� 2��
		4,
		// �޽��� ����µ� �̿�� �ϳ� �̻��� �÷���
		// ���⼭�� �޽��� ���� �޸� Ǯ���� �����ǵ��� �Ͽ���.
		D3DXMESH_MANAGED,
		// ������ �޽��� ����µ� �̿�� ����
		BillBoard::BillBoardVertex::DefaultFVF,
		device,
		// ������ �޽��� ��� ������
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
	// mesh�� �־��ٶ� fbxVertex�������� �־��ٰ��̴�.
	BillBoardVertex * billBoardV = nullptr;

	// ������ �޽��� ��װ� �׾ȿ� ���ؽ� ������ ����ִ´�.
	// ���ؽ� ������ ������ / �븻 / UV���̴�.
	mesh->LockVertexBuffer(0, (void**)&billBoardV);

	if (!billBoardV) return;

	/*
	  a	      b
		*	*
		*	*
	  c       d

	a : 0 / b : 1 / c : 2 / d : 3
	// z���� ��� 0�̴�.
	*/

	Vector3 normal =(-1) * transform->getForward();

	float halfWidth = width / 2;
	float halfHeight = height / 2;
	// ���⼭�� ������ ��ǥ�� �������� �������ش�. 
	// ���߿� transformMatrix�� ���� ��ġ�� �����̼��� �����ȴ�.
	// Normal // �׻� back���� ���� z�� 0�� xy��� ���� // �����̼� ���� �׻� ���� back���� �����ߴ�. // ����Ĵ���ϸ� �ٸ� ���⿡�� ��ο���
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

	// ������ �޽��� ��װ� �׾ȿ� ���ؽ� ������ ����ִ´�.
	// ���ؽ� ������ ������ / �븻 / UV���̴�.
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

	// �ٽ� �Ӽ� �����͸� �����ϱ� ���ؼ� ���۸� ����ش�.
	mesh->LockAttributeBuffer(0, &attributeBuffer);

	// ������� 0������ �������ش�.
	for (int i = 0; i < mesh->GetNumFaces(); ++i)
		attributeBuffer[i] = 0;

	// ������ �Ϸ�Ǿ����Ƿ� �ٽ� ����ش�.
	mesh->UnlockAttributeBuffer();
}

void BillBoard::optimizeMesh()
{
	if (!mesh) return;

	// �������۸� �̿��� ����ȭ �۾�
	vector<unsigned long> adjacencyBuffer(mesh->GetNumFaces() * 3);
	// �޽��� ���������� �޾ƿ´�.
	mesh->GenerateAdjacency(0.0f, &adjacencyBuffer[0]);


	HRESULT hr = 0;
	// �� ���� ������ ������� ����ȭ �۾��� �����Ѵ�.
	hr = mesh->OptimizeInplace
	(
		// �Ӽ�����(����±���) �ﰢ������ �����ϰ�, ������ �Ӽ����̺��� ����
		// GetAttributeTable�Լ��� �̿��ؼ� D3DXATTRIBUTERANGE����ü �迭�� �޾ƿ� �� �ִ�.
		// ���� �������� �����ID / �� ��� ���ؽ��� �� / �� ���� ������ ����ִ�.
		// �Ӽ� ���̺��� ���׸��� �޽��� �� ����°� �����Ǹ�, 
		// ������� ������������ �����Ǵ� ���ؽ�/ �ε��� ���۳��� �޸� ����� �����Ѵ�.
		D3DXMESHOPT_ATTRSORT |
		// �޽����� �̿����� �ʴ� �ε����� ���ؽ��� �����Ѵ�.
		D3DXMESHOPT_COMPACT |
		// ���ؽ� ĳ���� ��Ʈ���� ���δ�.
		D3DXMESHOPT_VERTEXCACHE,

		// ����ȭ �������� �޽��� ���� �迭 ������
		// �����迭�� �ʿ�������?	// ����ȭ ��Ͽ� ���������� �ʿ��� �κ��� �־ �ʿ��ҵ�
		&adjacencyBuffer[0],
		// ����ȭ�� �޽��� ���� �迭 ������
		nullptr,
		// �������� Face
		nullptr,
		// �������� Vertex
		nullptr
	);
}

