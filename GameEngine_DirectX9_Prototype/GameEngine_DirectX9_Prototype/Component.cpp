#include "Component.h"
#include "GameObject.h"
#include "Scene.h"
#include "Audio.h"
#include "Physics.h"
#include "Utility.h"


void MoveScript::update()
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
		transform->setPosition(transform->getPosition() + (transform->getForward()));
	if (::GetAsyncKeyState(VK_DOWN) & 0x8000f)
		transform->setPosition(transform->getPosition() + (-1)*(transform->getForward()));

	if (::GetAsyncKeyState(VK_RIGHT) & 0x8000f)
		transform->setPosition(transform->getPosition() + (transform->getRight()));

	if (::GetAsyncKeyState(VK_LEFT) & 0x8000f)
		transform->setPosition(transform->getPosition() + (-1)*(transform->getRight()));

	if (::GetAsyncKeyState(VK_SPACE) & 0x8000f)
		gameObject->getPhysics().addForce(gameObject, (-1)*transform->getForward());


	if (::GetAsyncKeyState('N') & 0x8000f)
			transform->setRotation(transform->getRotation() + Vector3{ 0,-.05f,0 });

	if (::GetAsyncKeyState('M') & 0x8000f)
			transform->setRotation(transform->getRotation() + Vector3{ 0,.05f,0 });
}

void MoveScript::start()
{
	AudioSource * temp = gameObject->getAudio().getAudioSource("BS_BackGround_1.mp3");
	if (temp)
	{
		temp->play(false);
		temp->setVolume(7.0f);
	}
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

const D3DXVECTOR3 Transform::WorldRight_DX{ 1,0,0 };
const D3DXVECTOR3 Transform::WorldUp_DX{ 0,1,0 };
const D3DXVECTOR3 Transform::WorldForward_DX{ 0, 0, 1 };

const D3DXMATRIX Transform::IdentityMatrix_DX
{
	1,0,0,0,
	0,1,0,0,
	0,0,1,0,
	0,0,0,1
};


void Transform::transformUpdate(bool dirty,  const D3DXMATRIX & parentPositionMatrix, const D3DXMATRIX & parentRotationMatrix)
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
		setTransformMatrix_DX(parentPositionMatrix, parentRotationMatrix);
		this->dirty = false;
	}


	vector<GameObject*> & children = gameObject->getChildren();


	// ����� ��Ƽ�÷��װ� �����־ �߰� ���ϵ忡 �������� ���ɼ��� �ֱ� ������
	// ��� ��ü�� ���ؼ� �������ش�.
	for (auto it : children)
	{
		it->getTransform()->transformUpdate(dirty, getPositionMatrix_DX(),getRotationMatrix_DX());
	}

}

void Transform::calcRotationMatrix_DX(const D3DXMATRIX & parentRotationMatrix)
{
	D3DXMATRIX temp;
	D3DXMatrixIdentity(&rotationMatrix_DX);
	D3DXMatrixMultiply(&rotationMatrix_DX, &rotationMatrix_DX, D3DXMatrixRotationX(&temp, rotation.getX()));
	D3DXMatrixMultiply(&rotationMatrix_DX, &rotationMatrix_DX, D3DXMatrixRotationY(&temp, rotation.getY()));
	D3DXMatrixMultiply(&rotationMatrix_DX, &rotationMatrix_DX, D3DXMatrixRotationZ(&temp, rotation.getZ()));
	D3DXMatrixMultiply(&rotationMatrix_DX, &rotationMatrix_DX, &parentRotationMatrix);
}

void Transform::calcPositionMatrix_DX(const D3DXMATRIX & parentPositionMatrix)
{
	Vector3 result{};
	GameObject * parent = gameObject->getParent();
	// �θ�ü �������� right up forward�������� ������ش�.
	// �θ�ü�� ���� ��Ʈ ������Ʈ�� ��� ���� right up forward�� ����Ѵ�.
	if (!parent)
		result = Vector3::Right * position.getX() + Vector3::Up * position.getY() + Vector3::Forward * position.getZ();
	else
		result = parent->getTransform()->getRight() * position.getX() + parent->getTransform()->getUp() * position.getY()
		+ parent->getTransform()->getForward() * position.getZ();

	D3DXMatrixIdentity(&positionMatrix_DX);

	D3DXMatrixTranslation(&positionMatrix_DX, result.getX(), result.getY(), result.getZ());
	// ���������� �θ�ü�� ��ġ��ŭ �̵���Ų��.
	D3DXMatrixMultiply(&positionMatrix_DX, &positionMatrix_DX, &parentPositionMatrix);
}

