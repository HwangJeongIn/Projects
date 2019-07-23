//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: d3dxcreatemeshfvf.cpp
// 
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Demonstrates how to create an empty ID3DXMesh object with D3DXCreateMeshFVF, 
//       how to fill the vertex, index, and attribute buffers, how to optimize a mesh
//       and gnerate an attribute table, and how to render it.
//          
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "d3dUtility.h"
#include <fstream>
#include <vector>

//
// Globals
//

IDirect3DDevice9* Device = 0; 

const int Width  = 640;
const int Height = 480;

// 사용할 메쉬 포인터, 메쉬가 가질 서브셋 3개로 지정
// 텍스처의 갯수역시 서브셋과 똑같다.
ID3DXMesh*         Mesh = 0;
const DWORD        NumSubsets = 3;
IDirect3DTexture9* Textures[3] = {0, 0, 0};// texture for each subset

// 메쉬 데이터를 파일에 덤프하는데 이용된다.
std::ofstream OutFile; // used to dump mesh data to file

//
// Prototypes
//

// 메쉬 요소의 디버깅과 검사를 위해서 메쉬의 내용을 파일로 덤프하는 함수들을 구현
// 뒷부분에서 확인해본다.
void dumpVertices(std::ofstream& outFile, ID3DXMesh* mesh);
void dumpIndices(std::ofstream& outFile, ID3DXMesh* mesh);
void dumpAttributeBuffer(std::ofstream& outFile, ID3DXMesh* mesh);
void dumpAdjacencyBuffer(std::ofstream& outFile, ID3DXMesh* mesh);
void dumpAttributeTable(std::ofstream& outFile, ID3DXMesh* mesh);

//
// Classes and Structures
//
struct Vertex
{
	Vertex(){}

	Vertex(float x, float y, float z, 
		float nx, float ny, float nz, float u, float v)
	{
		 _x = x;   _y = y;   _z = z;
		_nx = nx; _ny = ny; _nz = nz;
		 _u = u;   _v = v;
	}

	float _x, _y, _z, _nx, _ny, _nz, _u, _v;

	static const DWORD FVF;
};
const DWORD Vertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;

