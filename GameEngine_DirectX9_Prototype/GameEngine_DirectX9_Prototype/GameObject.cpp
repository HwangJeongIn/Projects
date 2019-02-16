#include "GameObject.h"
#include "Scene.h"
#include "Locator.h"

GameObject::GameObject(const string & name, const string & tag, 
	const Vector3 & position, const Vector3 & rotation, const Vector3 & scale,
	GameObject * parent, vector<GameObject*>* children, vector<Component*>* components)
	: name(name), tag(tag), transform(new Transform(this, position, rotation, scale)), parent(parent), destroyed(false) //, gameObject(this)
{
	//// Transform초기화
	//// Transform은 필수 컴포넌트 / 파라미터로 잘못들어오면 객체생성에 실패한것이기 때문에 리턴한다.
	//if (!transform) return;

	// transform은 따로 작업해주고 싶어서 넣어주지 않았다.
	// 렌더링해야하는 객체는 Transform 컴포넌트에서 월드좌표계 포지션을 받아서 렌더링한다.

	// this->components.push_back(transform);


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

	GameObject * temp = nullptr;
	while (children.size() !=0)
	{
		temp = children.back();
		FinalDestroy(temp);
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
	// isDestroyed 플래그가 꺼져있을때만 컴포넌트들을 업데이트 해준다.
	if (!destroyed)
	{
		for (auto it : components)
		{
			it->update();
		}
	}


	for (auto it : children)
	{
		it->update();
	}


}

void GameObject::fixedUpdate()
{
	if (!destroyed)
	{
		for (auto it : components)
		{
			it->fixedUpdate();
		}
	}

	for (auto it : children)
	{
		it->fixedUpdate();
	}

}


// erase할때 반복자가 최신화된다.
// 따라서 더이상 쓸수 없는 반복자.. 새롭게 바뀐 자료형에서의 end와 begin은 그전의 end와 begin과 다르다?
// 해결책 : 바뀌기전의 리스트의 end를 미리 받아놓는다.
// 기존 반복자에서 ++it을 할수가 없다.. 못쓰는듯 erase함수를 사용해야할듯

// 차일드 객체를 순회하는 도중 차일드 객체 본인이 부모객체의 차일드리스트에서 삭제되어야 할경우
// FinalDestroy에서 같이 해주는게 낫다고 생각했다.
// 다른곳에서 호출을 해줘야 할 경우 잊어버리고 부모객체의 차일드리스트에서 삭제 안하는 경우가 생길 수 있기 때문이다.
void GameObject::destroyUpdate()
{
	auto it = children.begin();

	while (it != children.end())
	{
		// 미리 다음걸 받아서 현재를 업데이트 시키고 나중에 최신화 시킨다
		// 중간에 삭제된 반복자를 가리키고 있을 가능성이 있기 때문이다.

		if ((*it)->destroyed)
		{
			// 루트오브젝트라면 부모객체의 리스트에서 삭제할 필요가 없다.
			if ((*it)->isRootObj() == true)
			{
				FinalDestroy(*it); continue;
			}

			vector<GameObject *>& childrenOfParent = (*it)->parent->children;
			// 만약 자식객체라면 부모객체의 리스트에서도 삭제해줘야 한다.
			// 시간복잡도 고려해서 바꿀수있으면 바꿀예정

			for (auto it2 = childrenOfParent.begin(); it2 != childrenOfParent.end(); ++it2)
			{
				// 순회하다가 만약 발견하면 지운다.
				if (*it2 == *it)
				{
					childrenOfParent.erase(it);
					break;
				}
			}

			// 최종적으로 삭제
			FinalDestroy(*it);
			
		}
		else
		{
			// 삭제되어야 하지 않는다면 자식객체를 확인해준다.
			(*it)->destroyUpdate();
			++it;
		}
	}

}

/*
Scene의 base작업은 
루트객체 :: 루트객체 리스트 추가 / 삭제 + 맵에 등록 / 해제
루트객체X :: 맵에 등록 / 해제
*/

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

	child->setWhetherDestroyed(true);
}
GameObject * GameObject::addChild(GameObject * child)
{
	if (!child) return nullptr;

	child->parent = this;
	// 먼저 객체 children에 등록해주고
	children.push_back(child);

	child->getScene().baseInstantiate(child, child->isRootObj());

	return child;
}



GameObject * GameObject::addChild(const string & name, const string & tag,
	const Vector3 & position, const Vector3 & rotation, const Vector3 & scale,
	vector<GameObject *> *children, vector<Component *> * components)
{
	GameObject * tempGameObject = new GameObject(name, tag, position, rotation, scale, this, children, components);
	this->children.push_back(tempGameObject);


	tempGameObject->getScene().baseInstantiate(tempGameObject, tempGameObject->isRootObj());
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
	GameObject * tempGameObject = new GameObject(name, tag, position, rotation, scale, parent, children, components);


	tempGameObject->getScene().baseInstantiate(tempGameObject, tempGameObject->isRootObj());

	// 나중에 관리하기 편하기 위해서 만든 map에 추가한다. // find함수에서 사용

	return tempGameObject;
}

void GameObject::Destroy(GameObject * other)
{
	if (!other) return;
	other->setWhetherDestroyed(true);

	//other->getScene().baseDestroy( other, other->isRootObj() );
	//
	//// 등록되어있지 않다고 하더라도
	//// 마지막으로 삭제
	//delete other;
}

// addChild와 removeChild같은 경우 부모객체에서 직관적으로 작업하기 위해서 만들었다.
// 제공하다가 나중에 불필요하다고 판단되면 지울 에정이다.
// 실제로 삭제된건지 알기 위해 bool형을 반환
void GameObject::FinalDestroy(GameObject * other)
{
	// removeChild Destroy에서는 그냥 플래그만 켜준다.
	if (!other) return;
	bool rootObj = other->isRootObj();

	//// 자식객체라면
	//if (!rootObj)
	//{
	//	GameObject * parent = other->parent;
	//	// 만약 자식객체라면 부모객체의 리스트에서도 삭제해줘야 한다.
	//	// 시간복잡도 고려해서 바꿀수있으면 바꿀예정
	//	auto it = parent->children.begin();
	//	for (; it != parent->children.end(); ++it)
	//	{
	//		// 순회하다가 만약 발견하면 지운다.
	//		if (other == *it)
	//		{
	//			parent->children.erase(it);
	//			break;
	//		}
	//	}
	//}
	// 만약에 발견하지 못한경우라면 그냥 빠져나온다
	//if ((it == parent->children.end()))

	other->getScene().baseDestroy( other, rootObj);

	delete other;
}
