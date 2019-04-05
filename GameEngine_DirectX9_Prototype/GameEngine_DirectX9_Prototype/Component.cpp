#include "Component.h"
#include "GameObject.h"

#include "Scene.h"
#include "Audio.h"
#include "Physics.h"
#include "FbxParser.h"

#include "FbxModelMesh.h"
#include "FbxModelSkeletonBones.h"
#include "FbxModelAnimations.h"

#include "Utility.h"
#include "Trace.h"


void PlayerScript::update()
{
	//if (::GetAsyncKeyState('Q') & 0x8000f)
	//	transform->setRotation(transform->getRotation() + Vector3{ 0,-.05f,0 });
	//if (::GetAsyncKeyState('E') & 0x8000f)
	//	transform->setRotation(transform->getRotation() + Vector3{ 0,.05f,0 });

	//if (::GetAsyncKeyState('Z') & 0x8000f)
	//	transform->setRotation(transform->getRotation() + Vector3{ -.05f,0,0 });
	//if (::GetAsyncKeyState('C') & 0x8000f)
	//	transform->setRotation(transform->getRotation() + Vector3{ .05f,0,0 });

	if (::GetAsyncKeyState(VK_UP) & 0x8000f)
	{
		if (gameObject->getRigidBody())
		{
			gameObject->getRigidBody()->addForce(FrameTime::GetDeltaTime()* .1f *(transform->getForward()));
		}
		//transform->setPosition(transform->getPosition() + FrameTime::GetDeltaTime()* .1f *(transform->getForward()));
	}
	if (::GetAsyncKeyState(VK_DOWN) & 0x8000f)
	{
		if (gameObject->getRigidBody())
		{
			gameObject->getRigidBody()->addForce(FrameTime::GetDeltaTime()* -.1f *(transform->getForward()));
		}
		//transform->setPosition(transform->getPosition() + FrameTime::GetDeltaTime()*.1f *(-1)*(transform->getForward()));
	}

	if (::GetAsyncKeyState(VK_RIGHT) & 0x8000f)
	{
		transform->setPosition(transform->getPosition() + FrameTime::GetDeltaTime()*.1f * (transform->getRight()));
	}

	if (::GetAsyncKeyState(VK_LEFT) & 0x8000f)
	{
		transform->setPosition(transform->getPosition() + FrameTime::GetDeltaTime()*.1f * (-1)*(transform->getRight()));
	}

	if (::GetAsyncKeyState(VK_SPACE) & 0x8000f)
	{
		Vector3 direction = transform->getForward() + transform->getUp();
		GameObject * bullet = GameObject::Instantiate("bullet1", "Bullet");
		bullet->addComponent<MeshRenderer>()->loadXFile("car.x");
		bullet->getTransform()->setPosition(transform->getPosition()+direction);

		RigidBody * bulletRigidBody = bullet->addComponent<RigidBody>();
		bulletRigidBody->setSphereCollider(1);
		bulletRigidBody->addForce(direction * 20.0f);
		bullet->addComponent<BulletScript>();

		//car1RigidBody->setGravity(Vector3(0, 0, 0));
		//car1->addComponent<BoxCollider>();
	}

	if (::GetAsyncKeyState('N') & 0x8000f)
			transform->setRotation(transform->getRotation() + FrameTime::GetDeltaTime() *.1f * Vector3{ 0,-1,0 });

	if (::GetAsyncKeyState('M') & 0x8000f)
			transform->setRotation(transform->getRotation() + FrameTime::GetDeltaTime() *.1f * Vector3{ 0,1,0 });
}

void PlayerScript::start()
{
	//animationFSM = gameObject->getComponent<AnimationFSM>();
	AudioSource * temp = gameObject->getAudio().getAudioSource("BS_BackGround_1.mp3");
	if (temp)
	{
		temp->play(false);
		temp->setVolume(7.0f);
	}
}

void PlayerScript::onCollisionStay(GameObjectWithCollision & other)
{
	
}

void MoveScript_C::start()
{

}

