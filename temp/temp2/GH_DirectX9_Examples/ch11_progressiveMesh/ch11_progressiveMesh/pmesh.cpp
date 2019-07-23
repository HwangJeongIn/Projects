//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: pmesh.cpp
// 
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Demonstrates how to use the progressive mesh interface (ID3DXPMesh).  Use
//       the 'A' key to add triangles, use the 'S' key to remove triangles.  Note
//       that we outline the triangles in yellow so that you can see them get 
//       removed and added.
//          
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "d3dUtility.h"
#include <vector>

//
// Globals
//

IDirect3DDevice9* Device = 0; 

const int Width  = 640;
const int Height = 480;

ID3DXMesh*                      SourceMesh = 0;
// 프로그레시브 메쉬를 만들기 위한 변수 지정
ID3DXPMesh*                     PMesh      = 0; // progressive mesh
std::vector<D3DMATERIAL9>       Mtrls(0);
std::vector<IDirect3DTexture9*> Textures(0);

//
// Framework functions
//
bool Setup()
{
	HRESULT hr = 0;

	//
	// Load the XFile data.
	//

	ID3DXBuffer* adjBuffer  = 0;
	ID3DXBuffer* mtrlBuffer = 0;
	DWORD        numMtrls   = 0;

	// 똑같이 파일열어서 텍스처랑 재질 설정하는 것은 똑같다
	hr = D3DXLoadMeshFromX(  
		"bigship1.x",
		D3DXMESH_MANAGED,
		Device,
		&adjBuffer,
		&mtrlBuffer,
		0,
		&numMtrls,
		&SourceMesh);

	if(FAILED(hr))
	{
		::MessageBox(0, "D3DXLoadMeshFromX() - FAILED", 0, 0);
		return false;
	}

	//
	// Extract the materials, load textures.
	//

	if( mtrlBuffer != 0 && numMtrls != 0 )
	{
		D3DXMATERIAL* mtrls = (D3DXMATERIAL*)mtrlBuffer->GetBufferPointer();

		for(int i = 0; i < numMtrls; i++)
		{
			// the MatD3D property doesn't have an ambient value set
			// when its loaded, so set it now:
			mtrls[i].MatD3D.Ambient = mtrls[i].MatD3D.Diffuse;

			// save the ith material
			Mtrls.push_back( mtrls[i].MatD3D );

			// check if the ith material has an associative texture
			if( mtrls[i].pTextureFilename != 0 )
			{
				// yes, load the texture for the ith subset
				IDirect3DTexture9* tex = 0;
				D3DXCreateTextureFromFile(
					Device,
					mtrls[i].pTextureFilename,
					&tex);

				// save the loaded texture
				Textures.push_back( tex );
			}
			else
			{
				// no texture for the ith subset
				Textures.push_back( 0 );
			}
		}
	}
	d3d::Release<ID3DXBuffer*>(mtrlBuffer); // done w/ buffer

	//
	// Optimize the mesh.
	//

	hr = SourceMesh->OptimizeInplace(		
		D3DXMESHOPT_ATTRSORT |
		D3DXMESHOPT_COMPACT  |
		D3DXMESHOPT_VERTEXCACHE,
		(DWORD*)adjBuffer->GetBufferPointer(),
		(DWORD*)adjBuffer->GetBufferPointer(), // new adjacency info
		0, 0);

	if(FAILED(hr))
	{
		::MessageBox(0, "OptimizeInplace() - FAILED", 0, 0);
		d3d::Release<ID3DXBuffer*>(adjBuffer); // free
		return false;
	}

	//
	// Generate the progressive mesh. 
	//

	// 만들어진 메쉬를 기반으로 프로그레시브 메쉬를 만들어준다.
	hr = D3DXGeneratePMesh(
		// 프로그레시브 메쉬를 생성하기 위한 소스메쉬
		SourceMesh,

		// 인접정보를 포함하는 배열포인터
		(DWORD*)adjBuffer->GetBufferPointer(), // adjacency

		// i번째가 i번째 버텍스와 대응되며 속성의 영향을 지정하는 D3DXATTRIBUTEWEIGHTS의 포인터
		// 단순화 과정동안에 버텍스가 제거될 확률을 결정한다.
		// NULL전달시 디폴트 버텍스 속성 영향력 사용
		0,                  // default vertex attribute weights

		// 위 파라미터와 하는 것은 비슷하지만, 속성값이 아닌 float값 하나로 가중치를 결정한다.
		0,                  // default vertex weights

		// 단순화의 결과로 얻어질 최소한의 버텍스나 면
		1,                  // simplify as low as possible

		// 위의 값이 어디에 적용될 것인지 정해준다. // 여기서는 면에서 적용되게 하였다.
		D3DXMESHSIMP_FACE,  // simplify by face count

		// 내부적으로 초기화될 프로그레시브메쉬의 포인터를 넘겨준다.
		&PMesh);
	// C++도 C#처럼 입력파라미터에 키워드가 붙으면 내부에서 초기화 되는지 확실히 알 수 있을텐데 아쉽다

	// 프로그레시브 메쉬를 초기화하는데 사용되었던 정보들을 릴리즈 시켜준다.
	d3d::Release<ID3DXMesh*>(SourceMesh);  // done w/ source mesh
	d3d::Release<ID3DXBuffer*>(adjBuffer); // done w/ buffer

	if(FAILED(hr))
	{
		::MessageBox(0, "D3DXGeneratePMesh() - FAILED", 0, 0);
		return false;
	}

	// set to original detail
	// 초기값을 최대의 면으로 정해준다.
	DWORD maxFaces = PMesh->GetMaxFaces();
	PMesh->SetNumFaces(maxFaces);

	//
	// Set texture filters.
	//

	Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	// 
	// Set Lights.
	//

	D3DXVECTOR3 dir(1.0f, -1.0f, 1.0f);
	D3DXCOLOR col(1.0f, 1.0f, 1.0f, 1.0f);
	D3DLIGHT9 light = d3d::InitDirectionalLight(&dir, &col);

	Device->SetLight(0, &light);
	Device->LightEnable(0, true);
	Device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	Device->SetRenderState(D3DRS_SPECULARENABLE, true);

	//
	// Set camera.
	//

	D3DXVECTOR3 pos(-8.0f, 4.0f, -12.0f);
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);

	D3DXMATRIX V;
	D3DXMatrixLookAtLH(
		&V,
		&pos,
		&target,
		&up);

	Device->SetTransform(D3DTS_VIEW, &V);

	//
	// Set projection matrix.
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
	d3d::Release<ID3DXPMesh*>(PMesh);

	for(int i = 0; i < Textures.size(); i++)
		d3d::Release<IDirect3DTexture9*>( Textures[i] );
}

