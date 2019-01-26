#ifndef SCENE_H
#define SCENE_H


#include <vector>
#include <map>
#include <Windows.h>
#include "Utility.h"
#include "Component.h"
#include "GameObject.h"
#include "InputManager.h"
#include "Trace.h"


// ��ü���� ���ӷ����� ���ư��� Ŭ����
// GameObject����Ʈ�� ������ ������ �� ����Ʈ�� 
// ���ӷ����ӿ��� ��� �ֽ�ȭ�ȴ�.

class Scene
{
private :
	string name;
	vector<GameObject *> gameObjects;

	// ���� �θ�ü�� ���� �͵��� ������ �ߺ��� ���� ������ ���Ѵ�.

	// �θ�ü�� ���� ��ü���� �����ϱ� ���� table
	// string , GameObject *

	// ���� ��ü������ ��� // ���簴ü ������ :: .../.../ �θ� ��ü�� �̸� / ���� ��ü�� �̸�
	// ���� ��ü�� ������
	map<string, GameObject *> gameObjectsTable;

	// �θ�ü�� �ִ� ��ü���� �����ϱ� ���� table




	Scene(string name = "default")
		: name(name)
	{
		gameObjects.push_back(new GameObject("player name","player"));
		
		gameObjects[0]->addComponent<MoveScript>();


		// ��� �ݺ��Ǵ� ����
		// �� �Լ���(addChild) ���������� getInstance()�Լ��� �����ϰ� �ִµ�
		// �̴¾��� ������ �Ϸ�� ��Ȳ�� �ƴϿ��� nullptr�̱� ������
		// Scene��ü�� �� �����ϰ� �ǰ�, ��������� ���ѷ����� ������ �ȴ�.
		// gameObjects[0]->addChild("player name1", "player1");


		Trace::LoadFileNames();
		Trace::Write("TAG_DEBUG", "hello");
		Trace::Write("TAG_DEBUG", "my");
		Trace::Write("TAG_DEBUG", "name");
		Trace::Write("TAG_DEBUG", "is");
		Trace::Write("TAG_DEBUG", "Init_Scene class", "jeongin", true);
	}

	static Scene * instance;
	static void Destroy()
	{
		delete instance;
	}

	// GameObject(Transform * transform, const string & name = "default GO name", const string & tag = "default GO tag",
	//	vector<GameObject *> *children = nullptr, vector<Component *> * components = nullptr)

public :

	static Scene * getInstance()
	{
		if (!instance)
		{
			instance = new Scene();

			// �ݹ����� ������ ���������� ������ش�
			atexit(Destroy);
		}
		return instance;
	}

	void gameLoop()
	{
		/*
		GameObject * temp = nullptr;
		temp = gameObjects[0]->addChild("player name1", "player1");
		temp->addComponent<MoveScript>();

		temp->addChild("player name1", "player1")->addComponent<MoveScript>();
		temp->addChild("player name1", "player1")->addComponent<MoveScript>();
		temp->addChild("player name1", "player1")->addComponent<MoveScript>();
		temp->addChild("player name1", "player1")->addComponent<MoveScript>();
		temp->addChild("player name1", "player1")->addComponent<MoveScript>();
		temp->addChild("player name1", "player1")->addComponent<MoveScript>();
		*/

		while (1)
		{
			// escŰ �Է� Ȯ��
			if (InputManager::GetKeyDown(KeyCode::Esc))
				break;

			// ������Ʈ
			update();

			// ������

			

			// ������ �����ϰ� ������
			Sleep(1500);
		}

	}

	// ���� ���ӿ�����Ʈ ����Ʈ�� ������Ʈ ��Ų��.
	void update()
	{
		for (auto it : gameObjects)
		{
 			it->update();
		}
	}

	void changeNameIfItExists(GameObject * other)
	{
		// ��ΰ� �������� �ʴ´ٸ�(��ϵǾ����� �ʴٸ�)
		// ��θ��� ���������ʰ� �ٷ� ����ص� �Ǵ� ��Ȳ�̴�. / ����

		// �߰ߵ��� �ʾ����� gameObjectsTable.end()�� ��ȯ�Ѵ�.
		string path;
		other->getPath(path);
		// �̸��� �ٲ��ʿ䰡����.
		if (gameObjectsTable.find(path) == gameObjectsTable.end())
			return;

		// �̸��� �ٲ��ʿ䰡�ִ�.

		// ���� ��ΰ� ���� / �ڿ� ���ڸ� ���δ� �Ȱ��� �̸��� �Ȼ��涧����
		// ... / ... / �θ� ��ü�� �̸� / ���� ��ü�� �̸� + ����
		char nameNumber = 48; // '0'
		string tempPath;
		tempPath = path + nameNumber;

		// ��ϵȰ��� ���������� Ȯ���Ѵ�.
		while (gameObjectsTable.find(tempPath) != gameObjectsTable.end())
		{
			++nameNumber;
			tempPath = path + nameNumber;
		}
		
		// �̸��� �ٲ��ش�.
		other->setName(other->getName() + nameNumber);
	}

