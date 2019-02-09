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
	void Scene::gameLoop()
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



