#include "Scene.h"
#include "Audio.h"
#include "Physics.h"

	Scene::Scene()
		: name("Scene1"), previousTime(0), currentTime(0), lagTime(0)
	{
		// Instantiate함수는 Scene이 완전히 초기화 된 다음에 가능하다.
		//rootGameObjects.push_back(GameObject::Instantiate("player name", "player"));
		//rootGameObjects[0]->addComponent<MoveScript>();

		// 계속 반복되는 현상
		// 이 함수는(addChild) 내부적으로 getInstance()함수를 포함하고 있는데
		// 이는아직 생성이 완료된 상황이 아니여서 nullptr이기 때문에
		// Scene객체를 또 생성하게 되고, 재귀적으로 무한루프에 빠지게 된다.
		// gameObjects[0]->addChild("player name1", "player1");

	}

	void drawScene(IDirect3DDevice9 & device_s)
	{
		/*
		
		D3DXCreateTeapot(&device_s, &Teapot, 0);
		D3DXMATRIX W;
		D3DXMatrixIdentity(&W);
		device_s.SetTransform(D3DTS_WORLD, &W);

		Teapot->DrawSubset(0);

		D3DXMatrixTranslation(&W, 5, 0, 0);

		device_s.SetTransform(D3DTS_WORLD, &W);

		Teapot->DrawSubset(0);

		D3DXMatrixTranslation(&W, 0, 0, 5);

		device_s.SetTransform(D3DTS_WORLD, &W);

		Teapot->DrawSubset(0);

		D3DXMatrixTranslation(&W, -5, 0, 0);

		device_s.SetTransform(D3DTS_WORLD, &W);

		Teapot->DrawSubset(0);

		D3DXMatrixTranslation(&W, 0, 0, -5);

		device_s.SetTransform(D3DTS_WORLD, &W);

		Teapot->DrawSubset(0);

		D3DXMatrixTranslation(&W, 0, 5, 0);

		device_s.SetTransform(D3DTS_WORLD, &W);

		Teapot->DrawSubset(0);
		*/
	}

	void Scene::gameLoop()
	{
		// 카메라는 씬이 시작하기 전에 등록해주었다.
		// 만약 등록된 카메라가 없으면 게임 루프는 돌지 않는다.
		if (mainObjectsTable.find(MOT_MAINCAMERA) == mainObjectsTable.end()) return;

		GameObject * mainCamera = mainObjectsTable[MOT_MAINCAMERA];
		IDirect3DDevice9 & device_s = mainCamera->getDevice();

		/*
		기본적인 월드를 계산해서 그린다.
		*/

		// 여기서 하드코딩으로 해결하는 방법외에는 없을까?
		if (previousTime == 0)
			previousTime = clock();
		
		currentTime = clock();

		while (lagTime >= MS_PER_FIXEDUPDATE)
		{
			// 일정시간 간격마다 업데이트 되는 함수이다
			// 물리 계산도 일정시간마다 업데이트 되어야 하기 때문에 여기서 같이 처리해줄 것이다.
			//Trace::Write("TAG_DEBUG", "fixedUpdate");
			fixedUpdate();

			// 충돌처리 + 물리처리
			physicsUpdate((float)MS_PER_FIXEDUPDATE / 1000.0f);
				//colliderUpdate();

			// 오디오 업데이트 // 스트리밍 형식으로 되기 때문에 업데이트를 해주어야 하긴 하지만
			// 계속해서 호출해줄 필요는 없기 때문에 fixedUpdate()에서 해준다.
			audioUpdate();

			//Trace::Write("TAG_DEBUG", (float)MS_PER_FIXEDUPDATE / 1000.0f);
			lagTime -= MS_PER_FIXEDUPDATE;
		}

		// 업데이트 / 렌더링
		// 업데이트는 매 프레임당 실행되는데 MeshRenderer 컴포넌트에 의해 렌더링과 커플링 되어있다.

		// 이때문에 일정시간 마다 호출할 수 있는 FixedUpdate를 추가한다.
		// 업데이트 내에서 활용되는 계산들은 컴퓨터 성능에 따라 영향을 받을 수 있기 때문에,
		// 프레임간 간격 deltaTime을 한 프레임당 업데이트 함으로써 일정한 계산을 할 수 있도록 제공해줄 생각이다.
		// 그러면 빠른 컴퓨터에서는 deltaTime의 값이 적게 나와서 적은 값을 계산하게 되고
		// 느린 컴퓨터에서는 deltaTime의 값이 크게나와서 큰 값을 계산하게 된다.


		// transform 행렬 최신화
		// 내부적으로 더티플래그를 이용한 자식 위치 최신화가 이루어진다.
		transformUpdate();

		// 화면을 지워준다.
		device_s.Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);

		// 중간에 렌더링 작업도 들어가기 때문에 씬을 그리는 작업을 해준다.
		device_s.BeginScene();


		update();

		clock_t temp = currentTime - previousTime;
		FrameTime::setDeltaTime(temp);
		//Trace::Write("TAG_DEBUG", FrameTime::getDeltaTime());
		previousTime = currentTime;

		lagTime += temp;

		/*
		프레임의 끝단계 일정시간을 딜레이 시키기 전에 여러가지 작업을 해준다.
		*/

		// 업데이트 되고난후 삭제처리
		destroyUpdate();

		// 60프레임 기준으로 시간이 남았다면 딜레이
		
		//Trace::Write("TAG_DEBUG", FrameTime::getDeltaTime());
		
		if(MS_PER_FRAME - FrameTime::getDeltaTime()>0)
			Sleep(MS_PER_FRAME - FrameTime::getDeltaTime());

		device_s.EndScene();
		device_s.Present(0, 0, 0, 0);

	}

	// 현재 게임오브젝트 리스트를 업데이트 시킨다.
	void Scene::update()
	{
		for (int i = 0; i < rootGameObjects.size(); ++i)
		{
			rootGameObjects[i]->update();
		}
		//for (auto it : rootGameObjects)
		//{
		//	it->update();
		//}
	}

	void Scene::fixedUpdate()
	{
		for (int i = 0; i < rootGameObjects.size(); ++i)
		{
			rootGameObjects[i]->fixedUpdate();
		}
		//for (auto it : rootGameObjects)
		//{
		//	it->fixedUpdate();
		//}
	}

	void Scene::audioUpdate()
	{
		// 시스템에 접근하기 위해서는 게임오브젝트의 함수를 통해서만 접근이 가능하다.
		// 게임에서 항상 있어야 할 오브젝트는 카메라 오브젝트이기 때문에 카메라오브젝트를 찾아서 업데이트 시켜준다.
		if (mainObjectsTable.find(MainObjTag::MOT_MAINCAMERA) == mainObjectsTable.end()) return;
		GameObject * camera = mainObjectsTable[MainObjTag::MOT_MAINCAMERA];

		camera->getAudio().update();

	}

	void Scene::physicsUpdate(float deltaTime)
	{
		// 시스템에 접근하기 위해서는 게임오브젝트의 함수를 통해서만 접근이 가능하다.
		// 게임에서 항상 있어야 할 오브젝트는 카메라 오브젝트이기 때문에 카메라오브젝트를 찾아서 업데이트 시켜준다.
		if (mainObjectsTable.find(MainObjTag::MOT_MAINCAMERA) == mainObjectsTable.end()) return;
		GameObject * camera = mainObjectsTable[MainObjTag::MOT_MAINCAMERA];

		camera->getPhysics().update(deltaTime);
	}

	void Scene::transformUpdate()
	{
		for (auto it : rootGameObjects)
		{
			it->getTransform()->transformUpdate(it->getTransform()->getDirty(), Transform::IdentityMatrix_DX, Transform::IdentityMatrix_DX);
		}
	}

	// 현재 물리라이브러리로 작업중
	void Scene::colliderUpdate()
	{
		BoxCollider * bc_obj1 = nullptr;
		BoxCollider * bc_obj2 = nullptr;
		for (auto it1 : rootGameObjects)
		{
			bc_obj1 = (*it1).getComponent<BoxCollider>();
			if (!bc_obj1) continue;
			for (auto it2 : rootGameObjects)
			{
				bc_obj2 = (*it2).getComponent<BoxCollider>();
				if (!bc_obj2) continue;
				if (bc_obj1->checkCollided(*bc_obj2))
					bc_obj1->physicalReaction(*bc_obj2);
			}
		}
	}

	void Scene::destroyUpdate()
	{
		/*
		이런식으로 하면 생기는 문제점
		예를들어 5개의 원소가 있고
		postIt이 마지막 5번째원소를 가리키고
		it이 4번째원소를 가리킨다고 가정해보자

		// 여기서부터 추측
		만약 it가 지워지면 총 크기는 4가 되고 postIt은 5번째를 그대로 가리키고 있다. 
		// 내용물은 맞지만 // 카운트 자체는 5번째인듯
		즉 ++할경우 end를 넘어가게 되어 오류가 발생한다.
		*/
		unsigned int index = 0;
		bool isDestroyed = false;
		while (index < rootGameObjects.size())
		{
			isDestroyed = rootGameObjects[index]->destroyUpdate();
			if (!isDestroyed)
			{
				// 안지워진 경우는 그냥 증가시키면 된다.
				++index;
			}
			else
			{
				// 지워진 경우라면 인덱스를 증가시키지 않는다
			}
		}



		//------------------------------------------------------------------------------
		
		//auto it = rootGameObjects.begin();
		//auto postIt = it;
		//bool isDestroyed = false;
		//// 자식객체가 삭제되었는지 알기 위한 플래그
		//while (it != rootGameObjects.end())
		//{
		//	++postIt;
		//	isDestroyed = (*it)->destroyUpdate();
		//	// 지워지지않았을때만 반복자를 증가
		//	// 내부적으로 FinalDestroy가 호출되어 지워졌을 경우는 it가
		//	if (!isDestroyed)
		//	{
		//		// 안지워진 경우는 그냥 증가시키면 된다.
		//		it = postIt;
		//	}
		//	else
		//	{
		//		// 지워진 경우라면 postIt의 하나전으로 만들어준다.

		//		// 여기서 오류생긴다. 반복자로 전체 컨테이너에서 삭제되었을때
		//		// 미리 받아둔 반복자를 사용하는 것은 피해야할것같다.
		//		--postIt;
		//		it = postIt;
		//	}
		//}
		//------------------------------------------------------------------------------

		//auto it = rootGameObjects.begin();
		//auto end = rootGameObjects.end();
		//auto postIt = it;
		//// 자식객체가 삭제되었는지 알기 위한 플래그
		//while (it != end)
		//{
		//	// 미리 다음걸 받아서 현재를 업데이트 시키고 나중에 최신화 시킨다
		//	// 중간에 삭제된 반복자를 가리키고 있을 가능성이 있기 때문이다.
		//	++postIt;
		//	(*it)->destroyUpdate();
		//	it = postIt;
		//}
	}

	void Scene::changeNameIfItExists(GameObject * other)
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



	// 맵등록
	void Scene::registerGameObject(GameObject * other)
	{
		// 널포인터가 아닐때만 작업
		if (!other) return;

		// 맵에서 확인해보고 등록되지 않았다면 등록한다.
		// 복사 한번 막기위해 레퍼런스로 받아옴
		string temp; other->getPath(temp);
		if (gameObjectsTable.find(temp) == gameObjectsTable.end())	
			gameObjectsTable[temp] = other;							

		// string 복사일어나는줄 알았는데 find 입력파라미터가 key자료형의 레퍼런스형이다 복사X
		// string 복사

		// 복사를 한번막기위해
		// map<string & , GameObject *> or map<string * , GameObject *>으로 바꿀까?

		// string * 를 사용하면 계속해서 남아있어야 하기 때문에 동적할당을 필수적이다. // 비교적 관리가 어렵다.
		// string & 역시 변수를 참조하여 값을 읽기 때문에 동적할당으로 계속해서 객체가 남아있어야한다. // 근데 정의자체가 안된다

		// 간단한 클래스이기 때문에 동적할당까지 필요없다고 생각하여 map<string, GameObject *>형을 채택하였다.
										
	}

	// 맵등록해제
	void Scene::unregisterGameObject(GameObject * other)
	{
		// 널포인터가 아닐때만 작업
		if (!other) return;

		// 맵에서 확인해보고 등록되어 있으면 삭제한다.
		string temp; other->getPath(temp);
		if (gameObjectsTable.find(temp) != gameObjectsTable.end())	// string 복사일어나는줄 알았는데 find 입력파라미터가 key자료형의 레퍼런스형이다 복사X
			gameObjectsTable.erase(temp);

	}

	void Scene::baseInstantiate(GameObject * other, bool rootObj)
	{
		// 만약에 같은 부모를 가진 객체중 중복된 이름이 있다면
		// 이름을 바꿔준다.

		changeNameIfItExists(other);

		// 맵등록
		registerGameObject(other);

		if (!rootObj) return;

		// 루트라면 게임오브젝트 리스트에 추가
		rootGameObjects.push_back(other);
	}

	void Scene::baseDestroy(GameObject * other, bool rootObj)
	{
		unregisterGameObject(other);

		// 나중에 된다면 좀더 빠르게 수정할 예정
		// 현재 선형적으로 설계
		// 루트 객체에 대해서만 필요한 작업
		if (!rootObj) return;

		for (auto it = rootGameObjects.begin(); it < rootGameObjects.end(); ++it)
		{
			if (other == *it)
			{
				rootGameObjects.erase(it);
				break;
			}
		}
	}

	void Scene::registerMainObject(GameObject * other, MainObjTag tag)
	{
		// 널포인터가 아닐때만 작업
		if (!other) return;

		// 맵에서 확인해보고 등록되지 않았다면 등록한다.
		if(mainObjectsTable.find(tag) == mainObjectsTable.end())
			mainObjectsTable[tag] = other;
	}

	void Scene::unregisterMainObject(GameObject * other, MainObjTag tag)
	{
		// 널포인터가 아닐때만 작업
		if (!other) return;
		
		// 맵에서 확인해보고 등록되어 있으면 삭제한다.
		if (mainObjectsTable.find(tag) != mainObjectsTable.end())
			mainObjectsTable.erase(tag);
	}

