#include "GameObject.h"
#include "Scene.h"
#include "Locator.h"

GameObject::GameObject(const string & name, const string & tag, 
	const Vector3 & position, const Vector3 & rotation, const Vector3 & scale,
	GameObject * parent, vector<GameObject*>* children, vector<Component*>* components)
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

GameObject::~GameObject()
{
	// ������ ����� ���� Scene���� ����ߴ��͵��� �����ش�.
	// �̶� �� ��ü�� ���� ���� ��ü���� Ȯ���Ѵ�.
	bool isRootObj = parent == nullptr;
	getScene().baseDestroy(this, isRootObj);
	// �ܺο��� �����Ҵ� �ߴ����� �Ѳ����� GameObject �Ҹ��ڿ��� ó���Ѵ�.

	// ���ο� �ִ� �ڽİ�ü��� ������Ʈ �����Ҵ��ص״� ���� �����Ѵ�
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
	// ���߿� ����
	if (parent != nullptr && this->parent == nullptr)
	{
		// ��Ʈ ��ü > �ٸ� �ڽ� ��ü
		getScene().baseDestroy(this, true);
		getScene().baseInstantiate(this, false);
	}
	if (parent == nullptr && this->parent != nullptr)
	{
		// �ٸ� �ڽ� ��ü > ��Ʈ��ü
		getScene().baseDestroy(this, false);
		getScene().baseInstantiate(this, true);
	}

	if (parent != nullptr && this->parent != nullptr)
	{
		// �ٸ� �ڽ� ��ü > �ٸ� �ڽ� ��ü
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
	2���� ��� // �Ƹ� ���߿� ���2�� �����ҵ�

	1. children����Ʈ�� ��ȸ�ϸ鼭 ���� Ž��

	2. �����̸��� ������� path����   .../parent's name/�����̸�/child's name
	Scene �̱��ϸŴ������� multimap�� ����ؼ� �ٷ� �����ؼ� ��ȯ

	*/
	// ���� ���1

	// foreach���� ������������(or���۷�����) �ƴ��� ��Ȯ���ϸ鼭 ���� // ���������� �ƴ϶�� ���� ���簡 �Ͼ��.
	for (auto it : children)
	{
		if (it->getName() == name)
			return it;
	}

	// �߰߸��������
	return nullptr;
}


void GameObject::removeChild(GameObject * child)
{
	if (!child) return;
	// �ð����⵵ ����ؼ� �ٲܼ������� �ٲܿ���
	for (auto it = children.begin(); it != children.end(); ++it)
	{
		// ��ȸ�ϴٰ� ���� �߰��ϸ� �����.
		if (child == *it)
		{
			children.erase(it);
			break;
		}
	}

	delete child;

	// ����� ��Ʈ ��ü �������� ���� ������Ʈ���� �� �����Ƿ� �ٷ� �����ϸ� �ȴ�.
	// ���������� ������ ���� 
	// getScene().Destroy(child);
}
GameObject * GameObject::addChild(GameObject * child)
{
	if (!child) return nullptr;

	child->parent = this;
	// ���� ��ü children�� ������ְ�
	children.push_back(child);

	// ����� ��Ʈ ��ü �������� ���� ������Ʈ���� �� �����Ƿ� �ٷ� ����ϸ� �ȴ�.
	// �������� ������ ���� ����� ���ش�.
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
	// ���࿡ ���� �θ� ���� ��ü�� �ߺ��� �̸��� �ִٸ�
	// �̸��� �ٲ��ش�.
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
	��ü�� �ʱ�ȭ�Ǵ� ����
	1. GameObject ���� // �ΰ������� transform ����
	2. ���

	// ������ : ���ÿ� �����Ϸ��ϱ� ���ΰ� ������ �����͸� �ʿ�� �ϱ� ������ �Ұ����ϴ�.

	�ذ�å
	1. transform�� nullptr�� �ʱ�ȭ�Ѵ����� ���߿� �ʱ�ȭ�ϴ� ����� ����Ѵ�.

	2. ����Ʈ ������ GameObject �̴ϼȶ��������� Transfrom ����

	3. GameObject �Է��Ķ���͸� �ٲ㼭 pos, rotation, scale�� �߰�����
	Transfrom��ü�� GameObject �̴ϼȶ��������� ����

	// 3�� ä��
	*/

	// ��ü�� �������ش�.
	GameObject * tempGameObject = new GameObject(name, tag, position, rotation, scale, parent, children, components);


	tempGameObject->getScene().baseInstantiate(tempGameObject, tempGameObject->isRootObj());

	// ���߿� �����ϱ� ���ϱ� ���ؼ� ���� map�� �߰��Ѵ�. // find�Լ����� ���

	return tempGameObject;
}

void GameObject::Destroy(GameObject * other)
{
	if (!other) return;
	
	other->getScene().baseDestroy( other, other->isRootObj() );
	

	// ��ϵǾ����� �ʴٰ� �ϴ���
	// ���������� ����
	delete other;
}
