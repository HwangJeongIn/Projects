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

//
// Globals
//

IDirect3DDevice9* Device = 0; 

const int Width  = 640;
const int Height = 480;

Camera TheCamera(Camera::LANDOBJECT);

//
// Framework functions
//
bool Setup()
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

	// 투영행렬
	// 이는 절면체를 정의하고,
	// 줄두체 내의 기하물체를 투영 윈도우에 투영하는 과정을 담당한다.
	Device->SetTransform(D3DTS_PROJECTION, &proj);

	return true;
}

void Cleanup()
{
	// pass 0 for the first parameter to instruct cleanup.
	d3d::DrawBasicScene(0, 0.0f);
}

bool Display(float timeDelta)
{
	if( Device )
	{
		//
		// Update: Update the camera.
		//
		
		// 델타타임을 곱하는 이유 
		// 프레임이 잘나와서 상대적으로 많이 호출되는 경우에는 한프레임당 움직이는 거리가 적어지고
		// 프레임이 잘안나와서 상대적으로 적게 호출되는 경우에는 한프레임당 움직이는 거리가 길어진다.
		// 컴퓨터 성능에 관계없이 일정한 속도를 보여준다. 다만 부드럽게 움직이냐아니냐의 차이는 있다.
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
		// 앞서 계산된 값들에 대하여 카메라 행렬을 계산하고
		TheCamera.getViewMatrix(&V);
		// 카메라 뷰로 적용시킨다.
		// 여기서 뷰스페이스 변환을 할때, 스페이스내 모든 물체들은 카메라와 함께 변환된다.
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
