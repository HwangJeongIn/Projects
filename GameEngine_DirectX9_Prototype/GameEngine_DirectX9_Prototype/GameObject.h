#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Utility.h"
#include "Component.h"


class GameObject
{
private:
	string name;
	string tag;
	vector<Component *> components;
	vector<GameObject *> children;
	Transform * transform;
	GameObject * parent;

public:
	GameObject(const string & name = "default GO name",const string & tag = "default GO tag",
		const Vector3 & position = Vector3::Zero, const Vector3 & rotation = Vector3::Zero, const Vector3 & scale = Vector3::One,
		GameObject * parent = nullptr, vector<GameObject *> *children = nullptr,
		vector<Component *> * components = nullptr)
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


	virtual ~GameObject()
	{
		// 외부에서 동적할당 했던것을 한꺼번에 GameObject 소멸자에서 처리한다.

		// 내부에 있는 자식객체들과 컴포넌트 동적할당해뒀던 것을 삭제한다
		if (components.size() != 0)
		{
			for (int i = 0; i<components.size(); ++i)
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

	// read only
	const string & getTag() const { return tag; }
	const string & getName() const { return name; }

	void setTag(const string & tag) { this->tag = tag; }
	void setName(const string & name) { this->name = name; }
	
	Transform * getTransform() { return transform; }
	GameObject * getParent() { return parent; }

	// Transform은 고정적으로 GameObject와 함께하기 때문에 따로 set해줄필요는없다.
	void setParent(GameObject * parent) 
	{
		if (!parent) return;
		this->parent = parent;
	}

	// 현재 깊이를 반환하고 / 파라미터로 string 객체를 레퍼런스형으로 받아서 내부에서 초기화시켜준다.
	// 깊이는 0기준 부모객체가 없는것이다.
	int getPath(string & path)
	{
		int depth = -1;
		GameObject * currentGO = this;
		path = "";

		while (currentGO != nullptr)
		{
			path = "/"+ currentGO->name + path;
			++depth;
			currentGO = currentGO->parent;
		}

		return depth;
	}

	// children과 components를 각각 최신화 시켜야한다.
	void update()
	{
		for (auto it : components)
		{
			it->update();
		}

		// 어차피 자식객체들도 모두 Scene에서 등록되기 때문에 여기서 안해줘도 된다.
		/*
		for (auto it : children)
		{
			it->update();
		}
		*/

	}

	void fixedUpdate()
	{
		for (auto it : components)
		{
			it->fixedUpdate();
		}

	}

	//vector<Component *> components;

	template <typename T>
	T* getComponent()
	{
		/*
		// 실체가 없음
		// Component클래스의 상속을 받지 않았다면 nullptr 리턴
		if (dynamic_cast<Component*>(T*) == nullptr)
			return nullptr;
		*/


		T* temp = nullptr;
		// 순회하면서 있는지 찾아본다.
		for (auto it = components.begin(); it != components.end(); ++it)
		{
			temp = dynamic_cast<T*>(*it);
			// 있으면 반환
			if (temp != nullptr)
				return temp;
		}

		return nullptr;
	}

	// Transform 컴포넌트 제외 모든 컴포넌트는 생성자에서 입력파라미터로
	// GameObject * / Transform * 차례로 받도록 해주어야한다.
	template <typename T>
	void addComponent()
	{
		T* temp = new T(this, this->getTransform());

		// 만약 컴포넌트 상속받지 않았다면 만들었던걸 지운다.
		if (dynamic_cast<Component*>(temp) == nullptr)
		{
			delete temp;
			return;
		}

		components.push_back(temp);
	}

	GameObject * getChild(const string & name);
	void removeChild(GameObject * child);
	GameObject *  addChild(GameObject * child);
	GameObject *  addChild(const string & name = "default GO name", const string & tag = "default GO tag",
		const Vector3 & position = Vector3::Zero, const Vector3 & rotation = Vector3::Zero, const Vector3 & scale = Vector3::One,
		vector<GameObject *> *children = nullptr, vector<Component *> * components = nullptr);
	//void addChild(const string & name, const string & tag,
	//	const Vector3 & position, const Vector3 & rotation, const Vector3 & scale,
	//	vector<GameObject *> *children, vector<Component *> * components);

};

#endif