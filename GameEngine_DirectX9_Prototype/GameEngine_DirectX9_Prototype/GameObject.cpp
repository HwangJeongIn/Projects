#include "GameObject.h"
#include "Scene.h"
#include "Locator.h"

GameObject::~GameObject()
{
	// �ܺο��� �����Ҵ� �ߴ����� �Ѳ����� GameObject �Ҹ��ڿ��� ó���Ѵ�.
	// ���ο� �ִ� �ڽİ�ü��� ������Ʈ �����Ҵ��ص״� ���� �����Ѵ�


	Component * tComponent = nullptr;
 	auto it = components.begin();
	while (it != components.end())
	{
		tComponent = *it;
		++it;
		delete tComponent;
	}
	
	GameObject * tGameObject = nullptr;
	auto it2 = children.begin();
	while (it2 != children.end())
	{
		tGameObject = *it2;
		++it2;
		FinalDestroy(tGameObject);
	}

	// ���������� �����Ѵ�.
	// ���������� deleteȣ���ϱ� ������ ���ѷ��� ����
	// �ٸ� �۾��� ������
	//FinalDestroy(this);
	getScene().baseDestroy(this, isRootObj());
}

Scene & GameObject::getScene()
{
	return Locator::getScene(); 
}

IDirect3DDevice9 & GameObject::getDevice()
{
	return Locator::getDevice();
}

Audio & GameObject::getAudio()
{
	return Locator::getAudio();
}

Physics & GameObject::getPhysics()
{
	return Locator::getPhysics();
}

FbxParser & GameObject::getFbxParser()
{
	return Locator::getFbxParser();
}

GameUI & GameObject::getGameUI()
{
	return Locator::getGameUI();
}


void GameObject::setParent(GameObject * parent)
{
	// ���� �θ�ü�� �ִٸ� ���� �θ��Ͽ��� �������ش�.
	if (this->parent != nullptr)
	{
		for (auto it = this->parent->children.begin(); it != this->parent->children.end(); ++it)
		{
			if ((*it) == this)
			{
				this->parent->children.erase(it);
				break;
			}
		}
	}


	// ���߿� ����
	if (parent != nullptr && this->parent == nullptr)
	{
		// ���� �θ�ü���� �ٲ��ش�.
		this->parent = parent;
		// ��Ʈ ��ü > �ٸ� �ڽ� ��ü
		getScene().baseDestroy(this, true);
		getScene().baseInstantiate(this, false);

		// ��Ʈ��ü�� �ƴҶ� ����θ�ü�� ���ؼ��� �ڽĵ���� ���ش�.
		parent->children.push_back(this);
	}

	if (parent == nullptr && this->parent != nullptr)
	{
		// ���� �θ�ü���� �ٲ��ش�.
		this->parent = parent;
		// �ٸ� �ڽ� ��ü > ��Ʈ��ü
		getScene().baseDestroy(this, false);
		getScene().baseInstantiate(this, true);
	}

	if (parent != nullptr && this->parent != nullptr)
	{
		// ���� �θ�ü���� �ٲ��ش�.
		this->parent = parent;
		// �ٸ� �ڽ� ��ü > �ٸ� �ڽ� ��ü
		getScene().baseDestroy(this, false);
		getScene().baseInstantiate(this, false);

		// ��Ʈ��ü�� �ƴҶ� ����θ�ü�� ���ؼ��� �ڽĵ���� ���ش�.
		parent->children.push_back(this);
	}

	if (parent == nullptr && this->parent == nullptr)
	{
		getScene().baseDestroy(this, true);
		getScene().baseInstantiate(this, true);
	}


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
	if (destroyed) return;

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
	if (destroyed) return;

	for (auto it : components)
	{
		it->fixedUpdate();
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
bool GameObject::destroyUpdate()
{
	// ���� �����Ǿ�� �Ѵٸ� �������ְ� �����Ѵ�.
	// �Ҹ��ڿ��� ���� ��ü�� ��� ���� // ������Ʈ / �ڽĿ�����Ʈ
	if (destroyed)
	{
		FinalDestroy(this);
		return true;
	}

	auto it = children.begin();
	while ( it != children.end())
	{
		// �̸� ������ �޾Ƽ� ���縦 ������Ʈ ��Ű�� ���߿� �ֽ�ȭ ��Ų��
		// �߰��� ������ �ݺ��ڸ� ����Ű�� ���� ���ɼ��� �ֱ� �����̴�.
		// ���������� ����
		GameObject * temp = *it;

		if ((*it)->destroyed)
		{
			// ��Ʈ������Ʈ��� �θ�ü�� children ����Ʈ���� ������ �ʿ䰡 ����.
			if ((*it)->isRootObj() == true)
			{
				FinalDestroy(temp); continue;
			}

			it = children.erase(it);
			FinalDestroy(temp);

			//// ���� �ڽİ�ü��� �θ�ü�� ����Ʈ������ ��������� �Ѵ�.
			//// �ð����⵵ ����ؼ� �ٲܼ������� �ٲܿ���
			//for (auto it2 = children.begin(); it2 != children.end(); ++it2)
			//{
			//	// ��ȸ�ϴٰ� ���� �߰��ϸ� �����.
			//	if (*it2 == *it)
			//	{
			//		children.erase(it2);
			//		break;
			//	}
			//}


		}
		else
		{
			// �����Ǿ�� ���� �ʴ´ٸ� �ڽİ�ü�� �ڽİ�ü�� Ȯ�����ش�.
			++it;
			temp->destroyUpdate();
		}
	}
	return false;

}

void GameObject::collisionUpdate(GameObjectWithCollision & other)
{
	// ��� ������Ʈ�� ���ؼ� onCollisionXX�Լ��� �����Ų��.


	// ���߿� enter / exit�� �߰��� ����
	for (auto it = components.begin(); it != components.end(); ++it)
	{
		(*it)->onCollisionStay(other);
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

// ��� �θ� �ڽ� �Լ��� setParent�߽����� �̷������.
// ���������� Scene�� �ٽõ�� / ����
// �θ��� �ڽİ�ü ��Ͽ��� ���� / �ڽİ�ü�� �θ����� �̷������. 
void GameObject::removeChild(GameObject * child)
{
	if (!child) return;

	// �ڽĸ�Ͽ��� �����Ͽ��� ������ �ڽİ�ü�� ��Ʈ ��ü�� �ȴ�.
	child->setParent(nullptr);

	//for (auto it = children.begin(); it != children.end(); ++it)
	//{
	//	if ((*it) == child)
	//	{
	//		children.erase(it);
	//		return;
	//	}
	//}
	//child->setWhetherDestroyed(true);
}
GameObject * GameObject::addChild(GameObject * child)
{
	if (!child) return nullptr;

	child->setParent(this);

	//child->parent = this;
	//// ���� ��ü children�� ������ְ�
	//children.push_back(child);

	//child->getScene().baseInstantiate(child, child->isRootObj());

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
	
	if(parent !=nullptr)
		parent->children.push_back(tempGameObject);

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
