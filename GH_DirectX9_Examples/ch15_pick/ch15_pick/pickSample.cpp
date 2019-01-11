//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: pickSample.cpp
// 
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Demonstrates picking.
//          
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "d3dUtility.h"

//
// Globals
//

IDirect3DDevice9* Device = 0; 

const int Width  = 640;
const int Height = 480;

ID3DXMesh* Teapot = 0;
ID3DXMesh* Sphere = 0;

D3DXMATRIX World;
d3d::BoundingSphere BSphere;

//
// Functions
//
/*

- 참고
뷰포트는 3d 화면이 프로젝션되는 2차원 사각형이다.
뷰포트 변환행렬은 뷰스페이스의 투영창을 뷰포트 좌표계로 변환하는 행렬이다.

// 전체적인 과정
3d물체들은 로컬 스페이스 내에 정의 
-> 월드 스페이스 시스템으로 옮겨짐
-> 투영을 위해 추려내기와 같은 작업 (클리핑) 

-> 뷰스페이스 좌표계로 물체를 변환(카메라 원점 + 양의 z축을 내려다보도록하는 과정 진행)
// D3DXMatrixLookAtLH(v, 카메라위치, 보는 지점, up벡터) >> Device->SetTransform(D3DTS_VIEW, &v)

-> 뷰스페이스 내에 놓여진 물체들은 투영 윈도우로 투영 // 이때 z값은 일반적으로 1이다. // 뷰 스페이스 중심(현재 0,0,0)에서 투영창까지 거리가 1
// 이때 3D를 z값이 일정수치로 고정된 좌표를 얻게되므로 3D>2D라고 말할 수 있다.
// D3DXMatrixPerspectiveFovLH(proj, 시야각, 종횡비, 각 절단면까지의 거리) >> Device->SetTransform(D3DTS_PROJECTION, &proj)

-> 뷰 포트 변환을 통해 투영 윈도우의 기하물체가 뷰포트로 변환 // 이때 z값이 날아간다.
// proj의 proj(0,0)이 x의 배율을 proj(1,1)이 y의 배율을 조절해서 뷰포트로 변환한다.
// D3DVIEWPORT vp = {X(시작지점), Y(시작지점), 너비, 높이, 깊이버퍼 최소, 깊이버퍼 최대} >> Device->SetViewPort(&vp)

 */
