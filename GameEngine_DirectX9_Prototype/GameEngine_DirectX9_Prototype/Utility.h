#ifndef UTILITY_H
#define UTILITY_H

#include <iostream>
#include <string>
#include <conio.h>
#include <map>

using namespace std;


class Vector3
{
private:
	float x;
	float y;
	float z;
public:
	Vector3(float x = 0, float y = 0, float z = 0)
		: x(x), y(y), z(z) {}

	Vector3(const Vector3& other)
		: x(other.x), y(other.y), z(other.z) {}

	// read only
	const Vector3& getVector3() const
	{
		return *this;
	}

	void setVector3(const Vector3 & other)
	{
		*this = other;
	}

	Vector3 operator=(const Vector3 & other)
	{
		x = other.x;
		y = other.y;
		z = other.z;
	}

	Vector3 operator+(const Vector3 & other)
	{
		return Vector3(x + other.x, y + other.y, z + other.z);
	}
	Vector3 operator-(const Vector3 & other)
	{
		return Vector3(x - other.x, y - other.y, z - other.z);
	}

	Vector3 & operator+=(const Vector3 & other)
	{
		this->x += other.x;
		this->y += other.y;
		this->z += other.z;
		return *this;
	}

	Vector3 & operator-=(const Vector3 & other)
	{
		this->x -= other.x;
		this->y -= other.y;
		this->z -= other.z;
		return *this;
	}


	
	// static ����
	static Vector3 Zero;
	static Vector3 One;
	static Vector3 Up;
	static Vector3 Right;
	static Vector3 Forward;

};

// �̷��������ϸ� �� ��������� �����ϴ� ������ ���� ������
// ������ ���ǵȴ�. �ѹ��� ���ǵǵ��� cpp�� �и���������Ѵ�. // static �������
/*
Vector3 Vector3::Zero{ 0,0,0 };
Vector3 Vector3::One{ 1,1,1 };
Vector3 Vector3::Up{ 0,1,0 };
Vector3 Vector3::Right{ 0,0,1 };
Vector3 Vector3::Forward{ 1,0,0 };
*/
enum class KeyCode
{
	None,
	Space,

	W,
	A,
	S,
	D,

	Esc,

	UpArrow,
	DownArrow,
	RightArrow,
	LeftArrow
};

class InputManager
{
private:


	static map<KeyCode, pair<int, int>> KeyCodeTable;

	static int Major;
	static int Minor;

public:
	static bool GetKeyDown(KeyCode keyCode)
	{
		Major = -1;
		Minor = -1;

		if (_kbhit())
		{
			// ����ȿ� ����������
			// 1. Ű�ԷµȻ����ε� Ű�ڵ�������� �ٸ����
			// 2. Ű�ԷµȻ����ε� Ű�ڵ�������� �������

			// Ű����� 2���� Ű������ �޾���
			Major = _getch();
			Minor = _getch();

			//cout << "major" << Major << endl;
			//cout << "minor" << Minor << endl;

			// �� �ΰ��� Ű���� ���ؼ� ��Ű�� ���ȴ��� Ȯ���Ѵ�.
			auto keyPair = KeyCodeTable[keyCode];
			return (keyPair == pair<int, int>{Major, Minor});

		}
		return false;
	}

};

// �����Ӱ� ������ millisec������ ��Ÿ�� Ŭ����
class FrameTime
{
private :
	static unsigned long deltaTime;
public :
	static unsigned long getDeltaTime() { return deltaTime; }
	static void setDeltaTime(unsigned long other)
	{
		// �����̱� ������ ������ ����.
		if (other < 0)
			other = 0;
		deltaTime = other;
	}

};

#endif

