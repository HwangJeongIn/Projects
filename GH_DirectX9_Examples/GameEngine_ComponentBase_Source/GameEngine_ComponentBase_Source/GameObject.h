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
		//// Transform�ʱ�ȭ
		//// Transform�� �ʼ� ������Ʈ / �Ķ���ͷ� �߸������� ��ü������ �����Ѱ��̱� ������ �����Ѵ�.
		//if (!transform) return;

		this->components.push_back(transform);

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
	/*
		GameObject(GameObject * other)
		// �� �׸���� �������ش�.
		: name(other->name), tag(other->tag)
	{
		// ���� �Ķ���Ϳ� �ڽİ�ü�� ������ �ʱ�ȭ

		for (int i = 0; i < other->children.size(); ++i)
		{
			// ���⼭�� �������� ��ü�� �����Ǿ�� �ϱ� ������
			// ��������ڸ� ȣ���ؼ� ������ش�.
			// child�ȿ� �� child����� ������ �װ͵� ���������� �ʱ�ȭ�ȴ�.
			this->children.push_back(new GameObject(other->children[i]));
		}


		// ���� �Ķ���Ϳ� ������Ʈ�� ������ �ʱ�ȭ

		for (int i = 0; i < other->components.size(); ++i)
		{
			// �ʱ�ȭ ����
			

			1. ������ � Component���� Ȯ���� �Ѵ�.
			2. 

			

			this->components.push_back(new Component(other->components[i]));
		}

	}
	*/


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

};

#endif