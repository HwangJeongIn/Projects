#include "Locator.h"

#include "Scene.h"
#include "Audio.h"

/*
�⺻���� static���� �ʱ�ȭ
*/
NullScene Locator::nullScene{};
Scene * Locator::scene = &nullScene;

NullAudio Locator::nullAudio{};
Audio * Locator::audio = &nullAudio;

//NullDeviceWrapper Locator::nullDeviceWrapper{};
//DeviceWrapper * Locator::deviceWrapper = &nullDeviceWrapper;
IDirect3DDevice9 * Locator::device = nullptr;

void Locator::provideAudio(SystemType type)
{
	// ���� ó�� ����� �ٸ� ���� Debug����϶� �̴�. ����ó�����ش�.
	if (type == SystemType::DEBUGTYPE)
	{
		// ���� ������ �ִ� Scene�� NullScene�̰ų� nullptr�̸� �ٷ���������ش�.
		// ����ü�� ���ڷ��̼��ϴ� ����� Ŭ���� ��ü�� �����.
		if (audio == &nullAudio || audio == nullptr)
		{
			audio = new DebuggingAudio(nullAudio);
			return;
		}

		audio = new DebuggingAudio(*audio);
		return;
	}

	// ���Ӱ� �Ҵ��ؾ� �Ǳ� ������ ���� �ƴ� �͵��� �� ������ �����Ѵ�.
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
	// ���� ó�� ����� �ٸ� ���� Debug����϶� �̴�. ����ó�����ش�.
	if (type == SystemType::DEBUGTYPE)
	{
		// ���� ������ �ִ� Scene�� NullScene�̰ų� nullptr�̸� �ٷ���������ش�.
		if (scene == &nullScene || scene == nullptr)
		{
			scene = new DebuggingScene(nullScene);
			return;
		}

		scene = new DebuggingScene(*scene);
		return;
	}

	// ���Ӱ� �Ҵ��ؾ� �Ǳ� ������ ���� �ƴ� �͵��� �� ������ �����Ѵ�.
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
		// ���ο��� Device������ ������ �� ���� DeviceŬ���� ��ü�� �ܺ� Ŭ�����̱� �����̴�.
		// �� �ܺο��� ���� ��ü�� �����ؼ� ����� �� �ۿ� ����.
		if (Locator::device == nullptr)
		{
			// ���� ��ϵǾ� �ִ� ��ü�� ������ �ٷ� ������ش�.
			Locator::device = device;
			return;
		}

		// ���� ��ϵǾ� �ִ� ��ü�� �ִ� ���� �����ش�.
		Locator::device->Release();
		Locator::device = device;
	}
}













/*
void Locator::provideDeviceWrapper(IDirect3DDevice9 * device, SystemType type)
{
	bool isNull = false;
	// ���� ó�� ����� �ٸ� ���� Debug����϶� �̴�. ����ó�����ش�.
	if (type == SystemType::DEBUGTYPE)
	{
		// ���� ������ �ִ� Scene�� NullScene�̰ų� nullptr�̸� �ٷ���������ش�.
		if (deviceWrapper == &nullDeviceWrapper || deviceWrapper == nullptr)
		{
			isNull = true;
			deviceWrapper = new DebuggingDeviceWrapper(nullDeviceWrapper);
			return;
		}

		deviceWrapper = new DebuggingDeviceWrapper(*deviceWrapper);
		return;
	}

	// ���Ӱ� �Ҵ��ؾ� �Ǳ� ������ ���� �ƴ� �͵��� �� ������ �����Ѵ�.
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

