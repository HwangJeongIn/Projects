//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: silhouetteEdges.cpp
// 
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Generates the silhouette geometry of a mesh and renders it.  Note
//       that we assume mesh vertex formats as described in MeshVertex.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "silhouetteEdges.h"

// 이 포맷은 기본적인 도형을 생성하는 함수를 사용했을때 나오는 포멧이랑 일치한다.
const DWORD MeshVertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL;

SilhouetteEdges::SilhouetteEdges(IDirect3DDevice9* device,
								 ID3DXMesh* mesh, 
								 ID3DXBuffer* adjBuffer)
{
	// 디바이스 빼고 나머지를 초기화 해준다.
	_device   = device;
	_vb       = 0;
	_ib       = 0;
	_decl     = 0;
	_numVerts = 0;
	_numFaces = 0;
	// 인접버퍼와 메쉬클래스 객체를 이용해서 윤곽버텍스를 생성한다.
	genEdgeVertices(mesh, adjBuffer);
	genEdgeIndices(mesh);

	// 매핑하기 위한 버텍스정보 선언을 생성한다.
	createVertexDeclaration();
}

SilhouetteEdges::~SilhouetteEdges()
{
	d3d::Release<IDirect3DVertexBuffer9*>(_vb);
	d3d::Release<IDirect3DIndexBuffer9*>(_ib);
	d3d::Release<IDirect3DVertexDeclaration9*>(_decl);
}

bool SilhouetteEdges::createVertexDeclaration()
{
	HRESULT hr = 0;

	D3DVERTEXELEMENT9 decl[] = 
	{
		/*
		나중에 셰이더 코드의 구조체
		struct VS_INPUT
		{
			vector positi : POSITION // POSITION0
						  : NORMAL0
						  : NORMAL1
						  : NORMAL2
		}
		과 매핑된다.

		*/
		// offsets in bytes
		{0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0},
		{0, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   1},
		{0, 36, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   2},
		D3DDECL_END()
	};

	// 버텍스 선언을 만들어준다.
	hr = _device->CreateVertexDeclaration(decl, &_decl);
	if(FAILED(hr))
	{
		::MessageBox(0, "CreateVertexDeclaration() - FAILED", 0, 0);
		return false;
	}

	return true;
}

void SilhouetteEdges::getFaceNormal(ID3DXMesh* mesh,
									DWORD faceIndex,
									D3DXVECTOR3* faceNormal)
{
	MeshVertex* vertices = 0;
	mesh->LockVertexBuffer(0, (void**)&vertices);

	WORD* indices = 0;
	mesh->LockIndexBuffer(0, (void**)&indices);

	// Get the triangle's indices
	// 삼각형번호를 가지고 그 삼각형을 이루는 버텍스의 인덱스들을 받아온다
	// 몇번째 삼각형인지에 따라서 삼각형 인덱스가 달라진다.
	WORD index0 = indices[faceIndex * 3];
	WORD index1 = indices[faceIndex * 3 + 1];
	WORD index2 = indices[faceIndex * 3 + 2];

	// Now extract the triangles vertices positions
	// 버텍스 인덱스를 가지고 버텍스의 값을 구해준다.
	D3DXVECTOR3 v0 = vertices[index0].position;
	D3DXVECTOR3 v1 = vertices[index1].position;
	D3DXVECTOR3 v2 = vertices[index2].position;

	// Compute face normal
	D3DXVECTOR3 edge0, edge1;

	// v0에서 v1, v2로의 벡터를 구해준다.
	edge0 = v1 - v0;
	edge1 = v2 - v0;

	// 현재 면에 대한 법선벡터를 구해준다.
	// 외적을할때도 주의해야할점은 첫번째 외곽선과 두번째 외곽선을 순서에 맞게 넣어주어야 한다는 것이다.

	/*
			   v2
		   	   /\
			  /  \
			 /____\
			v0	   v1
	*/

	// 시계 : 앞면 // 반시계 : 뒷면 을 나타내는 것이므로
	// 앞면 기준으로 저 버텍스들을 그림으로 표현해보면 시계방향으로 번호가 매겨진다.

	// 이렇게 버텍스들이 방향을 가지기 때문에 두개의 외적값이 다를수밖에없다.

	// edge0 cross edge1 : 평면기준 윗쪽방향 법선벡터
	// edge1 cross edge0 : 평면기준 아랫쪽방향 법선벡터


	D3DXVec3Cross(faceNormal, &edge0, &edge1);

	// 순수 방향만 나타내기 위해서 정규화시킨다
	D3DXVec3Normalize(faceNormal, faceNormal);

	// 다시 잠궈준다.
	mesh->UnlockVertexBuffer();
	mesh->UnlockIndexBuffer();
}

