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
	// ��� virtual�Լ�
};

class NullDeviceWrapper : public DeviceWrapper
{
private:

	NullDeviceWrapper()
		: DeviceWrapper(nullptr) {}
public:
	friend class Locator;
	// ��� virtual�Լ� �Լ��ٵ� X
	
};
class DebuggingDeviceWrapper : public DeviceWrapper
{
private:
	DeviceWrapper & wrapped;
	DebuggingDeviceWrapper(DeviceWrapper & deviceWrapper)
		: DeviceWrapper(nullptr), wrapped(deviceWrapper) {}
	// ���ε� ��ü�� ������ ���� ����� Ŭ������ Device�� �ʱ�ȭ �������� �ʱ�� �ߴ�.

	void log(const char * message)
	{
		if (!message) return;
		cout << message << endl;
	}

public:
	friend class Locator;
	// ��� virtual�Լ� �����ؼ� �տ� �α׸� ����ش�.

};

#endif