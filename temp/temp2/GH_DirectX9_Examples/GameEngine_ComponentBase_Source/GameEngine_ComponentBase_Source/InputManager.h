#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <conio.h>
#include <iostream>
#include <map>

using namespace std;

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
private :
	static map<KeyCode,pair<int,int>> KeyCodeTable;

	static int Major;
	static int Minor;
	
public :
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

			cout << "major" << Major << endl;
			cout << "minor" << Minor << endl;

			// �� �ΰ��� Ű���� ���ؼ� ��Ű�� ���ȴ��� Ȯ���Ѵ�.
			auto keyPair = KeyCodeTable[keyCode];
			return (keyPair == pair<int, int>{Major, Minor});

		}
		return false;
	}

};

#endif