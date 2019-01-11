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

- ����
����Ʈ�� 3d ȭ���� �������ǵǴ� 2���� �簢���̴�.
����Ʈ ��ȯ����� �佺���̽��� ����â�� ����Ʈ ��ǥ��� ��ȯ�ϴ� ����̴�.

// ��ü���� ����
3d��ü���� ���� �����̽� ���� ���� 
-> ���� �����̽� �ý������� �Ű���
-> ������ ���� �߷������ ���� �۾� (Ŭ����) 

-> �佺���̽� ��ǥ��� ��ü�� ��ȯ(ī�޶� ���� + ���� z���� �����ٺ������ϴ� ���� ����)
// D3DXMatrixLookAtLH(v, ī�޶���ġ, ���� ����, up����) >> Device->SetTransform(D3DTS_VIEW, &v)

-> �佺���̽� ���� ������ ��ü���� ���� ������� ���� // �̶� z���� �Ϲ������� 1�̴�. // �� �����̽� �߽�(���� 0,0,0)���� ����â���� �Ÿ��� 1
// �̶� 3D�� z���� ������ġ�� ������ ��ǥ�� ��ԵǹǷ� 3D>2D��� ���� �� �ִ�.
// D3DXMatrixPerspectiveFovLH(proj, �þ߰�, ��Ⱦ��, �� ���ܸ������ �Ÿ�) >> Device->SetTransform(D3DTS_PROJECTION, &proj)

-> �� ��Ʈ ��ȯ�� ���� ���� �������� ���Ϲ�ü�� ����Ʈ�� ��ȯ // �̶� z���� ���ư���.
// proj�� proj(0,0)�� x�� ������ proj(1,1)�� y�� ������ �����ؼ� ����Ʈ�� ��ȯ�Ѵ�.
// D3DVIEWPORT vp = {X(��������), Y(��������), �ʺ�, ����, ���̹��� �ּ�, ���̹��� �ִ�} >> Device->SetViewPort(&vp)

 */
