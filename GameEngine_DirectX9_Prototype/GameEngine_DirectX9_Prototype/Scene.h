#ifndef SCENE_H
#define SCENE_H


#include <vector>
#include <map>
#include <Windows.h>
#include "Utility.h"
#include "Component.h"
#include "GameObject.h"
#include "Trace.h"
#include "d3dUtility.h"
#include <chrono>


// ��ü���� ���ӷ����� ���ư��� Ŭ����
// GameObject����Ʈ�� ������ ������ �� ����Ʈ�� 
// ���ӷ����ӿ��� ��� �ֽ�ȭ�ȴ�.

class Scene
{
private :
	string name;
	vector<GameObject *> gameObjects;
	IDirect3DDevice9* device;
	// ���� �θ�ü�� ���� �͵��� ������ �ߺ��� ���� ������ ���Ѵ�.

	// �θ�ü�� ���� ��ü���� �����ϱ� ���� table
	// string , GameObject *

	// ���� ��ü������ ��� // ���簴ü ������ :: .../.../ �θ� ��ü�� �̸� / ���� ��ü�� �̸�
	// ���� ��ü�� ������
	map<string, GameObject *> gameObjectsTable;

	// �θ�ü�� �ִ� ��ü���� �����ϱ� ���� table

	// 60�������϶� 1�����Ӵ� �ð�
	static const DWORD MS_PER_FRAME = 16;

	// 2�����Ӹ��� �ֽ�ȭ�ϰ� �ȴ�.
	static const DWORD MS_PER_FIXEDUPDATE = MS_PER_FRAME*2;

	clock_t previousTime;
	clock_t currentTime;
	clock_t lagTime;


	Scene(IDirect3DDevice9* device =nullptr)
		: name("Scene1"), device(device), previousTime(0), currentTime(0), lagTime(0)
	{


		gameObjects.push_back(new GameObject("player name","player"));
		
		gameObjects[0]->addComponent<MoveScript>();


		// ��� �ݺ��Ǵ� ����
		// �� �Լ���(addChild) ���������� getInstance()�Լ��� �����ϰ� �ִµ�
		// �̴¾��� ������ �Ϸ�� ��Ȳ�� �ƴϿ��� nullptr�̱� ������
		// Scene��ü�� �� �����ϰ� �ǰ�, ��������� ���ѷ����� ������ �ȴ�.
		// gameObjects[0]->addChild("player name1", "player1");

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

	void gameLoop()
	{
		// ���⼭ �ϵ��ڵ����� �ذ��ϴ� ����ܿ��� ������?
		if (previousTime == 0)
			previousTime = clock();

		currentTime = clock();



		// ������Ʈ / ������
		// ������Ʈ�� �� �����Ӵ� ����Ǵµ� MeshRenderer ������Ʈ�� ���� �������� Ŀ�ø� �Ǿ��ִ�.

		// �̶����� �����ð� ���� ȣ���� �� �ִ� FixedUpdate�� �߰��Ѵ�.
		// ������Ʈ ������ Ȱ��Ǵ� ������ ��ǻ�� ���ɿ� ���� ������ ���� �� �ֱ� ������,
		// �����Ӱ� ���� deltaTime�� �� �����Ӵ� ������Ʈ �����ν� ������ ����� �� �� �ֵ��� �������� �����̴�.
		// �׷��� ���� ��ǻ�Ϳ����� deltaTime�� ���� ���� ���ͼ� ���� ���� ����ϰ� �ǰ�
		// ���� ��ǻ�Ϳ����� deltaTime�� ���� ũ�Գ��ͼ� ū ���� ����ϰ� �ȴ�.
		update();

		clock_t temp = currentTime - previousTime;
		FrameTime::setDeltaTime(temp);

		previousTime = currentTime;

		char t[100]{};
		//_itoa_s(timeDelta, t, 10);
		sprintf(t, "%f", temp);
		Trace::Write("TAG_DEBUG", "deltaTime", t);

		lagTime += temp;

		
		//while (lagTime >= MS_PER_FIXEDUPDATE)
		//{
		//	// �����ð� ���ݸ��� ������Ʈ �Ǵ� �Լ��̴�
		//	// ���� ��굵 �����ð����� ������Ʈ �Ǿ�� �ϱ� ������ ���⼭ ���� ó������ ���̴�.
		//	//Trace::Write("TAG_DEBUG", "fixedUpdate");
		//	fixedUpdate();
		//	lagTime -= MS_PER_FIXEDUPDATE;
		//}




		//// 60������ �������� �ð��� ���Ҵٸ� ������
		//if(MS_PER_FRAME - FrameTime::getDeltaTime()>0)
		//	Sleep(MS_PER_FRAME - FrameTime::getDeltaTime());

	}

	// ���� ���ӿ�����Ʈ ����Ʈ�� ������Ʈ ��Ų��.
	void update()
	{
		for (auto it : gameObjects)
		{
 			it->update();
		}
	}

	void fixedUpdate()
	{
		for (auto it : gameObjects)
		{
			it->fixedUpdate();
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

