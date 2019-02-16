#include "GameObject.h"
#include "Scene.h"
#include "Locator.h"

GameObject::GameObject(const string & name, const string & tag, 
	const Vector3 & position, const Vector3 & rotation, const Vector3 & scale,
	GameObject * parent, vector<GameObject*>* children, vector<Component*>* components)
	: name(name), tag(tag), transform(new Transform(this, position, rotation, scale)), parent(parent), destroyed(false) //, gameObject(this)
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
	// isDestroyed �÷��װ� ������������ ������Ʈ���� ������Ʈ ���ش�.
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


// erase�Ҷ� �ݺ��ڰ� �ֽ�ȭ�ȴ�.
// ���� ���̻� ���� ���� �ݺ���.. ���Ӱ� �ٲ� �ڷ��������� end�� begin�� ������ end�� begin�� �ٸ���?
// �ذ�å : �ٲ������ ����Ʈ�� end�� �̸� �޾Ƴ��´�.
// ���� �ݺ��ڿ��� ++it�� �Ҽ��� ����.. �����µ� erase�Լ��� ����ؾ��ҵ�

// ���ϵ� ��ü�� ��ȸ�ϴ� ���� ���ϵ� ��ü ������ �θ�ü�� ���ϵ帮��Ʈ���� �����Ǿ�� �Ұ��
// FinalDestroy���� ���� ���ִ°� ���ٰ� �����ߴ�.
// �ٸ������� ȣ���� ����� �� ��� �ؾ������ �θ�ü�� ���ϵ帮��Ʈ���� ���� ���ϴ� ��찡 ���� �� �ֱ� �����̴�.
void GameObject::destroyUpdate()
{
	auto it = children.begin();

	while (it != children.end())
	{
		// �̸� ������ �޾Ƽ� ���縦 ������Ʈ ��Ű�� ���߿� �ֽ�ȭ ��Ų��
		// �߰��� ������ �ݺ��ڸ� ����Ű�� ���� ���ɼ��� �ֱ� �����̴�.

		if ((*it)->destroyed)
		{
			// ��Ʈ������Ʈ��� �θ�ü�� ����Ʈ���� ������ �ʿ䰡 ����.
			if ((*it)->isRootObj() == true)
			{
				FinalDestroy(*it); continue;
			}

			vector<GameObject *>& childrenOfParent = (*it)->parent->children;
			// ���� �ڽİ�ü��� �θ�ü�� ����Ʈ������ ��������� �Ѵ�.
			// �ð����⵵ ����ؼ� �ٲܼ������� �ٲܿ���

			for (auto it2 = childrenOfParent.begin(); it2 != childrenOfParent.end(); ++it2)
			{
				// ��ȸ�ϴٰ� ���� �߰��ϸ� �����.
				if (*it2 == *it)
				{
					childrenOfParent.erase(it);
					break;
				}
			}

			// ���������� ����
			FinalDestroy(*it);
			
		}
		else
		{
			// �����Ǿ�� ���� �ʴ´ٸ� �ڽİ�ü�� Ȯ�����ش�.
			(*it)->destroyUpdate();
			++it;
		}
	}

}

/*
Scene�� base�۾��� 
��Ʈ��ü :: ��Ʈ��ü ����Ʈ �߰� / ���� + �ʿ� ��� / ����
��Ʈ��üX :: �ʿ� ��� / ����
*/

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

	child->setWhetherDestroyed(true);
}
GameObject * GameObject::addChild(GameObject * child)
{
	if (!child) return nullptr;

	child->parent = this;
	// ���� ��ü children�� ������ְ�
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
	GameObject * tempGameObject = new GameObject(name, tag, position, rotation, scale, parent, children, components);


	tempGameObject->getScene().baseInstantiate(tempGameObject, tempGameObject->isRootObj());

	// ���߿� �����ϱ� ���ϱ� ���ؼ� ���� map�� �߰��Ѵ�. // find�Լ����� ���

	return tempGameObject;
}

void GameObject::Destroy(GameObject * other)
{
	if (!other) return;
	other->setWhetherDestroyed(true);

	//other->getScene().baseDestroy( other, other->isRootObj() );
	//
	//// ��ϵǾ����� �ʴٰ� �ϴ���
	//// ���������� ����
	//delete other;
}

// addChild�� removeChild���� ��� �θ�ü���� ���������� �۾��ϱ� ���ؼ� �������.
// �����ϴٰ� ���߿� ���ʿ��ϴٰ� �ǴܵǸ� ���� �����̴�.
// ������ �����Ȱ��� �˱� ���� bool���� ��ȯ
void GameObject::FinalDestroy(GameObject * other)
{
	// removeChild Destroy������ �׳� �÷��׸� ���ش�.
	if (!other) return;
	bool rootObj = other->isRootObj();

	//// �ڽİ�ü���
	//if (!rootObj)
	//{
	//	GameObject * parent = other->parent;
	//	// ���� �ڽİ�ü��� �θ�ü�� ����Ʈ������ ��������� �Ѵ�.
	//	// �ð����⵵ ����ؼ� �ٲܼ������� �ٲܿ���
	//	auto it = parent->children.begin();
	//	for (; it != parent->children.end(); ++it)
	//	{
	//		// ��ȸ�ϴٰ� ���� �߰��ϸ� �����.
	//		if (other == *it)
	//		{
	//			parent->children.erase(it);
	//			break;
	//		}
	//	}
	//}
	// ���࿡ �߰����� ���Ѱ���� �׳� �������´�
	//if ((it == parent->children.end()))

	other->getScene().baseDestroy( other, rootObj);

	delete other;
}
