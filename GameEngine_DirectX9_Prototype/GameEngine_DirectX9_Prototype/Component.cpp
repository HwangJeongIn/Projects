#include "Component.h"
#include "GameObject.h"
#include "Scene.h"
#include "Audio.h"
#include "Physics.h"
#include "FbxParser.h"
#include "FbxMeshInfo.h"
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
		setTransformMatrix_DX(parentPositionMatrix, parentRotationMatrix);
		this->dirty = false;
	}


	vector<GameObject*> & children = gameObject->getChildren();


	// 현재는 더티플래그가 꺼져있어도 중간 차일드에 켜져있을 가능성이 있기 때문에
	// 모든 객체에 대해서 전달해준다.
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
	// 부모객체 기준으로 right up forward기준으로 계산해준다.
	// 부모객체가 없는 루트 오브젝트인 경우 월드 right up forward를 사용한다.
	if (!parent)
		result = Vector3::Right * position.getX() + Vector3::Up * position.getY() + Vector3::Forward * position.getZ();
	else
		result = parent->getTransform()->getRight() * position.getX() + parent->getTransform()->getUp() * position.getY()
		+ parent->getTransform()->getForward() * position.getZ();

	D3DXMatrixIdentity(&positionMatrix_DX);

	D3DXMatrixTranslation(&positionMatrix_DX, result.getX(), result.getY(), result.getZ());
	// 마지막으로 부모객체의 위치만큼 이동시킨다.
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
		// 값이 변경되었을때 리지드바디가 있다면 물리에서도 처리해준다.
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
	rotation.setX(constrainLessThan360(x));
	rotation.setY(constrainLessThan360(y));
	rotation.setZ(constrainLessThan360(z));
	setDirectionVectorWithRotation_DX();
	dirty = true;
	// 값이 변경되었을때 리지드바디가 있다면 물리에서도 처리해준다.
	if (gameObject->getRigidBody())
		gameObject->getPhysics().setTransformToSystem(gameObject);
}



void MainCamera::update()
{
	// 메인카메라에서 오디오를 최신화 시킨다.
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


	// 위치 반대로 계산
	D3DXMatrixMultiply(&v, &v, D3DXMatrixTranslation(&temp, -transform->getPosition().getX(),
		-transform->getPosition().getY(), -transform->getPosition().getZ()));

	// 로테이션 반대로 계산
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


	// 위치 반대로 계산
	D3DXMatrixMultiply(v, v, D3DXMatrixTranslation(&temp, -transform->getPosition().getX(),
		-transform->getPosition().getY(), -transform->getPosition().getZ()));

	// 로테이션 반대로 계산
	D3DXMatrixMultiply(v, v, D3DXMatrixRotationX(&temp, -transform->getRotation().getX()));
	D3DXMatrixMultiply(v, v, D3DXMatrixRotationY(&temp, -transform->getRotation().getY()));
	D3DXMatrixMultiply(v, v, D3DXMatrixRotationZ(&temp, -transform->getRotation().getZ()));


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
	device->SetTransform(D3DTS_WORLD, &transform->getTransformMatrix_DX());

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

const unsigned long FbxMeshRenderer::FbxVertex::DefaultFVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
const unsigned long FbxMeshRenderer::DefaultOptimizeFlag = D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_COMPACT | D3DXMESHOPT_VERTEXCACHE;

void FbxMeshRenderer::render()
{
	if (meshs.size() ==0) return;

	// 현재 transform행렬로 적용시킨다.
	device->SetTransform(D3DTS_WORLD, &transform->getTransformMatrix_DX());

	//device->SetMaterial(nullptr);
	//device->SetTexture(0, nullptr);
	int meshCount = meshs.size();
	int fbxCount = fbxMeshInfos.size();

	//meshs[2]->DrawSubset(0);
	//return;

	for (auto it = meshs.begin(); it != meshs.end(); ++it)
	{
		//// 텍스처와 머티리얼 적용
		//device->SetMaterial(nullptr);
		//device->SetTexture(0, nullptr);
		// 널포인터가 아닐때 그려준다.
		// 여기서 텍스처와 머티리얼 추가
		if((*it) !=nullptr)
			(*it)->DrawSubset(0);
	}
	//mesh->DrawSubset(0);
	// 파일이 로드되어서 값이 있는 경우만 그려준다.
	//for (int i = 0; i < mtrls.size(); ++i)
	//{
	//	device->SetMaterial(&mtrls[i]);
	//	device->SetTexture(0, textures[i]);
	//	mesh->DrawSubset(i);
	//}
}

FbxMeshRenderer::FbxMeshRenderer(GameObject * go, Transform * tf)
	: Component(go, tf), scene(nullptr)
{
	device = &(gameObject->getDevice());
}

void FbxMeshRenderer::loadFbxFile(const string & fileName)
{
	//gameObject->getFbxParser().loadSceneFromFbxFile(fileName, (fbxInfo.getFbxScene()));
	// 씬이 제대로 초기화 되어있지 않다면 리턴
	//if (!(*fbxInfo.getFbxScene())) return;

	gameObject->getFbxParser().loadSceneFromFbxFile(fileName, &scene);

	if (scene == nullptr) return;

	FbxNode * rootNode = scene->GetRootNode();
	// 루트 노드를 받아온다.
	//FbxNode * rootNode = (*fbxInfo.getFbxScene())->GetRootNode();

	// 루트노드를 시작으로 모든 메쉬 정보를 가진 노드들을 추출한다.
	// 저장되는 곳은 fbxMeshInfos이다.
	getAllFbxMeshInfosFromRoot(rootNode);

	// 노드를 순회하면서 메쉬정보부터 초기화하고
	//fbxInfo.loadMeshFromNodes(rootNode);

	for (auto it = fbxMeshInfos.begin(); it != fbxMeshInfos.end(); ++it)
	{
		// 메쉬정보로부터 컨트롤 포인트를 초기화 한다.
		(*it)->processControlPoints();
		// 마지막으로 메쉬정보, 컨트롤 포인트로 버텍스 / 인덱스를 초기화 한다.
		(*it)->processVertices();

		// 미리 초기화된 FbxMesh를 가지는 FbxMeshInfo에서 부터 필요한 정보 추출 완료 // fbxMeshInfos에 담겨있음
		//---------------------------------------------------------------------------
		// 정보를 가지고 DirectX에서 지원하는 mesh형식으로 변환

		ID3DXMesh * mesh;
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
			FbxMeshRenderer::FbxVertex::DefaultFVF,
			device,
			// 복제된 메쉬가 담길 포인터
			&mesh);

		if (FAILED(hr))
		{
			mesh = nullptr;
			meshs.push_back(mesh);
			continue;
		}

		// mesh내부의 버텍스 정보와 인덱스 정보를 초기화 시켜준다.
		processVertices(mesh, (*it));
		processIndices(mesh, (*it));
		processSubsets(mesh);
		optimizeMesh(mesh);

		meshs.push_back(mesh);
	}

}

