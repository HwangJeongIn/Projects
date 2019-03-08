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
//#include <chrono>
//#include <cstdio>
#include <ctime>


// ��ü���� ���ӷ����� ���ư��� Ŭ����
// GameObject����Ʈ�� ������ ������ �� ����Ʈ�� 
// ���ӷ����ӿ��� ��� �ֽ�ȭ�ȴ�.

class Locator;

class Scene
{
public :
	enum MainObjTag
	{
		MOT_MAINCAMERA,
		MOT_LIGHT
	};

private :
	string name;
	vector<GameObject *> rootGameObjects;
	// ���� �θ�ü�� ���� �͵��� ������ �ߺ��� ���� ������ ���Ѵ�.

	// �θ�ü�� ���� ��ü���� �����ϱ� ���� table
	// string , GameObject *

	// ���� ��ü������ ��� // ���簴ü ������ :: .../.../ �θ� ��ü�� �̸� / ���� ��ü�� �̸�
	// ���� ��ü�� ������
	map<string, GameObject *> gameObjectsTable;

	// �θ�ü�� �ִ� ��ü���� �����ϱ� ���� table


	// �ֿ��� ������ ������ ������ ���� ���̺��̴�
	// ī�޶�, �� ���
	map<MainObjTag, GameObject *> mainObjectsTable;

	// 60�������϶� 1�����Ӵ� �ð�
	static const long MS_PER_FRAME = 16;

	// 2�����Ӹ��� �ֽ�ȭ�ϰ� �ȴ�.
	static const long MS_PER_FIXEDUPDATE = MS_PER_FRAME*2;

	clock_t previousTime;
	clock_t currentTime;
	clock_t lagTime;

protected :

	Scene();
	virtual ~Scene(){}

public :

	friend class Locator;
	virtual void gameLoop();
	virtual void update();
	virtual void fixedUpdate();
	virtual void audioUpdate();
	virtual void physicsUpdate(float deltaTime);
	virtual void transformUpdate();
	virtual void destroyUpdate();

	// ���� ���X
	virtual void colliderUpdate();
	
	
	virtual void changeNameIfItExists(GameObject * other);
	virtual void registerGameObject(GameObject * other);
	virtual void unregisterGameObject(GameObject * other);
	virtual void baseInstantiate(GameObject * other, bool rootObj);
	virtual void baseDestroy(GameObject * other, bool rootObj);

	void registerMainObject(GameObject * other, MainObjTag tag);
	void unregisterMainObject(GameObject * other, MainObjTag tag);



};

class NullScene : public Scene
{
private:

	NullScene()
		: Scene() {}
	virtual ~NullScene() {}

public :
	friend class Locator;
	/*
		virtual void gameLoop(){}
	virtual void update(){}
	virtual void fixedUpdate(){}
	virtual void changeNameIfItExists(GameObject * other){}
	virtual void registerGameObject(GameObject * other){}
	virtual void unregisterGameObject(GameObject * other){}
	*/

};

class DebuggingScene : public Scene
{
private :
	Scene & wrapped;
	DebuggingScene(Scene & scene)
		: wrapped(scene) {}
	virtual ~DebuggingScene() {}

	void log(const char * message)
	{
		if (!message) return;
		cout << message << endl;
	}

public :
	friend class Locator;
	/*
	virtual void gameLoop() { wrapped.gameLoop(); }
	virtual void update() { wrapped.update(); }
	virtual void fixedUpdate() { wrapped.fixedUpdate(); }
	virtual void changeNameIfItExists(GameObject * other) { wrapped.changeNameIfItExists(other); }
	virtual GameObject * Instantiate(GameObject * other) { wrapped.Instantiate(other) }
	virtual GameObject * Instantiate
	(const string & name = "default GO name", const string & tag = "default GO tag",
	const Vector3 & position = Vector3::Zero, const Vector3 & rotation = Vector3::Zero, const Vector3 & scale = Vector3::One,
	GameObject * parent = nullptr, vector<GameObject *> *children = nullptr,
	vector<Component *> * components = nullptr) {}

	virtual void Destroy(GameObject * other) {}
	virtual void registerGameObject(GameObject * other) {}
	virtual void unregisterGameObject(GameObject * other) {}
	*/

};

#endif // !SCENE_H

