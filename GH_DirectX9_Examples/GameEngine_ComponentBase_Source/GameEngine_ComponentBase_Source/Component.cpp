#include "Component.h"
#include "GameObject.h"


void MoveScript::start()
{
	cout << "Movescript start" << endl;
	string temp;
	cout << getGameObject()->getPath(temp) << endl;
	cout << temp << endl;
}