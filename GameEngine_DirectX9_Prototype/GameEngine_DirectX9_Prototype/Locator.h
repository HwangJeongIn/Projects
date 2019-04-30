#ifndef LOCATOR_H
#define LOCATOR_H

#include <cassert>
//#include "DeviceWrapper.h"
#include <d3d9.h>

/*
구현부가 아닌 여기서 여러가지를 인클루드 해버리면 
이 중개자를 사용하는 곳에서 서로를 포함하는 관계가 생길 수 있다.
예를들어서 Scene클래스는 GameObject를 포함한다. 내부적으로 여러가지 구현을 하고 있기 때문에
근데 GameObject에서 Locator를 상속받았을때 Scene클래스를 인클루드 하고 있으면 서로를 포함하는 관계가 된다.
*/


/*
중재자를 상속받아서 여러시스템에서 get할 수 있게 제한

일부 시스템에서는 바로 호출 할 수 있게 만들었고(Device)
일부 시스템에서는 널객체와 디버깅객체를 선언해서 디버깅용도와 
등록하지 않았을때 기본동작으로 사용할 수 있게 만들었다.
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
	디바이스 같은경우는 간단히 등록한 객체를 바로 받아 올 수 있게 설계
	디바이스를 가지고 호출 할 수 있는 함수가 매우 많기 때문에 
	그 함수를 다 래핑하고 널객체 디버깅객체를 정의하기가 너무 비효율적이라고 판단했다
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
		// 만약 널포인터인데 포인터를 얻으려고 한다면 시스템을 종료한다.
		// 디바이스를 등록하지 않았다면 아무것도 할 수 없다.
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