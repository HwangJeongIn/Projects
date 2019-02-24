#include <iostream>
#include "Locator.h"

using namespace std;

class GameObject : public Locator
{
public :
	Scene& getScene() { return Locator::getScene();}
};

int main()
{
	Locator::provideScene(Locator::SystemType::DEBUGTYPE);
	Locator::provideScene(Locator::SystemType::RELEASETYPE);
	GameObject a;
	a.getScene().func2();
	

	return 0;
}