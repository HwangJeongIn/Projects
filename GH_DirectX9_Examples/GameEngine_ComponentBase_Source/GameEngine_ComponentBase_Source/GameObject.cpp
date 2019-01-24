#include "GameObject.h"
#include "Scene.h"


GameObject * GameObject::getChild(const string & name)
{
	/*
	2가지 방법 // 아마 나중에 방법2로 수정할듯

	1. children리스트를 순회하면서 선형 탐색

	2. 현재이름을 기반으로 path생성   .../parent's name/현재이름/child's name
	Scene 싱글턴매니저에서 multimap을 사용해서 바로 접근해서 반환

	*/
	// 현재 방법1

	// foreach쓸때 포인터형인지 아닌지 잘확인하면서 쓰자 // 포인터형이 아니라면 다 복사된다.
	for (auto it : children)
	{
		if (it->getName() == name)
			return it;
	}

	// 발견못했을경우
	return nullptr;
}


void GameObject::removeChild(GameObject * child)
{
	// 시간복잡도 고려해서 바꿀수있으면 바꿀예정
	for (auto it = children.begin(); it != children.end(); ++it)
	{
		// 순회하다가 만약 발견하면 지운다.
		if (child == *it)
		{
			children.erase(it);
			break;
		}
	}
	
	// 내부적으로 완전히 삭제
	Scene::getInstance()->Destroy(child);
}
void GameObject::addChild(GameObject * child)
{
	child->parent = this;
	// 먼저 객체 children에 등록해주고
	children.push_back(child);

	// 여러가지 관리를 위한 등록을 해준다.
	Scene::getInstance()->Instantiate(child);
}


//void GameObject::addChild(const string & name = "default GO name", const string & tag = "default GO tag",
//	const Vector3 & position = Vector3::Zero, const Vector3 & rotation = Vector3::Zero, const Vector3 & scale = Vector3::One,
//	vector<GameObject *> *children = nullptr, vector<Component *> * components = nullptr) 


void GameObject::addChild(const string & name, const string & tag,
	const Vector3 & position, const Vector3 & rotation, const Vector3 & scale,
	vector<GameObject *> *children, vector<Component *> * components)
{
	GameObject * tempGameObject = new GameObject(name, tag, position, rotation, scale, this, children, components);
	this->children.push_back(tempGameObject);

	Scene::getInstance()->Instantiate(tempGameObject);
}

