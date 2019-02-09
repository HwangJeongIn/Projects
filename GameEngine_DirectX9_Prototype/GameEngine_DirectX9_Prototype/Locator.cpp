#include "Locator.h"

#include "Scene.h"
#include "Audio.h"

/*
기본적인 static변수 초기화
*/
NullScene Locator::nullScene{};
Scene * Locator::scene = &nullScene;

//NullDeviceWrapper Locator::nullDeviceWrapper{};
//DeviceWrapper * Locator::deviceWrapper = &nullDeviceWrapper;
IDirect3DDevice9 * Locator::device = nullptr;

void Locator::provideScene(SystemType type)
{
	bool isNull = false;
	// 가장 처리 방식이 다른 경우는 Debug모드일때 이다. 먼저처리해준다.
	if (type == SystemType::DEBUGTYPE)
	{
		// 현재 가지고 있는 Scene이 NullScene이거나 nullptr이면 바로적용시켜준다.
		if (scene == &nullScene || scene == nullptr)
		{
			isNull = true;
			scene = new DebuggingScene(nullScene);
			return;
		}

		scene = new DebuggingScene(*scene);
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

