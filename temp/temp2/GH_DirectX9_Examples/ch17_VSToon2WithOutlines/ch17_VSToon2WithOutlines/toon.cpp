//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: toon.cpp
// 
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Demonstrates toon rendering with silhouette edges outlined using a
//       vertex shader.  Note that you will have to switch to the REF device 
//       to view this sample if your graphics card does not support vertex shaders.  Or you
//       can use software vertex processing: D3DCREATE_SOFTWARE_VERTEXPROCESSING.
//          
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "d3dUtility.h"
#include "silhouetteEdges.h"

//
// Globals
//

IDirect3DDevice9* Device = 0; 

const int Width  = 640;
const int Height = 480;

IDirect3DVertexShader9* ToonShader = 0;
ID3DXConstantTable* ToonConstTable = 0;

ID3DXMesh*  Meshes[4] = {0, 0, 0, 0};
D3DXMATRIX  WorldMatrices[4];
D3DXVECTOR4 MeshColors[4];

D3DXMATRIX ProjMatrix;

IDirect3DTexture9* ShadeTex  = 0;

D3DXHANDLE ToonWorldViewHandle     = 0;
D3DXHANDLE ToonWorldViewProjHandle = 0;
D3DXHANDLE ToonColorHandle    = 0;
D3DXHANDLE ToonLightDirHandle = 0;

SilhouetteEdges* MeshOutlines[4] = {0, 0, 0, 0};
IDirect3DVertexShader9* OutlineShader = 0;
ID3DXConstantTable* OutlineConstTable = 0;

D3DXHANDLE OutlineWorldViewHandle = 0;
D3DXHANDLE OutlineProjHandle = 0;

