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

	// �ڿ� const�� ������ // static Vector3 operator+(const Vector3 & other1 , const Vector3 & other2)
	// �Ⱥ����� // static Vector3 operator+(Vector3 & other1 , const Vector3 & other2)
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


	// �������� ����ȭ�� ���縦 ���ϱ� ���ؼ� ���۷��� ������ �޾Ƽ� ���ο��� �ʱ�ȭ
	// ���������� ����Ǵ� ���� �򰥸� �� �־ �����ͷ� �Ϸ�������, ���� ������ ����(������ �˻� ���Ϸ���)
	// ���۷��� ������ ����
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

		// Line ����
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
		//// x1, y1 �»�
		//// x2, y2 ����

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
		// ����
		D3DXVECTOR3 upLeftTop = position + D3DXVECTOR3(-xyz.x, xyz.y, xyz.z);
		D3DXVECTOR3 upLeftBottom = position + D3DXVECTOR3(-xyz.x, xyz.y, -xyz.z);

		D3DXVECTOR3 upRightTop = position + D3DXVECTOR3(xyz.x, xyz.y, xyz.z);
		D3DXVECTOR3 upRightBottom = position + D3DXVECTOR3(xyz.x, xyz.y, -xyz.z);

		// �Ʒ���
		D3DXVECTOR3 downLeftTop = position + D3DXVECTOR3(-xyz.x, -xyz.y, xyz.z);
		D3DXVECTOR3 downLeftBottom = position + D3DXVECTOR3(-xyz.x, -xyz.y, -xyz.z);

		D3DXVECTOR3 downRightTop = position + D3DXVECTOR3(xyz.x, -xyz.y, xyz.z);
		D3DXVECTOR3 downRightBottom = position + D3DXVECTOR3(xyz.x, -xyz.y, -xyz.z);

		// ���� �׸���
		DrawLine(upLeftTop, upLeftBottom, color);
		DrawLine(upLeftTop, upRightTop, color);
		DrawLine(upRightBottom, upRightTop, color);
		DrawLine(upRightBottom, upLeftBottom, color);

		// �Ʒ��� �׸���
		DrawLine(downLeftTop, downLeftBottom, color);
		DrawLine(downLeftTop, downRightTop, color);
		DrawLine(downRightBottom, downRightTop, color);
		DrawLine(downRightBottom, downLeftBottom, color);

		// ���� �׸���
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
* ���� InputManager���� ������ ����
- _kbhit()��� =>> (GetAsyncKeyState(virtual keycode) & 0x8000f) ����
// ������ ���α׷��ֿ����� _kbhit()����� �� ���� // msdn ����

- �������������� prefetch�� �������Ӵ� �ϳ��� Ű�� �޾ƿ� �� �־���. // �� �����̸鼭 ������ ���Ѵ�. // ex. Arrow_up + Space
=>> ��� ����Ű�� ���ؼ� (256��) �������� ���۵ɶ� ��ǲ�� ��� �޾Ƽ� ���� ����Ű ���� �迭�� ����
�� Ű�� ���ȴ��� �ȴ��ȴ����� �� ����Ű ���� �迭��(���������� / ����������) �����ؼ� ��������� �Ǻ�(up / down / stay)
*/
class InputManager
{
private:

	// �̷��� ���ִ� ������ �ƹ��͵� �����ִٰ� �ٽ� ���� Ű�� ������ ��쵵 downó���� ���ֱ� ���ؼ� �̴�.
	// down ó���� ����Ű�ڵ�� ����Ű�ڵ尡 �ٸ��� / ���� Ű�ڵ尡 ����Ű�ڵ�� ������ true�� ��ȯ�Ѵ�.
	// up ó���� ����Ű�ڵ�� ����Ű�ڵ尡 �ٸ��� / ���� Ű�ڵ尡 ���� Ű�ڵ�� ������ true�� ��ȯ�Ѵ�.
	// stay ó���� ������ġ�� �� ���¿��� ����Ű�ڵ�� ���� Ű�ڵ尡 ������ true�� ��ȯ�Ѵ�.

