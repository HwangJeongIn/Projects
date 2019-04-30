#ifndef UTILITY_H
#define UTILITY_H

#include <iostream>
#include <string>
#include <conio.h>
#include <map>
#include <vector>
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

	Vector3 & operator=(const Vector3 & other)
	{
		x = other.x;
		y = other.y;
		z = other.z;
		return *this;
	}

	bool operator==(const Vector3 & other) const
	{
		return (x == other.x && y == other.y && z == other.z);
	}

	// 뒤에 const가 붙으면 // static Vector3 operator+(const Vector3 & other1 , const Vector3 & other2)
	// 안붙으면 // static Vector3 operator+(Vector3 & other1 , const Vector3 & other2)
	Vector3 operator+(const Vector3 & other) const
	{
		return{ x + other.x, y + other.y, z + other.z };
	}
	Vector3 operator-(const Vector3 & other) const
	{
		return{ x - other.x, y - other.y, z - other.z };
	}

	//friend bool operator==(const Vector3 & other1, const Vector3 & other2)
	//{
	//	return (other1.x == other2.x && other1.y == other2.y && other1.z == other2.z);
	//}
	//friend Vector3 operator+(const Vector3 & other1, const Vector3 & other2)
	//{
	//	return{ other1.x + other2.x, other1.y + other2.y, other1.z + other2.z };
	//}
	//friend Vector3 operator-(const Vector3 & other1, const Vector3 & other2)
	//{
	//	return{ other1.x - other2.x, other1.y - other2.y, other1.z - other2.z };
	//}

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


	static float Distance(const Vector3 & other1, const Vector3 & other2)
	{
		return sqrtf(powf(other1.x-other2.x, 2) + powf(other1.y - other2.y, 2) + powf(other1.z  -  other2.z, 2));
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

class Gizmos
{
private :
	static IDirect3DDevice9 * Device;
	static ID3DXLine * Line;
	static bool Active;
public:


	static void InitGizmos(IDirect3DDevice9 * device)
	{
		Gizmos::Device = device;

		// Line 생성
		D3DXCreateLine(Device, &Line);
		Line->SetWidth(2);
	}

	static void TrunOnGizmos() { Active = true; }
	static void TrunOffGizmos() { Active = false; }

	//struct LineVertex
	//{
	//	D3DXVECTOR3 position;
	//	/*float x, y, z;
	//	D3DCOLOR color;*/
	//	
	//	//LineVertex(float x=0, float y=0, float z=0, D3DCOLOR color= 0xffff)
	//	//	: x(x), y(y), z(z), color(color) {}
	//	//void setInfo(float x, float y, float z, D3DCOLOR color = 0xffff)
	//	//{
	//	//	this->x = x; this->y = y; this->z = z; this->color = color;
	//	//}

	//};

	static const DWORD LineFVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;

	static void DrawLine(const D3DXVECTOR3 & from, const D3DXVECTOR3 & to, D3DCOLOR color = 0xffff)
	{
		if (!Active) return;
		if (!Device || !Line) return;

		D3DXVECTOR3 lineVertices[2] = { from, to };//{ {0,0,0}, {10,10,10} };//{ from, to };
		D3DXMATRIX view;
		D3DXMATRIX proj;

		//D3DMATRIX finalMatrix = view * proj;

		Device->GetTransform(D3DTS_VIEW, &view);
		Device->GetTransform(D3DTS_PROJECTION, &proj);

		Line->Begin();
		Line->DrawTransform(lineVertices, 2, &(view * proj), D3DCOLOR_XRGB(0, 255, 0));
		Line->End();

		//device.DrawTransform

		//lineVertices[0].setInfo(from.getX(), from.getY(), from.getZ());
		//lineVertices[0].color = D3DCOLOR_XRGB(0, 255, 0);// color;

		//lineVertices[1].setInfo(to.getX(), to.getY(), to.getZ());
		//lineVertices[1].color = D3DCOLOR_XRGB(0, 255, 0);// color;

		//device.SetFVF(LineFVF);
		//device.DrawPrimitiveUP(D3DPT_LINELIST, 1, lineVertices, sizeof(LineVertex));

		//D3DRECT rect;
		//// x1, y1 좌상
		//// x2, y2 우하

		//rect.x1 = position.getX()
		//rec.x1 = bx - bw;//makes line longer/shorter going lef
		//rec.y1 = by; / base y
		//	rec.x2 = bx + bw;//makes line longer/shorter going right
		//rec.y2 = by + 1;//makes line one pixel tall
		//device.Clear(1, &rect, D3DCLEAR_TARGET, color, 0, 0);
	}


	static void DrawBox(const D3DXVECTOR3 & position, const D3DXVECTOR3 & xyz, D3DCOLOR color = 0xffff)
	{
		if (!Active) return;
		// 윗면
		D3DXVECTOR3 upLeftTop = position + D3DXVECTOR3(-xyz.x, xyz.y, xyz.z);
		D3DXVECTOR3 upLeftBottom = position + D3DXVECTOR3(-xyz.x, xyz.y, -xyz.z);

		D3DXVECTOR3 upRightTop = position + D3DXVECTOR3(xyz.x, xyz.y, xyz.z);
		D3DXVECTOR3 upRightBottom = position + D3DXVECTOR3(xyz.x, xyz.y, -xyz.z);

		// 아랫면
		D3DXVECTOR3 downLeftTop = position + D3DXVECTOR3(-xyz.x, -xyz.y, xyz.z);
		D3DXVECTOR3 downLeftBottom = position + D3DXVECTOR3(-xyz.x, -xyz.y, -xyz.z);

		D3DXVECTOR3 downRightTop = position + D3DXVECTOR3(xyz.x, -xyz.y, xyz.z);
		D3DXVECTOR3 downRightBottom = position + D3DXVECTOR3(xyz.x, -xyz.y, -xyz.z);

		// 윗면 그리기
		DrawLine(upLeftTop, upLeftBottom, color);
		DrawLine(upLeftTop, upRightTop, color);
		DrawLine(upRightBottom, upRightTop, color);
		DrawLine(upRightBottom, upLeftBottom, color);

		// 아랫면 그리기
		DrawLine(downLeftTop, downLeftBottom, color);
		DrawLine(downLeftTop, downRightTop, color);
		DrawLine(downRightBottom, downRightTop, color);
		DrawLine(downRightBottom, downLeftBottom, color);

		// 옆면 그리기
		DrawLine(upLeftTop, downLeftTop, color);
		DrawLine(upRightTop, downRightTop, color);
		DrawLine(upLeftBottom, downLeftBottom, color);
		DrawLine(upRightBottom, downRightBottom, color);

	}
};

//enum class KeyCode
//{
//	None,
//	PNone,
//	Space,
//
//	W,
//	A,
//	S,
//	D,
//
//	Esc,
//	Shift,
//
//	UpArrow,
//	DownArrow,
//	RightArrow,
//	LeftArrow,
//
//	LButton,
//	RButton,
//	MButton
//};

//enum class KeyState
//{
//	NONE,
//	STAY,
//	UP,
//	DOWN
//};


/*
* 기존 InputManager에서 수정된 사항
- _kbhit()사용 =>> (GetAsyncKeyState(virtual keycode) & 0x8000f) 변경
// 윈도우 프로그래밍에서는 _kbhit()사용할 수 없음 // msdn 참고

- 기존구조에서는 prefetch로 한프레임당 하나의 키를 받아올 수 있었다. // 즉 움직이면서 공격을 못한다. // ex. Arrow_up + Space
=>> 모든 가상키에 대해서 (256개) 프레임이 시작될때 인풋을 모두 받아서 현재 가상키 상태 배열에 저장
그 키가 눌렸는지 안눌렸는지는 그 가상키 상태 배열들(현재프레임 / 이전프레임) 참고해서 어떤상태인지 판별(up / down / stay)
*/
class InputManager
{
private:

	// 이렇게 해주는 이유는 아무것도 안해주다가 다시 같은 키를 누르는 경우도 down처리를 해주기 위해서 이다.
	// down 처리는 현재키코드와 이전키코드가 다르고 / 들어온 키코드가 현재키코드와 같을때 true를 반환한다.
	// up 처리는 현재키코드와 이전키코드가 다르고 / 들어온 키코드가 이전 키코드와 같을때 true를 반환한다.
	// stay 처리는 프리페치가 된 상태에서 현재키코드와 들어온 키코드가 같을때 true를 반환한다.

	// 0 ~ 255의 virtual keycode 들의 현재 눌렸는지 여부를 저장
	// 예를들어 0번키가 눌렸있으면 0번인덱스 값이 true이다.
	static vector<bool> CurrentPressedState;
	static vector<bool> PreviousPressedState;


	//static map<KeyCode, pair<int, int>> KeyCodeTable;
	//static int Major;
	//static int Minor;

	//// GetAsyncKeyState 함수를 싸는 함수를 만들어주기 위해서 정의
	//// 특정키에 대해서만 검사를 하면 현재 어떤게 눌렸는지 확인하기 어렵다.
	//static map<KeyCode, int> VirtualKeyCodeTable;

	//// 어떤키가 최초에 다운되었는지 알기 위한 테이블 
	//// 최초에 한번 true가 되고 다시 false가 된다.
	////static map<KeyCode, bool> VirtualKeyCodeDownTable;

	//static KeyCode CurrentKeyCode;
	//static KeyCode PreviousKeyCode;

	//static bool Prefetch;

public:
	static bool GetKeyDown(unsigned char virtualKeyCode)
	{
		// 이전프레임에서는 안눌렸지만 현재 프레임에서 눌린경우 true반환
		return CurrentPressedState[virtualKeyCode] == true && PreviousPressedState[virtualKeyCode] == false;
	}

	static bool GetKeyStay(unsigned char virtualKeyCode)
	{
		// 현재프레임에서 눌려있으면 true반환
		return CurrentPressedState[virtualKeyCode] == true;
	}
	static bool GetKeyUp(unsigned char virtualKeyCode)
	{
		// 이전프레임에서는 눌렀지만 현재 프레임에서 안눌린경우 true반환
		return CurrentPressedState[virtualKeyCode] == false && PreviousPressedState[virtualKeyCode] == true;
	}

	// 프레임이 시작할때 입력된 키들 모두 저장
	static void UpdateFrameStart()
	{
		for (int i = 0; i < 256; ++i)
		{
			CurrentPressedState[i] = (GetAsyncKeyState(i) & 0x8000f);
		}
	}

	// 프레임이 끝날때 이전프레임 눌림상태를 현재프레임 눌림상태로 초기화해준다.
	static void UpdateFrameEnd()
	{
		PreviousPressedState = CurrentPressedState;
	}

	// window programming에서는 _kbhit()을 사용할 수 없다.
	//static bool GetKeyDown(KeyCode keyCode)
	//{
	//	if (!Prefetch)
	//	{

	//		if (_kbhit())
	//		{
	//			// 여기안에 들어왔을경우는
	//			// 1. 키입력된상태인데 키코드비교했을때 다른경우
	//			// 2. 키입력된상태인데 키코드비교했을때 같은경우

	//			// 키보드는 2개의 키값으로 받아짐
	//			Major = _getch();
	//			Minor = _getch();

	//			//cout << "major" << Major << endl;
	//			//cout << "minor" << Minor << endl;


	//		}
	//		Prefetch = true;
	//	}
	//	// 그 두개의 키값을 비교해서 그키가 눌렸는지 확인한다.
	//	auto keyPair = KeyCodeTable[keyCode];
	//	return (keyPair == pair<int, int>{Major, Minor});

	//}


	// 프리페치 이후
	/*
	현재 키코드를 검사했을때

	 - current == previous
		stay : true
		down : false
		up : false

	- current != previous
		stay : true
		down : (current)true
		up : (previous) true
	*/
	

	//static bool GetKeyStay(KeyCode keyCode)
	//{
	//	if (!Prefetch)
	//	{
	//		// 키코드가 등록되어 있지않다면 리턴
	//		map<KeyCode, int>::iterator it = VirtualKeyCodeTable.find(keyCode);
	//		if (it == VirtualKeyCodeTable.end()) return false;

	//		// 키가 눌렸는지 확인
	//		if (!(GetAsyncKeyState(it->second) & 0x8000f)) return false;

	//		// 키코드가 등록되어 있고 눌렸다면 키가눌린것을 한프레임에서 유지
	//		Prefetch = true;
	//		CurrentKeyCode = keyCode;
	//		return true;

	//	}
	//	
	//	// stay일때는 CurrentKeyCode와 비교하면된다.
	//	// prefetch에 의해 항상 CurrentKeyCode가 최신화 되어 있다.
	//	if (CurrentKeyCode != keyCode)
	//		return false;
	//	return true;
	//}

	//static bool GetKeyDown(KeyCode keyCode)
	//{
	//	if (!Prefetch)
	//	{
	//		// 키코드가 등록되어 있지않다면 리턴
	//		map<KeyCode, int>::iterator it = VirtualKeyCodeTable.find(keyCode);
	//		if (it == VirtualKeyCodeTable.end()) return false;

	//		// 키가 눌렸는지 확인
	//		if (!(GetAsyncKeyState(it->second) & 0x8000f)) return false;

	//		// 키코드가 등록되어 있고 눌렸다면 키가눌린것을 한프레임에서 유지
	//		Prefetch = true;
	//		CurrentKeyCode = keyCode;
	//		return true;

	//	}
	//	// 현재 키가 확정된 상태에서 // prefetch에 의해
	//	// PreviousKeyCode 와 CurrentKeyCode와 다르고 들어온 키코드와 CurrentKeyCode가 같다면 true 반환
	//	if (CurrentKeyCode != keyCode)
	//		return false;
	//	return true;
	//}

	//static bool GetKeyUp(KeyCode keyCode)
	//{
	//	if (!Prefetch)
	//	{
	//		// 키코드가 등록되어 있지않다면 리턴
	//		map<KeyCode, int>::iterator it = VirtualKeyCodeTable.find(keyCode);
	//		if (it == VirtualKeyCodeTable.end()) return false;

	//		// 키가 눌렸는지 확인
	//		if (!(GetAsyncKeyState(it->second) & 0x8000f)) return false;

	//		// 키코드가 등록되어 있고 눌렸다면 키가눌린것을 한프레임에서 유지
	//		Prefetch = true;
	//		CurrentKeyCode = keyCode;
	//		return true;

	//	}

	//	if (CurrentKeyCode != keyCode)
	//		return false;
	//	return true;
	//}


	//static void Reset()
	//{
	//	// for KeyCodeTable
	//	Major = -1;
	//	Minor = -1;
	//	// for VirtualKeyCodeTable
	//	//CurrentKeyCode = KeyCode::None;
	//	PreviousKeyCode = CurrentKeyCode;

	//	// 만약에 현재 키가 최신화가 안된 상태라면 
	//	if (Prefetch == false)
	//	{
	//		// None으로 현재 키를 바꿔준다.
	//		CurrentKeyCode = KeyCode::None;
	//		// PNone으로 이전 키를 바꿔준다.
	//		PreviousKeyCode = KeyCode::PNone;

	//		// 이렇게 해주는 이유는 아무것도 안해주다가 다시 같은 키를 누르는 경우도 down처리를 해주기 위해서 이다.
	//		// down 처리는 현재키코드와 이전키코드가 다르고 / 들어온 키코드가 현재키코드와 같을때 true를 반환한다.
	//		// up 처리는 현재키코드와 이전키코드가 다르고 / 들어온 키코드가 이전 키코드와 같을때 true를 반환한다.
	//		// stay 처리는 프리페치가 된 상태에서 현재키코드와 들어온 키코드가 같을때 true를 반환한다.
	//	}

	//	Prefetch = false;
	//}

};

// 프레임간 간격을 millisec단위로 나타낸 클래스
class FrameTime
{
private :
	static long DeltaTime;
public :
	static long GetDeltaTime() { return DeltaTime; }
	static void SetDeltaTime(long other)
	{
		// 간격이기 때문에 음수가 없다.
		if (other < 0)
			other = 0;
		DeltaTime = other;
	}

};

#endif