void SilhouetteEdges::getFaceNormals(ID3DXMesh*   mesh,
								     ID3DXBuffer* adjBuffer,
									 D3DXVECTOR3* currentFaceNormal,
									 D3DXVECTOR3  adjFaceNormals[3],
									 DWORD        faceIndex)

{
	// 메쉬의 버텍스 버퍼를 받아온다
	MeshVertex* vertices = 0;
	mesh->LockVertexBuffer(0, (void**)&vertices);

	// 메쉬의 인덱스 버퍼를 받아온다.
	WORD* indices = 0;
	mesh->LockIndexBuffer(0, (void**)&indices);

	// 인접정보 배열을 받아온다.
	DWORD* adj = (DWORD*)adjBuffer->GetBufferPointer();

	//
	// Get the face normal.
	// 현재삼각형의 법선벡터를 구해준다.
	getFaceNormal(mesh, faceIndex, currentFaceNormal);

	//
	// Get adjacent face indices
	// 인접면들의 인덱스를 구해준다.
	// 한면당 3가지 인덱스를 가지고 있다(삼각형의 인접할수있는 삼각형은 최대 3개이다.) 
	// 단 인접한면이 없을때는 3개다 -1로 지정한다

	// 1번째 인접인덱스 : 버텍스 인덱스의 1번쨰와 2번째에 의해 만들어지는 외곽선에 인접하는 삼각형의 인덱스
	// 2번째 인접인덱스 : 버텍스 인덱스의 2번쨰와 3번째에 의해 만들어지는 외곽선에 인접하는 삼각형의 인덱스
	// 3번째 인접인덱스 : 버텍스 인덱스의 1번쨰와 3번째에 의해 만들어지는 외곽선에 인접하는 삼각형의 인덱스 
	WORD faceIndex0 = adj[faceIndex * 3];
	WORD faceIndex1 = adj[faceIndex * 3 + 1];
	WORD faceIndex2 = adj[faceIndex * 3 + 2];

	//
	// Get adjacent face normals, if there is no adjacent face,
	// then set the adjacent face normal to the opposite of the
	// "currentFaceNormal".  Recall we do this because edges that
	// don't have an adjacent triangle are automatically considered
	// silhouette edges.  And in order to make that happen, we need
	// the current face normal and adjacent face normal to point
	// in the opposite direction.  Also, recall that an entry
	// in the adjacency buffer equal to -1 denotes that the edge
	// doesn't have an adjacent triangle.

	/*
	인접면X >> 그 모서리는 실루엣모서리로 간주된다.
	현재 면의 법선의 반대방향으로 설정
	*/

	D3DXVECTOR3 faceNormal0, faceNormal1, faceNormal2;
    
	// 첫번째 인접한 면에 대해서 계산해준다.
	if( faceIndex0 != USHRT_MAX ) // is there an adjacent triangle?
	{
		// 인접한 면이 존재하면
		// 면의 3개의 버텍스를 구해준다

		// 나중에 셰이더 코드에서
		// 면이있을때 실루엣 외곽선 판별은 그냥 계산으로 쉽게 나온다

		WORD i0 = indices[faceIndex0 * 3];
		WORD i1 = indices[faceIndex0 * 3 + 1];
		WORD i2 = indices[faceIndex0 * 3 + 2];

		D3DXVECTOR3 v0 = vertices[i0].position;
		D3DXVECTOR3 v1 = vertices[i1].position;
		D3DXVECTOR3 v2 = vertices[i2].position;

		// 그버텍스 기준으로 삼각형을 만들고 그 삼각형의 법선벡터를 설정해준다.
		D3DXVECTOR3 edge0 = v1 - v0;
		D3DXVECTOR3 edge1 = v2 - v0;
		D3DXVec3Cross(&faceNormal0, &edge0, &edge1);
		D3DXVec3Normalize(&faceNormal0, &faceNormal0);
	}
	else
	{
		// 인접면이 없는 경우
		// 실루엣 외곽선이기 때문에 법선벡터를 현재면 법선벡터의 반대로 지정해준다
		// 이는 무조건 음수가 나오게 하기 위해서 그냥 설정해준값이다. 나중에 부등식을 성립하게 하기 위해서
		// 없는 삼각형의 법선벡터를 그냥 설정해준것이다.
		faceNormal0 = -(*currentFaceNormal);
	}

	if( faceIndex1 != USHRT_MAX ) // is there an adjacent triangle?
	{
		WORD i0 = indices[faceIndex1 * 3];
		WORD i1 = indices[faceIndex1 * 3 + 1];
		WORD i2 = indices[faceIndex1 * 3 + 2];

		D3DXVECTOR3 v0 = vertices[i0].position;
		D3DXVECTOR3 v1 = vertices[i1].position;
		D3DXVECTOR3 v2 = vertices[i2].position;

		D3DXVECTOR3 edge0 = v1 - v0;
		D3DXVECTOR3 edge1 = v2 - v0;
		D3DXVec3Cross(&faceNormal1, &edge0, &edge1);
		D3DXVec3Normalize(&faceNormal1, &faceNormal1);
	}
	else
	{
		faceNormal1 = -(*currentFaceNormal);
	}

	if( faceIndex2 != USHRT_MAX ) // is there an adjacent triangle?
	{
		WORD i0 = indices[faceIndex2 * 3];
		WORD i1 = indices[faceIndex2 * 3 + 1];
		WORD i2 = indices[faceIndex2 * 3 + 2];

		D3DXVECTOR3 v0 = vertices[i0].position;
		D3DXVECTOR3 v1 = vertices[i1].position;
		D3DXVECTOR3 v2 = vertices[i2].position;

		D3DXVECTOR3 edge0 = v1 - v0;
		D3DXVECTOR3 edge1 = v2 - v0;
		D3DXVec3Cross(&faceNormal2, &edge0, &edge1);
		D3DXVec3Normalize(&faceNormal2, &faceNormal2);
	}
	else
	{
		faceNormal2 = -(*currentFaceNormal);
	}

	// save adjacent face normals
	// 인접면들의 법선벡터들도 다 초기화 시켜준다.
	adjFaceNormals[0] = faceNormal0;
	adjFaceNormals[1] = faceNormal1;
	adjFaceNormals[2] = faceNormal2;

	mesh->UnlockVertexBuffer();
	mesh->UnlockIndexBuffer();
}