void Transform::setPosition_physics(const Vector3 & other)
{
	position = other; dirty = true;
}

void Transform::setRotation_physics(const Vector3 & other)
{
	rotation.setX(constrainLessThan360(other.getX()));
	rotation.setY(constrainLessThan360(other.getY()));
	rotation.setZ(constrainLessThan360(other.getZ()));
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
		// ���� ����Ǿ����� ������ٵ� �ִٸ� ���������� ó�����ش�.
	if(gameObject->getRigidBody())
		gameObject->getPhysics().setTransformToSystem(gameObject);
}

void Transform::setRotation(const Vector3 & other)
{
	rotation.setX(constrainLessThan360(other.getX()));
	rotation.setY(constrainLessThan360(other.getY()));
	rotation.setZ(constrainLessThan360(other.getZ()));
	setDirectionVectorWithRotation_DX();
	dirty = true;
	// ���� ����Ǿ����� ������ٵ� �ִٸ� ���������� ó�����ش�.
	if(gameObject->getRigidBody())
		gameObject->getPhysics().setTransformToSystem(gameObject);
}

void Transform::setPosition(float x, float y, float z)

{
	position.setX(x);
	position.setY(y);
	position.setZ(z);
	dirty = true;
	// ���� ����Ǿ����� ������ٵ� �ִٸ� ���������� ó�����ش�.
	if (gameObject->getRigidBody())
		gameObject->getPhysics().setTransformToSystem(gameObject);
}

void Transform::setRotation(float x, float y, float z)
{
	rotation.setX(constrainLessThan360(x));
	rotation.setY(constrainLessThan360(y));
	rotation.setZ(constrainLessThan360(z));
	setDirectionVectorWithRotation_DX();
	dirty = true;
	// ���� ����Ǿ����� ������ٵ� �ִٸ� ���������� ó�����ش�.
	if (gameObject->getRigidBody())
		gameObject->getPhysics().setTransformToSystem(gameObject);
}



MainCamera::MainCamera(GameObject * go, Transform * tf)
	: Component(go, tf)
{
	start();
	
	mainObjTag = Scene::MainObjTag::MOT_MAINCAMERA;
	gameObject->getScene().registerMainObject(gameObject, (Scene::MainObjTag)mainObjTag);
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


	// ��ġ �ݴ�� ���
	D3DXMatrixMultiply(&v, &v, D3DXMatrixTranslation(&temp, -transform->getPosition().getX(),
		-transform->getPosition().getY(), -transform->getPosition().getZ()));

	// �����̼� �ݴ�� ���
	D3DXMatrixMultiply(&v, &v, D3DXMatrixRotationX(&temp, -transform->getRotation().getX()));
	D3DXMatrixMultiply(&v, &v, D3DXMatrixRotationY(&temp, -transform->getRotation().getY()));
	D3DXMatrixMultiply(&v, &v, D3DXMatrixRotationZ(&temp, -transform->getRotation().getZ()));

	gameObject->getDevice().SetTransform(D3DTS_VIEW, &v);
}

void MainCamera::getViewMatrix(D3DXMATRIX* v)
{
	if (!v) return;


	D3DXMATRIX temp;
	D3DXMatrixIdentity(v);


	// ��ġ �ݴ�� ���
	D3DXMatrixMultiply(v, v, D3DXMatrixTranslation(&temp, -transform->getPosition().getX(),
		-transform->getPosition().getY(), -transform->getPosition().getZ()));

	// �����̼� �ݴ�� ���
	D3DXMatrixMultiply(v, v, D3DXMatrixRotationX(&temp, -transform->getRotation().getX()));
	D3DXMatrixMultiply(v, v, D3DXMatrixRotationY(&temp, -transform->getRotation().getY()));
	D3DXMatrixMultiply(v, v, D3DXMatrixRotationZ(&temp, -transform->getRotation().getZ()));


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
	d3d::Release<ID3DXBuffer*>(mtrlBuffer);

	optimizeMesh(adjBuffer);

	// ���Ϸ��� �������� ������
	d3d::Release<ID3DXBuffer*>(adjBuffer); // done w/ buffer
}

void MeshRenderer::render()
{
	if (!mesh) return;

	// ���� transform��ķ� �����Ų��.
	device->SetTransform(D3DTS_WORLD, &transform->getTransformMatrix_DX());

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
	gameObject->getPhysics().setTransformFromSystem(gameObject);
	// ����������Ʈ�� �Ǿ��� ������ ��Ƽ�÷��׸� ���ش�.
	transform->setDirty(true);
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
}

void RigidBody::setSphereCollider(float radius)
{
	gameObject->getPhysics().setSphereCollider(gameObject, radius);
}

