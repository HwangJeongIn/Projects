//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: light_tex_effect.cpp
// 
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Deomstrates using an effect file to light and texture a 3D model.
//       Use the arrow keys to rotate.
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

ID3DXMesh*                      Mesh = 0;
std::vector<D3DMATERIAL9>       Mtrls(0);
std::vector<IDirect3DTexture9*> Textures(0);

ID3DXEffect* LightTexEffect = 0;

D3DXHANDLE WorldMatrixHandle  = 0;
D3DXHANDLE ViewMatrixHandle   = 0;
D3DXHANDLE ProjMatrixHandle   = 0;
D3DXHANDLE TexHandle          = 0;

D3DXHANDLE LightTexTechHandle = 0;

//
// Framework functions
//
bool Setup()
{
	HRESULT hr = 0;

	//
	// Load the XFile data.  
	//

	ID3DXBuffer* mtrlBuffer = 0;
	DWORD        numMtrls   = 0;

	hr = D3DXLoadMeshFromX(  
		"mountain.x",
		D3DXMESH_MANAGED,
		Device,
		0,
		&mtrlBuffer,
		0,
		&numMtrls,
		&Mesh);

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
	// Create effect.
	//

	ID3DXBuffer* errorBuffer = 0;
	hr = D3DXCreateEffectFromFile(
		// ����� ��ġ
		Device,

		// ����Ʈ �ҽ��ڵ� �ؽ�Ʈ���ϸ�
		"light_tex.txt",

		0,				// no preprocessor definitions

		// ����Ʈ ���Ե��� �������̵��ϱ� ���� �Է��Ķ����
		0,                // no ID3DXInclude interface

		D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY |D3DXSHADER_DEBUG, // compile flags
		
		// ����Ʈ���ڰ� �ٸ� ����Ʈ �ν��Ͻ��� �����Ǵ� ����� ����
		// �����������̸�, null������ ����Ʈ ���ϰ� ���ڸ� �������� �ʴ´�.
		0,                // don't share parameters

		// ������� ����Ʈ�� ��Ÿ���� �������̽����� ������
		&LightTexEffect,
		&errorBuffer);

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

	// 
	// Save Frequently Accessed Parameter Handles
	//

	// ������ �ڵ��� �̸����� �޾ƿ´�
	WorldMatrixHandle  = LightTexEffect->GetParameterByName(0, "WorldMatrix");
	ViewMatrixHandle   = LightTexEffect->GetParameterByName(0, "ViewMatrix");
	ProjMatrixHandle   = LightTexEffect->GetParameterByName(0, "ProjMatrix");
	TexHandle          = LightTexEffect->GetParameterByName(0, "Tex");

	// �����ų ��ũ���ڵ鵵 �޾ƿ´�
	LightTexTechHandle = LightTexEffect->GetTechniqueByName("LightAndTexture");
	
	//
	// Set Effect Parameters
	//

	//
	// Set Matrices
	D3DXMATRIX W, P;

	// ����Ʈ �ڵ��� ���ؼ� ����Ʈ �ڵ峻�� ���������� �ʱ�ȭ��Ų��.
	D3DXMatrixIdentity(&W);
	LightTexEffect->SetMatrix( WorldMatrixHandle, &W);

	D3DXMatrixPerspectiveFovLH(
		&P,	D3DX_PI * 0.25f, // 45 - degree
		(float)Width / (float)Height,
		1.0f, 1000.0f);

	LightTexEffect->SetMatrix( ProjMatrixHandle, &P);

	//
	// Set texture
	IDirect3DTexture9* tex = 0;
	D3DXCreateTextureFromFile(Device, "Terrain_3x_diffcol.jpg", &tex);

	// ���� �ؽ�ó �ڵ��� �̿��ؼ� �ؽ�ó�� �ʱ�ȭ�����ش�.
	LightTexEffect->SetTexture(TexHandle, tex);

	d3d::Release<IDirect3DTexture9*>(tex);

	return true;
}

void Cleanup()
{
	d3d::Release<ID3DXMesh*>(Mesh);

	for(int i = 0; i < Textures.size(); i++)
		d3d::Release<IDirect3DTexture9*>( Textures[i] );

	d3d::Release<ID3DXEffect*>(LightTexEffect);
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

		D3DXVECTOR3 position( cosf(angle) * 10.0f, height, sinf(angle) * 10.0f );
		D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
		D3DXMATRIX V;
		D3DXMatrixLookAtLH(&V, &position, &target, &up);

		// �� �����̽� ��ȯ����� ����ؼ� �ʱ�ȭ������Ѵ�.
		LightTexEffect->SetMatrix(ViewMatrixHandle, &V);

		//
		// Activate the Technique and Render
		//

		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
		Device->BeginScene();

		// set the technique to use

		// �̹� �޾ƿԴ� ��ũ�� �ڵ�� ��ũ���� �����Ѵ�.
		LightTexEffect->SetTechnique( LightTexTechHandle );

		UINT numPasses = 0;

		// numPasses������ ���ؼ� ��ũ���� �н��� ���� �޾ƿ´�.
    	LightTexEffect->Begin(&numPasses, 0);

		for(int i = 0; i < numPasses; i++)
		{
			// �н��� �����ϰ� �޽��� �� �׷��ְ�
			// ���� �н��� ���� �� �� �н��� �°� �޽��� �� �׷��ش�. // �н��� ������ŭ

			// ���� ���
			// �����н� ����
			LightTexEffect->BeginPass(i);
			
			// ������ ���ǰ� ���� ���� ����
			//LightTexEffect->Pass(i);

			for(int j = 0; j < Mtrls.size(); j++)
			{
				// ����Ʈ �ҽ��ڵ� ���� ��ũ�� ���� �����н��� �������� ���������� �޽��� �������ȴ�.
				Mesh->DrawSubset(j);
			}

			// ���� ���
			LightTexEffect->EndPass();
		}

		// �������� �˸���.
		LightTexEffect->End();

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


