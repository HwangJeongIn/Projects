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


	
	// static 변수
	static Vector3 Zero;
	static Vector3 One;
	static Vector3 Up;
	static Vector3 Right;
	static Vector3 Forward;

};

// 이런식으로하면 이 헤더파일을 포함하는 파일이 많기 때문에
// 여러번 정의된다. 한번만 정의되도록 cpp로 분리시켜줘야한다. // static 멤버변수
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
			// 여기안에 들어왔을경우는
			// 1. 키입력된상태인데 키코드비교했을때 다른경우
			// 2. 키입력된상태인데 키코드비교했을때 같은경우

			// 키보드는 2개의 키값으로 받아짐
			Major = _getch();
			Minor = _getch();

			//cout << "major" << Major << endl;
			//cout << "minor" << Minor << endl;

			// 그 두개의 키값을 비교해서 그키가 눌렸는지 확인한다.
			auto keyPair = KeyCodeTable[keyCode];
			return (keyPair == pair<int, int>{Major, Minor});

		}
		return false;
	}

};

// 프레임간 간격을 millisec단위로 나타낸 클래스
class FrameTime
{
private :
	static unsigned long deltaTime;
public :
	static unsigned long getDeltaTime() { return deltaTime; }
	static void setDeltaTime(unsigned long other)
	{
		// 간격이기 때문에 음수가 없다.
		if (other < 0)
			other = 0;
		deltaTime = other;
	}

};

#endif

