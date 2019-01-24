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


// 전체적인 게임루프가 돌아가는 클래스
// GameObject리스트를 가지고 있으며 이 리스트는 
// 게임루프속에서 계속 최신화된다.

class Scene
{
private :
	string name;
	vector<GameObject *> gameObjects;

	Scene(string name = "default")
		: name(name)
	{
		gameObjects.push_back(new GameObject("player name","player"));
		
		gameObjects[0]->addComponent<MoveScript>();

		// 계속 반복되는 현상
		// 이 함수는(addChild) 내부적으로 getInstance()함수를 포함하고 있는데
		// 이는아직 생성이 완료된 상황이 아니여서 nullptr이기 때문에
		// Scene객체를 또 생성하게 되고, 재귀적으로 무한루프에 빠지게 된다.
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

			// 콜백으로 끝날때 지워지도록 등록해준다
			atexit(Destroy);
		}
		return instance;
	}

	void gameLoop()
	{
		//gameObjects[0]->addChild("player name1", "player1");
		//gameObjects[1]->addComponent<MoveScript>();
		while (1)
		{
			// esc키 입력 확인

			// 업데이트
			update();

			// 렌더링


			// 프레임 일정하게 딜레이
			Sleep(66);
		}

	}

	// 현재 게임오브젝트 리스트를 업데이트 시킨다.
	void update()
	{
		for (auto it : gameObjects)
		{
			it->update();
		}
	}


	// GameObject(Transform * transform, const string & name = "default GO name", const string & tag = "default GO tag",
	//	vector<GameObject *> *children = nullptr, vector<Component *> * components = nullptr)

	GameObject * Instantiate(GameObject * other)
	{
		// 만약에 같은 부모를 가진 객체중 중복된 이름이 있다면
		// 이름을 바꿔준다.








		// 게임오브젝트 리스트에 추가하고
		getInstance()->gameObjects.push_back(other);

		// 나중에 관리하기 편하기 위해서 만든 map에 추가한다. // find함수에서 사용

		// 맵등록
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
		객체가 초기화되는 과정
		1. GameObject 생성 // 부가적으로 transform 생성
		2. 등록

		// 문제점 : 동시에 생성하려니까 서로가 서로의 포인터를 필요로 하기 때문에 불가능하다.
		
		해결책
		1. transform을 nullptr로 초기화한다음에 나중에 초기화하는 방법을 사용한다.

		2. 디폴트 값으로 GameObject 이니셜라이저에서 Transfrom 생성

		3. GameObject 입력파라미터를 바꿔서 pos, rotation, scale을 추가한후 
		Transfrom객체를 GameObject 이니셜라이저에서 생성

		// 3번 채택
		*/

		// 객체를 생성해준다.
		GameObject * tempGameObject = new GameObject(name, tag, position, rotation, scale, parent, children, components);

		// 만약에 같은 부모를 가진 객체중 중복된 이름이 있다면
		// 이름을 바꿔준다.
		







		// 게임오브젝트 리스트에 추가하고
		getInstance()->gameObjects.push_back(tempGameObject);

		// 나중에 관리하기 편하기 위해서 만든 map에 추가한다. // find함수에서 사용

		// 맵등록
		registerGameObject(tempGameObject);


		return getInstance()->gameObjects.back();
	}

	void Destroy(GameObject * other)
	{
		if (!other) return;

		int gameObjectsSize = gameObjects.size();

		// 나중에 좀더 빠르게 수정할 예정
		// 현재 선형적으로 설계
		for (auto it = gameObjects.begin(); it < gameObjects.end(); ++it)
		{
			if (other == *it)
			{
				gameObjects.erase(it);
				break;
			}
		}

		// 만약 현재리스트에 없으면 리턴
		// 삭제된것이 없다는 뜻 즉 게임오브젝트 리스트에 있지 않았다.
		if (gameObjects.size() == gameObjectsSize) return;

		// 게임오브젝트 리스트에 있을때

		// 맵 등록해제
		unregisterGameObject(other);

		// 마지막으로 삭제
		delete other;
	}

	// 맵등록
	void registerGameObject(GameObject * other)
	{
		// 널포인터가 아닐때만 작업
		if (!other) return;

		// 맵에서 확인해보고 등록되지 않았다면 등록한다.

		// 맵 멀티맵에 모두 등록

	}

	// 맵등록해제
	void unregisterGameObject(GameObject * other)
	{
		// 널포인터가 아닐때만 작업
		if (!other) return;

		// 맵에서 확인해보고 등록되지 않았다면 리턴한다.

		// 맵 멀티맵에 등록해놨던 것들을 모두 해제

	}

};


#endif // !SCENE_H