	// 0 ~ 255�� virtual keycode ���� ���� ���ȴ��� ���θ� ����
	// ������� 0��Ű�� ���������� 0���ε��� ���� true�̴�.
	static vector<bool> CurrentPressedState;
	static vector<bool> PreviousPressedState;


	//static map<KeyCode, pair<int, int>> KeyCodeTable;
	//static int Major;
	//static int Minor;

	//// GetAsyncKeyState �Լ��� �δ� �Լ��� ������ֱ� ���ؼ� ����
	//// Ư��Ű�� ���ؼ��� �˻縦 �ϸ� ���� ��� ���ȴ��� Ȯ���ϱ� ��ƴ�.
	//static map<KeyCode, int> VirtualKeyCodeTable;

	//// �Ű�� ���ʿ� �ٿ�Ǿ����� �˱� ���� ���̺� 
	//// ���ʿ� �ѹ� true�� �ǰ� �ٽ� false�� �ȴ�.
	////static map<KeyCode, bool> VirtualKeyCodeDownTable;

	//static KeyCode CurrentKeyCode;
	//static KeyCode PreviousKeyCode;

	//static bool Prefetch;

public:
	static bool GetKeyDown(unsigned char virtualKeyCode)
	{
		// ���������ӿ����� �ȴ������� ���� �����ӿ��� ������� true��ȯ
		return CurrentPressedState[virtualKeyCode] == true && PreviousPressedState[virtualKeyCode] == false;
	}

	static bool GetKeyStay(unsigned char virtualKeyCode)
	{
		// ���������ӿ��� ���������� true��ȯ
		return CurrentPressedState[virtualKeyCode] == true;
	}
	static bool GetKeyUp(unsigned char virtualKeyCode)
	{
		// ���������ӿ����� �������� ���� �����ӿ��� �ȴ������ true��ȯ
		return CurrentPressedState[virtualKeyCode] == false && PreviousPressedState[virtualKeyCode] == true;
	}

	// �������� �����Ҷ� �Էµ� Ű�� ��� ����
	static void UpdateFrameStart()
	{
		for (int i = 0; i < 256; ++i)
		{
			CurrentPressedState[i] = (GetAsyncKeyState(i) & 0x8000f);
		}
	}

	// �������� ������ ���������� �������¸� ���������� �������·� �ʱ�ȭ���ش�.
	static void UpdateFrameEnd()
	{
		PreviousPressedState = CurrentPressedState;
	}

	// window programming������ _kbhit()�� ����� �� ����.
	//static bool GetKeyDown(KeyCode keyCode)
	//{
	//	if (!Prefetch)
	//	{

	//		if (_kbhit())
	//		{
	//			// ����ȿ� ����������
	//			// 1. Ű�ԷµȻ����ε� Ű�ڵ�������� �ٸ����
	//			// 2. Ű�ԷµȻ����ε� Ű�ڵ�������� �������

	//			// Ű����� 2���� Ű������ �޾���
	//			Major = _getch();
	//			Minor = _getch();

	//			//cout << "major" << Major << endl;
	//			//cout << "minor" << Minor << endl;


	//		}
	//		Prefetch = true;
	//	}
	//	// �� �ΰ��� Ű���� ���ؼ� ��Ű�� ���ȴ��� Ȯ���Ѵ�.
	//	auto keyPair = KeyCodeTable[keyCode];
	//	return (keyPair == pair<int, int>{Major, Minor});

	//}


