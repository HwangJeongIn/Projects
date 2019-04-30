#include "Utility.h"
#include <d3d9.h>

// 헤더파일에서는 그 헤더파일을 포함하는 파일이 많기 때문에
// 여러번 정의될수 있기 때문에 static 멤버변수는 여기서 초기화시켜준다.
Vector3 Vector3::Zero{ 0,0,0 };
Vector3 Vector3::One{ 1,1,1 };
Vector3 Vector3::Up{ 0,1,0 };
Vector3 Vector3::Right{ 1,0,0 };
Vector3 Vector3::Forward{ 0,0,1 };





//map<KeyCode, pair<int, int>> InputManager::KeyCodeTable
//{
//	{ KeyCode::W,{ 119,0 } },
//	{ KeyCode::A,{ 97,0 } },
//	{ KeyCode::S,{ 115,0 } },
//	{ KeyCode::D,{ 100,0 } },
//
//	{ KeyCode::Esc,{ 27,0 } },
//	{ KeyCode::Space,{ 32,0 } },
//
//	{ KeyCode::UpArrow,{ 224,72 } },
//	{ KeyCode::DownArrow,{ 224,80 } },
//	{ KeyCode::RightArrow,{ 224,77 } },
//	{ KeyCode::LeftArrow,{ 224,75 } }
//};

//map<KeyCode, int> InputManager::VirtualKeyCodeTable
//{
//	{ KeyCode::W,0x57 },
//	{ KeyCode::A,0x41 },
//	{ KeyCode::S,0x53 },
//	{ KeyCode::D,0x44 },
//
//	{ KeyCode::Esc, 0x1B },
//	{ KeyCode::Space,0x20 },
//	{ KeyCode::Shift, 0x10 },
//
//	{ KeyCode::UpArrow,0x26 },
//	{ KeyCode::DownArrow,0x28 },
//	{ KeyCode::RightArrow,0x27 },
//	{ KeyCode::LeftArrow,0x25 },
//
//	{ KeyCode::LButton, 0x01 },
//	{ KeyCode::RButton, 0x02 },
//	{ KeyCode::MButton, 0x04 }
//};

// 초기값은 모두 눌려있지않다
vector<bool> InputManager::CurrentPressedState(256, false);
vector<bool> InputManager::PreviousPressedState(256, false);


//map<KeyCode, bool> InputManager::VirtualKeyCodeDownTable
//{
//	{ KeyCode::W,false},
//	{ KeyCode::A,false},
//	{ KeyCode::S,false},
//	{ KeyCode::D,false},
//
//	{ KeyCode::Esc, false},
//	{ KeyCode::Space,false},
//	{ KeyCode::Shift,false },
//
//	{ KeyCode::UpArrow,false },
//	{ KeyCode::DownArrow,false },
//	{ KeyCode::RightArrow,false },
//	{ KeyCode::LeftArrow,false },
//
//	{ KeyCode::LButton, false },
//	{ KeyCode::RButton, false },
//	{ KeyCode::MButton, false }
//};


//int InputManager::Major = -1;
//int InputManager::Minor = -1;
//bool InputManager::Prefetch = false;
//KeyCode InputManager::CurrentKeyCode = KeyCode::None;
//KeyCode InputManager::PreviousKeyCode = KeyCode::None;



long FrameTime::DeltaTime = 0;



IDirect3DDevice9 * Gizmos::Device = nullptr;
ID3DXLine * Gizmos::Line = nullptr;
bool Gizmos::Active = true;