d3d::Ray CalcPickingRay(int x, int y)
{
	// �佺���̽� ����â ��ǥ���� ���̸� ����ϱ� ���� �Լ�
	// �Ķ���ͷ� ���� x, y ���� ��ũ�� ��ǥ�̴�.
	// ��ũ����ǥ > �佺���̽� ����â ��ǥ

	/*
	1. ��ũ�� ��ǥ�� �������� ����â ��ǥ�� �����ش�.
	2. �佺���̽��� �������� ����â ��ǥ�� ���͸� ���⺤�ͷ� �����ش�
	*/

	float px = 0.0f;
	float py = 0.0f;

	// ��ũ����ǥ���� ����â ��ǥ�� ��ȯ��Ű�� ���ؼ� ��ũ����ǥ�� ��� ���Դ��� �����غ���.
	// ��ũ����ǥ�� ����Ʈ ��ȯ��Ŀ� ���ؼ� �ٲ����. �ݴ��� ������ ���ָ� ����â ��ǥ�� ��ȯ��ų�� �ִ�.

	// ����Ʈ�� ������ �ޱ� ���� ����Ʈ�� �޾ƿ´�
	// X(��������), Y(��������), �ʺ�, ����, ���̹��� �ּ�, ���̹��� �ִ�

	D3DVIEWPORT9 vp;
	Device->GetViewport(&vp);
	//Device->GetViewport(&vp);


	// ����Ʈ�� ��ȯ�ϴ� ����� ���غ���
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
	// ����!!! Height�� unsigned long(DWORD)
	// ĳ������ ����� ������ ǥ���� �����ϴ�.
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



	// ����â ��ǥ(px, py, k, 1) // ����â�� z = 1�϶� (px, py, 1, 1)�̴�.
	// ����â�� z = 1�϶� ����â��ǥ�� ����Ʈ��ȯ��İ� �����ָ�(sx, sy, MaxZ, 1)�� ���´�.
	D3DXMatrixInverse(&vpM, 0, &vpM);
	D3DXVECTOR3 temp = { (float)x, (float)y, vp.MaxZ/*, 1*/};
	

	// w = 1�� ���
	// ����Ʈ ��ȯ����� ������� �����־
	// ���� ��ũ����ǥ > �佺���̽� ����â ��ǥ�� �ٲپ� �־���.
	// �׷��� ��Ʈ�����ʴ� ���ܻ�Ȳ�� �߻��Ѵ�.
	// �������� �ٱ����� ������
	D3DXVec3TransformCoord(&temp,&temp, &vpM);

	D3DXMATRIX proj;
	Device->GetTransform(D3DTS_PROJECTION, &proj);

	//  proj(0, 0) : ����Ʈ�� ��ȯ�Ҷ� x��ǥ�� �������� ����
	//  proj(1, 1) : ����Ʈ�� ��ȯ�Ҷ� y��ǥ�� �������� ����
	temp.x /= proj(0, 0);
	temp.y /= proj(1, 1);


	// ����â > ����Ʈ ����
	/*
	// ����Ʈ�� ������ X, Y�� ���� �����ش�.
	// ������ �°� ��ȯ�ϱ� ���� Width/2, Height/2�� ���ϰ� �����ش�
	x = px*(vp.Width/2) + X + vp.Width/2

	// �������� �������� �ϱ� ������ ����Ʈ�� ��ȯ�ɶ� y�� ���� �����Ǿ� -�� �ȴ�.
	// ���������� y���� �����Ѵ�.
	y = -py*(vp.Height/2) + Y + vp.Height/2

	// ����Ʈ ��ȯ����� �̻��ϰ� �����ϴ� �Լ��� ����.
	// �Ƹ� MaxZ�� MinZ�� ������ ������� �������� �ʴ� ĳ�̽��� �����
	// MaxZ�� MinZ�� �̻��� ���� ������ ����ó���� ���Ѱ��� �ƴұ�?

	// ����Ʈ ��ȯ ��ķ� ǥ���غ���
	Width/2		0			0			0
	0			-Height/2	0			0
	0			0			MaxZ-MinZ	0
	X+Width/2	Y+Height/2	MinZ		1

	����â ��ǥ(px, py, k, 1) // ����â�� z = 1�϶� (px, py, 1, 1)�̴�.
	�̸� ����Ʈ��ȯ��İ� �����ָ� (sx, sy, MaxZ, 1)�� ���´�.

	*/

	// �佺���̽��� ����â > ����Ʈ�� �ݴ뿬���� ���ش�.
	px = ((( 2.0f*x) / vp.Width)  - 1.0f) / proj(0, 0);
	py = (((-2.0f*y) / vp.Height) + 1.0f) / proj(1, 1);

	d3d::Ray ray;
	// ������ ������ �佺���̽��� �����̴�. �� 0,0,0
	ray._origin    = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	
	// �Ϲ����� ����â�� z = 1 �� ���� ��ġ�Ѵ�.
	// �ٵ� ����������ǿ� ���� ���Ҽ��� ���� �� ����.

	// ����â�� ��ǥ - ������ ���� = ����
	//ray._direction = D3DXVECTOR3(px, py, 1.0f);

	// ���� ��İ������ ������
	ray._direction = D3DXVECTOR3(temp.x, temp.y, 1.0f/*temp.z*/);
	return ray;
}

void TransformRay(d3d::Ray* ray, D3DXMATRIX* T)
{
	// ���� T����� �佺���̽� > ���彺���̽��� ��ȯ��Ű�� �佺���̽���ȯ ������̴�.
	/*
	�ռ� ������ ��������, �ױ����� �佺���̽� ��ǥ���� �����̴�.
	������ ��ǥ �ý��� ���� ��ġ�ϰ� �ϱ� ���ؼ��� 2���� ����� �ִµ�,
	1. ��ü�� �佺���̽� ��ǥ��� ��ȯ
	2. ������ ���� �����̽��� ��ü�� ���� �����̽��� ������ ��ȯ

	1������ �ϸ� ��� ��ü�� ��ȯ�ؾ� ������, 2���� ������ ��ȯ��Ű��ȴ�.
	���� 2���� ä���Ѵ�.
	*/

	// transform the ray's origin, w = 1.
	// w=1�� �ν��Ѵ�.
	// ����Ʈ�� �̵�
	D3DXVec3TransformCoord(
		&ray->_origin,
		&ray->_origin,
		T);

	// transform the ray's direction, w = 0.
	// w=0���� �ν��Ѵ�.
	// ������ �̵��� ���� ���� // 0 �̸� �̵����� �ʴ´�.
	D3DXVec3TransformNormal(
		&ray->_direction,
		&ray->_direction,
		T);

	// normalize the direction
	// ������ ����ȭ
	D3DXVec3Normalize(&ray->_direction, &ray->_direction);
}

