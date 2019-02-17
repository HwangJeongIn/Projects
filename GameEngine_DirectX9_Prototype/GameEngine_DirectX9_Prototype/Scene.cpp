#include "Scene.h"

	Scene::Scene()
		: name("Scene1"), previousTime(0), currentTime(0), lagTime(0)
	{
		// Instantiate�Լ��� Scene�� ������ �ʱ�ȭ �� ������ �����ϴ�.
		//rootGameObjects.push_back(GameObject::Instantiate("player name", "player"));
		//rootGameObjects[0]->addComponent<MoveScript>();

		// ��� �ݺ��Ǵ� ����
		// �� �Լ���(addChild) ���������� getInstance()�Լ��� �����ϰ� �ִµ�
		// �̴¾��� ������ �Ϸ�� ��Ȳ�� �ƴϿ��� nullptr�̱� ������
		// Scene��ü�� �� �����ϰ� �ǰ�, ��������� ���ѷ����� ������ �ȴ�.
		// gameObjects[0]->addChild("player name1", "player1");

	}
	ID3DXMesh* Teapot = 0;

	void drawScene(IDirect3DDevice9 & device_s)
	{


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
	}

	void Scene::gameLoop()
	{
		// ī�޶�� ���� �����ϱ� ���� ������־���.
		// ���� ��ϵ� ī�޶� ������ ���� ������ ���� �ʴ´�.
		if (mainObjectsTable.find(MOT_MAINCAMERA) == mainObjectsTable.end()) return;

		GameObject * mainCamera = mainObjectsTable[MOT_MAINCAMERA];
		IDirect3DDevice9 & device_s = mainCamera->getDevice();

		/*
		�⺻���� ���带 ����ؼ� �׸���.
		*/

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


		//D3DXVECTOR3 position(0.0f, 0.0f, -3.0f);
		//D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
		//D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);

		//D3DXMatrixLookAtLH(&V, &position, &target, &up);
		//D3DXMATRIX V;
		//mainCamera->getComponent<MainCamera>()->getViewMatrix(&V);
		//device_s.SetTransform(D3DTS_VIEW, &V);

		
		// transform ��� �ֽ�ȭ
		transformUpdate();


		// ȭ���� �����ش�.
		device_s.Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);

		// �߰��� ������ �۾��� ���� ������ ���� �׸��� �۾��� ���ش�.
		device_s.BeginScene();


		// �ӽ�
		//drawScene(device_s);

		update();

		device_s.EndScene();
		device_s.Present(0, 0, 0, 0);

		// ������Ʈ �ǰ��� ����ó��
		destroyUpdate();

		Sleep(20);

		if(Teapot)
			Teapot->Release();

		return;

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
	void Scene::update()
	{
		for (auto it : rootGameObjects)
		{
			it->update();
		}
	}

	void Scene::fixedUpdate()
	{
		for (auto it : rootGameObjects)
		{
			it->fixedUpdate();
		}
	}

	void Scene::transformUpdate()
	{
		for (auto it : rootGameObjects)
		{
			it->getTransform()->transformUpdate(it->getTransform()->getDirty(), Transform::IdentityMatrix_DX, Transform::IdentityMatrix_DX);
		}
	}

	// �ϴ� �浹�Ǹ� �Ȱ�ġ�� �����̴� �ɷ� ����.
	void Scene::colliderUpdate()
	{
		for (auto it1 : rootGameObjects)
		{
			
			for (auto it2 : rootGameObjects)
			{

			}
		}
	}

	void Scene::destroyUpdate()
	{
		auto it = rootGameObjects.begin();
		auto end = rootGameObjects.end();
		auto postIt = it;
		// �ڽİ�ü�� �����Ǿ����� �˱� ���� �÷���
		while (it != end)
		{
			// �̸� ������ �޾Ƽ� ���縦 ������Ʈ ��Ű�� ���߿� �ֽ�ȭ ��Ų��
			// �߰��� ������ �ݺ��ڸ� ����Ű�� ���� ���ɼ��� �ֱ� �����̴�.
			++postIt;
			(*it)->destroyUpdate();

			it = postIt;
		}
	}

	void Scene::changeNameIfItExists(GameObject * other)
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



	// �ʵ��
	void Scene::registerGameObject(GameObject * other)
	{
		// �������Ͱ� �ƴҶ��� �۾�
		if (!other) return;

		// �ʿ��� Ȯ���غ��� ��ϵ��� �ʾҴٸ� ����Ѵ�.
		// ���� �ѹ� �������� ���۷����� �޾ƿ�
		string temp; other->getPath(temp);
		if (gameObjectsTable.find(temp) == gameObjectsTable.end())	
			gameObjectsTable[temp] = other;							

		// string �����Ͼ���� �˾Ҵµ� find �Է��Ķ���Ͱ� key�ڷ����� ���۷������̴� ����X
		// string ����

		// ���縦 �ѹ���������
		// map<string & , GameObject *> or map<string * , GameObject *>���� �ٲܱ�?

		// string * �� ����ϸ� ����ؼ� �����־�� �ϱ� ������ �����Ҵ��� �ʼ����̴�. // ���� ������ ��ƴ�.
		// string & ���� ������ �����Ͽ� ���� �б� ������ �����Ҵ����� ����ؼ� ��ü�� �����־���Ѵ�. // �ٵ� ������ü�� �ȵȴ�

		// ������ Ŭ�����̱� ������ �����Ҵ���� �ʿ���ٰ� �����Ͽ� map<string, GameObject *>���� ä���Ͽ���.
										
	}

	// �ʵ������
	void Scene::unregisterGameObject(GameObject * other)
	{
		// �������Ͱ� �ƴҶ��� �۾�
		if (!other) return;

		// �ʿ��� Ȯ���غ��� ��ϵǾ� ������ �����Ѵ�.
		string temp; other->getPath(temp);
		if (gameObjectsTable.find(temp) != gameObjectsTable.end())	// string �����Ͼ���� �˾Ҵµ� find �Է��Ķ���Ͱ� key�ڷ����� ���۷������̴� ����X
			gameObjectsTable.erase(temp);

	}

	void Scene::baseInstantiate(GameObject * other, bool rootObj)
	{
		// ���࿡ ���� �θ� ���� ��ü�� �ߺ��� �̸��� �ִٸ�
		// �̸��� �ٲ��ش�.

		changeNameIfItExists(other);

		// �ʵ��
		registerGameObject(other);

		if (!rootObj) return;

		// ��Ʈ��� ���ӿ�����Ʈ ����Ʈ�� �߰�
		rootGameObjects.push_back(other);
	}

	void Scene::baseDestroy(GameObject * other, bool rootObj)
	{
		unregisterGameObject(other);

		// ���߿� �ȴٸ� ���� ������ ������ ����
		// ���� ���������� ����
		// ��Ʈ ��ü�� ���ؼ��� �ʿ��� �۾�
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
		// �������Ͱ� �ƴҶ��� �۾�
		if (!other) return;

		// �ʿ��� Ȯ���غ��� ��ϵ��� �ʾҴٸ� ����Ѵ�.
		if(mainObjectsTable.find(tag) == mainObjectsTable.end())
			mainObjectsTable[tag] = other;
	}

	void Scene::unregisterMainObject(GameObject * other, MainObjTag tag)
	{
		// �������Ͱ� �ƴҶ��� �۾�
		if (!other) return;
		
		// �ʿ��� Ȯ���غ��� ��ϵǾ� ������ �����Ѵ�.
		if (mainObjectsTable.find(tag) != mainObjectsTable.end())
			mainObjectsTable.erase(tag);
	}

