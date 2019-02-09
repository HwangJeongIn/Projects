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