bool RaySphereIntTest(d3d::Ray* ray, d3d::BoundingSphere* sphere)
{
	// ��ü �� �Ǻ�

	// ���� �߽������� ������ ������������ ���͸� ���Ѵ�.
	D3DXVECTOR3 v = ray->_origin - sphere->_center;
	/*
	// ���� �����Ŀ� ���̸� ����������
	// �Ʒ��� ���·� ���´�
	At^2 + Bt + C = 0

	// �ڼ��� ����
	// || ray->_origin + t*ray->_direction - sphere->_center || - sphere->radius = 0

	// �캯���� �������� �ѱ�� �����ϸ� �������Ŀ� ���ؼ�
	// (ray->_origin + t*ray->_direction - sphere->_center) dot (ray->_origin + t*ray->_direction - sphere->_center)
	// = sphere->radius^2

	// �����ϸ� ray->_origin - sphere->_center�� �Ѻ��ͷ� ���� ����ϸ�
	// || ray->_direction ||^2 * t^2 + 2*(t*ray->_direction) dot (ray->_origin - sphere->_center) 
	+ ||ray->_origin - sphere->_center||^2 - sphere->radius^2 = 0


	// ���⼭ ray->_direction�� ���̴� 1�̱� ������ A = 1�̵ȴ�.
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

	// �ϳ��� ����ذ� ������ ������ �ִٴ� ���̵ȴ�.
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
	// ���� pos�������� ���� ������ Ÿ����ġ�� �ٶ󺸰� �Ͽ���.
	// ���� ����� ��ġ�� �����ش�
	// ���������� ���Ľ����ش�.
	Device->SetTransform(D3DTS_VIEW, &V);

	//
	// Set projection matrix.
	//

	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
			&proj,
			D3DX_PI * 0.25f, // 45 - degree
			(float)Width / (float)Height,
		// ����â�� z = 1�̴�
			1.0f,
			1000.0f);
	// �������
	// �þ߰��� ����ü ����
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

		// angle�� 0 ~ 360�� �ݺ��ϱ� ������ cosf , sinf�� ���� ������ �׸��� �ȴ�.
		// r���� v������ �þ�� �پ��� ������ ���� Ŀ���� �����˵��� ���������.
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
		// Ŭ���� ��ũ�� ��ǥ�� �Ѱ��ְ� ����ǥ �������� �佺���̽� ��ǥ�� �����ش�.
		d3d::Ray ray = CalcPickingRay(LOWORD(lParam), HIWORD(lParam));
		
		// transform the ray to world space
		D3DXMATRIX view;
		// ����̽����� ����Ʈ ��ȯ ����� �޾ƿ´�.
		// ������ ��ü�� �������� �����ص� ����� �ִ�.
		// ch12�� ī�޶��� �������� �ִ� ������ �޸� �佺���̽���ȯ ����� �������Ѽ� �Ѱ��� �ٶ󺻴�.
		// ���࿡ ī�޶� �������� �ϴ� ��Ȳ�̶�� �佺���̽���ȯ ��ĵ� ī�޶� ���� ��� �������ְ�, ������Ѿ��Ѵ�.
		// �׷��� �ؾ� �ùٸ� �佺���̽���ȯ ������� ���� �� �ְ�, ��Ȯ�� ����ĳ��Ʈ�� �����ϴ�.
		Device->GetTransform(D3DTS_VIEW, &view);

		D3DXMATRIX viewInverse;
		D3DXMatrixInverse(&viewInverse,	0, &view);
		
		// ���� ���̴� �佺���̽� �������� ���Ǿ� �ִ�.
		// ���� ��ǥ�� ��ü�� �佺���̽���ǥ�� ��ȯ�ϰ� �׸� �������� ����� �־���.
		// ����� �Ϸ�Ǿ��� ������ �ٽ� ���̸� ���� ��ǥ��� ��ȯ�ϱ� ���ؼ�
		// �佺���̽� > ������ǥ��� �������ش�.
		// �׸� ���ؼ� ������ǥ�� > �佺���̽���ȯ ����� �����(���� ��ǥ�� > �佺���̽�)�� ������Ѽ� ���̸� ��ȯ�Ѵ�.
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
