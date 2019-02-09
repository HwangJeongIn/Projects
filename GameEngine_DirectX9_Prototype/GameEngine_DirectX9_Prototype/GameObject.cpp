#include "GameObject.h"
#include "Scene.h"
#include "Locator.h"

GameObject::GameObject(const string & name, const string & tag, 
	const Vector3 & position, const Vector3 & rotation, const Vector3 & scale,
	GameObject * parent, vector<GameObject*>* children, vector<Component*>* components)
	: name(name), tag(tag), transform(new Transform(this, position, rotation, scale)), parent(parent) //, gameObject(this)
{
	//// Transform초기화
	//// Transform은 필수 컴포넌트 / 파라미터로 잘못들어오면 객체생성에 실패한것이기 때문에 리턴한다.
	//if (!transform) return;

	this->components.push_back(transform);


	// 나중에 안쓸수도 있음 아래에있는것들
	// 아예 컴포넌트와 자식객체를 생성하기 위해서 함수로 분리할 예정

	// 들어온 파라미터에 자식객체가 있으면 초기화
	if (children != nullptr)
	{
		for (int i = 0; i < children->size(); ++i)
		{
			this->children.push_back((*children)[i]);
		}
	}

	// 들어온 파라미터에 컴포넌트가 있으면 초기화
	if (components != nullptr)
	{
		for (int i = 0; i < components->size(); ++i)
		{
			this->components.push_back((*components)[i]);
		}
	}
}

GameObject::~GameObject()
{
	// 완전히 지우기 전에 Scene에서 등록했던것들을 지워준다.
	// 이때 이 객체가 제일 상위 객체인지 확인한다.
	bool isRootObj = parent == nullptr;
	getScene().baseDestroy(this, isRootObj);
	// 외부에서 동적할당 했던것을 한꺼번에 GameObject 소멸자에서 처리한다.

	// 내부에 있는 자식객체들과 컴포넌트 동적할당해뒀던 것을 삭제한다
	if (components.size() != 0)
	{
		for (int i = 0; i < components.size(); ++i)
		{
			delete components[i];
		}
	}

	if (children.size() != 0)
	{
		for (int i = 0; i < children.size(); ++i)
		{
			delete children[i];
		}
	}

}

Scene & GameObject::getScene()
{
	return Locator::getScene(); 
}

IDirect3DDevice9 & GameObject::getDevice()
{
	return Locator::getDevice();
}


void GameObject::setParent(GameObject * parent)
{
	// 나중에 수정
	if (parent != nullptr && this->parent == nullptr)
	{
		// 루트 객체 > 다른 자식 객체
		getScene().baseDestroy(this, true);
		getScene().baseInstantiate(this, false);
	}
	if (parent == nullptr && this->parent != nullptr)
	{
		// 다른 자식 객체 > 루트객체
		getScene().baseDestroy(this, false);
		getScene().baseInstantiate(this, true);
	}

	if (parent != nullptr && this->parent != nullptr)
	{
		// 다른 자식 객체 > 다른 자식 객체
		getScene().baseDestroy(this, false);
		getScene().baseInstantiate(this, false);
	}
	this->parent = parent;
}

int GameObject::getPath(string & path)
{
	int depth = -1;
	GameObject * currentGO = this;
	path = "";

	while (currentGO != nullptr)
	{
		path = "/" + currentGO->name + path;
		++depth;
		currentGO = currentGO->parent;
	}

	return depth;
}

void GameObject::update()
{
	for (auto it : components)
	{
		it->update();
	}


	for (auto it : children)
	{
		it->update();
	}


}

void GameObject::fixedUpdate()
{
	for (auto it : components)
	{
		it->fixedUpdate();
	}

	for (auto it : children)
	{
		it->fixedUpdate();
	}

}

GameObject * GameObject::getChild(const string & name)
{
	/*
	2가지 방법 // 아마 나중에 방법2로 수정할듯

	1. children리스트를 순회하면서 선형 탐색

	2. 현재이름을 기반으로 path생성   .../parent's name/현재이름/child's name
	Scene 싱글턴매니저에서 multimap을 사용해서 바로 접근해서 반환

	*/
	// 현재 방법1

	// foreach쓸때 포인터형인지(or레퍼런스형) 아닌지 잘확인하면서 쓰자 // 포인터형이 아니라면 어마어마한 복사가 일어난다.
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
	if (!child) return;
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

	delete child;

	// 현재는 루트 객체 기준으로 씬의 오브젝트들이 들어가 있으므로 바로 삭제하면 된다.
	// 내부적으로 완전히 삭제 
	// getScene().Destroy(child);
}
GameObject * GameObject::addChild(GameObject * child)
{
	if (!child) return nullptr;

	child->parent = this;
	// 먼저 객체 children에 등록해주고
	children.push_back(child);

	// 현재는 루트 객체 기준으로 씬의 오브젝트들이 들어가 있으므로 바로 등록하면 된다.
	// 여러가지 관리를 위한 등록을 해준다.
	// Scene::getInstance()->Instantiate(child);

	return child;
}



GameObject * GameObject::addChild(const string & name, const string & tag,
	const Vector3 & position, const Vector3 & rotation, const Vector3 & scale,
	vector<GameObject *> *children, vector<Component *> * components)
{
	GameObject * tempGameObject = new GameObject(name, tag, position, rotation, scale, this, children, components);
	this->children.push_back(tempGameObject);


	//Scene::getInstance()->Instantiate(tempGameObject);

	return tempGameObject;
}

GameObject * GameObject::Instantiate(GameObject * other)
{
	if (!other) return nullptr;
	// 만약에 같은 부모를 가진 객체중 중복된 이름이 있다면
	// 이름을 바꿔준다.
	other->getScene().baseInstantiate(other, other->isRootObj());

	return other;
}

GameObject * GameObject::Instantiate
(const string & name, const string & tag,
	const Vector3 & position, const Vector3 & rotation, const Vector3 & scale,
	GameObject * parent, vector<GameObject *> *children,
	vector<Component *> * components)
{
	/*
	객체가 초기화되는 과정
	1. GameObject 생성 // 부가적으로 transform 생성
	2. 등록

	// 문제점 : 동시에 생성하려니까 서로가 서로의 포인터를 필요로 하기 때문에 불가능하다.

	해결책
	1. transform을 nullptr로 초기화한다음에 나중에 초기화하는 방법을 사용한다.

	2. 디폴트 값으로 GameObject 이니셜라이저에서 Transfrom 생성

	3. GameObject 입력파라미터를 바꿔서 pos, rotation, scale을 추가한후
	Transfrom객체를 GameObject 이니셜라이저에서 생성

	// 3번 채택
	*/

	// 객체를 생성해준다.
	GameObject * tempGameObject = new GameObject(name, tag, position, rotation, scale, parent, children, components);


	tempGameObject->getScene().baseInstantiate(tempGameObject, tempGameObject->isRootObj());

	// 나중에 관리하기 편하기 위해서 만든 map에 추가한다. // find함수에서 사용

	return tempGameObject;
}

void GameObject::Destroy(GameObject * other)
{
	if (!other) return;
	
	other->getScene().baseDestroy( other, other->isRootObj() );
	

	// 등록되어있지 않다고 하더라도
	// 마지막으로 삭제
	delete other;
}
