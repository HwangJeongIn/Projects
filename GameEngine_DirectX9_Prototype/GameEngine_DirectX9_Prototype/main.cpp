//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: cameraApp.cpp
// 
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Demonstrates using the Camera class.
//         
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "d3dUtility.h"
#include "camera.h"
#include "Locator.h"
#include "Scene.h"
//
// Globals
//


const int Width  = 640;
const int Height = 480;

Camera TheCamera(Camera::LANDOBJECT);

//
// Framework functions
//
/*bool Setup()
{
	//
	// Setup a basic scene.  The scene will be created the
	// first time this function is called.
	//

	d3d::DrawBasicScene(Device, 0.0f); 

	//
	// Set projection matrix.
	//

	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
			&proj,
			D3DX_PI * 0.25f, // 45 - degree
			(float)Width / (float)Height,
			1.0f,
			1000.0f);
	Device->SetTransform(D3DTS_PROJECTION, &proj);


	Trace::LoadFileNames();
	Trace::Clear("TAG_DEBUG");



	return true;
}*/

void Cleanup()
{
	// pass 0 for the first parameter to instruct cleanup.
	d3d::DrawBasicScene(0, 0.0f);
}
/*
bool Display(float timeDelta)
{
	if( Device )
	{
		//
		// Update: Update the camera.
		//

		if( ::GetAsyncKeyState('W') & 0x8000f )
			TheCamera.walk(4.0f * timeDelta);

		if( ::GetAsyncKeyState('S') & 0x8000f )
			TheCamera.walk(-4.0f * timeDelta);

		if( ::GetAsyncKeyState('A') & 0x8000f )
			TheCamera.strafe(-4.0f * timeDelta);

		if( ::GetAsyncKeyState('D') & 0x8000f )
			TheCamera.strafe(4.0f * timeDelta);

		if( ::GetAsyncKeyState('R') & 0x8000f )
			TheCamera.fly(4.0f * timeDelta);

		if( ::GetAsyncKeyState('F') & 0x8000f )
			TheCamera.fly(-4.0f * timeDelta);

		if( ::GetAsyncKeyState(VK_UP) & 0x8000f )
			TheCamera.pitch(1.0f * timeDelta);

		if( ::GetAsyncKeyState(VK_DOWN) & 0x8000f )
			TheCamera.pitch(-1.0f * timeDelta);

		if( ::GetAsyncKeyState(VK_LEFT) & 0x8000f )
			TheCamera.yaw(-1.0f * timeDelta);
			
		if( ::GetAsyncKeyState(VK_RIGHT) & 0x8000f )
			TheCamera.yaw(1.0f * timeDelta);

		if( ::GetAsyncKeyState('N') & 0x8000f )
			TheCamera.roll(1.0f * timeDelta);

		if( ::GetAsyncKeyState('M') & 0x8000f )
			TheCamera.roll(-1.0f * timeDelta);

		// Update the view matrix representing the cameras 
        // new position/orientation.
		D3DXMATRIX V;
		TheCamera.getViewMatrix(&V);
		Device->SetTransform(D3DTS_VIEW, &V);

		//
		// Render
		//

		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
		Device->BeginScene();

		d3d::DrawBasicScene(Device, 1.0f);

		Device->EndScene();
		Device->Present(0, 0, 0, 0);
	}
	return true;
}
*/
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
	IDirect3DDevice9* device = 0;


	if(!d3d::InitD3D(hinstance,
		Width, Height, true, D3DDEVTYPE_HAL, &device))
	{
		::MessageBox(0, "InitD3D() - FAILED", 0, 0);
		return 0;
	}
		
	//if(!Setup())
	//{
	//	::MessageBox(0, "Setup() - FAILED", 0, 0);
	//	return 0;
	//}


	d3d::DrawBasicScene(device, 0.0f);

	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
		&proj,
		D3DX_PI * 0.25f, // 45 - degree
		(float)Width / (float)Height,
		1.0f,
		1000.0f);
	device->SetTransform(D3DTS_PROJECTION, &proj);


	Trace::LoadFileNames();
	Trace::Clear("TAG_DEBUG");

	/*
	서비스 등록
	: Scene / Device
	*/
	Locator::provideScene();
	Locator::provideDevice(device);
	GameObject * camera1 = GameObject::Instantiate();
	Scene & scene = camera1->getScene();
	
	/*
	Scene의 카메라와 기본 월드 세팅을 해준다.
	*/
	//scene.Instantiate

	// 클래스 멤버함수의 함수포인터는 또 다른식으로 정의해줘야한다.
	// 일단 클래스 명으로 지정 / 넘길때도 &을 붙여서 넘겨줌 / 사용할때는 그 클래스의 객체 기준으로 사용
	d3d::EnterMsgLoop( &Scene::gameLoop/*&(Scene::getInstance()->gameLoop)*/,scene );

	// void(*ptr_display)(void)
	Cleanup();

	device->Release();

	return 0;
}
