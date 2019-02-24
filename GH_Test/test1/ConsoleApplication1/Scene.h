#pragma once
#include <iostream>
#include <cassert>

using namespace std;


class Scene
{
private :
	static bool instantiated;
protected :
	Scene(){}
	~Scene(){}
public :
	friend class Locator;
	/*
	Scene() 
	{ 
		// �̹� ������ ����� ������ ��ȯ�ϰ� �ǰ� ���α׷��� �װ� �ȴ�.
		assert(!instantiated);

		instantiated = true;
	}
	~Scene() { instantiated = false; }
	*/


	virtual void func2() { cout << "hello2" << endl; }

};

class NullScene : public Scene
{
private :
	NullScene() {}
	~NullScene() {}
	//static bool instantiated;
public :
	friend class Locator;
	/*
	NullScene()
	{
	// �̹� ������ ����� ������ ��ȯ�ϰ� �ǰ� ���α׷��� �װ� �ȴ�.
	assert(!instantiated);

	instantiated = true;
	}
	~NullScene() { instantiated = false; }
	*/

	virtual void func2() { cout << "hello2_null" << endl; }
};

class LoggedScene : public Scene
{
private:
	LoggedScene(Scene & wrapped)
		: wrapped(wrapped) {}
	~LoggedScene() {}

	//static bool instantiated;
	Scene & wrapped;
	void log(const char * message)
	{
		if (!message) return;
		cout << message << endl;
	}
public:
	friend class Locator;
	/*
	LoggedScene(Scene & wrapped)
	: wrapped(wrapped)
	{
	// �̹� ������ ����� ������ ��ȯ�ϰ� �ǰ� ���α׷��� �װ� �ȴ�.
	assert(!instantiated);

	instantiated = true;
	}
	~LoggedScene() { instantiated = false; }
	*/


	virtual void func2() { log( "hello2_logged" ); wrapped.func2(); }
};