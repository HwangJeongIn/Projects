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

void Transform::transformUpdate(bool dirty)
{
	// ��Ʈ �������� ���������� ������ �ֽ�ȭ �޽����� �޾Ұų�, 
	// ��ü������ �ֽ�ȭ�ϴ� ��쿡 �ֽ�ȭ ���ְ� �ڽİ�ü���Ե� �ֽ�ȭ �϶�� �����ش�.

	dirty |= this->dirty;
	if (dirty)
	{
		// ���� ��ȯ�� �θ� ��ȯ�� �����ؼ� ����Ѵ�.
		this->dirty = false;
	}
	else
	{
		// ���࿡ ���̻� �ֽ�ȭ���ѵ� ���� ������ ����������.
		return;
	}

	vector<GameObject*> & children = gameObject->getChildren();

	for (auto it : children)
	{
		it->getTransform()->transformUpdate(dirty);
	}

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

void MainCamera::getViewMatrix(D3DXMATRIX* V)
{
	// Keep camera's axes orthogonal to eachother
	D3DXVec3Normalize(&_look, &_look);

	D3DXVec3Cross(&_up, &_look, &_right);
	D3DXVec3Normalize(&_up, &_up);

	D3DXVec3Cross(&_right, &_up, &_look);
	D3DXVec3Normalize(&_right, &_right);

	// Build the view matrix:
	float x = -D3DXVec3Dot(&_right, &_pos);
	float y = -D3DXVec3Dot(&_up, &_pos);
	float z = -D3DXVec3Dot(&_look, &_pos);

	(*V)(0, 0) = _right.x; (*V)(0, 1) = _up.x; (*V)(0, 2) = _look.x; (*V)(0, 3) = 0.0f;
	(*V)(1, 0) = _right.y; (*V)(1, 1) = _up.y; (*V)(1, 2) = _look.y; (*V)(1, 3) = 0.0f;
	(*V)(2, 0) = _right.z; (*V)(2, 1) = _up.z; (*V)(2, 2) = _look.z; (*V)(2, 3) = 0.0f;
	(*V)(3, 0) = x;        (*V)(3, 1) = y;     (*V)(3, 2) = z;       (*V)(3, 3) = 1.0f;
}