bool Display(float timeDelta)
{
	if( Device )
	{
		//
		// Update: Mesh resolution.
		//

		// Get the current number of faces the pmesh has.
		int numFaces = PMesh->GetNumFaces();

		// Add a face, note the SetNumFaces() will  automatically
		// clamp the specified value if it goes out of bounds.
		if( ::GetAsyncKeyState('A') & 0x8000f )
		{
			// Sometimes we must add more than one face to invert
			// an edge collapse transformation
			PMesh->SetNumFaces( numFaces + 1 );

			// 만약에 늘렸는데도 값이 변하지 않는다면 2개의 면을 추가해야한다.
			// 이런처리를 하는 이유는 ID3DXPMesh 인터페이스의 내부적 구현의 
			// 특성때문에 하나의 면을 더하는 것으로 메쉬내의 면의 수가 변하지 않는 경우가 있기 때문이다.
			if (PMesh->GetNumFaces() == numFaces)
				PMesh->SetNumFaces(numFaces + 2);
		}

		// Remove a face, note the SetNumFaces() will  automatically
		// clamp the specified value if it goes out of bounds.
		if( ::GetAsyncKeyState('S') & 0x8000f )
			PMesh->SetNumFaces( numFaces - 1 );
		
		//
		// Render
		//

		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
		Device->BeginScene();

		for(int i = 0; i < Mtrls.size(); i++)
		{
			// draw pmesh
			Device->SetMaterial( &Mtrls[i] );
			Device->SetTexture(0, Textures[i]);
			PMesh->DrawSubset(i);

			// draw wireframe outline
			// 처음그려주고 그위에다시 프레임모드 노란색으로 그려준다.
			Device->SetMaterial(&d3d::YELLOW_MTRL);
			Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
			PMesh->DrawSubset(i);
			Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		}	

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