void FbxMeshRenderer::getAllFbxMeshInfosFromRoot(FbxNode * root)
{

	FbxNodeAttribute * nodeAtrribute = root->GetNodeAttribute();

	// 모든 노드를 돌면서 Mesh 속성이 붙은것들을 추출한다.
	if (nodeAtrribute)
	{
		if (nodeAtrribute->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			FbxMeshInfo * temp = new FbxMeshInfo();
			temp->setMesh(root->GetMesh());
			fbxMeshInfos.push_back(temp);
		}
	}

	// 자식들을 돌면서 나머지 메쉬를 찾는다.
	const int childCount = root->GetChildCount();
	for (unsigned int i = 0; i < childCount; ++i)
	{
		getAllFbxMeshInfosFromRoot(root->GetChild(i));
	}
}

void FbxMeshRenderer::processVertices(ID3DXMesh * mesh, FbxMeshInfo * fbxMeshInfo)
{
	if (!mesh || !fbxMeshInfo) return;
	// mesh에 넣어줄때 fbxVertex형식으로 넣어줄것이다.
	FbxVertex * fbxV = nullptr;
	MyVertex * tVertex = nullptr;

	int t1 = mesh->GetNumVertices();
	int t2 = fbxMeshInfo->getVertexCount();

	// 생성한 메쉬를 잠그고 그안에 버텍스 정보를 집어넣는다.
	// 버텍스 정보는 포지션 / 노말 / UV값이다.
	mesh->LockVertexBuffer(0, (void**)&fbxV);
	for (int i = 0; i < fbxMeshInfo->getVertexCount(); ++i)
	{
		tVertex = fbxMeshInfo->getVertex(i);
		// 값이 있을때
		if (tVertex)
		{
			// 각각의 정보를 초기화 시켜준다.
			fbxV[i] = {
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
		fbxV[i] = {
			// Position
			0,0,0,
			// Normal
			0,0,0,
			// UV
			0,0
		};
	}

	t1 = mesh->GetNumVertices();
	t2 = fbxMeshInfo->getVertexCount();

	mesh->UnlockVertexBuffer();
}

void FbxMeshRenderer::processIndices(ID3DXMesh * mesh, FbxMeshInfo * fbxMeshInfo)
{
	if (!mesh || !fbxMeshInfo) return;

	// mesh에 넣어줄때 fbxVertex형식으로 넣어줄것이다.
	unsigned short * fbxI = nullptr;
	int tIndex = -1;

	int t1 = mesh->GetNumFaces();
	int t2 = fbxMeshInfo->getIndexCount();
	int t3 = fbxMeshInfo->getVertexCount();
	int t4 = fbxMeshInfo->getTriCount();

	// 생성한 메쉬를 잠그고 그안에 버텍스 정보를 집어넣는다.
	// 버텍스 정보는 포지션 / 노말 / UV값이다.
	mesh->LockIndexBuffer(0, (void**)&fbxI);
	for (int i = 0; i < fbxMeshInfo->getIndexCount(); ++i)
	{

		tIndex = fbxMeshInfo->getIndex(i);
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

void FbxMeshRenderer::processSubsets(ID3DXMesh * mesh)
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

void FbxMeshRenderer::optimizeMesh(ID3DXMesh * mesh)
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
