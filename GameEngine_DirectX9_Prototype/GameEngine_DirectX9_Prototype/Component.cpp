#include "Component.h"
#include "GameObject.h"
#include "Scene.h"
#include "Audio.h"
#include "Physics.h"
#include "FbxParser.h"
#include "FbxModelMesh.h"
#include "Utility.h"
#include "Trace.h"

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
		transform->setPosition(transform->getPosition() + FrameTime::getDeltaTime()* .1f *(transform->getForward()));
	if (::GetAsyncKeyState(VK_DOWN) & 0x8000f)
		transform->setPosition(transform->getPosition() + FrameTime::getDeltaTime()*.1f *(-1)*(transform->getForward()));

	if (::GetAsyncKeyState(VK_RIGHT) & 0x8000f)
		transform->setPosition(transform->getPosition() + FrameTime::getDeltaTime()*.1f * (transform->getRight()));

	if (::GetAsyncKeyState(VK_LEFT) & 0x8000f)
		transform->setPosition(transform->getPosition() + FrameTime::getDeltaTime()*.1f * (-1)*(transform->getRight()));

	if (::GetAsyncKeyState(VK_SPACE) & 0x8000f)
		gameObject->getPhysics().addForce(gameObject, FrameTime::getDeltaTime() *.1f *(-1)*transform->getForward());


	if (::GetAsyncKeyState('N') & 0x8000f)
			transform->setRotation(transform->getRotation() + FrameTime::getDeltaTime() *.1f * Vector3{ 0,-.05f,0 });

	if (::GetAsyncKeyState('M') & 0x8000f)
			transform->setRotation(transform->getRotation() + FrameTime::getDeltaTime() *.1f * Vector3{ 0,.05f,0 });
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
		// ���� ����Ǿ����� ������ٵ� �ִٸ� ���������� ó�����ش�.
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
	rotation.setX(constrainLessThan180(x));
	rotation.setY(constrainLessThan180(y));
	rotation.setZ(constrainLessThan180(z));
	setDirectionVectorWithRotation_DX();
	dirty = true;
	// ���� ����Ǿ����� ������ٵ� �ִٸ� ���������� ó�����ش�.
	if (gameObject->getRigidBody())
		gameObject->getPhysics().setTransformToSystem(gameObject);
}



