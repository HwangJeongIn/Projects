#include "Locator.h"

#include "Scene.h"
#include "Audio.h"
#include "Physics.h"
#include "FbxParser.h"

/*
기본적인 static변수 초기화
*/
NullScene Locator::nullScene{};
Scene * Locator::scene = &nullScene;
Scene Locator::mainScene("MainScene");
Scene Locator::startScene("StartScene");
Scene Locator::endScene("EndScene");

NullAudio Locator::nullAudio{};
Audio * Locator::audio = &nullAudio;

NullPhysics Locator::nullPhysics{};
Physics * Locator::physics = &nullPhysics;

NullFbxParser Locator::nullFbxParser{};
FbxParser * Locator::fbxParser = &nullFbxParser;

//NullDeviceWrapper Locator::nullDeviceWrapper{};
//DeviceWrapper * Locator::deviceWrapper = &nullDeviceWrapper;
IDirect3DDevice9 * Locator::device = nullptr;

void Locator::provideAudio(SystemType type)
{
	if (type == SystemType::DEBUGTYPE)
	{

		// 여기서 현재 디버깅 모드로 동적하고 있다면 그냥 리턴한다. // 여러번 래핑되는 것을 막기 위해서
		DebuggingAudio * tAudio = dynamic_cast<DebuggingAudio *> (audio);
		if (tAudio)
			return;

		if (audio == &nullAudio || audio == nullptr)
		{
			audio = new DebuggingAudio(nullAudio);
			return;
		}

		// 만약에 동적할당한 것을 다시 래핑했을때 해제를 2번에해주어야한다. // 릴리즈에서 처리해준다.
		audio = new DebuggingAudio(*audio);
		return;
	}

	// 새롭게 할당해야 되기 때문에 널이 아닌 것들이 들어가 있으면 삭제한다.
	if (audio != &nullAudio && audio != nullptr)
		delete audio;

	switch (type)
	{
	case SystemType::RELEASETYPE:
		audio = new Audio();
		break;

	case SystemType::NULLTYPE: default:
		audio = &nullAudio;
		break;
	}
}

void Locator::provideScene(SystemType systemType, SceneType sceneType)
{
	// 새롭게 할당해야 디버깅모드일 경우 새롭게 할당될 가능성이 있기 때문에
	// 널과 세팅해둔 씬이 아닌 것들이 들어가 있거나 래핑되어 있는 경우에 삭제해준다
	if (scene != &nullScene && scene != nullptr && scene != &startScene && scene != &endScene &&  scene != &mainScene)
		delete scene;

	// 씬타입으로 설정해준다.
	chagneScene(sceneType);

	

	// 가장 처리 방식이 다른 경우는 Debug모드일때 이다. 먼저처리해준다.
	if (systemType == SystemType::DEBUGTYPE)
	{

		// 여기서 현재 디버깅 모드로 동적하고 있다면 그냥 리턴한다. // 여러번 래핑되는 것을 막기 위해서
		DebuggingScene * tScene = dynamic_cast<DebuggingScene *> (scene);
		if (tScene)
			return;

		// 현재 가지고 있는 Scene이 NullScene이거나 nullptr이면 바로적용시켜준다.
		if (scene == nullptr)
		{
			scene = new DebuggingScene(nullScene);
			return;
		}

		scene = new DebuggingScene(*scene);
		return;
	}


	switch (systemType)
	{
	case SystemType::RELEASETYPE:
		// 위에서 씬타입으로 설정해주었기 때문에 아무것도 해주지않는다.
		break;

	case SystemType::NULLTYPE: default:
		// 위에서 씬타입으로 설정해주었지만 systemType가 nullType이면 널씬을 갖게된다.
		scene = &nullScene;
		break;
	}
}
void Locator::chagneScene(SceneType type)
{
	switch (type)
	{
	case SceneType::MAIN:
		scene = &mainScene;
		break;

	case SceneType::START:
		scene = &startScene;
		break;

	case SceneType::END:
		scene = &endScene;
		break;

	default:
		scene = &nullScene;
		break;

	}


}
void Locator::provideDevice(IDirect3DDevice9 * device)
{

	// 내부에서 Device생성을 제한할 수 없다 Device클래스 자체는 외부 클래스이기 때문이다.
	// 즉 외부에서 받은 객체를 참조해서 사용할 수 밖에 없다.
	if (Locator::device == nullptr)
	{
		// 현재 등록되어 있는 객체가 없으면 바로 등록해준다.
		Locator::device = device;
		return;
	}

	// 현재 등록되어 있는 객체가 있다 먼저 지워준다.
	Locator::device->Release();
	Locator::device = device;

}

void Locator::providePhysics(SystemType type)
{
	// 가장 처리 방식이 다른 경우는 Debug모드일때 이다. 먼저처리해준다.
	if (type == SystemType::DEBUGTYPE)
	{

		// 여기서 현재 디버깅 모드로 동적하고 있다면 그냥 리턴한다. // 여러번 래핑되는 것을 막기 위해서
		DebuggingPhysics * tPhysics = dynamic_cast<DebuggingPhysics *> (physics);
		if (tPhysics)
			return;

		if (physics == &nullPhysics || physics == nullptr)
		{
			physics = new DebuggingPhysics(nullPhysics);
			return;
		}

		// 만약에 동적할당한 것을 다시 래핑했을때 해제를 2번에해주어야한다. // 릴리즈에서 처리해준다.
		physics = new DebuggingPhysics(*physics);
		return;
	}

	// 새롭게 할당해야 되기 때문에 널이 아닌 것들이 들어가 있으면 삭제한다.
	if (physics != &nullPhysics && physics != nullptr)
		delete physics;

	switch (type)
	{
	case SystemType::RELEASETYPE:
		physics = new Physics();
		break;

	case SystemType::NULLTYPE: default:
		physics = &nullPhysics;
		break;
	}
}