//
// Framework functions
//
// 만드는 순서
/*
1. 빈 메쉬 만들기
2. 상자의 기하 정보로 메쉬를 채운다.
3. 매쉬의 각 면이 존재하는 서브셋을 지정한다.
4. 메쉬의 인접 정보를 만든다.
5. 메쉬를 최적화한다.
6. 메쉬를 그려낸다.
*/
bool Setup()
{
	// 에러코드와 경고정보를 넘겨주기 위한 변수이다.
	HRESULT hr = 0;

	//
	// We are going to fill the empty mesh with the geometry of a box,
	// so we need 12 triangles and 24 vetices.
	//

	// 여기서는 상자를 표현하는데 필요한
	// 12개의 면과 24개의 버텍스를 가진 메쉬를 할당하였다.

	// 할당방식
	// 기존 : 인덱스 버퍼와 버텍스 버퍼를 이용해서 락과 언락을 하면서 정보를 초기화 해주었다.
	// 변경 : Mesh 변수를 D3DXCreateMeshFVF를 이용하여 할당해준다.

	// 그리는 방식
	// 기존 : SetStream함수를 이용하여 버퍼를 설정하고 DrawIndexedPrimitive와 같은 함수를 호출해서 그려준다.
	// 변경 : Mesh의 DrawSubset함수를 이용하여 그려준다.
	hr = D3DXCreateMeshFVF(
		// 메쉬가 가질 면의 개수 // 정육면체 = 삼각형 12개
		12,
		// 메쉬가 가질 버텍스의 수
		24,
		// 메쉬를 만드는데 이용될 하나 이상의 플래그
		// 여기서는 메쉬는 관리 메모리 풀내에 보관되도록 하였다.
		D3DXMESH_MANAGED,

		// 복제된 메쉬를 만드는데 이용될 포맷
		Vertex::FVF,
		Device,
		// 복제된 메쉬가 담길 포인터
		&Mesh);

	// 실패했을때 메시지 박스를 띄우고 함수를 빠져나간다.
	if(FAILED(hr))
	{
		::MessageBox(0, "D3DXCreateMeshFVF() - FAILED", 0, 0);
		return false;
	}

	//
	// Fill in vertices of a box
	//
	// 메쉬를 할당하기만 하였기 때문에 내부에 버텍스와 인덱스를 저장하는 작업이 필요하다.
	Vertex* v = 0;
	// 메쉬를 잠궈주고
	Mesh->LockVertexBuffer(0, (void**)&v);

	// 내부의 버텍스 정보를 넣어준다.
	// fill in the front face vertex data
	v[0] = Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[1] = Vertex(-1.0f,  1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[2] = Vertex( 1.0f,  1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);
	v[3] = Vertex( 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

	// fill in the back face vertex data
	v[4] = Vertex(-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	v[5] = Vertex( 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	v[6] = Vertex( 1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
	v[7] = Vertex(-1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);

	// fill in the top face vertex data
	v[8]  = Vertex(-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	v[9]  = Vertex(-1.0f, 1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	v[10] = Vertex( 1.0f, 1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);
	v[11] = Vertex( 1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);

	// fill in the bottom face vertex data
	v[12] = Vertex(-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f);
	v[13] = Vertex( 1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f);
	v[14] = Vertex( 1.0f, -1.0f,  1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f);
	v[15] = Vertex(-1.0f, -1.0f,  1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);

	// fill in the left face vertex data
	v[16] = Vertex(-1.0f, -1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[17] = Vertex(-1.0f,  1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[18] = Vertex(-1.0f,  1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	v[19] = Vertex(-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	// fill in the right face vertex data
	v[20] = Vertex( 1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[21] = Vertex( 1.0f,  1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[22] = Vertex( 1.0f,  1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	v[23] = Vertex( 1.0f, -1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	// 메쉬를 다시 풀어준다.
	Mesh->UnlockVertexBuffer();

	//
	// Define the triangles of the box
	//
	WORD* i = 0;
	// 인덱스의 저장을 위해서 다시 메쉬를 잠궈주고
	Mesh->LockIndexBuffer(0, (void**)&i);

	// fill in the front face index data
	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 0; i[4] = 2; i[5] = 3;

	// fill in the back face index data
	i[6] = 4; i[7]  = 5; i[8]  = 6;
	i[9] = 4; i[10] = 6; i[11] = 7;

	// fill in the top face index data
	i[12] = 8; i[13] =  9; i[14] = 10;
	i[15] = 8; i[16] = 10; i[17] = 11;

	// fill in the bottom face index data
	i[18] = 12; i[19] = 13; i[20] = 14;
	i[21] = 12; i[22] = 14; i[23] = 15;

	// fill in the left face index data
	i[24] = 16; i[25] = 17; i[26] = 18;
	i[27] = 16; i[28] = 18; i[29] = 19;

	// fill in the right face index data
	i[30] = 20; i[31] = 21; i[32] = 22;
	i[33] = 20; i[34] = 22; i[35] = 23;

	// 인덱스 설정 후 다시 풀어준다.
	Mesh->UnlockIndexBuffer();

	//
	// Specify the subset each triangle belongs to, in this example
	// we will use three subsets, the first two faces of the cube specified
	// will be in subset 0, the next two faces will be in subset 1 and
	// the the last two faces will be in subset 2.
	//
	DWORD* attributeBuffer = 0;

	// 다시 속성 데이터를 수정하기 위해서 버퍼를 잠궈준다.
	Mesh->LockAttributeBuffer(0, &attributeBuffer);

	// 삼각형 0-3은 서브셋을 0번으로 지정해준다.
	for(int a = 0; a < 4; a++)
		attributeBuffer[a] = 0;

	// 삼각형 4-7은 서브셋을 1번으로 지정해준다.
	for(int b = 4; b < 8; b++)
		attributeBuffer[b] = 1;

	// 삼각형 8-11은 서브셋을 2번으로 지정해준다.
	for(int c = 8; c < 12; c++)
		attributeBuffer[c] = 2;

	// 설정이 완료되었으므로 다시 잠궈준다.
	Mesh->UnlockAttributeBuffer();

	//
	// Optimize the mesh to generate an attribute table.
	//
	// 메쉬의 개수의 3배만큼 벡터자료형으로 인접리스트를 만들어준다.
	std::vector<DWORD> adjacencyBuffer(Mesh->GetNumFaces() * 3);
	// 이메쉬의 인접정보를 받아온다.
	Mesh->GenerateAdjacency(0.0f, &adjacencyBuffer[0]);

	// 그 인접 정보를 기반으로 최적화 작업을 시작한다.
	hr = Mesh->OptimizeInplace(		
		// 속성으로(서브셋기준) 삼각형으로 정렬하고, 별도의 속성테이블을 생성
		// GetAttributeTable함수를 이용해서 D3DXATTRIBUTERANGE구조체 배열을 받아올 수 있다.
		// 내부 정보에는 서브셋ID / 각 면과 버텍스의 수 / 각 시작 지점이 들어있다.
		// 속성 테이블의 각항목은 메쉬의 각 서브셋과 대응되며, 
		// 서브셋의 기하정보들이 보관되는 버텍스/ 인덱스 버퍼내의 메모리 블록을 지정한다.
		D3DXMESHOPT_ATTRSORT |

		// 메쉬에서 이용하지 않는 인덱스와 버텍스를 제거한다.
		D3DXMESHOPT_COMPACT  |

		// 버텍스 캐시의 히트율을 높인다.
		D3DXMESHOPT_VERTEXCACHE,

		// 최적화 되지않은 메쉬의 인접 배열 포인터
		// 인접배열이 필요한이유?	// 최적화 목록에 인접정보가 필요한 부분이 있어서 필요할듯
		// Mesh클래스 내부적으로 인접정보를 가지고 있으면 굳이 파라미터로 안넘겨도 되지않을까?
		&adjacencyBuffer[0],

		// 최적화된 메쉬의 인접 배열 포인터
		0,

		// 리맵정보 Face
		0, 

		// 리맵정보 Vertex
		0);

	//
	// Dump the Mesh Data to file.
	//

	// 나중에 디버깅용으로 편리하다.
	// 덤프데이터를 만들기 위한 작업
	// 파일을 열고
	OutFile.open("Mesh Dump.txt");

	// 버텍스 / 인덱스 / 속성테이블 / 속성데이터(서브셋ID) / 인접삼각형 배열 버퍼
	// 정보를 저장하고
	dumpVertices(OutFile, Mesh);
	dumpIndices(OutFile, Mesh);
	dumpAttributeTable(OutFile, Mesh); 	
	dumpAttributeBuffer(OutFile, Mesh);		
	dumpAdjacencyBuffer(OutFile, Mesh);
	

	// 파일을 닫는다.
	OutFile.close();

	//
	// Load the textures and set filters.
	//

	// 서브셋에 맞는 텍스처를 파일을 열어서 만들어주고 있다.
	D3DXCreateTextureFromFile(
		Device,
		"brick0.jpg",
		&Textures[0]);

	D3DXCreateTextureFromFile(
		Device,
		"brick1.jpg",
		&Textures[1]);

	D3DXCreateTextureFromFile(
		Device,
		"checker.jpg",
		&Textures[2]);

	// 텍스처 기본 세팅
	Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	// 
	// Disable lighting.
	//

	Device->SetRenderState(D3DRS_LIGHTING, false);

	//
	// Set camera.
	//

	D3DXVECTOR3 pos(0.0f, 0.f, -4.0f);
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
	d3d::Release<ID3DXMesh*>(Mesh);
	d3d::Release<IDirect3DTexture9*>(Textures[0]);
	d3d::Release<IDirect3DTexture9*>(Textures[1]);
	d3d::Release<IDirect3DTexture9*>(Textures[2]);
}

bool Display(float timeDelta)
{
	if( Device )
	{
		//
		// Update: Rotate the cube.
		//

		D3DXMATRIX xRot;
		D3DXMatrixRotationX(&xRot, D3DX_PI * 0.2f);

		static float y = 0.0f;
		D3DXMATRIX yRot;
		D3DXMatrixRotationY(&yRot, y);
		y += timeDelta;

		if( y >= 6.28f )
			y = 0.0f;

		D3DXMATRIX World = xRot * yRot;

		Device->SetTransform(D3DTS_WORLD, &World);

		//
		// Render
		//

		// 소거
		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
		Device->BeginScene();

		// 서브셋 개수만큼 그려준다.
		// 메쉬는 서브셋 기준으로 최적화가 되어있는 상태이다.
		for(int i = 0; i < NumSubsets; i++)
		{
			Device->SetTexture( 0, Textures[i] );
			Mesh->DrawSubset( i );
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

//
// Prototype Implementations
//
// 각 정보를 텍스트 파일에 저장한다.
// 잠그고 저장후 다시 풀어준다.
void dumpVertices(std::ofstream& outFile, ID3DXMesh* mesh)
{
	outFile << "Vertices:" << std::endl;
	outFile << "---------" << std::endl << std::endl;

	Vertex* v = 0;
	mesh->LockVertexBuffer(0, (void**)&v);
	for(int i = 0; i < mesh->GetNumVertices(); i++)
	{
		outFile << "Vertex " << i << ": (";
		outFile << v[i]._x  << ", " << v[i]._y  << ", " << v[i]._z  << ", ";
		outFile << v[i]._nx << ", " << v[i]._ny << ", " << v[i]._nz << ", ";
		outFile << v[i]._u  << ", " << v[i]._v  << ")"  << std::endl;
	}
	mesh->UnlockVertexBuffer();

	outFile << std::endl << std::endl;
}

void dumpIndices(std::ofstream& outFile, ID3DXMesh* mesh)
{
	outFile << "Indices:" << std::endl;
	outFile << "--------" << std::endl << std::endl;

	WORD* indices = 0;
	mesh->LockIndexBuffer(0, (void**)&indices);

	for(int i = 0; i < mesh->GetNumFaces(); i++)
	{
		outFile << "Triangle " << i << ": ";
		outFile << indices[i * 3    ] << " ";
		outFile << indices[i * 3 + 1] << " ";
		outFile << indices[i * 3 + 2] << std::endl;
	}
	mesh->UnlockIndexBuffer();

	outFile << std::endl << std::endl;
}

void dumpAttributeBuffer(std::ofstream& outFile, ID3DXMesh* mesh)
{
	outFile << "Attribute Buffer:" << std::endl;
	outFile << "-----------------" << std::endl << std::endl;

	DWORD* attributeBuffer = 0;
	mesh->LockAttributeBuffer(0, &attributeBuffer);

	// an attribute for each face
	for(int i = 0; i < mesh->GetNumFaces(); i++)
	{
		outFile << "Triangle lives in subset " << i << ": ";
		outFile << attributeBuffer[i] << std::endl;
	}
	mesh->UnlockAttributeBuffer();

	outFile << std::endl << std::endl;
}

void dumpAdjacencyBuffer(std::ofstream& outFile, ID3DXMesh* mesh)
{
	outFile << "Adjacency Buffer:" << std::endl;
	outFile << "-----------------" << std::endl << std::endl;

	// three enttries per face
	std::vector<DWORD> adjacencyBuffer(mesh->GetNumFaces() * 3);

	mesh->GenerateAdjacency(0.0f, &adjacencyBuffer[0]);

	for(int i = 0; i < mesh->GetNumFaces(); i++)
	{
		outFile << "Triangle's adjacent to triangle " << i << ": ";
		outFile << adjacencyBuffer[i * 3    ] << " ";
		outFile << adjacencyBuffer[i * 3 + 1] << " ";
		outFile << adjacencyBuffer[i * 3 + 2] << std::endl;
	}

	outFile << std::endl << std::endl;
}

void dumpAttributeTable(std::ofstream& outFile, ID3DXMesh* mesh)
{
	outFile << "Attribute Table:" << std::endl;
	outFile << "----------------" << std::endl << std::endl;	

	// number of entries in the attribute table
	DWORD numEntries = 0;

	mesh->GetAttributeTable(0, &numEntries);

	std::vector<D3DXATTRIBUTERANGE> table(numEntries);

	mesh->GetAttributeTable(&table[0], &numEntries);

	for(int i = 0; i < numEntries; i++)
	{
		outFile << "Entry " << i << std::endl;
		outFile << "-----------" << std::endl;

		outFile << "Subset ID:    " << table[i].AttribId    << std::endl;
		outFile << "Face Start:   " << table[i].FaceStart   << std::endl;
		outFile << "Face Count:   " << table[i].FaceCount   << std::endl;
		outFile << "Vertex Start: " << table[i].VertexStart << std::endl;
		outFile << "Vertex Count: " << table[i].VertexCount << std::endl;
		outFile << std::endl;
	}

	outFile << std::endl << std::endl;
}