d3d::Ray CalcPickingRay(int x, int y)
{
	// 뷰스페이스 투영창 좌표기준 레이를 계산하기 위한 함수
	// 파라미터로 들어온 x, y 값은 스크린 좌표이다.
	// 스크린좌표 > 뷰스페이스 투영창 좌표

	/*
	1. 스크린 좌표를 기준으로 투영창 좌표를 구해준다.
	2. 뷰스페이스의 원점에서 투영창 좌표의 벡터를 방향벡터로 구해준다
	*/

	float px = 0.0f;
	float py = 0.0f;

	// 스크린좌표에서 투영창 좌표로 변환시키기 위해서 스크린좌표가 어떻게 나왔는지 생각해본다.
	// 스크린좌표는 뷰포트 변환행렬에 의해서 바뀌었다. 반대의 연산을 해주면 투영창 좌표로 변환시킬수 있다.

	// 뷰포트의 정보를 받기 위해 뷰포트를 받아온다
	// X(시작지점), Y(시작지점), 너비, 높이, 깊이버퍼 최소, 깊이버퍼 최대

	D3DVIEWPORT9 vp;
	Device->GetViewport(&vp);
	//Device->GetViewport(&vp);


	// 뷰포트로 변환하는 행렬을 구해보자
	/*
	Width/2		0			0			0
	0			-Height/2	0			0
	0			0			MaxZ-MinZ	0
	X+Width/2	Y+Height/2	MinZ		1
	*/
	D3DXMATRIX vpM;

	vpM._11 = vp.Width / 2;
	vpM._12 = 0;
	vpM._13 = 0;
	vpM._14 = 0;

	vpM._21 = 0;
	// 주의!!! Height는 unsigned long(DWORD)
	// 캐스팅을 해줘야 음수값 표현이 가능하다.
	vpM._22 = -(float)(vp.Height / 2);
	vpM._23 = 0;
	vpM._24 = 0;

	vpM._31 = 0;
	vpM._32 = 0;
	vpM._33 = vp.MaxZ - vp.MinZ;
	vpM._34 = 0;

	vpM._41 = vp.X + vp.Width / 2;
	vpM._42 = vp.Y + vp.Height / 2;
	vpM._43 = vp.MinZ;
	vpM._44 = 1;



	// 투영창 좌표(px, py, k, 1) // 투영창이 z = 1일때 (px, py, 1, 1)이다.
	// 투영창이 z = 1일때 투영창좌표를 뷰포트변환행렬과 곱해주면(sx, sy, MaxZ, 1)이 나온다.
	D3DXMatrixInverse(&vpM, 0, &vpM);
	D3DXVECTOR3 temp = { (float)x, (float)y, vp.MaxZ/*, 1*/};
	

	// w = 1로 계산
	// 뷰포트 변환행렬의 역행렬을 곱해주어서
	// 현재 스크린좌표 > 뷰스페이스 투영창 좌표로 바꾸어 주었다.
	// 그런데 히트되지않는 예외상황이 발생한다.
	// 일정범위 바깥으로 나갈때
	D3DXVec3TransformCoord(&temp,&temp, &vpM);

	D3DXMATRIX proj;
	Device->GetTransform(D3DTS_PROJECTION, &proj);

	//  proj(0, 0) : 뷰포트로 변환할때 x좌표에 곱해지는 배율
	//  proj(1, 1) : 뷰포트로 변환할때 y좌표에 곱해지는 배율
	temp.x /= proj(0, 0);
	temp.y /= proj(1, 1);


	// 투영창 > 뷰포트 연산
	/*
	// 뷰포트의 기준점 X, Y를 먼저 더해준다.
	// 비율을 맞게 변환하기 위해 Width/2, Height/2를 곱하고 더해준다
	x = px*(vp.Width/2) + X + vp.Width/2

	// 왼쪽위를 원점으로 하기 때문에 뷰포트로 변환될때 y의 값은 반전되어 -가 된다.
	// 내려갈수록 y값이 증가한다.
	y = -py*(vp.Height/2) + Y + vp.Height/2

	// 뷰포트 변환행렬은 이상하게 제공하는 함수는 없다.
	// 아마 MaxZ와 MinZ이 같을때 역행렬이 존재하지 않는 캐이스가 생기고
	// MaxZ와 MinZ에 이상한 값이 들어갔을때 예외처리를 위한것이 아닐까?

	// 뷰포트 변환 행렬로 표현해보면
	Width/2		0			0			0
	0			-Height/2	0			0
	0			0			MaxZ-MinZ	0
	X+Width/2	Y+Height/2	MinZ		1

	투영창 좌표(px, py, k, 1) // 투영창이 z = 1일때 (px, py, 1, 1)이다.
	이를 뷰포트변환행렬과 곱해주면 (sx, sy, MaxZ, 1)이 나온다.

	*/

	// 뷰스페이스의 투영창 > 뷰포트의 반대연산을 해준다.
	px = ((( 2.0f*x) / vp.Width)  - 1.0f) / proj(0, 0);
	py = (((-2.0f*y) / vp.Height) + 1.0f) / proj(1, 1);

	d3d::Ray ray;
	// 광선의 원점은 뷰스페이스의 원점이다. 즉 0,0,0
	ray._origin    = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	
	// 일반적인 투영창은 z = 1 의 평면과 일치한다.
	// 근데 투영행렬정의에 따라서 변할수도 있을 것 같다.

	// 투영창의 좌표 - 광선의 원점 = 방향
	//ray._direction = D3DXVECTOR3(px, py, 1.0f);

	// 직접 행렬계산으로 만들어본것
	ray._direction = D3DXVECTOR3(temp.x, temp.y, 1.0f/*temp.z*/);
	return ray;
}

void TransformRay(d3d::Ray* ray, D3DXMATRIX* T)
{
	// 들어온 T행렬은 뷰스페이스 > 월드스페이스로 변환시키는 뷰스페이스변환 역행렬이다.
	/*
	앞서 광선을 구했지만, 그광선은 뷰스페이스 좌표계의 광선이다.
	동일한 좌표 시스템 내에 위치하게 하기 위해서는 2가지 방법이 있는데,
	1. 물체를 뷰스페이스 좌표계로 변환
	2. 광선을 월드 스페이스나 물체의 지역 스페이스로 광선을 변환

	1번으로 하면 모든 물체를 변환해야 하지만, 2번은 광선을 변환시키면된다.
	따라서 2번을 채택한다.
	*/

	// transform the ray's origin, w = 1.
	// w=1로 인식한다.
	// 포인트의 이동
	D3DXVec3TransformCoord(
		&ray->_origin,
		&ray->_origin,
		T);

	// transform the ray's direction, w = 0.
	// w=0으로 인식한다.
	// 벡터의 이동을 막기 위해 // 0 이면 이동하지 않는다.
	D3DXVec3TransformNormal(
		&ray->_direction,
		&ray->_direction,
		T);

	// normalize the direction
	// 방향을 정규화
	D3DXVec3Normalize(&ray->_direction, &ray->_direction);
}

