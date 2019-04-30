#ifndef LOCATOR_H
#define LOCATOR_H

#include <cassert>
//#include "DeviceWrapper.h"
#include <d3d9.h>

/*
�����ΰ� �ƴ� ���⼭ ���������� ��Ŭ��� �ع����� 
�� �߰��ڸ� ����ϴ� ������ ���θ� �����ϴ� ���谡 ���� �� �ִ�.
������ SceneŬ������ GameObject�� �����Ѵ�. ���������� �������� ������ �ϰ� �ֱ� ������
�ٵ� GameObject���� Locator�� ��ӹ޾����� SceneŬ������ ��Ŭ��� �ϰ� ������ ���θ� �����ϴ� ���谡 �ȴ�.
*/


/*
�����ڸ� ��ӹ޾Ƽ� �����ý��ۿ��� get�� �� �ְ� ����

�Ϻ� �ý��ۿ����� �ٷ� ȣ�� �� �� �ְ� �������(Device)
�Ϻ� �ý��ۿ����� �ΰ�ü�� ����밴ü�� �����ؼ� �����뵵�� 
������� �ʾ����� �⺻�������� ����� �� �ְ� �������.
*/

class Scene;
class NullScene;

class Audio;
class NullAudio;

class Physics;
class NullPhysics;

class FbxParser;
class NullFbxParser;

class GameUI;
class GameObject;

class Locator
{
public:

	enum SystemType
	{
		DEBUGTYPE,
		RELEASETYPE,
		NULLTYPE
	};

	enum SceneType
	{
		START,
		END,
		MAIN
	};

private :
	static Scene * scene;
	static NullScene nullScene;
	static Scene mainScene;
	static Scene startScene;
	static Scene endScene;

	static void chagneScene(SceneType type = MAIN);
	/*
	����̽� �������� ������ ����� ��ü�� �ٷ� �޾� �� �� �ְ� ����
	����̽��� ������ ȣ�� �� �� �ִ� �Լ��� �ſ� ���� ������ 
	�� �Լ��� �� �����ϰ� �ΰ�ü ����밴ü�� �����ϱⰡ �ʹ� ��ȿ�����̶�� �Ǵ��ߴ�
	*/
	// static DeviceWrapper * deviceWrapper;
	// static NullDeviceWrapper nullDeviceWrapper;

	static IDirect3DDevice9 * device;

	static Audio * audio;
	static NullAudio nullAudio;

	static Physics * physics;
	static NullPhysics nullPhysics;

	static FbxParser * fbxParser;
	static NullFbxParser nullFbxParser;

	static GameUI * gameUI;

protected :
	static Scene& getScene(){ return *scene; }
	static Audio& getAudio(){ return *audio; }
	static Physics& getPhysics() { return *physics; }
	static FbxParser& getFbxParser() { return *fbxParser; }
	//static DeviceWrapper& getDeviceWrapper() {return *deviceWrapper}
	static IDirect3DDevice9& getDevice() 
	{
		// ���� ���������ε� �����͸� �������� �Ѵٸ� �ý����� �����Ѵ�.
		// ����̽��� ������� �ʾҴٸ� �ƹ��͵� �� �� ����.
		assert(Locator::device);
		return *device;
	}

	static GameUI& getGameUI(){	return *gameUI;}

public :


	//static void provideScene(SystemType type = SystemType::RELEASETYPE);
	static void provideScene(SystemType systemType = SystemType::RELEASETYPE, SceneType sceneType = SceneType::MAIN);


	static void provideAudio(SystemType type = SystemType::RELEASETYPE);
	//static void provideDeviceWrapper(IDirect3DDevice9 * device, SystemType type = SystemType::RELEASETYPE);
	static void provideDevice(IDirect3DDevice9 * device);
	static void providePhysics(SystemType type = SystemType::RELEASETYPE);
	static void provideFbxParser(SystemType type = SystemType::RELEASETYPE);

	static void provideGameUI(IDirect3DDevice9 ** device, GameObject * objToAccessSystem);


	static void release();

};


#endif