#ifndef DEVICEWRAPPER_H
#define DEVICEWRAPPER_H
#include <d3d9.h>
#include <iostream>

using namespace std;


class DeviceWrapper
{
private :
	IDirect3DDevice9 * device;
protected :
	DeviceWrapper(IDirect3DDevice9 * device)
		: device(device) {}
	//DeviceWrapper(DeviceWrapper & deviceWrapper)
	//	: device(deviceWrapper.device) {}

	virtual ~DeviceWrapper() {}
public :
	friend class Locator;
	// 모두 virtual함수
};

class NullDeviceWrapper : public DeviceWrapper
{
private:

	NullDeviceWrapper()
		: DeviceWrapper(nullptr) {}
public:
	friend class Locator;
	// 모두 virtual함수 함수바디 X
	
};
class DebuggingDeviceWrapper : public DeviceWrapper
{
private:
	DeviceWrapper & wrapped;
	DebuggingDeviceWrapper(DeviceWrapper & deviceWrapper)
		: DeviceWrapper(nullptr), wrapped(deviceWrapper) {}
	// 래핑된 객체가 있으니 굳이 디버깅 클래스의 Device를 초기화 시켜주지 않기로 했다.

	void log(const char * message)
	{
		if (!message) return;
		cout << message << endl;
	}

public:
	friend class Locator;
	// 모두 virtual함수 래핑해서 앞에 로그만 찍어준다.

};

#endif