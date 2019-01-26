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


// 전체적인 게임루프가 돌아가는 클래스
// GameObject리스트를 가지고 있으며 이 리스트는 
// 게임루프속에서 계속 최신화된다.

class Scene
{
private :
	string name;
	vector<GameObject *> gameObjects;

	// 같은 부모객체를 가진 것들은 무조건 중복된 값을 가지지 못한다.

	// 부모객체가 없는 객체들을 저장하기 위한 table
	// string , GameObject *

	// 현재 객체까지의 경로 // 현재객체 미포함 :: .../.../ 부모 객체의 이름 / 현재 객체의 이름
	// 현재 객체의 포인터
	map<string, GameObject *> gameObjectsTable;

	// 부모객체가 있는 객체들을 저장하기 위한 table




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

			// 콜백으로 끝날때 지워지도록 등록해준다
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
			// esc키 입력 확인
			if (InputManager::GetKeyDown(KeyCode::Esc))
				break;

			// 업데이트
			update();

			// 렌더링

			

			// 프레임 일정하게 딜레이
			Sleep(1500);
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

	void changeNameIfItExists(GameObject * other)
	{
		// 경로가 존재하지 않는다면(등록되어있지 않다면)
		// 경로명을 변경하지않고 바로 등록해도 되는 상황이다. / 리턴

		// 발견되지 않았을때 gameObjectsTable.end()를 반환한다.
		string path;
		other->getPath(path);
		// 이름을 바꿀필요가없다.
		if (gameObjectsTable.find(path) == gameObjectsTable.end())
			return;

		// 이름을 바꿀필요가있다.

		// 같은 경로가 존재 / 뒤에 숫자를 붙인다 똑같은 이름이 안생길때까지
		// ... / ... / 부모 객체의 이름 / 현재 객체의 이름 + 숫자
		char nameNumber = 48; // '0'
		string tempPath;
		tempPath = path + nameNumber;

		// 등록된것이 없을때까지 확인한다.
		while (gameObjectsTable.find(tempPath) != gameObjectsTable.end())
		{
			++nameNumber;
			tempPath = path + nameNumber;
		}
		
		// 이름을 바꿔준다.
		other->setName(other->getName() + nameNumber);
	}

	// GameObject(Transform * transform, const string & name = "default GO name", const string & tag = "default GO tag",
	//	vector<GameObject *> *children = nullptr, vector<Component *> * components = nullptr)

	GameObject * Instantiate(GameObject * other)
	{
		// 만약에 같은 부모를 가진 객체중 중복된 이름이 있다면
		// 이름을 바꿔준다.
		changeNameIfItExists(other);


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

		changeNameIfItExists(tempGameObject);


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


		//int gameObjectsSize = gameObjects.size();

		// 나중에 된다면 좀더 빠르게 수정할 예정
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
		// if (gameObjects.size() == gameObjectsSize) return;
		

		// 게임오브젝트 리스트에 있을때
		
		// 맵 등록해제 // 등록되어있다면 삭제한다.
		unregisterGameObject(other);

		// 등록되어있지 않다고 하더라도
		// 마지막으로 삭제
		delete other;
	}

	// 맵등록
	void registerGameObject(GameObject * other)
	{
		// 널포인터가 아닐때만 작업
		if (!other) return;

		// 맵에서 확인해보고 등록되지 않았다면 등록한다.
		// 복사 한번 막기위해 레퍼런스로 받아옴
		string temp; other->getPath(temp);
		if (gameObjectsTable.find(temp) == gameObjectsTable.end())	// string 복사일어나는줄 알았는데 find 입력파라미터가 key자료형의 레퍼런스형이다 복사X
			gameObjectsTable[temp] = other;							// string 복사

		// 복사를 한번막기위해
		// map<string & , GameObject *> or map<string * , GameObject *>으로 바꿀까?

		// string * 를 사용하면 계속해서 남아있어야 하기 때문에 동적할당을 필수적이다. // 비교적 관리가 어렵다.
		// string & 역시 변수를 참조하여 값을 읽기 때문에 동적할당으로 계속해서 객체가 남아있어야한다. // 근데 정의자체가 안된다

		// 간단한 클래스이기 때문에 동적할당까지 필요없다고 생각하여 map<string, GameObject *>형을 채택하였다.
	}

	// 맵등록해제
	void unregisterGameObject(GameObject * other)
	{
		// 널포인터가 아닐때만 작업
		if (!other) return;

		// 맵에서 확인해보고 등록되어 있으면 삭제한다.
		string temp; other->getPath(temp);
		if (gameObjectsTable.find(temp) != gameObjectsTable.end())	// string 복사일어나는줄 알았는데 find 입력파라미터가 key자료형의 레퍼런스형이다 복사X
			gameObjectsTable.erase(temp);						

	}

};


#endif // !SCENE_H

