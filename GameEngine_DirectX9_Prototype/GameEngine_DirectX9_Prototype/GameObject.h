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
		//// Transform�ʱ�ȭ
		//// Transform�� �ʼ� ������Ʈ / �Ķ���ͷ� �߸������� ��ü������ �����Ѱ��̱� ������ �����Ѵ�.
		//if (!transform) return;

		this->components.push_back(transform);


		// ���߿� �Ⱦ����� ���� �Ʒ����ִ°͵�
		// �ƿ� ������Ʈ�� �ڽİ�ü�� �����ϱ� ���ؼ� �Լ��� �и��� ����

		// ���� �Ķ���Ϳ� �ڽİ�ü�� ������ �ʱ�ȭ
		if (children != nullptr)
		{
			for (int i = 0; i < children->size(); ++i)
			{
				this->children.push_back((*children)[i]);
			}
		}

		// ���� �Ķ���Ϳ� ������Ʈ�� ������ �ʱ�ȭ
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
		// �ܺο��� �����Ҵ� �ߴ����� �Ѳ����� GameObject �Ҹ��ڿ��� ó���Ѵ�.

		// ���ο� �ִ� �ڽİ�ü��� ������Ʈ �����Ҵ��ص״� ���� �����Ѵ�
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

	// Transform�� ���������� GameObject�� �Բ��ϱ� ������ ���� set�����ʿ�¾���.
	void setParent(GameObject * parent) 
	{
		if (!parent) return;
		this->parent = parent;
	}

	// ���� ���̸� ��ȯ�ϰ� / �Ķ���ͷ� string ��ü�� ���۷��������� �޾Ƽ� ���ο��� �ʱ�ȭ�����ش�.
	// ���̴� 0���� �θ�ü�� ���°��̴�.
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

	// children�� components�� ���� �ֽ�ȭ ���Ѿ��Ѵ�.
	void update()
	{
		for (auto it : components)
		{
			it->update();
		}

		// ������ �ڽİ�ü�鵵 ��� Scene���� ��ϵǱ� ������ ���⼭ �����൵ �ȴ�.
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
		// ��ü�� ����
		// ComponentŬ������ ����� ���� �ʾҴٸ� nullptr ����
		if (dynamic_cast<Component*>(T*) == nullptr)
			return nullptr;
		*/


		T* temp = nullptr;
		// ��ȸ�ϸ鼭 �ִ��� ã�ƺ���.
		for (auto it = components.begin(); it != components.end(); ++it)
		{
			temp = dynamic_cast<T*>(*it);
			// ������ ��ȯ
			if (temp != nullptr)
				return temp;
		}

		return nullptr;
	}

	// Transform ������Ʈ ���� ��� ������Ʈ�� �����ڿ��� �Է��Ķ���ͷ�
	// GameObject * / Transform * ���ʷ� �޵��� ���־���Ѵ�.
	template <typename T>
	void addComponent()
	{
		T* temp = new T(this, this->getTransform());

		// ���� ������Ʈ ��ӹ��� �ʾҴٸ� ��������� �����.
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