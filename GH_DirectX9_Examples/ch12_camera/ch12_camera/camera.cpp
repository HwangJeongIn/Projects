//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: camera.cpp
// 
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Defines a camera's position and orientation.
//         
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "camera.h"

Camera::Camera()
{
	// 디폴트로 날아다닐수 있는 카메라를 설정
	_cameraType = AIRCRAFT;
	// 초기값은 월드좌표계의 right / up / forward로 설정
	_pos   = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	_right = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	_up    = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	_look  = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
}

Camera::Camera(CameraType cameraType)
{
	_cameraType = cameraType;

	// 초기값은 월드좌표계의 right / up / forward로 설정
	_pos   = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	_right = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	_up    = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	_look  = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
}

Camera::~Camera()
{

}

void Camera::getPosition(D3DXVECTOR3* pos)
{
	// 포지션값을 리턴하지 않고 바로 내부에서 값만 복사해서 바꿔주고 있다.
	// 결국 원본정보는 안바뀐다.
	*pos = _pos;
}

void Camera::setPosition(D3DXVECTOR3* pos)
{
	_pos = *pos;
}

void Camera::getRight(D3DXVECTOR3* right)
{
	*right = _right;
}

void Camera::getUp(D3DXVECTOR3* up)
{
	*up = _up;
}

void Camera::getLook(D3DXVECTOR3* look)
{
	*look = _look;
}

void Camera::walk(float units)
{
	// move only on xz plane for land object

	// 날수없는 모드에서는 forward벡터 기준으로 y방향은 계산하지 않는다.
	// 위쪽은 바라볼수 있지만(pitch허용) 위쪽으로는 이동할 수 없다
	if( _cameraType == LANDOBJECT )
		_pos += D3DXVECTOR3(_look.x, 0.0f, _look.z) * units;

	if( _cameraType == AIRCRAFT )
		_pos += _look * units;
}

void Camera::strafe(float units)
{
	// move only on xz plane for land object

	// 날수없는 경우는 역시 right벡터에서도 y방향은 계산하지 않는다.
	if( _cameraType == LANDOBJECT )
		_pos += D3DXVECTOR3(_right.x, 0.0f, _right.z) * units;

	if( _cameraType == AIRCRAFT )
		_pos += _right * units;
}

void Camera::fly(float units)
{
	// move only on y-axis for land object

	// 그냥 y축기준으로만 움직이도록하였다.
	if( _cameraType == LANDOBJECT )
		_pos.y += units;

	if( _cameraType == AIRCRAFT )
		_pos += _up * units;
}

void Camera::pitch(float angle)
{
	// pitch는 날수없는 모드에서도 허용된다.
	D3DXMATRIX T;
	D3DXMatrixRotationAxis(&T, &_right,	angle);

	// rotate _up and _look around _right vector
	// right벡터를 축으로 회전 시키는 것이기 때문에 right벡터는 변하지 않고,
	// 나머지 벡터들이 회전각도만큼 회전한다.
	D3DXVec3TransformCoord(&_up,&_up, &T);
	D3DXVec3TransformCoord(&_look,&_look, &T);
}

void Camera::yaw(float angle)
{
	D3DXMATRIX T;

	// rotate around world y (0, 1, 0) always for land object
	// up벡터 기준으로 회전시키는 것인데, 만약에 카메라가 pitch로 인해 위쪽을 바라보고 있을경우
	// 부자연스러움을 막기위해서 월드 up벡터 기준으로 회전시킨다.
	if( _cameraType == LANDOBJECT )
		D3DXMatrixRotationY(&T, angle);

	// rotate around own up vector for aircraft
	// 현재 카메라 up벡터를 기준으로 회전시킨다.
	if( _cameraType == AIRCRAFT )
		D3DXMatrixRotationAxis(&T, &_up, angle);

	// rotate _right and _look around _up or y-axis
	D3DXVec3TransformCoord(&_right,&_right, &T);
	D3DXVec3TransformCoord(&_look,&_look, &T);
}

void Camera::roll(float angle)
{
	// only roll for aircraft type

	// 날수있는 버전에서만 회전시킨다.
	if( _cameraType == AIRCRAFT )
	{
		D3DXMATRIX T;
		D3DXMatrixRotationAxis(&T, &_look,	angle);

		// rotate _up and _right around _look vector
		D3DXVec3TransformCoord(&_right,&_right, &T);
		D3DXVec3TransformCoord(&_up,&_up, &T);
	}
}

void Camera::getViewMatrix(D3DXMATRIX* V)
{
	// Keep camera's axes orthogonal to eachother

	// 이작업을 해주는 이유는 오차를 막기위해서다.
	// forward벡터 기준으로 축을 교정해주는 역할을 한다.
	// 먼저 forward벡터 기준으로 정규화시켜서 다시 저장하고
	D3DXVec3Normalize(&_look, &_look);

	// forward와 right의 외적을 계산하고 정규화하여 up벡터를 다시 계산한다.
	D3DXVec3Cross(&_up, &_look, &_right);
	D3DXVec3Normalize(&_up, &_up);

	// 마찬가지로 forwaed와 up의 외적을 계산하고 정규화하여 right벡터를 다시 계산한다.
	D3DXVec3Cross(&_right, &_up, &_look);
	D3DXVec3Normalize(&_right, &_right);


	// 여기서 정규화하는 이유는 아래 행렬은 정규화된 벡터를 기준으로 계산되었기 때문이다.
	// 행렬의 기저(행, 열)가 정규직교기저일때 이는 직교행렬이되며, 역행렬과 전치행렬이 같다.
	// 이 성질을 이용하여 회전행렬이 나오게 되는데, 우리는 그 행렬을 사용하여 카메라를 회전시킬것이기 때문에
	// 이러한 정규화작업이 필요하다.


	// Build the view matrix:
	float x = -D3DXVec3Dot(&_right, &_pos);
	float y = -D3DXVec3Dot(&_up, &_pos);
	float z = -D3DXVec3Dot(&_look, &_pos);

	// 계산된 행렬의 값을 넣어준다.
	(*V)(0,0) = _right.x; (*V)(0, 1) = _up.x; (*V)(0, 2) = _look.x; (*V)(0, 3) = 0.0f;
	(*V)(1,0) = _right.y; (*V)(1, 1) = _up.y; (*V)(1, 2) = _look.y; (*V)(1, 3) = 0.0f;
	(*V)(2,0) = _right.z; (*V)(2, 1) = _up.z; (*V)(2, 2) = _look.z; (*V)(2, 3) = 0.0f;
	(*V)(3,0) = x;        (*V)(3, 1) = y;     (*V)(3, 2) = z;       (*V)(3, 3) = 1.0f;
}

void Camera::setCameraType(CameraType cameraType)
{
	_cameraType = cameraType;
}
