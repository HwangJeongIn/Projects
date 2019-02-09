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

class Locator
{
private :
	static Scene * scene;
	static NullScene nullScene;
	
	/*
	����̽� �������� ������ ����� ��ü�� �ٷ� �޾� �� �� �ְ� ����
	����̽��� ������ ȣ�� �� �� �ִ� �Լ��� �ſ� ���� ������ 
	�� �Լ��� �� �����ϰ� �ΰ�ü ����밴ü�� �����ϱⰡ �ʹ� ��ȿ�����̶�� �Ǵ��ߴ�
	*/
	//static DeviceWrapper * deviceWrapper;
	//static NullDeviceWrapper nullDeviceWrapper;
	static IDirect3DDevice9 * device;


	static Audio * audio;
	// static NullAudio nullAudio;

protected :
	static Scene& getScene(){ return *scene; }
	static Audio& getAudio(){ return *audio; }
	//static DeviceWrapper& getDeviceWrapper() {return *deviceWrapper}
	static IDirect3DDevice9& getDevice() 
	{
		// ���� ���������ε� �����͸� �������� �Ѵٸ� �ý����� �����Ѵ�.
		assert(Locator::device);

		return *device;
	}

public :	
	enum SystemType
	{
		DEBUGTYPE,
		RELEASETYPE,
		NULLTYPE
	};

	static void provideScene(SystemType type = SystemType::RELEASETYPE);
	//static void provideDeviceWrapper(IDirect3DDevice9 * device, SystemType type = SystemType::RELEASETYPE);
	static void provideDevice(IDirect3DDevice9 * device);



};


//NullScene Locator::nullScene{};
//Scene * Locator::scene = nullScene;
//NullAudio Locator::nullAudio{};
//Audio * Locator::audio = nullAdio;

#endif