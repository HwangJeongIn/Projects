#pragma once
#include "Scene.h"
#include "Locator.h"

/*
기본적으로는 nullScene을 제공한다.
처음에 초기화 할때 nullScene으로 초기화 한다.
getScene에서는 scene만 반환한다.

device같은 경우는 device변수를 래핑해서 필요한 메소드만 지원하도록 해준다.
여러번 생셩되는 경우 프로그램이 죽도록 설정?
friend 설정으로 생성을 여기서 제한? 
// 하지만 이경우 아예 friend선언을 받은 내부에서 알아서 해주는 수 밖에없다.
// 팩토리 패턴 // 즉 외부에서 생성하기가 불가능하다. 
// 아예 필요한 파라미터 자체만 넘겨주어야 한다. 이렇게되면 생성함수가 지저분해진다.
// 외부에서 생성을 아예 안해도 되는 경우에만 팩토리 패턴을 사용하는게 어떨까?
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
		// 빈번하게 호출될 수 있기 때문에 미리 계산을 해서 
		// 여기서는 간단히반환만하는 걸로 한다.

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
		// 중간에 바뀌는 경우가 있으니 먼저 동적할당이 되어 있다면 삭제부터 해준다.
		if (Locator::scene != &nullScene && Locator::scene != nullptr)
			delete scene;
		// nullptr이 들어왔을 경우에는 nullScene으로 초기화 시켜준다.
		if (scene == nullptr)
		{
			Locator::scene = &nullScene;
			return;
		}

		// 아닐 경우에는 현재 씬이 널 씬이 아닐때만 새로 만들어준다.

		
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
		// 가장 처리 방식이 다른 경우는 Debug모드일때 이다. 먼저처리해준다.
		if (type == SystemType::DEBUGTYPE)
		{
			// 현재 가지고 있는 Scene이 NullScene이거나 nullptr이면 바로적용시켜준다.
			if (Locator::scene == &nullScene || Locator::scene == nullptr)
			{
				isNull = true;
				scene = new LoggedScene(nullScene);
				return;
			}

			scene = new LoggedScene(*scene);
			return;
		}

		// 새롭게 할당해야 되기 때문에 널이 아닌 것들이 들어가 있으면 삭제한다.
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