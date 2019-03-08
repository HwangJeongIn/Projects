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


// 전체적인 게임루프가 돌아가는 클래스
// GameObject리스트를 가지고 있으며 이 리스트는 
// 게임루프속에서 계속 최신화된다.

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
	// 같은 부모객체를 가진 것들은 무조건 중복된 값을 가지지 못한다.

	// 부모객체가 없는 객체들을 저장하기 위한 table
	// string , GameObject *

	// 현재 객체까지의 경로 // 현재객체 미포함 :: .../.../ 부모 객체의 이름 / 현재 객체의 이름
	// 현재 객체의 포인터
	map<string, GameObject *> gameObjectsTable;

	// 부모객체가 있는 객체들을 저장하기 위한 table


	// 주요한 값들을 빠르게 얻어오기 위한 테이블이다
	// 카메라, 빛 등등
	map<MainObjTag, GameObject *> mainObjectsTable;

	// 60프레임일때 1프레임당 시간
	static const long MS_PER_FRAME = 16;

	// 2프레임마다 최신화하게 된다.
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

	// 현재 사용X
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

