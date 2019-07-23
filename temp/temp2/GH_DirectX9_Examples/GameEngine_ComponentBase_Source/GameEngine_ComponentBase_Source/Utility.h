#ifndef UTILITY_H
#define UTILITY_H

#include <iostream>
#include <string>

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


#endif

