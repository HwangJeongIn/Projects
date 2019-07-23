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

// �� ������ �⺻���� ������ �����ϴ� �Լ��� ��������� ������ �����̶� ��ġ�Ѵ�.
const DWORD MeshVertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL;

SilhouetteEdges::SilhouetteEdges(IDirect3DDevice9* device,
								 ID3DXMesh* mesh, 
								 ID3DXBuffer* adjBuffer)
{
	// ����̽� ���� �������� �ʱ�ȭ ���ش�.
	_device   = device;
	_vb       = 0;
	_ib       = 0;
	_decl     = 0;
	_numVerts = 0;
	_numFaces = 0;
	// �������ۿ� �޽�Ŭ���� ��ü�� �̿��ؼ� �������ؽ��� �����Ѵ�.
	genEdgeVertices(mesh, adjBuffer);
	genEdgeIndices(mesh);

	// �����ϱ� ���� ���ؽ����� ������ �����Ѵ�.
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
		���߿� ���̴� �ڵ��� ����ü
		struct VS_INPUT
		{
			vector positi : POSITION // POSITION0
						  : NORMAL0
						  : NORMAL1
						  : NORMAL2
		}
		�� ���εȴ�.

		*/
		// offsets in bytes
		{0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0},
		{0, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   1},
		{0, 36, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   2},
		D3DDECL_END()
	};

	// ���ؽ� ������ ������ش�.
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
	// �ﰢ����ȣ�� ������ �� �ﰢ���� �̷�� ���ؽ��� �ε������� �޾ƿ´�
	// ���° �ﰢ�������� ���� �ﰢ�� �ε����� �޶�����.
	WORD index0 = indices[faceIndex * 3];
	WORD index1 = indices[faceIndex * 3 + 1];
	WORD index2 = indices[faceIndex * 3 + 2];

	// Now extract the triangles vertices positions
	// ���ؽ� �ε����� ������ ���ؽ��� ���� �����ش�.
	D3DXVECTOR3 v0 = vertices[index0].position;
	D3DXVECTOR3 v1 = vertices[index1].position;
	D3DXVECTOR3 v2 = vertices[index2].position;

	// Compute face normal
	D3DXVECTOR3 edge0, edge1;

	// v0���� v1, v2���� ���͸� �����ش�.
	edge0 = v1 - v0;
	edge1 = v2 - v0;

	// ���� �鿡 ���� �������͸� �����ش�.
	// �������Ҷ��� �����ؾ������� ù��° �ܰ����� �ι�° �ܰ����� ������ �°� �־��־�� �Ѵٴ� ���̴�.

	/*
			   v2
		   	   /\
			  /  \
			 /____\
			v0	   v1
	*/

	// �ð� : �ո� // �ݽð� : �޸� �� ��Ÿ���� ���̹Ƿ�
	// �ո� �������� �� ���ؽ����� �׸����� ǥ���غ��� �ð�������� ��ȣ�� �Ű�����.

	// �̷��� ���ؽ����� ������ ������ ������ �ΰ��� �������� �ٸ����ۿ�����.

	// edge0 cross edge1 : ������ ���ʹ��� ��������
	// edge1 cross edge0 : ������ �Ʒ��ʹ��� ��������


	D3DXVec3Cross(faceNormal, &edge0, &edge1);

	// ���� ���⸸ ��Ÿ���� ���ؼ� ����ȭ��Ų��
	D3DXVec3Normalize(faceNormal, faceNormal);

	// �ٽ� ����ش�.
	mesh->UnlockVertexBuffer();
	mesh->UnlockIndexBuffer();
}

void SilhouetteEdges::getFaceNormals(ID3DXMesh*   mesh,
								     ID3DXBuffer* adjBuffer,
									 D3DXVECTOR3* currentFaceNormal,
									 D3DXVECTOR3  adjFaceNormals[3],
									 DWORD        faceIndex)

