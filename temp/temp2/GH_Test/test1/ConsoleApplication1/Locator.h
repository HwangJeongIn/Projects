#pragma once
#include "Scene.h"
#include "Locator.h"

/*
�⺻�����δ� nullScene�� �����Ѵ�.
ó���� �ʱ�ȭ �Ҷ� nullScene���� �ʱ�ȭ �Ѵ�.
getScene������ scene�� ��ȯ�Ѵ�.

device���� ���� device������ �����ؼ� �ʿ��� �޼ҵ常 �����ϵ��� ���ش�.
������ ���͵Ǵ� ��� ���α׷��� �׵��� ����?
friend �������� ������ ���⼭ ����? 
// ������ �̰�� �ƿ� friend������ ���� ���ο��� �˾Ƽ� ���ִ� �� �ۿ�����.
// ���丮 ���� // �� �ܺο��� �����ϱⰡ �Ұ����ϴ�. 
// �ƿ� �ʿ��� �Ķ���� ��ü�� �Ѱ��־�� �Ѵ�. �̷��ԵǸ� �����Լ��� ������������.
// �ܺο��� ������ �ƿ� ���ص� �Ǵ� ��쿡�� ���丮 ������ ����ϴ°� ���?
*/

class Locator
{
private :
	static Scene * scene;
	static NullScene nullScene;
	// static LoggedScene loggedScene;

protected :
	static Scene & getScene()
	{
		// ����ϰ� ȣ��� �� �ֱ� ������ �̸� ����� �ؼ� 
		// ���⼭�� ��������ȯ���ϴ� �ɷ� �Ѵ�.

		return *scene;
	}

public :
	enum SystemType
	{
		DEBUGTYPE,
		RELEASETYPE,
		NULLTYPE
	};

	/*
	static void provideScene(Scene * scene, bool debuggingMode = false)
	{
		// �߰��� �ٲ�� ��찡 ������ ���� �����Ҵ��� �Ǿ� �ִٸ� �������� ���ش�.
		if (Locator::scene != &nullScene && Locator::scene != nullptr)
			delete scene;
		// nullptr�� ������ ��쿡�� nullScene���� �ʱ�ȭ �����ش�.
		if (scene == nullptr)
		{
			Locator::scene = &nullScene;
			return;
		}

		// �ƴ� ��쿡�� ���� ���� �� ���� �ƴҶ��� ���� ������ش�.

		
		if (debuggingMode)
		{
			// Locator::scene = new LoggedScene;
			return;
		}

		Locator::scene = new Scene;
		
	}
	*/

	
	static void provideScene(SystemType type = SystemType::RELEASETYPE)
	{
		bool isNull = false;
		// ���� ó�� ����� �ٸ� ���� Debug����϶� �̴�. ����ó�����ش�.
		if (type == SystemType::DEBUGTYPE)
		{
			// ���� ������ �ִ� Scene�� NullScene�̰ų� nullptr�̸� �ٷ���������ش�.
			if (Locator::scene == &nullScene || Locator::scene == nullptr)
			{
				isNull = true;
				scene = new LoggedScene(nullScene);
				return;
			}

			scene = new LoggedScene(*scene);
			return;
		}

		// ���Ӱ� �Ҵ��ؾ� �Ǳ� ������ ���� �ƴ� �͵��� �� ������ �����Ѵ�.
		if (!isNull)
			delete scene;

		switch (type)
		{
		case SystemType::RELEASETYPE: 
			scene = new Scene();
			break;

		case SystemType::NULLTYPE: default: 
			scene = &nullScene;
			break;
		}
	}
	


};