//
// Framework functions
//
bool Setup()
{
	HRESULT hr = 0;

	//
	// Create geometry and compute corresponding world matrix and color
	// for each mesh.
	//

	ID3DXBuffer* adjBuffer[4] = {0, 0, 0, 0};

	// 인접정보와 메쉬정보를 생성할떄 담아준다.
	// 아래 기본적인 도형만드는 함수를 통해 생성된 메쉬의 FVF형식은 다음과 같다.
	// 나중에 구조체포인터(위치정보와 법선벡터 정보가 들어가있는 직접정의한 구조체를 사용한다)로 
	// 메쉬의 버텍스 정보를 받아와서 작업을 해준다.
	/*
	This function creates a mesh with the D3DXMESH_MANAGED creation option 
	and D3DFVF_XYZ | D3DFVF_NORMAL flexible vertex format (FVF).
	*/
	D3DXCreateTeapot(Device, &Meshes[0], &adjBuffer[0]);
	D3DXCreateSphere(Device, 1.0f, 20, 20, &Meshes[1], &adjBuffer[1]);
	D3DXCreateTorus(Device, 0.5f, 1.0f, 20, 20, &Meshes[2], &adjBuffer[2]);
	D3DXCreateCylinder(Device, 0.5f, 0.5f, 2.0f, 20, 20, &Meshes[3], &adjBuffer[3]);

	D3DXMatrixTranslation(&WorldMatrices[0],  0.0f,  2.0f, 0.0f);
	D3DXMatrixTranslation(&WorldMatrices[1],  0.0f, -2.0f, 0.0f);
	D3DXMatrixTranslation(&WorldMatrices[2], -3.0f,  0.0f, 0.0f);
	D3DXMatrixTranslation(&WorldMatrices[3],  3.0f,  0.0f, 0.0f);

	// 각 메쉬에 대한 칼라값을 지정해준다.
	MeshColors[0] = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
	MeshColors[1] = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
	MeshColors[2] = D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f);
	MeshColors[3] = D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f);

	//
	// Allocate mesh outlines
	//

	// 실루엣 외곽선 만들기 위해 실루엣 객체를 동적할당한다
	// 이때 실루엣 외곽선을 계산하기 위한 메쉬와, 인접정보를 가진 배열을 넘겨준다.
	// 그리고, 버텍스 정보 선언 생성 등 작업을 해주기 위해서 디바이스도 넘겨준다.
	MeshOutlines[0] = new SilhouetteEdges(Device, Meshes[0], adjBuffer[0]);
	MeshOutlines[1] = new SilhouetteEdges(Device, Meshes[1], adjBuffer[1]);
	MeshOutlines[2] = new SilhouetteEdges(Device, Meshes[2], adjBuffer[2]);
	MeshOutlines[3] = new SilhouetteEdges(Device, Meshes[3], adjBuffer[3]);

	d3d::Release<ID3DXBuffer*>(adjBuffer[0]);
	d3d::Release<ID3DXBuffer*>(adjBuffer[1]);
	d3d::Release<ID3DXBuffer*>(adjBuffer[2]);
	d3d::Release<ID3DXBuffer*>(adjBuffer[3]);

	//
	// Compile Toon Shader
	//

	ID3DXBuffer* toonCompiledCode = 0;
	ID3DXBuffer* toonErrorBuffer  = 0;

	hr = D3DXCompileShaderFromFile(
		"toon.txt",
		0,
		0,
		"Main", // entry point function name
		"vs_1_1",

		D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY|D3DXSHADER_DEBUG,
		&toonCompiledCode,
		&toonErrorBuffer,
		&ToonConstTable);

	// output any error messages
	if( toonErrorBuffer )
	{
		::MessageBox(0, (char*)toonErrorBuffer->GetBufferPointer(), 0, 0);
		d3d::Release<ID3DXBuffer*>(toonErrorBuffer);
	}

	if(FAILED(hr))
	{
		::MessageBox(0, "D3DXCompileShaderFromFile() - FAILED", 0, 0);
		return false;
	}

	hr = Device->CreateVertexShader(
		(DWORD*)toonCompiledCode->GetBufferPointer(),
		&ToonShader);

	if(FAILED(hr))
	{
		::MessageBox(0, "CreateVertexShader - FAILED", 0, 0);
		return false;
	}

	d3d::Release<ID3DXBuffer*>(toonCompiledCode);

	//
	// Compile Outline shader.
	//

	ID3DXBuffer* outlineCompiledCode = 0;
	ID3DXBuffer* outlineErrorBuffer  = 0;

	hr = D3DXCompileShaderFromFile(
		"outline.txt",
		0,
		0,
		"Main", // entry point function name
		"vs_1_1",
		D3DXSHADER_DEBUG, 
		&outlineCompiledCode,
		&outlineErrorBuffer,
		&OutlineConstTable);

	// output any error messages
	if( outlineErrorBuffer )
	{
		::MessageBox(0, (char*)outlineErrorBuffer->GetBufferPointer(), 0, 0);
		d3d::Release<ID3DXBuffer*>(outlineErrorBuffer);
	}

	if(FAILED(hr))
	{
		::MessageBox(0, "D3DXCompileShaderFromFile() - FAILED", 0, 0);
		return false;
	}

	hr = Device->CreateVertexShader(
		(DWORD*)outlineCompiledCode->GetBufferPointer(),
		&OutlineShader);

	if(FAILED(hr))
	{
		::MessageBox(0, "CreateVertexShader - FAILED", 0, 0);
		return false;
	}

	d3d::Release<ID3DXBuffer*>(outlineCompiledCode);

	//
	// Load textures.
	//

	// 여기서 텍스처 자체를 3가지 칼라만 가진 이미지로 해주었다.
	// 그리고 셰이더는 이 이미지의 uv좌표값을 계산해준다. 세로로 3가지 음영이 분리되기 때문에
	// v값은 .5로 고정하고 // 어떤값이든 상관없음
	// u값은 0-1의값(법선벡터와 빛의 방향의 내적) 이므로 값이 범위에 따라 3가지 음영으로 분리된다.
	D3DXCreateTextureFromFile(Device, "toonshade.bmp", &ShadeTex);

	Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	// 
	// Get Handles
	//

	ToonWorldViewHandle     = ToonConstTable->GetConstantByName(0, "WorldViewMatrix");
	ToonWorldViewProjHandle = ToonConstTable->GetConstantByName(0, "WorldViewProjMatrix");
	ToonColorHandle         = ToonConstTable->GetConstantByName(0, "Color");
	ToonLightDirHandle      = ToonConstTable->GetConstantByName(0, "LightDirection");

	OutlineWorldViewHandle = OutlineConstTable->GetConstantByName(0, "WorldViewMatrix");
	OutlineProjHandle      = OutlineConstTable->GetConstantByName(0, "ProjMatrix");

	//
	// Set shader constants:
	//
	
	// Light direction:
	D3DXVECTOR4 directionToLight(-0.57f, 0.57f, -0.57f, 0.0f);

	ToonConstTable->SetVector(
		Device, 
		ToonLightDirHandle,
		&directionToLight);

	ToonConstTable->SetDefaults(Device);
	OutlineConstTable->SetDefaults(Device);

	//
	// Calculate projection matrix.
	//

	D3DXMatrixPerspectiveFovLH(
		&ProjMatrix, D3DX_PI * 0.25f, 
		(float)Width / (float)Height, 1.0f, 1000.0f);

	return true;
}