{
	// �޽��� ���ؽ� ���۸� �޾ƿ´�
	MeshVertex* vertices = 0;
	mesh->LockVertexBuffer(0, (void**)&vertices);

	// �޽��� �ε��� ���۸� �޾ƿ´�.
	WORD* indices = 0;
	mesh->LockIndexBuffer(0, (void**)&indices);

	// �������� �迭�� �޾ƿ´�.
	DWORD* adj = (DWORD*)adjBuffer->GetBufferPointer();

	//
	// Get the face normal.
	// ����ﰢ���� �������͸� �����ش�.
	getFaceNormal(mesh, faceIndex, currentFaceNormal);

	//
	// Get adjacent face indices
	// ��������� �ε����� �����ش�.
	// �Ѹ�� 3���� �ε����� ������ �ִ�(�ﰢ���� �����Ҽ��ִ� �ﰢ���� �ִ� 3���̴�.) 
	// �� �����Ѹ��� �������� 3���� -1�� �����Ѵ�

	// 1��° �����ε��� : ���ؽ� �ε����� 1������ 2��°�� ���� ��������� �ܰ����� �����ϴ� �ﰢ���� �ε���
	// 2��° �����ε��� : ���ؽ� �ε����� 2������ 3��°�� ���� ��������� �ܰ����� �����ϴ� �ﰢ���� �ε���
	// 3��° �����ε��� : ���ؽ� �ε����� 1������ 3��°�� ���� ��������� �ܰ����� �����ϴ� �ﰢ���� �ε��� 
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
	������X >> �� �𼭸��� �Ƿ翧�𼭸��� ���ֵȴ�.
	���� ���� ������ �ݴ�������� ����
	*/

	D3DXVECTOR3 faceNormal0, faceNormal1, faceNormal2;
    
	// ù��° ������ �鿡 ���ؼ� ������ش�.
	if( faceIndex0 != USHRT_MAX ) // is there an adjacent triangle?
	{
		// ������ ���� �����ϸ�
		// ���� 3���� ���ؽ��� �����ش�

		// ���߿� ���̴� �ڵ忡��
		// ���������� �Ƿ翧 �ܰ��� �Ǻ��� �׳� ������� ���� ���´�

		WORD i0 = indices[faceIndex0 * 3];
		WORD i1 = indices[faceIndex0 * 3 + 1];
		WORD i2 = indices[faceIndex0 * 3 + 2];

		D3DXVECTOR3 v0 = vertices[i0].position;
		D3DXVECTOR3 v1 = vertices[i1].position;
		D3DXVECTOR3 v2 = vertices[i2].position;

		// �׹��ؽ� �������� �ﰢ���� ����� �� �ﰢ���� �������͸� �������ش�.
		D3DXVECTOR3 edge0 = v1 - v0;
		D3DXVECTOR3 edge1 = v2 - v0;
		D3DXVec3Cross(&faceNormal0, &edge0, &edge1);
		D3DXVec3Normalize(&faceNormal0, &faceNormal0);
	}
	else
	{
		// �������� ���� ���
		// �Ƿ翧 �ܰ����̱� ������ �������͸� ����� ���������� �ݴ�� �������ش�
		// �̴� ������ ������ ������ �ϱ� ���ؼ� �׳� �������ذ��̴�. ���߿� �ε���� �����ϰ� �ϱ� ���ؼ�
		// ���� �ﰢ���� �������͸� �׳� �������ذ��̴�.
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
	// ��������� �������͵鵵 �� �ʱ�ȭ �����ش�.
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
	// �Ѹ�� 3���� �ܰ���(����)�� ������, �� �ܰ����� 4���� ���ؽ��� �̷������ 
	// �ﰢ�� 2���� �Ƿ翧 �ܰ����� ǥ���ϱ� �����̴�.

	/*
	struct EdgeVertex
	{
		// position 1�� / normal 3���� �Ǿ��ִ�.
		D3DXVECTOR3 position;
		D3DXVECTOR3 normal;
		D3DXVECTOR3 faceNormal1;
		D3DXVECTOR3 faceNormal2;
	};
	*/
	// EdgeVertex�� ������ ���� 1���� �����ǰ� 3���� �������ͷ� �Ǿ��ִµ�,
	// �̴� ���̴��ڵ忡�� ����Ҷ� �ʿ��ѰͰ� ��ġ�Ѵ�.
	// �� ���̴� �ڵ��� �Է� ����ü�� ���ؽ� ���� ����� �����Ǳ� ������ �翬�� ���ؽ� ���� ������� ��ġ�Ѵ�.
	
	// �� ����ü �������� �Ѱ���� �ϱ� ������ �� ���ؽ����� ���������� �� ����.
	_numVerts = mesh->GetNumFaces() * 3 * 4;
	_device->CreateVertexBuffer(
		_numVerts * sizeof(EdgeVertex),
		D3DUSAGE_WRITEONLY,

		// FVF�� ���ؽ� ���� ������ ����ϱ� ������ ������� �ʴ´�.
		// ���ؽ� ���� ������ FVF���� Ȯ��� �����̶�� ����ȴ�.
		0, // using vertex declaration
		D3DPOOL_MANAGED,
		&_vb,
		0);

	// �ΰ����� �����ͷ� �޽��� �ִ� �ε������ۿ� ���ؽ� ���۸� �޾ƿ���, �ϳ��� �������� �����͸� �޾ƿ´�.
	// �Ʒ����� �� ������� ����� ���ٰ��̴�.
	MeshVertex* vertices = 0;
	mesh->LockVertexBuffer(0, (void**)&vertices);

	WORD* indices = 0;
	mesh->LockIndexBuffer(0, (void**)&indices);

	EdgeVertex* edgeVertices = 0;
	_vb->Lock(0, 0, (void**)&edgeVertices, 0);

	/*
	���鿡 ���ؼ�
	1. �������� ����鿡 ���� �������͸� ���Ѵ�.
	2. ������� ���ؽ� 2������ ���ؼ� ����AB�� ������ش�
	3. �� 2���� ����(A, B)�� �����ϼ� �ִ�(�������Ͱ� 0�� �ƴ�) ���ؽ� 2��(a, b)�� �������Ͱ� 0�� ���ؽ� 2��(c, d)�� ������ش�
	4. �̶� ���ؽ� ������ ��ġ / �������� / �����Ѹ��ǹ�������1 / �����Ѹ��ǹ�������2 �̴�.
	// �̸� ���ϱ����ؼ� ���ؽ� 2�������� ������� ���� �������� ������ ���� �������� 2���� �����ش�.
	// ���࿡ �Ƿ翧 �ܰ��� ������ ����(�����Ѹ�1 : �ո� && �����Ѹ�2 : �޸� || �����Ѹ�1 : O && �����Ѹ�2 : X)
	// ���ؽ�a,b�� ���ؽ� ���� �������� �̵��ϰԵǰ� ���̰� 0�̿��� �ﰢ�� �ΰ��� ���̸� ���� �簢������ ���ϰԵǸ鼭 �Ƿ翧 �ܰ����� ���̰Եȴ�.
	*/
	for(int i = 0; i < mesh->GetNumFaces(); i++)
	{
		// �����־�� �Ұ��� 
		// ���� ���ý��� ��������
		// ���� ���ؽ��� ����
		D3DXVECTOR3 currentFaceNormal;
		D3DXVECTOR3 adjFaceNormals[3];

		// �޽��� ���� ���ؽ��� �������Ϳ� �������� ���ؽ� ���۸� ��� �����ش�.
		// �ε��� 3���� �̷���� �ﰢ���� �����ﰢ���� ��ȣ�� ������ �ִ� �������� �迭�� ����Ұ��̴�
		// ���� �ﰢ�� ��ȣ�� �־��ش�.
		getFaceNormals(mesh, adjBuffer, &currentFaceNormal, adjFaceNormals, i);

		// get the indices for this face
		// �޽����� �ε����� ���ӵ� 3���� ���� �޾ƿ´�
		WORD index0 = indices[i * 3];
		WORD index1 = indices[i * 3 + 1];
		WORD index2 = indices[i * 3 + 2];

		// get the vertices for this face
		// 3���� ���ؽ��� �ε����� ���ؼ� 3���� ���ؽ��� �޾Ƽ� �����Ѵ�.
		/*
			struct MeshVertex
			{
				D3DXVECTOR3 position;
				D3DXVECTOR3 normal;
				static const DWORD FVF;
			};
		*/

		// ���� �ﰢ���� ���ؽ��� �޾ƿ´�.
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

		// �ϴ� ���̰� 0�� �ﰢ���� 2�� �̾ �簢���� ������
		// ���߿� �ξ�������ؼ� �ٽ� �����ִ� �簢������ ���Ұ��̴�.

		/*
				v2
				/\
			   /  \
			  /____\
			v0		v1
		*/

		// 3���� �ܰ����� ���ؼ� ����� ���־�� �Ѵ�.
		

		// 1��° �����ε��� : ���ؽ� �ε����� 1������ 2��°�� ���� ��������� �ܰ����� �����ϴ� �ﰢ���� �ε���
		// 2��° �����ε��� : ���ؽ� �ε����� 2������ 3��°�� ���� ��������� �ܰ����� �����ϴ� �ﰢ���� �ε���
		// 3��° �����ε��� : ���ؽ� �ε����� 1������ 3��°�� ���� ��������� �ܰ����� �����ϴ� �ﰢ���� �ε��� 
		// compute edge0 v0->v1, note adjacent face
		// normal is faceNormal0
		EdgeVertex A0, B0, C0, D0;

		// �������ʴ� ����1
		A0.position    = v0.position;
		A0.normal      = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		A0.faceNormal1 = currentFaceNormal;
		// v0->v1�� ���ϴ� ������ �ƴ� �������� ��������
		A0.faceNormal2 = adjFaceNormals[0];

		// �������ʴ� ����2
		B0.position    = v1.position;
		B0.normal      = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		B0.faceNormal1 = currentFaceNormal;
		// v0->v1�� ���ϴ� ������ �ƴ� �������� ��������
		B0.faceNormal2 = adjFaceNormals[0];

		// ������ ������ �� ������ ���̴��� ���� �簢���� ���� �� �ְ� �������͸� �����Ͽ���
		// ���̴��� �Ƿ翧 �ܰ��� �˻縦 �Ͽ� ������ �� �������� �������� ���ݾ� �����ش�.
		C0 = A0;
		C0.normal = v0.normal;

		D0 = B0;
		D0.normal = v1.normal;

		// ���̴��� ���� ���ؽ� ���Ŀ� �־��ְ� ī��Ʈ�� �÷��ش�.
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
		// v1->v2�� ���ϴ� ������ �ƴ� �������� ��������
		A1.faceNormal2 = adjFaceNormals[1];

		B1.position    = v2.position;
		B1.normal      = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		B1.faceNormal1 = currentFaceNormal;
		// v1->v2�� ���ϴ� ������ �ƴ� �������� ��������
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
		// v0->v2�� ���ϴ� ������ �ƴ� �������� ��������
		A2.faceNormal2 = adjFaceNormals[2];

		B2.position    = v2.position;
		B2.normal      = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		B2.faceNormal1 = currentFaceNormal;
		// v0->v2�� ���ϴ� ������ �ƴ� �������� ��������
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
	// 1���� ���� 3���� �ܰ������� �̷�����ִ�.
	DWORD numEdges = mesh->GetNumFaces() * 3;

	// �ϳ��� �ܰ����� 2���� �ﰢ������ �̷�����ִ�.
	_numFaces = numEdges * 2;
	
	_device->CreateIndexBuffer(
		// 2���� �ﰢ���� �̷�� ���� �ε����� ������ 6���̴�.
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

		// �簢���� ����� ���ؽ��� 4���� ����Ǿ��ִ�.
		// �װ��� 3�� 3���� �ɰ��� �ε����� ��������

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
	// ���ؽ� ������ Ȱ��ȭ��Ų��.
	_device->SetVertexDeclaration(_decl);

	// ��Ʈ���� �����ش�.
	_device->SetStreamSource(0, _vb, 0, sizeof(EdgeVertex));
	_device->SetIndices(_ib);

	_device->DrawIndexedPrimitive(
		D3DPT_TRIANGLELIST, 0, 0, _numVerts, 0, _numFaces);
}