void MoveScript_C::update()
{
	if (::GetAsyncKeyState('K') & 0x8000f)
	{
		transform->setRotation(transform->getRotation() + Vector3{ 0,-.05f,0 });
		AudioSource * temp = gameObject->getAudio().getAudioSource("BS_Effect_1.mp3");
		if (temp)
		{
			temp->play(false);
			temp->setVolume(2.0f);
		}
	}

	if (::GetAsyncKeyState('L') & 0x8000f)
	{
		transform->setRotation(transform->getRotation() + Vector3{ 0,.05f,0 });
		AudioSource * temp = gameObject->getAudio().getAudioSource("BS_Effect_1.mp3");
		if (temp)
		{
			temp->play(false);
			temp->setVolume(2.0f);
		}
	}



	if (::GetAsyncKeyState('P') & 0x8000f)
		GameObject::Destroy(gameObject);
}

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

void Transform::transformUpdate(bool dirty, const D3DXMATRIX & parentTransformMatrix)
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

void Transform::setTransformMatrix(const D3DXMATRIX & parentTransformMatrix)
{
	// 먼저 자식의 부모좌표계 기준으로 로테이션과 포지션 행렬을 구해준다.
	calcRotationMatrix(); calcPositionMatrix();

	// 먼저 자식기준(월드 기준 X / 부모를 월드로 생각했을때 기준이다) trasnformMatrix를 구하고 
	// rotationMatrix * positionMatrix
	// 최종적으로 위에서 부터 최신화된 부모의 transform행렬을 곱해준다.
	// 따라서 최종 행렬은
	transformMatrix = (rotationMatrix * positionMatrix) * parentTransformMatrix;
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

	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, degreeToRadian( rotation.getY()), degreeToRadian(rotation.getX()), degreeToRadian(rotation.getZ()));
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

void Transform::setPosition_physics(const Vector3 & other)
{
	position = other; dirty = true;
}

void Transform::setRotation_physics(const Vector3 & other)
{
	rotation.setX(constrainLessThan180(other.getX()));
	rotation.setY(constrainLessThan180(other.getY()));
	rotation.setZ(constrainLessThan180(other.getZ()));
	setDirectionVectorWithRotation_DX();
	dirty = true;
}

void Transform::setScale_physics(const Vector3 & other)
{
	scale = other;
}

void Transform::setPosition(const Vector3 & other)
{
	position = other; dirty = true;
		// 값이 변경되었을때 리지드바디가 있다면 물리에서도 처리해준다.
	if(gameObject->getRigidBody())
		gameObject->getPhysics().setTransformToSystem(gameObject);
}

void Transform::setRotation(const Vector3 & other)
{
	rotation.setX(constrainLessThan180(other.getX()));
	rotation.setY(constrainLessThan180(other.getY()));
	rotation.setZ(constrainLessThan180(other.getZ()));
	setDirectionVectorWithRotation_DX();
	dirty = true;
	// 값이 변경되었을때 리지드바디가 있다면 물리에서도 처리해준다.
	if(gameObject->getRigidBody())
		gameObject->getPhysics().setTransformToSystem(gameObject);
}

void Transform::setPosition(float x, float y, float z)

{
	position.setX(x);
	position.setY(y);
	position.setZ(z);
	dirty = true;
	// 값이 변경되었을때 리지드바디가 있다면 물리에서도 처리해준다.
	if (gameObject->getRigidBody())
		gameObject->getPhysics().setTransformToSystem(gameObject);
}