void Cleanup()
{
	for(int i = 0; i < 4; i++)
	{
		d3d::Release<ID3DXMesh*>(Meshes[i]);
		d3d::Delete<SilhouetteEdges*>(MeshOutlines[i]);
	}
	d3d::Release<IDirect3DTexture9*>(ShadeTex);
	d3d::Release<IDirect3DVertexShader9*>(ToonShader);
	d3d::Release<ID3DXConstantTable*>(ToonConstTable);
	d3d::Release<IDirect3DVertexShader9*>(OutlineShader);
	d3d::Release<ID3DXConstantTable*>(OutlineConstTable);
}

bool Display(float timeDelta)
{
	if( Device )
	{
		// 
		// Update the scene: Allow user to rotate around scene.
		//
		
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

		D3DXVECTOR3 position( cosf(angle) * 7.0f, height, sinf(angle) * 7.0f );
		D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
		D3DXMATRIX view;
		D3DXMatrixLookAtLH(&view, &position, &target, &up);

		//
		// Render
		//

		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
		Device->BeginScene();

		// Draw Cartoon
		Device->SetVertexShader(ToonShader);
		//Device->SetVertexShader(NULL);
		Device->SetTexture(0, ShadeTex);

		D3DXMATRIX worldView;
		D3DXMATRIX worldViewProj;
		for(int i = 0; i < 4; i++)
		{
			worldView = WorldMatrices[i] * view;
			worldViewProj = WorldMatrices[i] * view * ProjMatrix;

			// 중단점 찍어서 핸들 정보 보려고했지만 읽을수없었다.
			// auto a =ToonWorldViewHandle;
			// 셰이더 월드변환 행렬 세팅
			ToonConstTable->SetMatrix(
				Device, 
				ToonWorldViewHandle,
				&worldView);

			// 셰이더 뷰 투영 행렬 세팅
			ToonConstTable->SetMatrix(
				Device, 
				ToonWorldViewProjHandle,
				&worldViewProj);

			// 셰이더 내부 칼라값을 지정해준다.
			ToonConstTable->SetVector(
				Device,
				ToonColorHandle,
				&MeshColors[i]);

			// 셰이더를 통해서 그려준다.
			Meshes[i]->DrawSubset(0);
		}

		// Draw Outlines.

		// 외곽선 셰이더를 설정해준다.
		// 텍스처도 바꿔준다.
		Device->SetVertexShader(OutlineShader);

		Device->SetTexture(0, 0);

		Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

		for(int i = 0; i < 4; i++)
		{
			worldView = WorldMatrices[i] * view;

			OutlineConstTable->SetMatrix(
				Device, 
				OutlineWorldViewHandle,
				&worldView);

			OutlineConstTable->SetMatrix(
				Device, 
				OutlineProjHandle,
				&ProjMatrix);

			// 내부적으로 버텍스정보 선언을 활성화 시켜주고 그려준다.
			/*
			
			추측	 // 정확하지 않음

			** 셰이더 코드로 입력정보는 어떻게 들어갈까?

			1. 필수적으로 버텍스정보 선언이 필요하다 // 이는 입력구조체 내에 지정된 각 데이터 멤버의 의미로 정의된다.
			
			예를 들어 버텍스정보 선언에서 
			D3DVERTEXELEMENT9 {연결될 스트림, 오프셋, 데이터형, 세분화방식, 계획된 용도, 용도인덱스}

			// 애플리케이션 내 버텍스 정보 선언
			D3DVERTEXELEMENT9 decl[] =
			{
				// offsets in bytes
				{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
				{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,  0},
				{ 0, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,  1},
				{ 0, 36, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,  2},
				D3DDECL_END()
			};

			// 셰이더 코드내 구조체 정의
			struct VS_INPUT
			{
				vector position    : POSITION;
				vector normal      : NORMAL0;
				vector faceNormal1 : NORMAL1;
				vector faceNormal2 : NORMAL2;
			};

			2. 매핑원리

			D3DDECLUSAGE_POSITION, 0 >> POSITION or POSITION0
			D3DDECLUSAGE_NORMAL,   0 >> NORMAL or NORMAL0
			D3DDECLUSAGE_NORMAL,   1 >> NORMAL1
			D3DDECLUSAGE_NORMAL,   2 >> NORMAL2

			3. 매핑과정과 그리는원리
			
				3.1. 기본적으로 삼각형을 하나씩 그릴때
				셰이더를 설정한다.
				버텍스정보 선언을 해준다.
				함수(DrawIndexedPrimitive)를 이용해서 그릴때 인덱스 버퍼와 버텍스 버퍼를 참조하게 된다.
				이 함수는 아마 내부적으로 래핑시켜 셰이더 코드로 보내주는 작업을 해줄것 같은데,
				셰이더의 입력파라미터로 넘기기 위한 래핑작업을 할때 버텍스정보 선언을 참고하게 될것이다.
				// 위 코드 기준 매핑
				0-12메모리까지 INPUT의 POSITION0에 매핑
				12-24메모리까지 INPUT의 NORMAL0에 매핑
				24-36메모리까지 INPUT의 NORMAL1에 매핑
				36-48메모리까지 INPUT의 NORMAL2에 매핑
				하여 버텍스와 인덱스 버퍼 기준으로 래핑시켜서 셰이더의 main 입력파라미터로 넘겨준다.

				여기서는 외곽선을 메쉬클래스를 이용하지않고, 직접 삼각형기준으로 그렸다. 그리기전에는 버텍스정보 선언을 활성화 시켜주었다.
				

				3.2. 메쉬클래스를 이용해서 그려줄경우 // 확인중 // 추측
				셰이더를 설정한다.
				메쉬를 할당하고 락걸고 버텍스와 인덱스 초기화시켜준다. // 직접 메쉬클래스를 이용해서 점과 삼각형을 만들어준다.
				
				함수(Drawsubset)을 이용해서 그릴때 내부적으로 3.1.과 같이 래핑해서 보내주는 작업을 해줄것이다.
				버텍스정보 선언을 같이 초기화해야 하지 않을까 생각해본다.(메쉬 만드는 함수 D3DXCreateMesh 파라미터에 있음) << 나중에 확인할 예정
				이렇게 생각한 이유는 셰이더 코드와 통신을 하기 위한 약속을 버텍스정보 선언이 갖고 있기 때문에 꼭 필요하다고 생각한다.
				참고로 버텍스정보 선언이 안들어간 메쉬 생성함수도 있다. D3DCreateMeshFVF


				3.3. 기본적으로 제공하는 도형을 그릴때
				셰이더를 설정한다.
				이경우 따로 버텍스나 인텍스 버퍼를 설정하지 않아도 출력되는 모습을 볼 수 있었다.
				버텍스정보 선언도 따로 해주지 않아도 되는데, 이는 아마 내부적으로 버텍스와 인덱스를 참고하여
				내부적으로 선언된 버텍스정보 선언을 참조하여 셰이더 코드에 래핑해서 넘겨주는 것같다.


			** 셰이더 코드에서 반환되는 자료형은 어떻게 활용되는 것일까?

			// OutPut 형식에 맞게 그려준다

			// 아직 정보를 덜찾았음 확인중
			반환될때 있는 타입에 따라서 알아서 결정해줄것이다

			TEXCOORD : uv좌표를 적용시켜 텍스처를 입히기도 하고
			COLOR : 색을 씌우기도하며
			POSITION : 렌더링될 위치를 결정하기도 할 것이다.
			
			*/
			MeshOutlines[i]->render();
		}

		Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

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