	// GameObject(Transform * transform, const string & name = "default GO name", const string & tag = "default GO tag",
	//	vector<GameObject *> *children = nullptr, vector<Component *> * components = nullptr)

	GameObject * Instantiate(GameObject * other)
	{
		// ���࿡ ���� �θ� ���� ��ü�� �ߺ��� �̸��� �ִٸ�
		// �̸��� �ٲ��ش�.
		changeNameIfItExists(other);


		// ���ӿ�����Ʈ ����Ʈ�� �߰��ϰ�
		getInstance()->gameObjects.push_back(other);

		// ���߿� �����ϱ� ���ϱ� ���ؼ� ���� map�� �߰��Ѵ�. // find�Լ����� ���

		// �ʵ��
		registerGameObject(other);


		return getInstance()->gameObjects.back();
	}

	GameObject * Instantiate
	(const string & name = "default GO name", const string & tag = "default GO tag",
		const Vector3 & position = Vector3::Zero, const Vector3 & rotation = Vector3::Zero, const Vector3 & scale = Vector3::One,
		GameObject * parent = nullptr, vector<GameObject *> *children = nullptr, 
		vector<Component *> * components = nullptr)
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

		// ���࿡ ���� �θ� ���� ��ü�� �ߺ��� �̸��� �ִٸ�
		// �̸��� �ٲ��ش�.

		changeNameIfItExists(tempGameObject);


		// ���ӿ�����Ʈ ����Ʈ�� �߰��ϰ�
		getInstance()->gameObjects.push_back(tempGameObject);

		// ���߿� �����ϱ� ���ϱ� ���ؼ� ���� map�� �߰��Ѵ�. // find�Լ����� ���

		// �ʵ��
		registerGameObject(tempGameObject);


		return getInstance()->gameObjects.back();
	}

	void Destroy(GameObject * other)
	{
		if (!other) return;


		//int gameObjectsSize = gameObjects.size();

		// ���߿� �ȴٸ� ���� ������ ������ ����
		// ���� ���������� ����
		for (auto it = gameObjects.begin(); it < gameObjects.end(); ++it)
		{
			if (other == *it)
			{
				gameObjects.erase(it);
				break;
			}
		}

		// ���� ���縮��Ʈ�� ������ ����
		// �����Ȱ��� ���ٴ� �� �� ���ӿ�����Ʈ ����Ʈ�� ���� �ʾҴ�.
		// if (gameObjects.size() == gameObjectsSize) return;
		

		// ���ӿ�����Ʈ ����Ʈ�� ������
		
		// �� ������� // ��ϵǾ��ִٸ� �����Ѵ�.
		unregisterGameObject(other);

		// ��ϵǾ����� �ʴٰ� �ϴ���
		// ���������� ����
		delete other;
	}

	// �ʵ��
	void registerGameObject(GameObject * other)
	{
		// �������Ͱ� �ƴҶ��� �۾�
		if (!other) return;

		// �ʿ��� Ȯ���غ��� ��ϵ��� �ʾҴٸ� ����Ѵ�.
		// ���� �ѹ� �������� ���۷����� �޾ƿ�
		string temp; other->getPath(temp);
		if (gameObjectsTable.find(temp) == gameObjectsTable.end())	// string �����Ͼ���� �˾Ҵµ� find �Է��Ķ���Ͱ� key�ڷ����� ���۷������̴� ����X
			gameObjectsTable[temp] = other;							// string ����

		// ���縦 �ѹ���������
		// map<string & , GameObject *> or map<string * , GameObject *>���� �ٲܱ�?

		// string * �� ����ϸ� ����ؼ� �����־�� �ϱ� ������ �����Ҵ��� �ʼ����̴�. // ���� ������ ��ƴ�.
		// string & ���� ������ �����Ͽ� ���� �б� ������ �����Ҵ����� ����ؼ� ��ü�� �����־���Ѵ�. // �ٵ� ������ü�� �ȵȴ�

		// ������ Ŭ�����̱� ������ �����Ҵ���� �ʿ���ٰ� �����Ͽ� map<string, GameObject *>���� ä���Ͽ���.
	}

	// �ʵ������
	void unregisterGameObject(GameObject * other)
	{
		// �������Ͱ� �ƴҶ��� �۾�
		if (!other) return;

		// �ʿ��� Ȯ���غ��� ��ϵǾ� ������ �����Ѵ�.
		string temp; other->getPath(temp);
		if (gameObjectsTable.find(temp) != gameObjectsTable.end())	// string �����Ͼ���� �˾Ҵµ� find �Է��Ķ���Ͱ� key�ڷ����� ���۷������̴� ����X
			gameObjectsTable.erase(temp);						

	}

};


#endif // !SCENE_H

