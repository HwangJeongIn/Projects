//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: litPyramid.cpp
// 
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Renders a lit pyramid.  Demonstrates how to specify the vertex 
//       normals, how to create and set a material, and how to create
//       and set a directional light.
//          
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "d3dUtility.h"

//
// Globals
//

IDirect3DDevice9* Device = 0; 

const int Width  = 640;
const int Height = 480;

IDirect3DVertexBuffer9* Pyramid = 0;

//
// Classes and Structures
//
struct Vertex
{
	Vertex(){}

	Vertex(float x, float y, float z, float nx, float ny, float nz)
	{
		_x  = x;  _y  = y;	_z  = z;
		_nx = nx; _ny = ny; _nz = nz;
	}
	float  _x,  _y,  _z;
	float _nx, _ny, _nz;

	static const DWORD FVF;
};
const DWORD Vertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL;

//
// Framework Functions
//
bool Setup()
{
	//
	// Turn on lighting.
	//
	Device->SetRenderState(D3DRS_LIGHTING, true);

	//
	// Create the vertex buffer for the pyramid.
	//

	// 피라미드를 만들기 위한 버텍스 버퍼
	Device->CreateVertexBuffer(
		12 * sizeof(Vertex), 
		D3DUSAGE_WRITEONLY,
		Vertex::FVF,
		D3DPOOL_MANAGED,
		&Pyramid,
		0);

	//
	// Fill the vertex buffer with pyramid data.
	//

	Vertex* v;
	Pyramid->Lock(0, 0, (void**)&v, 0);

	// front face
	v[0] = Vertex(-1.0f, 0.0f, -1.0f, 0.0f, 0.707f, -0.707f);
	v[1] = Vertex( 0.0f, 1.0f,  0.0f, 0.0f, 0.707f, -0.707f);
	v[2] = Vertex( 1.0f, 0.0f, -1.0f, 0.0f, 0.707f, -0.707f);

	// left face
	v[3] = Vertex(-1.0f, 0.0f,  1.0f, -0.707f, 0.707f, 0.0f);
	v[4] = Vertex( 0.0f, 1.0f,  0.0f, -0.707f, 0.707f, 0.0f);
	v[5] = Vertex(-1.0f, 0.0f, -1.0f, -0.707f, 0.707f, 0.0f);

	// right face
	v[6] = Vertex( 1.0f, 0.0f, -1.0f, 0.707f, 0.707f, 0.0f);
	v[7] = Vertex( 0.0f, 1.0f,  0.0f, 0.707f, 0.707f, 0.0f);
	v[8] = Vertex( 1.0f, 0.0f,  1.0f, 0.707f, 0.707f, 0.0f);

	// back face
	v[9]  = Vertex( 1.0f, 0.0f,  1.0f, 0.0f, 0.707f, 0.707f);
	v[10] = Vertex( 0.0f, 1.0f,  0.0f, 0.0f, 0.707f, 0.707f);
	v[11] = Vertex(-1.0f, 0.0f,  1.0f, 0.0f, 0.707f, 0.707f);

	Pyramid->Unlock();

	// 머티리얼과 라이트 설정을 따로 해준다.
	//
	// Create and set the material.
	//
	// 머티리얼 설정
	D3DMATERIAL9 mtrl;

	// 환경광 // 다른 표면에서 반사되어 전반적인 장면을 밝게하는 빛
	mtrl.Ambient  = d3d::WHITE * .1f;

	// 난반사광 // 표면에 닿으면 모든 방향으로 동일하게 반사된다. 
	// 관찰자의 위치에 관계없이 관찰자의 눈에 빛이 도달한다.
	// 빛의 방향과 표면의 형태만을 고려하면 된다.
	mtrl.Diffuse  = d3d::WHITE;

	// 정반사광 // 반사되어서 눈으로 들어오는 빛
	// 특정한 방향으로 진행하며, 표면에 닿으면 한방향으로 반사하여 특정한 각도에서만 관찰가능
	// 빛의 방향과 표면의 형태, 그리고 관찰자의 시점을 모두 고려해야한다. // 반짝이는 표현에 빛이 반사하는 효과를 모델링하는데 이용
	mtrl.Specular = d3d::WHITE ;

	// 방출형
	mtrl.Emissive = d3d::BLACK;
	mtrl.Power    = 5.0f;

	Device->SetMaterial(&mtrl);

	//
	// Setup a directional light.
	//
	// 빛정보를 조절한다.

	D3DLIGHT9 dir;
	::ZeroMemory(&dir, sizeof(dir));
	dir.Type      = D3DLIGHT_DIRECTIONAL;
	dir.Diffuse   = d3d::WHITE ;
	dir.Specular  = d3d::WHITE * 0.3f;
	dir.Ambient   = d3d::WHITE ;
	dir.Direction = D3DXVECTOR3(1.0f, 0.0f, 0.0f);

	//
	// Set and Enable the light.
	//

	Device->SetLight(0, &dir);
	Device->LightEnable(0, true);

	//
	// Turn on specular lighting and instruct Direct3D
	// to renormalize normals.
	//

	Device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	Device->SetRenderState(D3DRS_SPECULARENABLE, true);

	//
	// Position and aim the camera.
	//

	D3DXVECTOR3 pos(0.0f, 1.0f, -3.0f);
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	D3DXMATRIX V;
	D3DXMatrixLookAtLH(&V, &pos, &target, &up);
	Device->SetTransform(D3DTS_VIEW, &V);

	//
	// Set the projection matrix.
	//

	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
			&proj,
			D3DX_PI * 0.5f, // 90 - degree
			(float)Width / (float)Height,
			1.0f,
			1000.0f);
	Device->SetTransform(D3DTS_PROJECTION, &proj);

	return true;
}

void Cleanup()
{
	d3d::Release<IDirect3DVertexBuffer9*>(Pyramid);
}

bool Display(float timeDelta)
{
	if( Device )
	{
		// 
		// Update the scene: Rotate the pyramid.
		//

		D3DXMATRIX yRot;

		static float y = 0.0f;

		D3DXMatrixRotationY(&yRot, y);
		y += timeDelta;

		if( y >= 6.28f )
			y = 0.0f;

		Device->SetTransform(D3DTS_WORLD, &yRot);

		//
		// Draw the scene:
		//

		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
		Device->BeginScene();

		Device->SetStreamSource(0, Pyramid, 0, sizeof(Vertex));
		Device->SetFVF(Vertex::FVF);
		Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 4);

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