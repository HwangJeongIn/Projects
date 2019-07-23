#include "Utility.h"

// 헤더파일에서는 그 헤더파일을 포함하는 파일이 많기 때문에
// 여러번 정의될수 있기 때문에 static 멤버변수는 여기서 초기화시켜준다.
Vector3 Vector3::Zero{ 0,0,0 };
Vector3 Vector3::One{ 1,1,1 };
Vector3 Vector3::Up{ 0,1,0 };
Vector3 Vector3::Right{ 0,0,1 };
Vector3 Vector3::Forward{ 1,0,0 };

