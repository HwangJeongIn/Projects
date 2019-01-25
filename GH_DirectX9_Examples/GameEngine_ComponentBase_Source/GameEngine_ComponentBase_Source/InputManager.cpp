#include "InputManager.h"

//enum class KeyCode
//{
//	None,
//	Space,
//
//	W,
//	A,
//	S,
//	D,
//
//	Esc,
//
//	UpArrow,
//	DownArrow,
//	RightArrow,
//	LeftArrow
//};

map<KeyCode, pair<int, int>> InputManager::KeyCodeTable
{
	{ KeyCode::W,{ 119,0 } },
	{ KeyCode::A,{ 97,0 } },
	{ KeyCode::S,{ 115,0 } },
	{ KeyCode::D,{ 100,0 } },

	{ KeyCode::Esc,{ 27,0 } },
	{ KeyCode::Space,{ 32,0 } },

	{ KeyCode::UpArrow,{ 224,72 } },
	{ KeyCode::DownArrow,{ 224,80 } },
	{ KeyCode::RightArrow,{ 224,77 } },
	{ KeyCode::LeftArrow,{ 224,75 } },
};

int InputManager::Major = -1;
int InputManager::Minor = -1;