void Transform::setRotation(float x, float y, float z)
{
	rotation.setX(constrainLessThan180(x));
	rotation.setY(constrainLessThan180(y));
	rotation.setZ(constrainLessThan180(z));
	setDirectionVectorWithRotation_DX();
	dirty = true;
	// 값이 변경되었을때 리지드바디가 있다면 물리에서도 처리해준다.
	if (gameObject->getRigidBody())
		gameObject->getPhysics().setTransformToSystem(gameObject);
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
		transform->setRotation(transform->getRotation() + Vector3{ 0,5.05f,0 });
	if (::GetAsyncKeyState('Q') & 0x8000f)
		transform->setRotation(transform->getRotation() + Vector3{ 0,-5.05f,0 });

	if (::GetAsyncKeyState('Z') & 0x8000f)
	{
		//transform->setRotation(transform->getRotation() + Vector3{ 0,.05f,.05f });
		transform->setRotation(transform->getRotation() + Vector3{ .05f,0,0 });
	}

	if (::GetAsyncKeyState('C') & 0x8000f)
	{
		//transform->setRotation(transform->getRotation() + Vector3{ 0,-.05f,-.05f });
		transform->setRotation(transform->getRotation() + Vector3{ -.05f,0,0 });
	}

	if (::GetAsyncKeyState('W') & 0x8000f)
		transform->setPosition(transform->getPosition() + (transform->getForward()));
	if (::GetAsyncKeyState('S') & 0x8000f)
		transform->setPosition(transform->getPosition() + (-1)*(transform->getForward()));

	if (::GetAsyncKeyState('D') & 0x8000f)
		transform->setPosition(transform->getPosition() + (transform->getRight()));
	if (::GetAsyncKeyState('A') & 0x8000f)
		transform->setPosition(transform->getPosition() + (-1)*(transform->getRight()));

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


	// 위치 반대로 계산
	D3DXMatrixMultiply(&v, &v, D3DXMatrixTranslation(&temp, -transform->getPosition().getX(),
		-transform->getPosition().getY(), -transform->getPosition().getZ()));

	// 로테이션 반대로 계산
	//D3DXMatrixMultiply(&v, &v, D3DXMatrixRotationX(&temp, -transform->getRotation().getX()));
	//D3DXMatrixMultiply(&v, &v, D3DXMatrixRotationY(&temp, -transform->getRotation().getY()));
	//D3DXMatrixMultiply(&v, &v, D3DXMatrixRotationZ(&temp, -transform->getRotation().getZ()));
	D3DXMatrixRotationYawPitchRoll(&temp,transform->degreeToRadian(-transform->getRotation().getY()), 
		transform->degreeToRadian(-transform->getRotation().getX()), transform->degreeToRadian(-transform->getRotation().getZ()));
	D3DXMatrixMultiply(&v, &v, &temp);


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
	d3d::Release<ID3DXBuffer*>(mtrlBuffer);

	optimizeMesh(adjBuffer);

	// 사용완료한 인접버퍼 릴리즈
	d3d::Release<ID3DXBuffer*>(adjBuffer); // done w/ buffer
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
	gameObject->getPhysics().setTransformFromSystem(gameObject);
	// 물리업데이트가 되었기 때문에 더티플래그를 켜준다.
	transform->setDirty(true);


}