bool RaySphereIntTest(d3d::Ray* ray, d3d::BoundingSphere* sphere)
{
	// 구체 픽 판별

	// 구의 중심점에서 레이의 시작점까지의 벡터를 구한다.
	D3DXVECTOR3 v = ray->_origin - sphere->_center;
	/*
	// 구의 방정식에 레이를 대입했을때
	// 아래의 형태로 나온다
	At^2 + Bt + C = 0

	// 자세한 식은
	// || ray->_origin + t*ray->_direction - sphere->_center || - sphere->radius = 0

	// 우변으로 반지름을 넘기고 제곱하면 내적공식에 의해서
	// (ray->_origin + t*ray->_direction - sphere->_center) dot (ray->_origin + t*ray->_direction - sphere->_center)
	// = sphere->radius^2

	// 정리하면 ray->_origin - sphere->_center을 한벡터로 보고 계산하면
	// || ray->_direction ||^2 * t^2 + 2*(t*ray->_direction) dot (ray->_origin - sphere->_center) 
	+ ||ray->_origin - sphere->_center||^2 - sphere->radius^2 = 0


	// 여기서 ray->_direction의 길이는 1이기 때문에 A = 1이된다.
	// B = 2*(ray->_direction) dot (ray->_origin - sphere->_center)
	// C = ||ray->_origin - sphere->_center||^2 - sphere->radius^2
	*/

	float b = 2.0f * D3DXVec3Dot(&ray->_direction, &v);
	float c = D3DXVec3Dot(&v, &v) - (sphere->_radius * sphere->_radius);

	// find the discriminant
	float discriminant = (b * b) - (4.0f * c);

	// test for imaginary number
	if( discriminant < 0.0f )
		return false;

	discriminant = sqrtf(discriminant);

	float s0 = (-b + discriminant) / 2.0f;
	float s1 = (-b - discriminant) / 2.0f;

	// if a solution is >= 0, then we intersected the sphere

	// 하나라도 양수해가 있으면 교점이 있다는 뜻이된다.
	if( s0 >= 0.0f || s1 >= 0.0f )
		return true;
	
	return false;
}

//
// Framework functions
//
bool Setup()
{
	//
	// Create the teapot.
	//

	D3DXCreateTeapot(Device, &Teapot, 0);

	//
	// Compute the bounding sphere.
	//

	BYTE* v = 0;
	Teapot->LockVertexBuffer(0, (void**)&v);

	D3DXComputeBoundingSphere(
		(D3DXVECTOR3*)v,
		Teapot->GetNumVertices(),
		D3DXGetFVFVertexSize(Teapot->GetFVF()),
		&BSphere._center,
		&BSphere._radius);

	Teapot->UnlockVertexBuffer();

	//
	// Build a sphere mesh that describes the teapot's bounding sphere.
	//

	D3DXCreateSphere(Device, BSphere._radius, 20, 20, &Sphere, 0);

	//
	// Set light.
	//

	D3DXVECTOR3 dir(0.707f, -0.0f, 0.707f);
	D3DXCOLOR col(1.0f, 1.0f, 1.0f, 1.0f);
	D3DLIGHT9 light = d3d::InitDirectionalLight(&dir, &col);

	Device->SetLight(0, &light);
	Device->LightEnable(0, true);
	Device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	Device->SetRenderState(D3DRS_SPECULARENABLE, false);	

	//
	// Set view matrix.
	//

	D3DXVECTOR3 pos(0.0f, 0.0f, -10.0f);
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);

	D3DXMATRIX V;
	D3DXMatrixLookAtLH(&V, &pos, &target, &up);
	// 현재 pos기준으로 보는 방향을 타겟위치로 바라보게 하였다.
	// 보는 방향과 위치를 정해준다
	// 내부적으로 정렬시켜준다.
	Device->SetTransform(D3DTS_VIEW, &V);

	//
	// Set projection matrix.
	//

	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
			&proj,
			D3DX_PI * 0.25f, // 45 - degree
			(float)Width / (float)Height,
		// 투영창은 z = 1이다
			1.0f,
			1000.0f);
	// 투영행렬
	// 시야각등 절두체 정의
	Device->SetTransform(D3DTS_PROJECTION, &proj);

	D3DVIEWPORT9 vp = { 0,0,640,480,0,1 };
	Device->SetViewport(&vp);

	return true;
}

void Cleanup()
{
	d3d::Release<ID3DXMesh*>(Teapot);
	d3d::Release<ID3DXMesh*>(Sphere);
}

