//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: vs_diffuse.cpp
// 
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Deomstrates diffuse lighting using a vertex shader.  You will
//       have to switch to the REF device to run this sample if your
//       hardware does not support shaders.  Or you
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

IDirect3DVertexShader9* DiffuseShader = 0;
ID3DXConstantTable* DiffuseConstTable = 0;

ID3DXMesh* Teapot            = 0;

D3DXHANDLE ViewMatrixHandle     = 0;
D3DXHANDLE ViewProjMatrixHandle = 0;
D3DXHANDLE AmbientMtrlHandle    = 0;
D3DXHANDLE DiffuseMtrlHandle    = 0;
D3DXHANDLE LightDirHandle       = 0;

D3DXMATRIX Proj;

//
// Framework functions
//
bool Setup()
{
	HRESULT hr = 0;

	//
	// Create geometry:
	//

	D3DXCreateTeapot(Device, &Teapot, 0);

	//
	// Compile shader
	//

	ID3DXBuffer* shader      = 0;
	ID3DXBuffer* errorBuffer = 0;

	hr = D3DXCompileShaderFromFile(
		// 소스코드를 포함하는 텍스트파일 이름
		"diffuse.txt",

		0,

		// 디폴트 포함 동작을 오버라이드 할수 있는 파라미터
		0,

		// 셰이더 코드에서의 진입하는 함수명
		"Main", // entry point function name

		// 컴파일할 세이더 버전을 지정하는 문자열
		"vs_1_1",

		// 오래된 버전과 호환가능하게 하는 플래그 + 디버그 정보 생성
		/*D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY|*/D3DXSHADER_DEBUG,

		// 컴파일 된 셰이더 코드를 포함하는 ID3DXBuffer로의 포인터 리턴
		// 픽셀 / 버텍스 셰이더를 만드는 인자로 사용된다.
		&shader,

		// 오류코드와 메시지 문자열을 포함하는 버퍼 리턴
		&errorBuffer,

		// 상수 테이블 리턴
		&DiffuseConstTable);

	// output any error messages
	if( errorBuffer )
	{
		::MessageBox(0, (char*)errorBuffer->GetBufferPointer(), 0, 0);
		d3d::Release<ID3DXBuffer*>(errorBuffer);
	}

	if(FAILED(hr))
	{
		::MessageBox(0, "D3DXCompileShaderFromFile() - FAILED", 0, 0);
		return false;
	}

	//
	// Create shader
	//

	// 버텍스 셰이더를 만든다.
	hr = Device->CreateVertexShader(
		(DWORD*)shader->GetBufferPointer(),
		&DiffuseShader);

	if(FAILED(hr))
	{
		::MessageBox(0, "CreateVertexShader - FAILED", 0, 0);
		return false;
	}

	// 정보는 더이상 필요없으므로 삭제한다.
	d3d::Release<ID3DXBuffer*>(shader);


	// 
	// Get Handles
	//

	// 셰이더를 컴파일할때 받았던 상수 테이블일 이용해서 전역변수들의 핸들을 얻어온다
	ViewMatrixHandle    = DiffuseConstTable->GetConstantByName(0, "ViewMatrix");
	ViewProjMatrixHandle= DiffuseConstTable->GetConstantByName(0, "ViewProjMatrix");
	AmbientMtrlHandle   = DiffuseConstTable->GetConstantByName(0, "AmbientMtrl");
	DiffuseMtrlHandle   = DiffuseConstTable->GetConstantByName(0, "DiffuseMtrl");
	LightDirHandle      = DiffuseConstTable->GetConstantByName(0, "LightDirection");

	//
	// Set shader constants:
	//


	// 받아온 전역변수들의 핸들을 이용해서 세팅한다.

	// Light direction:
	D3DXVECTOR4 directionToLight(-0.57f, 0.57f, -0.57f, 0.0f);
	DiffuseConstTable->SetVector(Device, LightDirHandle, &directionToLight);

	// Materials:
	D3DXVECTOR4 ambientMtrl(0.0f, 0.0f, 1.0f, 1.0f);
	D3DXVECTOR4 diffuseMtrl(0.0f, 0.0f, 1.0f, 1.0f);

	DiffuseConstTable->SetVector(Device,AmbientMtrlHandle,&ambientMtrl);
	DiffuseConstTable->SetVector(Device,DiffuseMtrlHandle,&diffuseMtrl);

	// 디폴트 값으로 상수값을 지정
	DiffuseConstTable->SetDefaults(Device);

	// Compute projection matrix.
	// 투영행렬 정의
	D3DXMatrixPerspectiveFovLH(
		&Proj,	D3DX_PI * 0.25f, 
		(float)Width / (float)Height, 1.0f, 1000.0f);

	return true;
}

void Cleanup()
{
	d3d::Release<ID3DXMesh*>(Teapot);
	d3d::Release<IDirect3DVertexShader9*>(DiffuseShader);
	d3d::Release<ID3DXConstantTable*>(DiffuseConstTable);
}

bool Display(float timeDelta)
{
	if( Device )
	{
		// 
		// Update the scene: Allow user to rotate around scene.
		//
		
		static float angle  = (3.0f * D3DX_PI) / 2.0f;
		static float height = 3.0f;
	
		if( ::GetAsyncKeyState(VK_LEFT) & 0x8000f )
			angle -= 0.5f * timeDelta;

		if( ::GetAsyncKeyState(VK_RIGHT) & 0x8000f )
			angle += 0.5f * timeDelta;

		if( ::GetAsyncKeyState(VK_UP) & 0x8000f )
			height += 5.0f * timeDelta;

		if( ::GetAsyncKeyState(VK_DOWN) & 0x8000f )
			height -= 5.0f * timeDelta;

		D3DXVECTOR3 position( cosf(angle) * 7.0f, height, sinf(angle) * 7.0f );
		D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
		D3DXMATRIX V;
		D3DXMatrixLookAtLH(&V, &position, &target, &up);

		// 뷰 스페이스 변환행렬은 사용자의 입력에 따라 계속 달라지기 때문에
		// 계속 업데이트 해주고 셰이더 코드내부에 있는 행렬들도 최신화 시켜주어야한다.

		// 뷰 스페이스 변환행렬로 셰이더 코드내에 있는 ViewMatrix를 초기화시켜준다
		DiffuseConstTable->SetMatrix(Device, ViewMatrixHandle, &V);

		D3DXMATRIX ViewProj = V * Proj;
		// 뷰 스페이스 변환 > 투영변환을 적용한 행렬로 셰이더 코드내에 있는 ViewProjMatrix를 초기화시켜준다.
		DiffuseConstTable->SetMatrix(Device, ViewProjMatrixHandle, &ViewProj);

		//
		// Render
		//

		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
		Device->BeginScene();

		// 셰이더 지정을 해준다.
		Device->SetVertexShader(DiffuseShader);

		// 셰이더로 그려준다.
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


