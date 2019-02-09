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