bool Display(float timeDelta)
{
	if( Device )
	{
		//
		// Update: Update Teapot.
		//
			
		static float r     = 0.0f;
		static float v     = 1.0f;
		static float angle = 0.0f;

		// angle은 0 ~ 360을 반복하기 때문에 cosf , sinf에 의해 원형을 그리게 된다.
		// r값은 v에의해 늘어나고 줄어들기 때문에 점점 커지는 원형궤도가 만들어진다.
		D3DXMatrixTranslation(&World, cosf(angle) * r, sinf(angle) * r, 10.0f);

		// transfrom the bounding sphere to match the teapots position in the
		// world.
		BSphere._center = D3DXVECTOR3(cosf(angle)*r, sinf(angle)*r, 10.0f);

		r += v * timeDelta;

		if( r >= 8.0f )
			v = -v; // reverse direction

		if( r <= 0.0f )
			v = -v; // reverse direction

		angle += 1.0f * D3DX_PI * timeDelta;
		if( angle >= D3DX_PI * 2.0f )
			angle = 0.0f;

		//
		// Render
		//

		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
		Device->BeginScene();

		// Render the teapot.
		Device->SetTransform(D3DTS_WORLD, &World);
		Device->SetMaterial(&d3d::YELLOW_MTRL);
		Teapot->DrawSubset(0);

		// Render the bounding sphere with alpha blending so we can see 
		// through it.
		Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
		Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		D3DMATERIAL9 blue = d3d::BLUE_MTRL;
		blue.Diffuse.a = 0.25f; // 25% opacity
		Device->SetMaterial(&blue);
		Sphere->DrawSubset(0);

		Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);

		Device->EndScene();
		Device->Present(0, 0, 0, 0);
	}
	return true;
}

//
// WndProc
//
LRESULT CALLBACK d3d::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch( msg )
	{
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
		
	case WM_KEYDOWN:
		if( wParam == VK_ESCAPE )
			::DestroyWindow(hwnd);

		break;
	case WM_LBUTTONDOWN:

		// compute the ray in view space given the clicked screen point
		// 클릭된 스크린 좌표를 넘겨주고 그좌표 기준으로 뷰스페이스 좌표를 구해준다.
		d3d::Ray ray = CalcPickingRay(LOWORD(lParam), HIWORD(lParam));
		
		// transform the ray to world space
		D3DXMATRIX view;
		// 디바이스에서 뷰포트 변환 행렬을 받아온다.
		// 기존에 물체를 기준으로 세팅해둔 행렬이 있다.
		// ch12의 카메라의 움직임이 있는 예제와 달리 뷰스페이스변환 행렬을 고정시켜서 한곳만 바라본다.
		// 만약에 카메라가 움직여야 하는 상황이라면 뷰스페이스변환 행렬도 카메라에 따라서 계속 갱신해주고, 적용시켜야한다.
		// 그렇게 해야 올바른 뷰스페이스변환 역행렬을 구할 수 있고, 정확한 레이캐스트가 가능하다.
		Device->GetTransform(D3DTS_VIEW, &view);

		D3DXMATRIX viewInverse;
		D3DXMatrixInverse(&viewInverse,	0, &view);
		
		// 현재 레이는 뷰스페이스 기준으로 계산되어 있다.
		// 월드 좌표계 물체를 뷰스페이스좌표로 변환하고 그를 기준으로 계산해 주었다.
		// 계산이 완료되었기 때문에 다시 레이를 월드 좌표계로 변환하기 위해서
		// 뷰스페이스 > 월드좌표계로 변경해준다.
		// 그를 위해서 윌드좌표계 > 뷰스페이스변환 행렬의 역행렬(월드 좌표계 > 뷰스페이스)을 적용시켜서 레이를 변환한다.
		TransformRay(&ray, &viewInverse);

		// test for a hit
		if( RaySphereIntTest(&ray, &BSphere) )
			::MessageBox(0, "Hit!", "HIT", 0);

		break;
	}
	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

//
// WinMain
//
int WINAPI WinMain(HINSTANCE hinstance,
				   HINSTANCE prevInstance, 
				   PSTR cmdLine,
				   int showCmd)
{
	if(!d3d::InitD3D(hinstance,
		Width, Height, true, D3DDEVTYPE_HAL, &Device))
	{
		::MessageBox(0, "InitD3D() - FAILED", 0, 0);
		return 0;
	}
		
	if(!Setup())
	{
		::MessageBox(0, "Setup() - FAILED", 0, 0);
		return 0;
	}

	d3d::EnterMsgLoop( Display );

	Cleanup();

	Device->Release();

	return 0;
}
