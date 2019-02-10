#ifndef UTILITY_H
#define UTILITY_H

#include <iostream>
#include <string>
#include <conio.h>
#include <map>
#include <cmath>
#include <d3dx9.h>

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
	
	const float getX() const { return x; }
	const float getY() const { return y; }
	const float getZ() const { return z; }

	void setX(float x) { this->x = x; }
	void setY(float y) { this->y = y; }
	void setZ(float z) { this->z = z; }

	void setVector3(const Vector3 & other)
	{
		*this = other;
	}

	// �������� ����ȭ�� ���縦 ���ϱ� ���ؼ� ���۷��� ������ �޾Ƽ� ���ο��� �ʱ�ȭ
	// ���������� ����Ǵ� ���� �򰥸� �� �־ �����ͷ� �Ϸ�������, ���� ������ ����(������ �˻� ���Ϸ���)
	// ���۷��� ������ ����
	static void Normalized(Vector3 & output, const Vector3 & input)
	{
		//if (!output || !input) return;
		float norm = sqrtf(powf(input.x,2) + powf(input.y,2) + powf(input.z,2));
		output.x = input.x /norm;
		output.y = input.y / norm;
		output.z = input.z / norm;
	}

	static void Cross(Vector3 &output, const Vector3 & input1, const Vector3 &input2)
	{
		//if (!output || !input1 || !input2) return;
		output.x = input1.y *input2.z - input1.z *input2.y;
		output.y = input1.z *input2.x - input1.x *input2.z;
		output.z = input1.x *input2.y - input1.y *input2.x;
	}

	static float Dot(const Vector3 & input1, const Vector3 &input2)
	{
		//if (!input1 || !input2) return std::numeric_limits<float>::infinity();
		return input1.x *input2.x + input1.y *input2.y + input1.z *input2.z;
	}

	static void ToD3DXVECTOR3(D3DXVECTOR3 & output, const Vector3 & input)
	{
		output.x = input.x;
		output.y = input.y;
		output.z = input.z;
	}

	static void ToVector3(Vector3 & output, const D3DXVECTOR3 & input)
	{
		output.x = input.x;
		output.y = input.y;
		output.z = input.z;
	}

	Vector3 & operator=(const Vector3 & other)
	{
		x = other.x;
		y = other.y;
		z = other.z;
		return *this;
	}

	friend Vector3 operator+(const Vector3 & other1, const Vector3 & other2)
	{
		return{ other1.x + other2.x, other1.y + other2.y, other1.z + other2.z };
	}
	friend Vector3 operator-(const Vector3 & other1, const Vector3 & other2)
	{
		return{ other1.x - other2.x, other1.y - other2.y, other1.z - other2.z };
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

	friend Vector3 operator*(const Vector3 & other, int num)
	{
		return{ other.x*num, other.y*num, other.z*num };
	}

	friend Vector3 operator*(int num, const Vector3 & other)
	{
		return{ other.x*num, other.y*num, other.z*num };
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