	// ������ġ ����
	/*
	���� Ű�ڵ带 �˻�������

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
	//		// Ű�ڵ尡 ��ϵǾ� �����ʴٸ� ����
	//		map<KeyCode, int>::iterator it = VirtualKeyCodeTable.find(keyCode);
	//		if (it == VirtualKeyCodeTable.end()) return false;

	//		// Ű�� ���ȴ��� Ȯ��
	//		if (!(GetAsyncKeyState(it->second) & 0x8000f)) return false;

	//		// Ű�ڵ尡 ��ϵǾ� �ְ� ���ȴٸ� Ű���������� �������ӿ��� ����
	//		Prefetch = true;
	//		CurrentKeyCode = keyCode;
	//		return true;

	//	}
	//	
	//	// stay�϶��� CurrentKeyCode�� ���ϸ�ȴ�.
	//	// prefetch�� ���� �׻� CurrentKeyCode�� �ֽ�ȭ �Ǿ� �ִ�.
	//	if (CurrentKeyCode != keyCode)
	//		return false;
	//	return true;
	//}

	//static bool GetKeyDown(KeyCode keyCode)
	//{
	//	if (!Prefetch)
	//	{
	//		// Ű�ڵ尡 ��ϵǾ� �����ʴٸ� ����
	//		map<KeyCode, int>::iterator it = VirtualKeyCodeTable.find(keyCode);
	//		if (it == VirtualKeyCodeTable.end()) return false;

	//		// Ű�� ���ȴ��� Ȯ��
	//		if (!(GetAsyncKeyState(it->second) & 0x8000f)) return false;

	//		// Ű�ڵ尡 ��ϵǾ� �ְ� ���ȴٸ� Ű���������� �������ӿ��� ����
	//		Prefetch = true;
	//		CurrentKeyCode = keyCode;
	//		return true;

	//	}
	//	// ���� Ű�� Ȯ���� ���¿��� // prefetch�� ����
	//	// PreviousKeyCode �� CurrentKeyCode�� �ٸ��� ���� Ű�ڵ�� CurrentKeyCode�� ���ٸ� true ��ȯ
	//	if (CurrentKeyCode != keyCode)
	//		return false;
	//	return true;
	//}

	//static bool GetKeyUp(KeyCode keyCode)
	//{
	//	if (!Prefetch)
	//	{
	//		// Ű�ڵ尡 ��ϵǾ� �����ʴٸ� ����
	//		map<KeyCode, int>::iterator it = VirtualKeyCodeTable.find(keyCode);
	//		if (it == VirtualKeyCodeTable.end()) return false;

	//		// Ű�� ���ȴ��� Ȯ��
	//		if (!(GetAsyncKeyState(it->second) & 0x8000f)) return false;

	//		// Ű�ڵ尡 ��ϵǾ� �ְ� ���ȴٸ� Ű���������� �������ӿ��� ����
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

	//	// ���࿡ ���� Ű�� �ֽ�ȭ�� �ȵ� ���¶�� 
	//	if (Prefetch == false)
	//	{
	//		// None���� ���� Ű�� �ٲ��ش�.
	//		CurrentKeyCode = KeyCode::None;
	//		// PNone���� ���� Ű�� �ٲ��ش�.
	//		PreviousKeyCode = KeyCode::PNone;

	//		// �̷��� ���ִ� ������ �ƹ��͵� �����ִٰ� �ٽ� ���� Ű�� ������ ��쵵 downó���� ���ֱ� ���ؼ� �̴�.
	//		// down ó���� ����Ű�ڵ�� ����Ű�ڵ尡 �ٸ��� / ���� Ű�ڵ尡 ����Ű�ڵ�� ������ true�� ��ȯ�Ѵ�.
	//		// up ó���� ����Ű�ڵ�� ����Ű�ڵ尡 �ٸ��� / ���� Ű�ڵ尡 ���� Ű�ڵ�� ������ true�� ��ȯ�Ѵ�.
	//		// stay ó���� ������ġ�� �� ���¿��� ����Ű�ڵ�� ���� Ű�ڵ尡 ������ true�� ��ȯ�Ѵ�.
	//	}

	//	Prefetch = false;
	//}

};

// �����Ӱ� ������ millisec������ ��Ÿ�� Ŭ����
class FrameTime
{
private :
	static long DeltaTime;
public :
	static long GetDeltaTime() { return DeltaTime; }
	static void SetDeltaTime(long other)
	{
		// �����̱� ������ ������ ����.
		if (other < 0)
			other = 0;
		DeltaTime = other;
	}

};

#endif

