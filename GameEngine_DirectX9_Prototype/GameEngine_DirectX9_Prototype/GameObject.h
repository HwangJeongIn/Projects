#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Utility.h"
#include "Component.h"
#include "Locator.h"


class GameObject : public Locator
{
private:
	string name;
	string tag;
	vector<Component *> components;
	vector<GameObject *> children;
	Transform * transform;
	GameObject * parent;

	// 각 게임 오브젝트를 업데이트 과정에서 이 플래그가 켜져있으면 업데이트를 시켜주지 않고 넘어간다.
	// 업데이트가 모두 완료된다음에 수행된다.
	bool destroyed;
	void setWhetherDestroyed(bool destroyed) { this->destroyed = destroyed; }
	GameObject(const string & name = "default GO name", const string & tag = "default GO tag",
		const Vector3 & position = Vector3::Zero, const Vector3 & rotation = Vector3::Zero, const Vector3 & scale = Vector3::One,
		GameObject * parent = nullptr, vector<GameObject *> *children = nullptr,
		vector<Component *> * components = nullptr);

	virtual ~GameObject();


public:
	/*
	중개자를 이용하여 사용할 시스템 포워딩
	*/
	Scene & getScene();
	IDirect3DDevice9 & getDevice();
	Audio & getAudio();

	const bool isDestroyed() const { return destroyed; }
 
	// read only
	const string & getTag() const { return tag; }
	const string & getName() const { return name; }

	void setTag(const string & tag) { this->tag = tag; }
	void setName(const string & name) { this->name = name; }
	
	Transform * getTransform() { return transform; }

	bool isRootObj(){ return parent == nullptr; }
	GameObject * getParent() { return parent; }

	// Transform은 고정적으로 GameObject와 함께하기 때문에 따로 set해줄필요는없다.
	void setParent(GameObject * parent);
	vector<GameObject *> & getChildren() { return children; }

	// 현재 깊이를 반환하고 / 파라미터로 string 객체를 레퍼런스형으로 받아서 내부에서 초기화시켜준다.
	// 깊이는 0기준 부모객체가 없는것이다.
	int getPath(string & path);

	void update();
	void fixedUpdate();

	void destroyUpdate();


	//vector<Component *> components;

	GameObject * getChild(const string & name);
	void removeChild(GameObject * child);
	GameObject *  addChild(GameObject * child);
	GameObject *  addChild(const string & name = "default GO name", const string & tag = "default GO tag",
		const Vector3 & position = Vector3::Zero, const Vector3 & rotation = Vector3::Zero, const Vector3 & scale = Vector3::One,
		vector<GameObject *> *children = nullptr, vector<Component *> * components = nullptr);

	static GameObject * Instantiate(GameObject * other);
	static GameObject * Instantiate(const string & name = "default GO name", const string & tag = "default GO tag",
		const Vector3 & position = Vector3::Zero, const Vector3 & rotation = Vector3::Zero, const Vector3 & scale = Vector3::One,
		GameObject * parent = nullptr, vector<GameObject *> *children = nullptr,
		vector<Component *> * components = nullptr);
	static void Destroy(GameObject * other);
	static void FinalDestroy(GameObject * other);
	//void addChild(const string & name, const string & tag,
	//	const Vector3 & position, const Vector3 & rotation, const Vector3 & scale,
	//	vector<GameObject *> *children, vector<Component *> * components);


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
	T * addComponent()
	{
		T* temp = new T(this, this->getTransform());

		// 만약 컴포넌트 상속받지 않았다면 만들었던걸 지운다.
		if (dynamic_cast<Component*>(temp) == nullptr)
		{
			delete temp;
			return nullptr;
		}
		// 수정됨 :: 외부에서 컴포넌트를 생성할 수 없다. protected이기 때문에
		// 외부에서 생성이 필요한 경우도 있을 것 같아서 다시 풀었다.
		// 여러가지 컴포넌트를 모아놨다가 생성하고 싶을때? 객체 자체는 필요없고 컴포넌트만 필요한경우?
		
		// 컴포넌트를 추가한다. 

		components.push_back(temp);
		return temp;
	}


};

#endif