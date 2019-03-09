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
	RigidBody * rigidBody;
	GameObject * parent;

	// �� ���� ������Ʈ�� ������Ʈ �������� �� �÷��װ� ���������� ������Ʈ�� �������� �ʰ� �Ѿ��.
	// ������Ʈ�� ��� �Ϸ�ȴ����� ����ȴ�.
	bool destroyed;
	void setWhetherDestroyed(bool destroyed) { this->destroyed = destroyed; }
	GameObject(const string & name = "default GO name", const string & tag = "default GO tag",
		const Vector3 & position = Vector3::Zero, const Vector3 & rotation = Vector3::Zero, const Vector3 & scale = Vector3::One,
		GameObject * parent = nullptr, vector<GameObject *> *children = nullptr,
		vector<Component *> * components = nullptr)
		: name(name), tag(tag), transform(new Transform(this, position, rotation, scale)), parent(parent), destroyed(false), rigidBody(nullptr) //, gameObject(this)
	{
		//// Transform�ʱ�ȭ
		//// Transform�� �ʼ� ������Ʈ / �Ķ���ͷ� �߸������� ��ü������ �����Ѱ��̱� ������ �����Ѵ�.
		//if (!transform) return;

		// transform�� ���� �۾����ְ� �; �־����� �ʾҴ�.
		// �������ؾ��ϴ� ��ü�� Transform ������Ʈ���� ������ǥ�� �������� �޾Ƽ� �������Ѵ�.

		// this->components.push_back(transform);


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

	virtual ~GameObject();


public:
	/*
	�߰��ڸ� �̿��Ͽ� ����� �ý��� ������
	*/
	Scene & getScene();
	IDirect3DDevice9 & getDevice();
	Audio & getAudio();
	Physics & getPhysics();

	const bool isDestroyed() const { return destroyed; }
 
	// read only
	const string & getTag() const { return tag; }
	const string & getName() const { return name; }

	void setTag(const string & tag) { this->tag = tag; }
	void setName(const string & name) { this->name = name; }
	void setTransform(Transform * transform)
	{
		if (!transform) return;
		this->transform = transform;
	}
	void setRigidBody(RigidBody * rigidBody)
	{
		if (!rigidBody) return;
		this->rigidBody = rigidBody;
	}

	Transform * getTransform() { return transform; }
	RigidBody * getRigidBody() { return rigidBody; }
	bool isRootObj(){ return parent == nullptr; }
	GameObject * getParent() { return parent; }

	// Transform�� ���������� GameObject�� �Բ��ϱ� ������ ���� set�����ʿ�¾���.
	void setParent(GameObject * parent);
	vector<GameObject *> & getChildren() { return children; }

	// ���� ���̸� ��ȯ�ϰ� / �Ķ���ͷ� string ��ü�� ���۷��������� �޾Ƽ� ���ο��� �ʱ�ȭ�����ش�.
	// ���̴� 0���� �θ�ü�� ���°��̴�.
	int getPath(string & path);

	void update();
	void fixedUpdate();
	//void physicsUpdate()
	//{
	//	if (destroyed) return;


	//	RigidBody * rb = getComponent<RigidBody>();
	//	if (rb != nullptr)
	//	{
	//		// trasnfrom ��ġ �޾Ƽ� �ֽ�ȭ
	//	}

	//	for (auto it : children)
	//	{
	//		it->physicsUpdate();
	//	}
	//}

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
	T * addComponent()
	{
		T* temp = new T(this, this->getTransform());

		// ���� ������Ʈ ��ӹ��� �ʾҴٸ� ��������� �����.
		if (dynamic_cast<Component*>(temp) == nullptr)
		{
			delete temp;
			return nullptr;
		}
		// ������ :: �ܺο��� ������Ʈ�� ������ �� ����. protected�̱� ������
		// �ܺο��� ������ �ʿ��� ��쵵 ���� �� ���Ƽ� �ٽ� Ǯ����.
		// �������� ������Ʈ�� ��Ƴ��ٰ� �����ϰ� ������? ��ü ��ü�� �ʿ���� ������Ʈ�� �ʿ��Ѱ��?
		
		// ������Ʈ�� �߰��Ѵ�. 

		components.push_back(temp);
		return temp;
	}


};

#endif