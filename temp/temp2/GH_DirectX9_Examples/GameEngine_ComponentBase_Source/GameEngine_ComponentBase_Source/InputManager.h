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
			// 여기안에 들어왔을경우는
			// 1. 키입력된상태인데 키코드비교했을때 다른경우
			// 2. 키입력된상태인데 키코드비교했을때 같은경우

			// 키보드는 2개의 키값으로 받아짐
			Major = _getch();
			Minor = _getch();

			cout << "major" << Major << endl;
			cout << "minor" << Minor << endl;

			// 그 두개의 키값을 비교해서 그키가 눌렸는지 확인한다.
			auto keyPair = KeyCodeTable[keyCode];
			return (keyPair == pair<int, int>{Major, Minor});

		}
		return false;
	}

};

#endif