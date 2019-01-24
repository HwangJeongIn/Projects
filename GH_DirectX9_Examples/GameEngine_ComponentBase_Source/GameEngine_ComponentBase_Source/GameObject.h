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

public:
	GameObject(const string & name = "default GO name",const string & tag = "default GO tag",
		const Vector3 & position = Vector3::Zero, const Vector3 & rotation = Vector3::Zero, const Vector3 & scale = Vector3::One,
		vector<GameObject *> *children = nullptr, vector<Component *> * components = nullptr)
		: name(name), tag(tag), transform(new Transform(this, position, rotation, scale)) //, gameObject(this)
	{
		//// Transform초기화
		//// Transform은 필수 컴포넌트 / 파라미터로 잘못들어오면 객체생성에 실패한것이기 때문에 리턴한다.
		//if (!transform) return;

		this->components.push_back(transform);

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
	/*
		GameObject(GameObject * other)
		// 각 항목들을 복사해준다.
		: name(other->name), tag(other->tag)
	{
		// 들어온 파라미터에 자식객체가 있으면 초기화

		for (int i = 0; i < other->children.size(); ++i)
		{
			// 여기서도 독립적인 객체가 생성되어야 하기 때문에
			// 복사생성자를 호출해서 만들어준다.
			// child안에 또 child목록이 있으면 그것도 연쇄적으로 초기화된다.
			this->children.push_back(new GameObject(other->children[i]));
		}


		// 들어온 파라미터에 컴포넌트가 있으면 초기화

		for (int i = 0; i < other->components.size(); ++i)
		{
			// 초기화 순서
			

			1. 실제로 어떤 Component인지 확인을 한다.
			2. 

			

			this->components.push_back(new Component(other->components[i]));
		}

	}
	*/


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

};

#endif