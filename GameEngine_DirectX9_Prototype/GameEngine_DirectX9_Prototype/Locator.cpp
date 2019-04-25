#include "Locator.h"

#include "Scene.h"
#include "Audio.h"
#include "Physics.h"
#include "FbxParser.h"

/*
�⺻���� static���� �ʱ�ȭ
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

		// ���⼭ ���� ����� ���� �����ϰ� �ִٸ� �׳� �����Ѵ�. // ������ ���εǴ� ���� ���� ���ؼ�
		DebuggingAudio * tAudio = dynamic_cast<DebuggingAudio *> (audio);
		if (tAudio)
			return;

		if (audio == &nullAudio || audio == nullptr)
		{
			audio = new DebuggingAudio(nullAudio);
			return;
		}

		// ���࿡ �����Ҵ��� ���� �ٽ� ���������� ������ 2�������־���Ѵ�. // ������� ó�����ش�.
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

void Locator::provideScene(SystemType systemType, SceneType sceneType)
{
	// ���Ӱ� �Ҵ��ؾ� ��������� ��� ���Ӱ� �Ҵ�� ���ɼ��� �ֱ� ������
	// �ΰ� �����ص� ���� �ƴ� �͵��� �� �ְų� ���εǾ� �ִ� ��쿡 �������ش�
	if (scene != &nullScene && scene != nullptr && scene != &startScene && scene != &endScene &&  scene != &mainScene)
		delete scene;

	// ��Ÿ������ �������ش�.
	chagneScene(sceneType);

	

	// ���� ó�� ����� �ٸ� ���� Debug����϶� �̴�. ����ó�����ش�.
	if (systemType == SystemType::DEBUGTYPE)
	{

		// ���⼭ ���� ����� ���� �����ϰ� �ִٸ� �׳� �����Ѵ�. // ������ ���εǴ� ���� ���� ���ؼ�
		DebuggingScene * tScene = dynamic_cast<DebuggingScene *> (scene);
		if (tScene)
			return;

		// ���� ������ �ִ� Scene�� NullScene�̰ų� nullptr�̸� �ٷ���������ش�.
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
		// ������ ��Ÿ������ �������־��� ������ �ƹ��͵� �������ʴ´�.
		break;

	case SystemType::NULLTYPE: default:
		// ������ ��Ÿ������ �������־����� systemType�� nullType�̸� �ξ��� ���Եȴ�.
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

void Locator::providePhysics(SystemType type)
{
	// ���� ó�� ����� �ٸ� ���� Debug����϶� �̴�. ����ó�����ش�.
	if (type == SystemType::DEBUGTYPE)
	{

		// ���⼭ ���� ����� ���� �����ϰ� �ִٸ� �׳� �����Ѵ�. // ������ ���εǴ� ���� ���� ���ؼ�
		DebuggingPhysics * tPhysics = dynamic_cast<DebuggingPhysics *> (physics);
		if (tPhysics)
			return;

		if (physics == &nullPhysics || physics == nullptr)
		{
			physics = new DebuggingPhysics(nullPhysics);
			return;
		}

		// ���࿡ �����Ҵ��� ���� �ٽ� ���������� ������ 2�������־���Ѵ�. // ������� ó�����ش�.
		physics = new DebuggingPhysics(*physics);
		return;
	}

	// ���Ӱ� �Ҵ��ؾ� �Ǳ� ������ ���� �ƴ� �͵��� �� ������ �����Ѵ�.
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
	// ���� ó�� ����� �ٸ� ���� Debug����϶� �̴�. ����ó�����ش�.
	if (type == SystemType::DEBUGTYPE)
	{

		// ���⼭ ���� ����� ���� �����ϰ� �ִٸ� �׳� �����Ѵ�. // ������ ���εǴ� ���� ���� ���ؼ�
		DebuggingFbxParser * tFbxParser = dynamic_cast<DebuggingFbxParser *> (fbxParser);
		if (tFbxParser)
			return;

		if (fbxParser == &nullFbxParser || fbxParser == nullptr)
		{
			fbxParser = new DebuggingFbxParser(nullFbxParser);
			return;
		}

		// ���࿡ �����Ҵ��� ���� �ٽ� ���������� ������ 2�������־���Ѵ�. // ������� ó�����ش�.
		fbxParser = new DebuggingFbxParser(*fbxParser);
		return;
	}

	// ���Ӱ� �Ҵ��ؾ� �Ǳ� ������ ���� �ƴ� �͵��� �� ������ �����Ѵ�.
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
	// device�� ������ ������ �ý����� ���� ���� �ý����̹Ƿ� ���⼭ ������ ��������Ѵ�.
	if (!audio && audio != &nullAudio)
	{
		DebuggingAudio * tAudio = dynamic_cast<DebuggingAudio *>(audio);
		if (tAudio)
		{
			// �������� �νý����� �ƴ����� ����� �ý����ΰ��
			// ���� ���εǾ��ִ� ��ü�� �νý����̾ƴϰ� �����Ҵ�� ���̶�� ���� �������ش�.
			Audio * wrappedAudio = tAudio->getWrappedAudio();
			if (!wrappedAudio && wrappedAudio != &nullAudio)
				delete wrappedAudio;
		}

		// �����ý��� �������� �νý����� �ƴ� ��Ȳ + ����뽺�������� �����Ҵ�� wrapped�� ó���� ��Ȳ
		delete audio;

	}

	// ���� ��� ���ΰ� �ִ� ��츸 �����ָ� �ȴ�. DebuggingScene �� ��쿡�� �����Ҵ��� �Ͼ��.
	// �ΰ� �����ص� ���� �ƴ� �͵��� �� �ְų� ���εǾ� �ִ� ��쿡 �������ش�
	if (scene != &nullScene && scene != nullptr&& scene != &mainScene && scene != &startScene && scene != &endScene)
		delete scene;



	if (!physics && physics != &nullPhysics)
	{
		DebuggingPhysics * tPhysics = dynamic_cast<DebuggingPhysics *>(physics);
		if (tPhysics)
		{
			// �������� �νý����� �ƴ����� ����� �ý����ΰ��
			// ���� ���εǾ��ִ� ��ü�� �νý����̾ƴϰ� �����Ҵ�� ���̶�� ���� �������ش�.
			Physics * wrappedPhysics = tPhysics->getWrappedPhysics();
			if (!wrappedPhysics && wrappedPhysics != &nullPhysics)
				delete wrappedPhysics;
		}

		// �����ý��� �������� �νý����� �ƴ� ��Ȳ + ����뽺�������� �����Ҵ�� wrapped�� ó���� ��Ȳ
		delete physics;

	}

	if (!fbxParser && fbxParser != &nullFbxParser)
	{
		DebuggingFbxParser * tFbxParser = dynamic_cast<DebuggingFbxParser *>(fbxParser);
		if (tFbxParser)
		{
			// �������� �νý����� �ƴ����� ����� �ý����ΰ��
			// ���� ���εǾ��ִ� ��ü�� �νý����̾ƴϰ� �����Ҵ�� ���̶�� ���� �������ش�.
			FbxParser * wrappedFbxParser = tFbxParser->getWrappedFbxParser();
			if (!wrappedFbxParser && wrappedFbxParser != &nullFbxParser)
				delete wrappedFbxParser;
		}

		// �����ý��� �������� �νý����� �ƴ� ��Ȳ + ����뽺�������� �����Ҵ�� wrapped�� ó���� ��Ȳ
		delete fbxParser;

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

