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
	string temp; getGameObject()->getPath(temp);
	cout << "MS update :: " << getGameObject()->getName() << " : " << temp << endl;

	if (InputManager::GetKeyDown(KeyCode::Space))
	{
  		if (getGameObject()->getTag() == "player1")
			Scene::getInstance()->Destroy(getGameObject());
	}
}