#include "GameObject.h"
#include "Scene.h"


GameObject * GameObject::getChild(const string & name)
{
	/*
	2���� ��� // �Ƹ� ���߿� ���2�� �����ҵ�

	1. children����Ʈ�� ��ȸ�ϸ鼭 ���� Ž��

	2. �����̸��� ������� path����   .../parent's name/�����̸�/child's name
	Scene �̱��ϸŴ������� multimap�� ����ؼ� �ٷ� �����ؼ� ��ȯ

	*/
	// ���� ���1

	// foreach���� ������������ �ƴ��� ��Ȯ���ϸ鼭 ���� // ���������� �ƴ϶�� �� ����ȴ�.
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
	
	// ���������� ������ ����
	Scene::getInstance()->Destroy(child);
}
void GameObject::addChild(GameObject * child)
{
	child->parent = this;
	// ���� ��ü children�� ������ְ�
	children.push_back(child);

	// �������� ������ ���� ����� ���ش�.
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

