#include "Locator.h"

#include "Scene.h"
#include "Audio.h"
#include "Physics.h"

/*
기본적인 static변수 초기화
*/
NullScene Locator::nullScene{};
Scene * Locator::scene = &nullScene;

NullAudio Locator::nullAudio{};
Audio * Locator::audio = &nullAudio;

NullPhysics Locator::nullPhysics{};
Physics * Locator::physics = &nullPhysics;

//NullDeviceWrapper Locator::nullDeviceWrapper{};
//DeviceWrapper * Locator::deviceWrapper = &nullDeviceWrapper;
IDirect3DDevice9 * Locator::device = nullptr;

void Locator::provideAudio(SystemType type)
{
	if (type == SystemType::DEBUGTYPE)
	{
		if (audio == &nullAudio || audio == nullptr)
		{
			audio = new DebuggingAudio(nullAudio);
			return;
		}

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

void Locator::provideScene(SystemType type)
{
	// 가장 처리 방식이 다른 경우는 Debug모드일때 이다. 먼저처리해준다.
	if (type == SystemType::DEBUGTYPE)
	{
		// 현재 가지고 있는 Scene이 NullScene이거나 nullptr이면 바로적용시켜준다.
		if (scene == &nullScene || scene == nullptr)
		{
			scene = new DebuggingScene(nullScene);
			return;
		}

		scene = new DebuggingScene(*scene);
		return;
	}

	// 새롭게 할당해야 되기 때문에 널이 아닌 것들이 들어가 있으면 삭제한다.
	if (scene != &nullScene && scene != nullptr)
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
void Locator::provideDevice(IDirect3DDevice9 * device)
{
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
}

void Locator::providePhysics(SystemType type)
{
	// 가장 처리 방식이 다른 경우는 Debug모드일때 이다. 먼저처리해준다.
	if (type == SystemType::DEBUGTYPE)
	{
		if (physics == &nullPhysics || physics == nullptr)
		{
			physics = new DebuggingPhysics(nullPhysics);
			return;
		}

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

void Locator::release()
{
	// device를 제외한 나머지 시스템은 직접 만든 시스템이므로 여기서 릴리즈 시켜줘야한다.
	if (!audio && audio != &nullAudio)
		delete audio;

	if (!scene && scene != &nullScene)
		delete scene;

	if (!physics && physics != &nullPhysics)
		delete physics;

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

