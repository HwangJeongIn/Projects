#ifndef SCENE_H
#define SCENE_H


#include <vector>
#include <map>
#include "Utility.h"
#include "Component.h"
#include "GameObject.h"
#include <Windows.h>



class InputManager
{
private :


public :


};


// ��ü���� ���ӷ����� ���ư��� Ŭ����
// GameObject����Ʈ�� ������ ������ �� ����Ʈ�� 
// ���ӷ����ӿ��� ��� �ֽ�ȭ�ȴ�.

class Scene
{
private :
	string name;
	vector<GameObject *> gameObjects;

	Scene(string name = "default")
		: name(name)
	{
		//gameObjects.push_back(new GameObject());
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

		while (1)
		{
			// escŰ �Է� Ȯ��

			// ������Ʈ

			// ������


			// ������ �����ϰ� ������

		}

	}



	// GameObject(Transform * transform, const string & name = "default GO name", const string & tag = "default GO tag",
	//	vector<GameObject *> *children = nullptr, vector<Component *> * components = nullptr)

	GameObject * Instantiate
	(const string & name = "default GO name", const string & tag = "default GO tag",
		const Vector3 & position = Vector3::Zero, const Vector3 & rotation = Vector3::Zero, const Vector3 & scale = Vector3::One,
		vector<GameObject *> *children = nullptr, vector<Component *> * components = nullptr)
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
		GameObject * tempGameObject = new GameObject(name, tag, position, rotation, scale, children, components);

		// ���࿡ ���� �θ� ���� ��ü�� �ߺ��� �̸��� �ִٸ�
		// �̸��� �ٲ��ش�.
		







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

		int gameObjectsSize = gameObjects.size();

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
		if (gameObjects.size() == gameObjectsSize) return;

		// ���ӿ�����Ʈ ����Ʈ�� ������

		// �� �������
		unregisterGameObject(other);

		// ���������� ����
		delete other;
	}

	// �ʵ��
	void registerGameObject(GameObject * other)
	{
		// �������Ͱ� �ƴҶ��� �۾�
		if (!other) return;

		// �ʿ��� Ȯ���غ��� ��ϵ��� �ʾҴٸ� ����Ѵ�.

		// �� ��Ƽ�ʿ� ��� ���

	}

	// �ʵ������
	void unregisterGameObject(GameObject * other)
	{
		// �������Ͱ� �ƴҶ��� �۾�
		if (!other) return;

		// �ʿ��� Ȯ���غ��� ��ϵ��� �ʾҴٸ� �����Ѵ�.

		// �� ��Ƽ�ʿ� ����س��� �͵��� ��� ����

	}

};


#endif // !SCENE_H