void Locator::provideFbxParser(SystemType type)
{
	// 가장 처리 방식이 다른 경우는 Debug모드일때 이다. 먼저처리해준다.
	if (type == SystemType::DEBUGTYPE)
	{

		// 여기서 현재 디버깅 모드로 동적하고 있다면 그냥 리턴한다. // 여러번 래핑되는 것을 막기 위해서
		DebuggingFbxParser * tFbxParser = dynamic_cast<DebuggingFbxParser *> (fbxParser);
		if (tFbxParser)
			return;

		if (fbxParser == &nullFbxParser || fbxParser == nullptr)
		{
			fbxParser = new DebuggingFbxParser(nullFbxParser);
			return;
		}

		// 만약에 동적할당한 것을 다시 래핑했을때 해제를 2번에해주어야한다. // 릴리즈에서 처리해준다.
		fbxParser = new DebuggingFbxParser(*fbxParser);
		return;
	}

	// 새롭게 할당해야 되기 때문에 널이 아닌 것들이 들어가 있으면 삭제한다.
	if (fbxParser != &nullFbxParser && fbxParser != nullptr)
		delete fbxParser;
	
	switch (type)
	{
	case SystemType::RELEASETYPE:
		fbxParser = new FbxParser();
		break;

	case SystemType::NULLTYPE: default:
		fbxParser = &nullFbxParser;
		break;
	}
}

void Locator::release()
{
	// device를 제외한 나머지 시스템은 직접 만든 시스템이므로 여기서 릴리즈 시켜줘야한다.
	if (!audio && audio != &nullAudio)
	{
		DebuggingAudio * tAudio = dynamic_cast<DebuggingAudio *>(audio);
		if (tAudio)
		{
			// 널포인터 널시스템은 아니지만 디버깅 시스템인경우
			// 만약 래핑되어있는 객체가 널시스템이아니고 동적할당된 것이라면 따로 삭제해준다.
			Audio * wrappedAudio = tAudio->getWrappedAudio();
			if (!wrappedAudio && wrappedAudio != &nullAudio)
				delete wrappedAudio;
		}

		// 디버깅시스템 널포인터 널시스템이 아닌 상황 + 디버깅스템이지만 동적할당된 wrapped을 처리한 상황
		delete audio;

	}

	// 씬의 경우 감싸고 있는 경우만 지워주면 된다. DebuggingScene 의 경우에만 동적할당이 일어난다.
	// 널과 세팅해둔 씬이 아닌 것들이 들어가 있거나 래핑되어 있는 경우에 삭제해준다
	if (scene != &nullScene && scene != nullptr&& scene != &mainScene && scene != &startScene && scene != &endScene)
		delete scene;



	if (!physics && physics != &nullPhysics)
	{
		DebuggingPhysics * tPhysics = dynamic_cast<DebuggingPhysics *>(physics);
		if (tPhysics)
		{
			// 널포인터 널시스템은 아니지만 디버깅 시스템인경우
			// 만약 래핑되어있는 객체가 널시스템이아니고 동적할당된 것이라면 따로 삭제해준다.
			Physics * wrappedPhysics = tPhysics->getWrappedPhysics();
			if (!wrappedPhysics && wrappedPhysics != &nullPhysics)
				delete wrappedPhysics;
		}

		// 디버깅시스템 널포인터 널시스템이 아닌 상황 + 디버깅스템이지만 동적할당된 wrapped을 처리한 상황
		delete physics;

	}

	if (!fbxParser && fbxParser != &nullFbxParser)
	{
		DebuggingFbxParser * tFbxParser = dynamic_cast<DebuggingFbxParser *>(fbxParser);
		if (tFbxParser)
		{
			// 널포인터 널시스템은 아니지만 디버깅 시스템인경우
			// 만약 래핑되어있는 객체가 널시스템이아니고 동적할당된 것이라면 따로 삭제해준다.
			FbxParser * wrappedFbxParser = tFbxParser->getWrappedFbxParser();
			if (!wrappedFbxParser && wrappedFbxParser != &nullFbxParser)
				delete wrappedFbxParser;
		}

		// 디버깅시스템 널포인터 널시스템이 아닌 상황 + 디버깅스템이지만 동적할당된 wrapped을 처리한 상황
		delete fbxParser;

	}

}













/*
void Locator::provideDeviceWrapper(IDirect3DDevice9 * device, SystemType type)
{
	bool isNull = false;
	// 가장 처리 방식이 다른 경우는 Debug모드일때 이다. 먼저처리해준다.
	if (type == SystemType::DEBUGTYPE)
	{
		// 현재 가지고 있는 Scene이 NullScene이거나 nullptr이면 바로적용시켜준다.
		if (deviceWrapper == &nullDeviceWrapper || deviceWrapper == nullptr)
		{
			isNull = true;
			deviceWrapper = new DebuggingDeviceWrapper(nullDeviceWrapper);
			return;
		}

		deviceWrapper = new DebuggingDeviceWrapper(*deviceWrapper);
		return;
	}

	// 새롭게 할당해야 되기 때문에 널이 아닌 것들이 들어가 있으면 삭제한다.
	if (!isNull)
		delete deviceWrapper;

	switch (type)
	{
	case SystemType::RELEASETYPE:
		deviceWrapper = new DeviceWrapper(device);
		break;

	case SystemType::NULLTYPE: default:
		deviceWrapper = &nullDeviceWrapper;
		break;
	}
}

*/