void MainCamera::update()
{
	// ����ī�޶󿡼� ������� �ֽ�ȭ ��Ų��.
	setViewSpace();

	if (::GetAsyncKeyState('E') & 0x8000f)
		transform->setRotation(transform->getRotation() + Vector3{ 0,.05f,0 });
	if (::GetAsyncKeyState('Q') & 0x8000f)
		transform->setRotation(transform->getRotation() + Vector3{ 0,-.05f,0 });

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

const unsigned long FbxModelRenderer::FbxModelVertex::DefaultFVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
const unsigned long FbxModelRenderer::DefaultOptimizeFlag = D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_COMPACT | D3DXMESHOPT_VERTEXCACHE;
const string FbxModelRenderer::filePathToLoadFbxMeshFiles = "../Fbx/Models/";
const string FbxModelRenderer::filePathToLoadFbxTextureFiles = "../Fbx/Textures/";

void FbxModelRenderer::render()
{
	if (meshs.size() ==0) return;

	// ���� transform��ķ� �����Ų��.
	device->SetTransform(D3DTS_WORLD, &transform->getTransformMatrix_DX());

	//device->SetMaterial(nullptr);
	//device->SetTexture(0, nullptr);
	int meshCount = meshs.size();
	int fbxCount = fbxModelMeshes.size();

	//meshs[2]->DrawSubset(0);
	//return;
	map<ID3DXMesh *, vector<D3DMATERIAL9>>::iterator tempMaterialForMesh;
	map<ID3DXMesh *, vector<IDirect3DTexture9*>>::iterator tempTexturesForMesh;
	D3DMATERIAL9 * material = nullptr;
	IDirect3DBaseTexture9 * texture = nullptr;

	for(int i =0; i< meshs.size(); ++i)
	{
		// �ϴ� �ʿ� ��ϵǾ��ִ��� Ȯ���Ѵ�.
		tempMaterialForMesh = materialsTable.find(meshs[i]);
		tempTexturesForMesh = texturesTable.find(meshs[i]);

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

		if(material)
			device->SetMaterial(material);
		if(texture)
			device->SetTexture(0, texture);

		//// �ؽ�ó�� ��Ƽ���� ����
		//device->SetMaterial(nullptr);
		//device->SetTexture(0, nullptr);
		// �������Ͱ� �ƴҶ� �׷��ش�.
		// ���⼭ �ؽ�ó�� ��Ƽ���� �߰�
		if(meshs[i] !=nullptr)
			meshs[i]->DrawSubset(0);
	}
	//mesh->DrawSubset(0);
	// ������ �ε�Ǿ ���� �ִ� ��츸 �׷��ش�.
	//for (int i = 0; i < mtrls.size(); ++i)
	//{
	//	device->SetMaterial(&mtrls[i]);
	//	device->SetTexture(0, textures[i]);
	//	mesh->DrawSubset(i);
	//}
}

FbxModelRenderer::FbxModelRenderer(GameObject * go, Transform * tf)
	: Component(go, tf), scene(nullptr)
{
	device = &(gameObject->getDevice());
}

void FbxModelRenderer::setScale(const Vector3 & value)
{

	float xScale = constrainNegativeNumber(value.getX());
	float yScale = constrainNegativeNumber(value.getY());
	float zScale = constrainNegativeNumber(value.getZ());
	
	for (int i = 0; i < meshs.size(); ++i)
	{
		ID3DXMesh * mesh = nullptr;
		mesh = meshs[i];
		FbxModelMesh * fbxModelMesh = nullptr;
		fbxModelMesh = fbxModelMeshes[i];

		if (!mesh||!fbxModelMesh) continue;

		FbxModelVertex * fbxV = nullptr;
		MyVertex * tVertex = nullptr;

		processVertices(mesh, fbxModelMesh);
		mesh->LockVertexBuffer(0, (void**)&fbxV);
		for (int j = 0; j < fbxModelMeshes[i]->getVertexCount(); ++j)
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
	//gameObject->getFbxParser().loadSceneFromFbxFile(fileName, (fbxInfo.getFbxScene()));
	// ���� ����� �ʱ�ȭ �Ǿ����� �ʴٸ� ����
	//if (!(*fbxInfo.getFbxScene())) return;

	gameObject->getFbxParser().loadSceneFromFbxFile(filePathToLoadFbxMeshFiles+fileName, &scene);

	if (scene == nullptr) return;

	FbxNode * rootNode = scene->GetRootNode();
	// ��Ʈ ��带 �޾ƿ´�.
	//FbxNode * rootNode = (*fbxInfo.getFbxScene())->GetRootNode();

	// ��Ʈ��带 �������� ��� �޽� ������ ���� ������ �����Ѵ�.
	// ����Ǵ� ���� fbxMeshInfos�̴�.
	getAllFbxModelMeshesFromRoot(rootNode);

	// ��带 ��ȸ�ϸ鼭 �޽��������� �ʱ�ȭ�ϰ�
	//fbxInfo.loadMeshFromNodes(rootNode);

	for (auto it = fbxModelMeshes.begin(); it != fbxModelMeshes.end(); ++it)
	{
		// �޽������κ��� ��Ʈ�� ����Ʈ�� �ʱ�ȭ �Ѵ�.
		(*it)->processControlPoints();
		// ���������� �޽�����, ��Ʈ�� ����Ʈ�� ���ؽ� / �ε����� �ʱ�ȭ �Ѵ�.
		(*it)->processVertices();

		// �̸� �ʱ�ȭ�� FbxMesh�� ������ fbxModelMesh���� ���� �ʿ��� ���� ���� �Ϸ� // fbxModelMeshes�� �������
		//---------------------------------------------------------------------------
		// ������ ������ DirectX���� �����ϴ� mesh�������� ��ȯ

		ID3DXMesh * mesh;
		HRESULT hr = 0;

		hr = D3DXCreateMeshFVF(
			// �޽��� ���� ���� ���� 
			// ���⼭ �ε��� ������ ũ�⵵ �ڵ����� �˼� �ִ�.
			// �ﰢ���� ����/3 = �ε����� ��
			(*it)->getTriCount(),

			// �޽��� ���� ���ؽ��� ��
			(*it)->getVertexCount(),

			// �޽��� ����µ� �̿�� �ϳ� �̻��� �÷���
			// ���⼭�� �޽��� ���� �޸� Ǯ���� �����ǵ��� �Ͽ���.
			D3DXMESH_MANAGED,

			// ������ �޽��� ����µ� �̿�� ����
			FbxModelRenderer::FbxModelVertex::DefaultFVF,
			device,
			// ������ �޽��� ��� ������
			&mesh);

		if (FAILED(hr))
		{
			mesh = nullptr;
			meshs.push_back(mesh);
			continue;
		}

		// mesh������ ���ؽ� ������ �ε��� ������ �ʱ�ȭ �����ش�.
		processVertices(mesh, (*it));
		processIndices(mesh, (*it));
		processTextures(mesh, (*it));

		processSubsets(mesh);
		optimizeMesh(mesh);

		meshs.push_back(mesh);
	}

}

void FbxModelRenderer::getAllFbxModelMeshesFromRoot(FbxNode * node)
{

	FbxNodeAttribute * nodeAtrribute = node->GetNodeAttribute();

	// ��� ��带 ���鼭 Mesh �Ӽ��� �����͵��� �����Ѵ�.
	if (nodeAtrribute)
	{
		switch (nodeAtrribute->GetAttributeType())
		{
			case FbxNodeAttribute::eMesh:
			{
				FbxModelMesh * temp = new FbxModelMesh();
				// ��ȯ�۾��� ���ش�. // ��Ʈ������Ʈ�� �̷���� ��� �͵��� �ﰢ��2���� �������ش�.
				gameObject->getFbxParser().convertGeometryInfo(&nodeAtrribute);
				temp->setNode(node);
				fbxModelMeshes.push_back(temp);
			}
			break;

			case FbxNodeAttribute::eSkeleton:
			{

			}
			break;
		}

	}

	// �ڽĵ��� ���鼭 ������ �޽��� ã�´�.
	const int childCount = node->GetChildCount();
	for (unsigned int i = 0; i < childCount; ++i)
	{
		getAllFbxModelMeshesFromRoot(node->GetChild(i));
	}
}

void FbxModelRenderer::processVertices(ID3DXMesh * mesh, FbxModelMesh * fbxModelMesh)
{
	if (!mesh || !fbxModelMesh) return;
	// mesh�� �־��ٶ� fbxVertex�������� �־��ٰ��̴�.
	FbxModelVertex * fbxV = nullptr;
	MyVertex * tVertex = nullptr;

	//int t1 = mesh->GetNumVertices();
	//int t2 = fbxModelMesh->getVertexCount();

	// ������ �޽��� ��װ� �׾ȿ� ���ؽ� ������ ����ִ´�.
	// ���ؽ� ������ ������ / �븻 / UV���̴�.
	mesh->LockVertexBuffer(0, (void**)&fbxV);
	for (int i = 0; i < fbxModelMesh->getVertexCount(); ++i)
	{
		tVertex = fbxModelMesh->getVertex(i);
		// ���� ������
		if (tVertex)
		{
			// ������ ������ �ʱ�ȭ �����ش�.
			fbxV[i] = {
				// Position
				(float)tVertex->getPosition().mData[0]/10,(float)tVertex->getPosition().mData[1]/10, (float)tVertex->getPosition().mData[2]/10,
				// Normal
				(float)tVertex->getNormal().mData[0]/10,(float)tVertex->getNormal().mData[1]/10, (float)tVertex->getNormal().mData[2]/10,
				// UV
				(float)tVertex->getUV().mData[0],(float)tVertex->getUV().mData[1]
			};
			//if (i == 0 || i ==1)
			//{
			//	Trace::Write("TAG_DEBUG", "!u", fbxV[i].uv[0]);
			//	Trace::Write("TAG_DEBUG", "!v", fbxV[i].uv[1]);
			//}

			continue;
		}
		// ���� ������ // nullptr�϶�
		fbxV[i] = {
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

void FbxModelRenderer::processIndices(ID3DXMesh * mesh, FbxModelMesh * fbxModelMesh)
{
	if (!mesh || !fbxModelMesh) return;

	// mesh�� �־��ٶ� fbxVertex�������� �־��ٰ��̴�.
	unsigned short * fbxI = nullptr;
	int tIndex = -1;

	//int t1 = mesh->GetNumFaces();
	//int t2 = fbxModelMesh->getIndexCount();
	//int t3 = fbxModelMesh->getVertexCount();
	//int t4 = fbxModelMesh->getTriCount();

	// ������ �޽��� ��װ� �׾ȿ� ���ؽ� ������ ����ִ´�.
	// ���ؽ� ������ ������ / �븻 / UV���̴�.
	mesh->LockIndexBuffer(0, (void**)&fbxI);
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
	mesh->UnlockIndexBuffer();
}

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

void FbxModelRenderer::processTextures(ID3DXMesh * mesh, FbxModelMesh * fbxModelMesh)
{
	if (!mesh || !fbxModelMesh) return;
	FbxNode * fbxMeshNode = fbxModelMesh->getNode();
	if (!fbxMeshNode) return;


	map<ID3DXMesh *, vector<D3DMATERIAL9>>::iterator materials_it = materialsTable.find(mesh);
	map<ID3DXMesh *, vector<IDirect3DTexture9*>>::iterator textures_it = texturesTable.find(mesh);

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

	// �ٽ� �Ӽ� �����͸� �����ϱ� ���ؼ� ���۸� ����ش�.
	mesh->LockAttributeBuffer(0, &attributeBuffer);

	int ttt = mesh->GetNumFaces();

	// ������� 0������ �������ش�.
	for (int i = 0; i < mesh->GetNumFaces(); ++i)
		attributeBuffer[i] = 0;


	// ������ �Ϸ�Ǿ����Ƿ� �ٽ� ����ش�.
	mesh->UnlockAttributeBuffer();
}

void FbxModelRenderer::optimizeMesh(ID3DXMesh * mesh)
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