void SilhouetteEdges::genEdgeVertices(ID3DXMesh* mesh,
									  ID3DXBuffer* adjBuffer)
{
	// 3 edges per face and 4 vertices per edge
	// 한면당 3개의 외곽선(선분)을 가지며, 그 외곽선은 4개의 버텍스로 이루어진다 
	// 삼각형 2개로 실루엣 외곽선을 표현하기 때문이다.

	/*
	struct EdgeVertex
	{
		// position 1개 / normal 3개로 되어있다.
		D3DXVECTOR3 position;
		D3DXVECTOR3 normal;
		D3DXVECTOR3 faceNormal1;
		D3DXVECTOR3 faceNormal2;
	};
	*/
	// EdgeVertex는 다음과 같이 1개의 포지션과 3개의 법선벡터로 되어있는데,
	// 이는 셰이더코드에서 계산할때 필요한것과 일치한다.
	// 또 셰이더 코드의 입력 구조체는 버텍스 정보 선언과 대응되기 때문에 당연히 버텍스 정보 선언과도 일치한다.
	
	// 그 구조체 기준으로 넘겨줘야 하기 때문에 한 버텍스에는 저정보들이 다 들어간다.
	_numVerts = mesh->GetNumFaces() * 3 * 4;
	_device->CreateVertexBuffer(
		_numVerts * sizeof(EdgeVertex),
		D3DUSAGE_WRITEONLY,

		// FVF는 버텍스 정보 선언을 사용하기 때문에 사용하지 않는다.
		// 버텍스 정보 선언은 FVF보다 확장된 버전이라고 보면된다.
		0, // using vertex declaration
		D3DPOOL_MANAGED,
		&_vb,
		0);

	// 두가지는 포인터로 메쉬에 있는 인덱스버퍼와 버텍스 버퍼를 받아오고, 하나는 생성해준 포인터를 받아온다.
	// 아래에서 그 정보들로 계산을 해줄것이다.
	MeshVertex* vertices = 0;
	mesh->LockVertexBuffer(0, (void**)&vertices);

	WORD* indices = 0;
	mesh->LockIndexBuffer(0, (void**)&indices);

	EdgeVertex* edgeVertices = 0;
	_vb->Lock(0, 0, (void**)&edgeVertices, 0);

	/*
	각면에 대해서
	1. 인접면들과 현재면에 대한 법선벡터를 구한다.
	2. 현재면의 버텍스 2지점을 정해서 선분AB을 만들어준다
	3. 그 2개의 지점(A, B)에 움직일수 있는(법선벡터가 0이 아님) 버텍스 2개(a, b)와 법선벡터가 0인 버텍스 2개(c, d)를 만들어준다
	4. 이때 버텍스 포맷은 위치 / 법선벡터 / 인접한면의법선벡터1 / 인접한면의법선벡터2 이다.
	// 이를 정하기위해서 버텍스 2지점으로 만들어진 선분 기준으로 인접한 면의 법선벡터 2개를 구해준다.
	// 만약에 실루엣 외곽선 판정이 나면(인접한면1 : 앞면 && 인접한면2 : 뒷면 || 인접한면1 : O && 인접한면2 : X)
	// 버텍스a,b는 버텍스 법선 방향으로 이동하게되고 넓이가 0이였던 삼각형 두개는 넓이를 가진 사각형으로 변하게되면서 실루엣 외곽선이 보이게된다.
	*/
	for(int i = 0; i < mesh->GetNumFaces(); i++)
	{
		// 구해주어야 할것은 
		// 현재 버택스의 법선벡터
		// 현재 버텍스가 포함
		D3DXVECTOR3 currentFaceNormal;
		D3DXVECTOR3 adjFaceNormals[3];

		// 메쉬의 현재 버텍스의 법선벡터와 인접면의 버텍스 버퍼를 모두 구해준다.
		// 인덱스 3개로 이루어진 삼각형의 인접삼각형의 번호를 가지고 있는 인접정보 배열을 사용할것이다
		// 현재 삼각형 번호도 넣어준다.
		getFaceNormals(mesh, adjBuffer, &currentFaceNormal, adjFaceNormals, i);

		// get the indices for this face
		// 메쉬에서 인덱스의 연속된 3개의 값을 받아온다
		WORD index0 = indices[i * 3];
		WORD index1 = indices[i * 3 + 1];
		WORD index2 = indices[i * 3 + 2];

		// get the vertices for this face
		// 3개의 버텍스의 인덱스를 통해서 3개의 버텍스를 받아서 저장한다.
		/*
			struct MeshVertex
			{
				D3DXVECTOR3 position;
				D3DXVECTOR3 normal;
				static const DWORD FVF;
			};
		*/

		// 현재 삼각형의 버텍스를 받아온다.
		MeshVertex v0 = vertices[index0];
		MeshVertex v1 = vertices[index1];
		MeshVertex v2 = vertices[index2];
		
		// A        B
		// *--------*
		// |  edge  |
		// *--------*
		// C        D
		// note, C and D are duplicates of A and B respectively, 
		// such that the quad is degenerate.  The vertex shader
		// will un-degenerate the quad if it is a silhouette edge.

		// 일단 넓이가 0인 삼각형을 2개 이어서 사각형을 만들자
		// 나중에 셰어더에의해서 다시 넓이있는 사각형으로 변할것이다.

		/*
				v2
				/\
			   /  \
			  /____\
			v0		v1
		*/

		// 3개의 외곽선에 대해서 계산을 해주어야 한다.
		

		// 1번째 인접인덱스 : 버텍스 인덱스의 1번쨰와 2번째에 의해 만들어지는 외곽선에 인접하는 삼각형의 인덱스
		// 2번째 인접인덱스 : 버텍스 인덱스의 2번쨰와 3번째에 의해 만들어지는 외곽선에 인접하는 삼각형의 인덱스
		// 3번째 인접인덱스 : 버텍스 인덱스의 1번쨰와 3번째에 의해 만들어지는 외곽선에 인접하는 삼각형의 인덱스 
		// compute edge0 v0->v1, note adjacent face
		// normal is faceNormal0
		EdgeVertex A0, B0, C0, D0;

		// 변하지않는 지점1
		A0.position    = v0.position;
		A0.normal      = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		A0.faceNormal1 = currentFaceNormal;
		// v0->v1과 접하는 현재면과 아닌 인접면의 법선벡터
		A0.faceNormal2 = adjFaceNormals[0];

		// 변하지않는 지점2
		B0.position    = v1.position;
		B0.normal      = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		B0.faceNormal1 = currentFaceNormal;
		// v0->v1과 접하는 현재면과 아닌 인접면의 법선벡터
		B0.faceNormal2 = adjFaceNormals[0];

		// 나머지 정보는 다 같지만 셰이더에 의해 사각형을 만들 수 있게 법선벡터를 설정하였다
		// 셰이더는 실루엣 외곽선 검사를 하여 맞으면 이 법선벡터 방향으로 조금씩 더해준다.
		C0 = A0;
		C0.normal = v0.normal;

		D0 = B0;
		D0.normal = v1.normal;

		// 셰이더에 쓰일 버텍스 형식에 넣어주고 카운트를 올려준다.
		*edgeVertices = A0; ++edgeVertices;
		*edgeVertices = B0; ++edgeVertices;
		*edgeVertices = C0; ++edgeVertices;
		*edgeVertices = D0; ++edgeVertices;	

		// compute edge0 v1->v2, note adjacent face
		// normal is faceNormal1
		EdgeVertex A1, B1, C1, D1;

		A1.position    = v1.position;
		A1.normal      = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		A1.faceNormal1 = currentFaceNormal;
		// v1->v2과 접하는 현재면과 아닌 인접면의 법선벡터
		A1.faceNormal2 = adjFaceNormals[1];

		B1.position    = v2.position;
		B1.normal      = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		B1.faceNormal1 = currentFaceNormal;
		// v1->v2과 접하는 현재면과 아닌 인접면의 법선벡터
		B1.faceNormal2 = adjFaceNormals[1];

	
		C1 = A1;
		C1.normal = v1.normal;
		
		D1 = B1;
		D1.normal = v2.normal;

		*edgeVertices = A1; ++edgeVertices;
		*edgeVertices = B1; ++edgeVertices;
		*edgeVertices = C1; ++edgeVertices;
		*edgeVertices = D1; ++edgeVertices;	

		// compute edge0 v0->v2, note adjacent face
		// normal is faceNormal2
		EdgeVertex A2, B2, C2, D2;

		A2.position    = v0.position;
		A2.normal      = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		A2.faceNormal1 = currentFaceNormal;
		// v0->v2과 접하는 현재면과 아닌 인접면의 법선벡터
		A2.faceNormal2 = adjFaceNormals[2];

		B2.position    = v2.position;
		B2.normal      = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		B2.faceNormal1 = currentFaceNormal;
		// v0->v2과 접하는 현재면과 아닌 인접면의 법선벡터
		B2.faceNormal2 = adjFaceNormals[2];

		C2 = A2;
		C2.normal = v0.normal;
		
		D2 = B2;
		D2.normal = v2.normal;

		*edgeVertices = A2; ++edgeVertices;
		*edgeVertices = B2; ++edgeVertices;
		*edgeVertices = C2; ++edgeVertices;
		*edgeVertices = D2; ++edgeVertices;	
	}

	_vb->Unlock();
	mesh->UnlockVertexBuffer();
	mesh->UnlockIndexBuffer();
}

