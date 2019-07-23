//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: transform.cpp
// 
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Deomstrates transforming an object by the view and projection matrices,
//       and setting the vertex color using a vertex shader.  You will have to
//       switch to the REF device if your hardware does not support shaders.  Or you
//       can use software vertex processing: D3DCREATE_SOFTWARE_VERTEXPROCESSING.
//    
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "d3dUtility.h"

//
// Globals
//

IDirect3DDevice9* Device = 0; 

const int Width  = 640;
const int Height = 480;

IDirect3DVertexShader9* TransformShader    = 0;
ID3DXConstantTable* TransformConstantTable = 0;

ID3DXMesh* Teapot = 0;

D3DXHANDLE TransformViewProjHandle = 0;

D3DXMATRIX ProjMatrix;

//
// Framework functions
//
bool Setup()
{
	HRESULT hr = 0;

	//
	// Create geometry.
	//

	D3DXCreateTeapot(Device, &Teapot, 0);

	//
	// Compile shader.
	//

	ID3DXBuffer* shader      = 0;
	ID3DXBuffer* errorBuffer = 0;

	hr = D3DXCompileShaderFromFile(
		// 셰이더 파일명
		"transform.txt",
		
		0,

		// 디폴트 포함 동작을 오버라이딩 할 수 있도록 해준다
		// 여기선 디폴트를 사용한다.
		0,

		// 진업점 함수이름을 지정하는 문자열
		"Main",   // entry point function name

		// 소스 코드를 컴파일할 셰이더 버전을 지정하는 문자열
		// 여러가지 셰이더 버전으로 컴파일이 가능하다.
		// 만약 어셈블리를 시용한다면 직접 손으로 포팅과정을 거쳐야한다.
		"vs_1_1", // shader version to compile to

		// 선택적인 컴파일링 플래그
		// 여기서는 컴파일러가 디버그 정보를 생성하도록 하였다.
		D3DXSHADER_DEBUG, 

		// 컴파일된 셰이더 코드를 포함하는 ID3DXBuffer로의 포인터를 리턴한다.
		// 컴파일로 만들어진 셰이더 코드는 실제로 버텍스 픽셀 셰이더를 만드는 다른함수에 전달하는 인자로 이용
		&shader,

		// 오류코드와 메시지 문자열을 포함하는 ID3DXBuffer로의 포인터를 리턴한다.
		&errorBuffer,

		// 셰이더의 상수테이블 데이터를 포함하는 포인터 리턴
		&TransformConstantTable);
		
	// output any error messages
	if( errorBuffer )
	{
		::MessageBox(0, (char*)errorBuffer->GetBufferPointer(), 0, 0);
		d3d::Release<ID3DXBuffer*>(errorBuffer);
	}

	if(FAILED(hr))
	{
		::MessageBox(0, "D3DXCreateEffectFromFile() - FAILED", 0, 0);
		return false;
	}

	hr = Device->CreateVertexShader(
		(DWORD*)shader->GetBufferPointer(),
		&TransformShader);

	if(FAILED(hr))
	{
		::MessageBox(0, "CreateVertexShader - FAILED", 0, 0);
		return false;
	}

	d3d::Release<ID3DXBuffer*>(shader);

	// 
	// Get Handles.
	// 
	// 상수테이블에서 현재 셰이더 코드상에 있는 ViewProjMatrix를 받아온다.
	// 첫번째 인자에 핸들을 넣으면 그핸들을 가진 구조체의 멤버변수에접근할 수 있다.
	// 최상위 변수로의 핸들을 얻고자 한다면 이 인자에 0 을 전달한다.
	TransformViewProjHandle = TransformConstantTable->GetConstantByName(0, "ViewProjMatrix");

	//
	// Set shader constants:
	//
	// 상수가 선언될 때 초기화 되는 값인 디폴트 값으로 상수 값을 지정하였다.
	// 세팅할때 한번만 호출되어야 한다.
	TransformConstantTable->SetDefaults(Device);
	 
	//
	// Set Projection Matrix.
	//

	// 투영행렬 정의
	D3DXMatrixPerspectiveFovLH(
			&ProjMatrix, D3DX_PI * 0.25f, 
			(float)Width / (float)Height, 1.0f, 1000.0f);

	//
	// Set Misc. States.
	//

	Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	return true;
}

void Cleanup()
{
	d3d::Release<ID3DXMesh*>(Teapot);
	d3d::Release<IDirect3DVertexShader9*>(TransformShader);
	d3d::Release<ID3DXConstantTable*>(TransformConstantTable);
}

bool Display(float timeDelta)
{
	if( Device )
	{
		// 
		// Update the scene: Allow user to rotate around scene.
		//
		
		// 한번만 초기화된다.
		static float angle  = (3.0f * D3DX_PI) / 2.0f;
		static float height = 5.0f;
	
		if( ::GetAsyncKeyState(VK_LEFT) & 0x8000f )
			angle -= 0.5f * timeDelta;

		if( ::GetAsyncKeyState(VK_RIGHT) & 0x8000f )
			angle += 0.5f * timeDelta;

		if( ::GetAsyncKeyState(VK_UP) & 0x8000f )
			height += 5.0f * timeDelta;

		if( ::GetAsyncKeyState(VK_DOWN) & 0x8000f )
			height -= 5.0f * timeDelta;

		D3DXVECTOR3 position( cosf(angle) * 10.0f, height, sinf(angle) * 10.0f );
		D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
		D3DXMATRIX V;

		// 뷰스페이스 변환 행렬 설정
		D3DXMatrixLookAtLH(&V, &position, &target, &up);

		// combine view and projection transformations
		// 
		// 투영행렬이랑 곱해서 
		D3DXMATRIX ViewProj = V * ProjMatrix;

		// 셰이더코드에 있는 변수를 핸들을 통해서 세팅시켜준다.
		TransformConstantTable->SetMatrix(
			Device,
			TransformViewProjHandle, 
			&ViewProj);

		//
		// Render
		//

		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
		Device->BeginScene();

		Device->SetVertexShader(TransformShader);

		Teapot->DrawSubset(0);
		
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


