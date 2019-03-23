#ifndef UTILITY_H
#define UTILITY_H

#include <iostream>
#include <string>
#include <conio.h>
#include <map>
#include <cmath>
#include <d3dx9.h>
#include <fbxsdk.h>

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

	// 내적외적 정규화는 복사를 안하기 위해서 레퍼런스 변수로 받아서 내부에서 초기화
	// 내부적으로 변경되는 것이 헷갈릴 수 있어서 포인터로 하려했지만, 빠른 연산을 위해(포인터 검사 안하려고)
	// 레퍼런스 변수로 지정
	static void Normalized(Vector3 & output, const Vector3 & input)
	{
		//if (!output || !input) return;
		float norm = sqrtf(powf(input.x,2) + powf(input.y,2) + powf(input.z,2));
		if (abs(norm) == 0) return;
		output.x = input.x / norm;
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

	friend Vector3 operator*(const Vector3 & other, float num)
	{
		return{ other.x*num, other.y*num, other.z*num };
	}

	friend Vector3 operator*(float num, const Vector3 & other)
	{
		return{ other.x*num, other.y*num, other.z*num };
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
class FbxDXConverter
{
public :
	
	static void ToD3DXVECTOR2(D3DXVECTOR2 & output, const FbxVector2 & input)
	{
		output.x = (float)input.mData[0];
		output.y = (float)input.mData[1];
	}

	static void ToD3DXVECTOR2_UV(D3DXVECTOR2 & output, const FbxVector2 & input)
	{
		output.x = (float)input.mData[0];
		output.y = 1- (float)input.mData[1];
	}

	static void ToD3DXVECTOR3(D3DXVECTOR3 & output, const FbxVector4 & input)
	{
		output.x = (float)input.mData[0];
		output.y = (float)input.mData[1];
		output.z = (float)input.mData[2];
	}

	static void ToD3DXVECTOR3(D3DXVECTOR3 & output, const FbxDouble3 & input)
	{
		output.x = (float)input.mData[0];
		output.y = (float)input.mData[1];
		output.z = (float)input.mData[2];
	}

	static void ToD3DXMATRIX(D3DXMATRIX & output, const FbxMatrix & input)
	{
		FbxVector4 r1 = input.GetRow(0);
		FbxVector4 r2 = input.GetRow(1);
		FbxVector4 r3 = input.GetRow(2);
		FbxVector4 r4 = input.GetRow(3);

		output =
		{
			(float)r1.mData[0], (float)r1.mData[1], (float)r1.mData[2], (float)r1.mData[3],
			(float)r2.mData[0], (float)r2.mData[1], (float)r2.mData[2], (float)r2.mData[3],
			(float)r3.mData[0], (float)r3.mData[1], (float)r3.mData[2], (float)r3.mData[3],
			(float)r4.mData[0], (float)r4.mData[1], (float)r4.mData[2], (float)r4.mData[3]
		};
	}
	
	static void ToFbxVector2( FbxVector2 & output, const D3DXVECTOR2 & input)
	{
		output.mData[0] = (double)input.x;
		output.mData[1] = (double)input.y;
	}

	static void ToFbxVector2_UV(FbxVector2 & output, const  D3DXVECTOR2 & input)
	{
		output.mData[0] = (double)input.x;
		output.mData[1] = 1- (double)input.y;
	}


	static void ToFbxDouble3(FbxDouble3  & output, const D3DXVECTOR3 & input)
	{
		output.mData[0] = (double)input.x;
		output.mData[1] = (double)input.y;
		output.mData[2] = (double)input.z;
	}

	static void ToFbxMatrix( FbxMatrix & output, const D3DXMATRIX & input)
	{
		output =
		{
			(double)input._11, (double)input._12, (double)input._13, (double)input._14,
			(double)input._21, (double)input._22, (double)input._23, (double)input._24,
			(double)input._31, (double)input._32, (double)input._33, (double)input._34,
			(double)input._41, (double)input._42, (double)input._43, (double)input._44
		};
	}

};

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
	static long deltaTime;
public :
	static long getDeltaTime() { return deltaTime; }
	static void setDeltaTime(long other)
	{
		// 간격이기 때문에 음수가 없다.
		if (other < 0)
			other = 0;
		deltaTime = other;
	}

};

#endif

