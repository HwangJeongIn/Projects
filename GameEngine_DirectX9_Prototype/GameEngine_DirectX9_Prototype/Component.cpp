#include "Component.h"
#include "GameObject.h"
#include "Scene.h"
#include "Utility.h"


void MoveScript::start()
{
	//cout << "Movescript start" << endl;
	//string temp; getGameObject()->getPath(temp);
	//cout<<"MS start :: "<<getGameObject()->getName()<<" : " << temp << endl;
}

void MoveScript::update()
{
	//string temp; gameObject->getPath(temp);
	//cout << "MS update :: " << gameObject->getName() << " : " << temp << endl;

	//if (InputManager::GetKeyDown(KeyCode::Space))
	//{
 // 		if (gameObject->getTag() == "player1")
	//		gameObject->getScene().Destroy(gameObject);
	//}
}

const D3DXVECTOR3 Transform::WorldRight_DX{ 1,0,0 };
const D3DXVECTOR3 Transform::WorldUp_DX{ 0,1,0 };
const D3DXVECTOR3 Transform::WorldForward_DX{ 0, 0, 1 };

void Transform::transformUpdate(bool dirty)
{
	// 비트 연산으로 최종적으로 위에서 최신화 메시지를 받았거나, 
	// 자체적으로 최신화하는 경우에 최신화 해주고 자식객체에게도 최신화 하라고 말해준다.

	dirty |= this->dirty;
	if (dirty)
	{
		// 현재 변환을 부모 변환과 결합해서 계산한다.
		this->dirty = false;
	}
	else
	{
		// 만약에 더이상 최신화시켜도 되지 않으면 빠져나간다.
		return;
	}

	vector<GameObject*> & children = gameObject->getChildren();

	for (auto it : children)
	{
		it->getTransform()->transformUpdate(dirty);
	}

}



void MainCamera::update()
{
	setViewSpace();

	if (::GetAsyncKeyState('E') & 0x8000f)
		transform->setRotation(transform->getRotation() + Vector3{0,.05f,0});
	if (::GetAsyncKeyState('Q') & 0x8000f)
		transform->setRotation(transform->getRotation() + Vector3{ 0,-.05f,0 });

	if (::GetAsyncKeyState('Z') & 0x8000f)
		transform->setRotation(transform->getRotation() + Vector3{ .05f,0,0 });
	if (::GetAsyncKeyState('C') & 0x8000f)
		transform->setRotation(transform->getRotation() + Vector3{ -.05f,0,0 });

	if (::GetAsyncKeyState('W') & 0x8000f)
		transform->setPosition(transform->getPosition() + (transform->getForward()) );
	if (::GetAsyncKeyState('S') & 0x8000f)
		transform->setPosition(transform->getPosition() + (-1)*(transform->getForward()));

	if (::GetAsyncKeyState('D') & 0x8000f)
		transform->setPosition(transform->getPosition() + (transform->getRight()));
	if (::GetAsyncKeyState('A') & 0x8000f)
		transform->setPosition(transform->getPosition() + (-1)*(transform->getRight()));



	//if (InputManager::GetKeyDown(KeyCode::A))
	//	transform->setRotation(transform->getRotation().getX(), transform->getRotation().getY() + 3, transform->getRotation().getX());
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