void RigidBody::update()
{
	if (colliderInfo.type == ColliderType::NONE) return;

	// 콜라이더를 그려준다.
	switch (colliderInfo.type)
	{

	case ColliderType::BOX:
	
		Gizmos::DrawBox({ transform->getPosition().getX(), transform->getPosition().getY(), transform->getPosition().getZ() },
		{ colliderInfo.size.getX(),colliderInfo.size.getY(),colliderInfo.size.getZ() });
		break;

	case ColliderType::SPHERE:
		Gizmos::DrawBox({ transform->getPosition().getX(), transform->getPosition().getY(), transform->getPosition().getZ() },
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
	if (meshes.size() ==0) return;

	// 현재 transform행렬로 적용시킨다.
	device->SetTransform(D3DTS_WORLD, &transform->getTransformMatrix());


	int meshCount = meshes.size();
	int fbxCount = fbxMeshes.size();


	map<ID3DXMesh *, vector<D3DMATERIAL9>>::iterator tempMaterialForMesh;
	map<ID3DXMesh *, vector<IDirect3DTexture9*>>::iterator tempTexturesForMesh;
	D3DMATERIAL9 * material = nullptr;
	IDirect3DBaseTexture9 * texture = nullptr;

	for(int i =0; i< meshes.size(); ++i)
	{
		if (!meshes[i]) continue;
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
			processVerticesWithAnimation(meshes[i], fbxMeshes[i]);

			// FbxModelMesh의 각 버텍스에서 MyFbxVertex->getPositionWithAnimation해서 초기화 
			//  position * animationmatrix // 	
			// 내부적으로 가중치 벡터를 가진 점에 대해서 내부 애니메이션 매트릭스 계산


			// 만약에 애니메이션이 멈춘다면 모든 메쉬들을 processVertice함수로 // 초기값으로 지정해준다.
		}




		
		//material, texture-----------------------------------------------------
		// 일단 맵에 등록되어있는지 확인한다.
		tempMaterialForMesh = materialsTable.find(meshes[i]);
		tempTexturesForMesh = texturesTable.find(meshes[i]);

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

		// rendering-----------------------------------------------------
		if(material)
			device->SetMaterial(material);
		if(texture)
			device->SetTexture(0, texture);
		
		meshes[i]->DrawSubset(0);
	}

}

void FbxModelRenderer::start()
{
	device = &(gameObject->getDevice());
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
		for (auto it = meshes.begin(); it != meshes.end(); ++it)
		{
			if ((*it) != nullptr)
				(*it)->Release();
			(*it) = nullptr;
		}
		meshes.clear();

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
	: Component(go, tf), scene(nullptr), importer(nullptr), skeletonBones(nullptr), animations(nullptr)
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

void FbxModelRenderer::setScale(const Vector3 & value)
{

	float xScale = constrainNegativeNumber(value.getX());
	float yScale = constrainNegativeNumber(value.getY());
	float zScale = constrainNegativeNumber(value.getZ());
	
	for (int i = 0; i < meshes.size(); ++i)
	{
		ID3DXMesh * mesh = nullptr;
		mesh = meshes[i];
		FbxModelMesh * fbxModelMesh = nullptr;
		fbxModelMesh = fbxMeshes[i];

		if (!mesh||!fbxModelMesh) continue;

		FbxModelVertex * fbxV = nullptr;
		MyFbxVertex * tVertex = nullptr;

		processVertices(mesh, fbxModelMesh);
		mesh->LockVertexBuffer(0, (void**)&fbxV);
		for (int j = 0; j < fbxMeshes[i]->getVertexCount(); ++j)
		{
			//tVertex = fbxMeshInfos[i]->getVertex(j);
			//if (tVertex)
			//{
			//	fbxV[j][0] = (float)tVertex->getPosition().mData[0] / 10;
			//	fbxV[j][1] = (float)tVertex->getPosition().mData[1] / 10;
			//	fbxV[j][2] = (float)tVertex->getPosition().mData[2] / 10;
			//}


			fbxV[j][0] *= xScale;
			fbxV[j][1] *= yScale;
			fbxV[j][2] *= zScale;
		}

		mesh->UnlockVertexBuffer();
	}
	
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

		ID3DXMesh * mesh = nullptr;
		HRESULT hr = 0;

		hr = D3DXCreateMeshFVF(
			// 메쉬가 가질 면의 개수 
			// 여기서 인덱스 버퍼의 크기도 자동으로 알수 있다.
			// 삼각형의 개수/3 = 인덱스의 수
			(*it)->getTriCount(),

			// 메쉬가 가질 버텍스의 수
			(*it)->getVertexCount(),

			// 메쉬를 만드는데 이용될 하나 이상의 플래그
			// 여기서는 메쉬는 관리 메모리 풀내에 보관되도록 하였다.
			D3DXMESH_MANAGED,

			// 복제된 메쉬를 만드는데 이용될 포맷
			FbxModelRenderer::FbxModelVertex::DefaultFVF,
			device,
			// 복제된 메쉬가 담길 포인터
			&mesh);

		if (FAILED(hr))
		{
			mesh = nullptr;
			meshes.push_back(mesh);
			continue;
		}

		// mesh내부의 버텍스 정보와 인덱스 정보를 초기화 시켜준다.
		processVertices(mesh, (*it));
		processIndices(mesh, (*it));
		processTextures(mesh, (*it));

		processSubsets(mesh);
		optimizeMesh(mesh);

		meshes.push_back(mesh);
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

void FbxModelRenderer::processVertices(ID3DXMesh * mesh, FbxModelMesh * fbxModelMesh)
{
	if (!mesh || !fbxModelMesh) return;
	// mesh에 넣어줄때 fbxVertex형식으로 넣어줄것이다.
	FbxModelVertex * fbxV = nullptr;
	MyFbxVertex * tVertex = nullptr;

	//int t1 = mesh->GetNumVertices();
	//int t2 = fbxModelMesh->getVertexCount();

	// 생성한 메쉬를 잠그고 그안에 버텍스 정보를 집어넣는다.
	// 버텍스 정보는 포지션 / 노말 / UV값이다.
	mesh->LockVertexBuffer(0, (void**)&fbxV);
	for (int i = 0; i < fbxModelMesh->getVertexCount(); ++i)
	{
		tVertex = fbxModelMesh->getVertex(i);
		// 값이 있을때
		if (tVertex)
		{
			// 각각의 정보를 초기화 시켜준다.
			fbxV[i] = 
			{
				// Position
				(float)tVertex->getPosition().mData[0]/10,(float)tVertex->getPosition().mData[1]/10, (float)tVertex->getPosition().mData[2]/10,
				// Normal
				(float)tVertex->getNormal().mData[0]/10,(float)tVertex->getNormal().mData[1]/10, (float)tVertex->getNormal().mData[2]/10,
				// UV
				(float)tVertex->getUV().mData[0],(float)tVertex->getUV().mData[1]
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
			0,0
		};
	}

	//t1 = mesh->GetNumVertices();
	//t2 = fbxModelMesh->getVertexCount();

	mesh->UnlockVertexBuffer();
}

void FbxModelRenderer::processVerticesWithAnimation(ID3DXMesh * mesh, FbxModelMesh * fbxModelMesh)
{
	if (!mesh || !fbxModelMesh) return;

	FbxModelVertex * fbxV = nullptr;
	MyFbxVertex * tVertex = nullptr;

	mesh->LockVertexBuffer(0, (void**)&fbxV);
	for (int i = 0; i < fbxModelMesh->getVertexCount(); ++i)
	{
		tVertex = fbxModelMesh->getVertex(i);
		// 값이 있을때
		if (tVertex)
		{
			// 내부적으로 애니메이션 행렬을 곱한 것들을 처리해준다.
			// 애니메이션 행렬을 곱하기 전에 본들의 가중치에 따른 애니메이션 행렬을 내부적으로 계산해준다.
			//fbxV[i][0] = (float)tVertex->getPositionWithAnimation().x / 10;
			//fbxV[i][1] = (float)tVertex->getPositionWithAnimation().y / 10;
			//fbxV[i][2] = (float)tVertex->getPositionWithAnimation().z / 10;
			// 각각의 정보를 초기화 시켜준다.
			fbxV[i] =
			{
				// Position
				(float)tVertex->getPositionWithAnimation().x / 10, (float)tVertex->getPositionWithAnimation().y / 10, (float)tVertex->getPositionWithAnimation().z / 10,
				// Normal
				(float)tVertex->getNormal().mData[0] / 10,(float)tVertex->getNormal().mData[1] / 10, (float)tVertex->getNormal().mData[2] / 10,
				// UV
				(float)tVertex->getUV().mData[0],(float)tVertex->getUV().mData[1]
			};
			continue;

			// 노말값이나 UV값은 안바꿔도 될까..
			// UV값은 어차피 같은 텍스처 좌표이기때문에 상관없을것같은데 노말값은 달라져야하는거아닐까?
		}
		// 값이 없을때 // 아무것도 해주지않는다.
	}


	mesh->UnlockVertexBuffer();
}

void FbxModelRenderer::processIndices(ID3DXMesh * mesh, FbxModelMesh * fbxModelMesh)
{
	if (!mesh || !fbxModelMesh) return;

	// mesh에 넣어줄때 fbxVertex형식으로 넣어줄것이다.
	unsigned short * fbxI = nullptr;
	int tIndex = -1;

	//int t1 = mesh->GetNumFaces();
	//int t2 = fbxModelMesh->getIndexCount();
	//int t3 = fbxModelMesh->getVertexCount();
	//int t4 = fbxModelMesh->getTriCount();

	// 생성한 메쉬를 잠그고 그안에 버텍스 정보를 집어넣는다.
	// 버텍스 정보는 포지션 / 노말 / UV값이다.
	mesh->LockIndexBuffer(0, (void**)&fbxI);
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
	mesh->UnlockIndexBuffer();
}

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

void FbxModelRenderer::processTextures(ID3DXMesh * mesh, FbxModelMesh * fbxModelMesh)
{
	if (!mesh || !fbxModelMesh) return;
	FbxNode * fbxMeshNode = fbxModelMesh->getNode();
	if (!fbxMeshNode) return;


	map<ID3DXMesh *, vector<D3DMATERIAL9>>::iterator materials_it = materialsTable.find(mesh);
	map<ID3DXMesh *, vector<IDirect3DTexture9*>>::iterator textures_it = texturesTable.find(mesh);

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

	}
}

void FbxModelRenderer::processSubsets(ID3DXMesh * mesh)
{
	if (!mesh ) return;

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

void FbxModelRenderer::optimizeMesh(ID3DXMesh * mesh)
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

void AnimationFSM::start()
{
	// 현재 컴포넌트가 있을때 초기화 된다.
	fbxModelRenderer = gameObject->getComponent<FbxModelRenderer>();

	// FbxModelRenderer의 컴포넌트가 있는 경우 애니메이션 매너저를 받아온다.
	if (fbxModelRenderer)
	{
		fbxModelAnimations = fbxModelRenderer->getAnimations();
	}

	registerAnimation("standing run forward.fbx");
	registerAnimation("standing run back.fbx");
	registerAnimation("standing run right.fbx");
	registerAnimation("standing run left.fbx");
	setDefaultState("standing run forward.fbx");

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

	// forward > back / left / right

	// float형으로 등록된 "speed"가 0보다 작을때 forward > back
	makeTransition("standing run forward.fbx", "standing run back.fbx", "speed", -1, AnimationFSM::ValueType::FLOATTYPE, 0);
	// float형으로 등록된 "sideSpeed"가 0보다 작을때 forward > left
	makeTransition("standing run forward.fbx", "standing run left.fbx", "sideSpeed", -1, AnimationFSM::ValueType::FLOATTYPE, 0);
	// float형으로 등록된 "sideSpeed"가 0보다 클때 forward > right
	makeTransition("standing run forward.fbx", "standing run right.fbx", "sideSpeed", 1, AnimationFSM::ValueType::FLOATTYPE, 0);

	// back > forward / left / right

	// float형으로 등록된 "speed"가 0보다 클때 back > forward
	makeTransition("standing run back.fbx", "standing run forward.fbx", "speed", 1, AnimationFSM::ValueType::FLOATTYPE, 0);
	// float형으로 등록된 "sideSpeed"가 0보다 작을때 back > left
	makeTransition("standing run back.fbx", "standing run left.fbx", "sideSpeed", -1, AnimationFSM::ValueType::FLOATTYPE, 0);
	// float형으로 등록된 "speed"가 0보다 클때 back > right
	makeTransition("standing run back.fbx", "standing run right.fbx", "sideSpeed", 1, AnimationFSM::ValueType::FLOATTYPE, 0);

	// left > forward / back / right

	// float형으로 등록된 "speed"가 0보다 작을때 left > forward
	makeTransition("standing run left.fbx", "standing run forward.fbx", "speed", 1, AnimationFSM::ValueType::FLOATTYPE, 0);
	// float형으로 등록된 "speed"가 0보다 클때 left > back
	makeTransition("standing run left.fbx", "standing run back.fbx", "speed", -1, AnimationFSM::ValueType::FLOATTYPE, 0);
	// float형으로 등록된 "sideSpeed"가 0보다 클때 left > right
	makeTransition("standing run left.fbx", "standing run right.fbx", "sideSpeed", 1, AnimationFSM::ValueType::FLOATTYPE, 0);

	// right > forward / back / left

	// float형으로 등록된 "sideSpeed"가 0보다 작을때 right > left
	makeTransition("standing run right.fbx", "standing run left.fbx", "sideSpeed", -1, AnimationFSM::ValueType::FLOATTYPE, 0);
	// float형으로 등록된 "sideSpeed"가 0보다 클때 right > forward
	makeTransition("standing run right.fbx", "standing run forward.fbx", "speed", 1, AnimationFSM::ValueType::FLOATTYPE, 0);
	// float형으로 등록된 "speed"가 0보다 작을때 right > back
	makeTransition("standing run right.fbx", "standing run back.fbx", "sidepeed", -1, AnimationFSM::ValueType::FLOATTYPE, 0);

}

void AnimationFSM::update()
{
	if (!fbxModelAnimations) return;

	auto it = stateTable.find(currentState);
	// 만약에 현재 상태를 테이블에서 찾지 못한다면 디폴트로 해준다.
	if (it == stateTable.end())
		currentState = defaultState;



	if (::GetAsyncKeyState(VK_UP) & 0x8000f)
	{
		setFloat("speed", 1);
		setFloat("sideSpeed", 0);
	}
	else if (::GetAsyncKeyState(VK_DOWN) & 0x8000f)
	{
		setFloat("speed", -1);
		setFloat("sideSpeed", 0);
	}
	else if (::GetAsyncKeyState(VK_RIGHT) & 0x8000f)
	{
		setFloat("sideSpeed", 1);
		setFloat("speed", 0);
	}
	else if (::GetAsyncKeyState(VK_LEFT) & 0x8000f)
	{
		setFloat("sideSpeed", -1);
		setFloat("speed", 0);
	}
	else
	{
		setFloat("sideSpeed", 0);
		setFloat("speed", 0);
	}



	// 애니메이션을 플레이해주고 // 현재플레이중이면 계속플레이
	fbxModelAnimations->playWithFileName(currentState);

	updateAllTrasitions(currentState);
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

void BulletScript::onCollisionStay(GameObjectWithCollision & other)
{
	if (other.gameObject->getTag() == "Remover")
	{
		GameObject::Destroy(gameObject);
	}
}

const string Terrain::filePathToLoadTerrain = "../Terrain/";

void Terrain::render()
{
	if (!mesh) return;

	// 현재 transform행렬로 적용시킨다.
	device->SetTransform(D3DTS_WORLD, &transform->getTransformMatrix());

	mesh->DrawSubset(0);

	return;

	// 파일이 로드되어서 값이 있는 경우만 그려준다.
	for (int i = 0; i < mtrls.size(); ++i)
	{
		device->SetMaterial(&mtrls[i]);
		device->SetTexture(0, textures[i]);
		mesh->DrawSubset(i);
	}
}

void Terrain::start()
{
	this->device = &(gameObject->getDevice());
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

	ifstream iFStream((filePathToLoadTerrain + fileName).c_str(), ios_base::binary);

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
		heightMap[i] = infoToRead[i] * heightValue;

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
		D3DXMESH_MANAGED,

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
	// 맵사이즈 // 버텍스 총 개수(행 버텍스 개수 * 열 버텍스 개수) // 만큼 순회하면서 값들을 채워준다.
	for (int i = 0; i < verticesPerColumn; ++i)
	{

		for (int j = 0; j < verticesPerRow; ++j)
		{
			// position 계산
			x = j * distanceOfVertices;
			z = i * distanceOfVertices;
			//Trace::Write("TAG_DEBUG", "x", x);
			//Trace::Write("TAG_DEBUG", "z", z);
			//Trace::Write("TAG_DEBUG", "");

			u = j * distanceForU;
			v = i * distanceForV;

			terrainV[i] =
			{
				// Position
				x, heightMap[i], z,
				// Normal // 일단 항상 up으로 설정
				0,1,0,
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

			// abc
			terrainI[indexCounter++] = i * verticesPerRow + j;
			terrainI[indexCounter++] = i * verticesPerRow + j + 1;
			terrainI[indexCounter++] = (i + 1) * verticesPerRow + j;

			// cbd
			terrainI[indexCounter++] = (i + 1) * verticesPerRow + j;
			terrainI[indexCounter++] = i * verticesPerRow + j + 1;
			terrainI[indexCounter++] = (i + 1) * verticesPerRow + j + 1;
		}


	}
	indexCounter;
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