void SilhouetteEdges::genEdgeIndices(ID3DXMesh* mesh)
{
	// 1개의 면은 3개의 외곽선으로 이루어져있다.
	DWORD numEdges = mesh->GetNumFaces() * 3;

	// 하나의 외곽선은 2개의 삼각형으로 이루어져있다.
	_numFaces = numEdges * 2;
	
	_device->CreateIndexBuffer(
		// 2개의 삼각형을 이루기 위한 인덱스의 갯수는 6개이다.
		numEdges * 6 * sizeof(WORD), // 2 triangles per edge
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&_ib,
		0);

	WORD* indices = 0;

	_ib->Lock(0, 0, (void**)&indices, 0);

	// 0        1
	// *--------*
	// |  edge  |
	// *--------*
	// 2        3

	for(UINT i = 0; i < numEdges; i++)
	{
		// Six indices to define the triangles of the edge,
		// so every edge we skip six entries in the 
		// index buffer.  Four vertices to define the edge,
		// so every edge we skip four entries in the 
		// vertex buffer.

		// 사각형을 만드는 버텍스는 4개씩 저장되어있다.
		// 그것을 3개 3개로 쪼개서 인덱스에 저장하자

		indices[i * 6]     = i * 4 + 0;
		indices[i * 6 + 1] = i * 4 + 1;
		indices[i * 6 + 2] = i * 4 + 2;
		indices[i * 6 + 3] = i * 4 + 1;
		indices[i * 6 + 4] = i * 4 + 3;
		indices[i * 6 + 5] = i * 4 + 2;
	}

	_ib->Unlock();
}

void SilhouetteEdges::render()
{
	// 버텍스 선언을 활성화시킨다.
	_device->SetVertexDeclaration(_decl);

	// 스트림을 정해준다.
	_device->SetStreamSource(0, _vb, 0, sizeof(EdgeVertex));
	_device->SetIndices(_ib);

	_device->DrawIndexedPrimitive(
		D3DPT_TRIANGLELIST, 0, 0, _numVerts, 0, _numFaces);
}
