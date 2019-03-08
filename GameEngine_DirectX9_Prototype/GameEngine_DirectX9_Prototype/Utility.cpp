#include "Utility.h"
#include <d3d9.h>

// ������Ͽ����� �� ��������� �����ϴ� ������ ���� ������
// ������ ���ǵɼ� �ֱ� ������ static ��������� ���⼭ �ʱ�ȭ�����ش�.
Vector3 Vector3::Zero{ 0,0,0 };
Vector3 Vector3::One{ 1,1,1 };
Vector3 Vector3::Up{ 0,1,0 };
Vector3 Vector3::Right{ 1,0,0 };
Vector3 Vector3::Forward{ 0,0,1 };

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

long FrameTime::deltaTime = 0;

