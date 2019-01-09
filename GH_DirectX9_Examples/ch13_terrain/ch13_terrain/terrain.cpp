//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: terrain.cpp
// 
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Represents a 3D terrain. 
//          
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "terrain.h"
#include <fstream>
#include <cmath>

const DWORD Terrain::TerrainVertex::FVF = D3DFVF_XYZ | D3DFVF_TEX1;

Terrain::Terrain(IDirect3DDevice9* device,
				 std::string heightmapFileName,
				 int numVertsPerRow,
				 int numVertsPerCol,
				 int cellSpacing,
				 float heightScale)
{
	_device         = device;
	_numVertsPerRow = numVertsPerRow;
	_numVertsPerCol = numVertsPerCol;
	_cellSpacing    = cellSpacing;

	_numCellsPerRow = _numVertsPerRow - 1;
	_numCellsPerCol = _numVertsPerCol - 1;

	_width = _numCellsPerRow * _cellSpacing;
	_depth = _numCellsPerCol * _cellSpacing;

	_numVertices  = _numVertsPerRow * _numVertsPerCol;
	_numTriangles = _numCellsPerRow * _numCellsPerCol * 2;

	_heightScale = heightScale;

	// load heightmap
	if( !readRawFile(heightmapFileName) )
	{
		::MessageBox(0, "readRawFile - FAILED", 0, 0);
		::PostQuitMessage(0);
	}

	// scale heights
	for(int i = 0; i < _heightmap.size(); i++)
		_heightmap[i] *= heightScale;

	// compute the vertices
	if( !computeVertices() )
	{
		::MessageBox(0, "computeVertices - FAILED", 0, 0);
		::PostQuitMessage(0);
	}

	// compute the indices
	if( !computeIndices() )
	{
		::MessageBox(0, "computeIndices - FAILED", 0, 0);
		::PostQuitMessage(0);
	}
}

Terrain::~Terrain()
{
	d3d::Release<IDirect3DVertexBuffer9*>(_vb);
	d3d::Release<IDirect3DIndexBuffer9*>(_ib);
	d3d::Release<IDirect3DTexture9*>(_tex);
}

int Terrain::getHeightmapEntry(int row, int col)
{
	return _heightmap[row * _numVertsPerRow + col];
}

void Terrain::setHeightmapEntry(int row, int col, int value)
{
	_heightmap[row * _numVertsPerRow + col] = value;
}

bool Terrain::computeVertices()
{
	HRESULT hr = 0;

	hr = _device->CreateVertexBuffer(
		// 버텍스의 개수와 크기를 곱한 값만큼 공간을 할당.
		_numVertices * sizeof(TerrainVertex),
		D3DUSAGE_WRITEONLY,
		TerrainVertex::FVF,
		D3DPOOL_MANAGED,
		&_vb,
		0);

	if(FAILED(hr))
		return false;

	// coordinates to start generating vertices at
	int startX = -_width / 2;
	int startZ =  _depth / 2;

	// coordinates to end generating vertices at
	int endX =  _width / 2;
	int endZ = -_depth / 2;

	// compute the increment size of the texture coordinates
	// from one vertex to the next.
	// 간격정의
	float uCoordIncrementSize = 1.0f / (float)_numCellsPerRow;
	float vCoordIncrementSize = 1.0f / (float)_numCellsPerCol;

	TerrainVertex* v = 0;
	_vb->Lock(0, 0, (void**)&v, 0);

	int i = 0;
	// 시작점에서 끝지점까지 셀간격마다 실행
	for(int z = startZ; z >= endZ; z -= _cellSpacing)
	{
		int j = 0;
		for(int x = startX; x <= endX; x += _cellSpacing)
		{
			// compute the correct index into the vertex buffer and heightmap
			// based on where we are in the nested loop.

			// 맞는 인덱스 값을 계산해준다. 하이트 맵 받아오기 위해서 
			int index = i * _numVertsPerRow + j;

			v[index] = TerrainVertex(
				(float)x,
				// 하이트맵의 값을 받아온다.
				(float)_heightmap[index],
				(float)z,
				// 간격만큼계산해서 UV좌표 계산
				(float)j * uCoordIncrementSize,
				(float)i * vCoordIncrementSize);

			j++; // next column
		}
		i++; // next row
	}

	_vb->Unlock();

	return true;
}

bool Terrain::computeIndices()
{
	HRESULT hr = 0;

	hr = _device->CreateIndexBuffer(
		_numTriangles * 3 * sizeof(WORD), // 3 indices per triangle
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&_ib,
		0);

	if(FAILED(hr))
		return false;

	WORD* indices = 0;
	_ib->Lock(0, 0, (void**)&indices, 0);

	// index to start of a group of 6 indices that describe the
	// two triangles that make up a quad
	int baseIndex = 0;

	// loop through and compute the triangles of each quad
	for(int i = 0; i < _numCellsPerCol; i++)
	{
		for(int j = 0; j < _numCellsPerRow; j++)
		{
			// ij번째 사각형을 2개의 삼각형으로 계산해서 인덱스 버퍼에 넣어준다.
			/*
			AㅁㅁB
			CㅁㅁD
			라고 가정한다
			*/

			// 삼각형ABC의 인덱스
			// A의 인덱스 
			indices[baseIndex]     =   i   * _numVertsPerRow + j;
			// B의 인덱스 
			indices[baseIndex + 1] =   i   * _numVertsPerRow + j + 1;
			// C의 인덱스 
			indices[baseIndex + 2] = (i+1) * _numVertsPerRow + j;

			// 삼각형 BCD의 인덱스
			// B의 인덱스 
			indices[baseIndex + 3] = (i+1) * _numVertsPerRow + j;
			// C의 인덱스 
			indices[baseIndex + 4] =   i   * _numVertsPerRow + j + 1;
			// D의 인덱스 
			indices[baseIndex + 5] = (i+1) * _numVertsPerRow + j + 1;

			// next quad
			// 삼각형 2개의 값을 넣어주었기 때문에 다음 사각형을 정의하기 위해서 인덱스 값을 6을 올려준다.
			baseIndex += 6;
		}
	}

	_ib->Unlock();

	return true;
}

bool Terrain::loadTexture(std::string fileName)
{
	HRESULT hr = 0;

	// 파일을 로드시켜서 텍스처를 불러오는 방식 
	hr = D3DXCreateTextureFromFile(
		_device,
		fileName.c_str(),
		&_tex);

	if(FAILED(hr))
		return false;

	return true;
}

bool Terrain::genTexture(D3DXVECTOR3* directionToLight)
{
	// Method fills the top surface of a texture procedurally.  Then
	// lights the top surface.  Finally, it fills the other mipmap
	// surfaces based on the top surface data using D3DXFilterTexture.
	
	// 먼저 빈 텍스처를 만들고 미리 정의된 인자를 바탕으로 코드에서 각 텍셀의 컬러를 계산
	HRESULT hr = 0;

	// texel for each quad cell
	int texWidth  = _numCellsPerRow;
	int texHeight = _numCellsPerCol;

	// create an empty texture
	// 빈텍스처 만들기
	hr = D3DXCreateTexture(
		_device,
		texWidth, texHeight,
		0, // create a complete mipmap chain
		0, // usage
		D3DFMT_X8R8G8B8,// 32 bit XRGB format
		D3DPOOL_MANAGED, &_tex);

	if(FAILED(hr))
		return false;


	D3DSURFACE_DESC textureDesc; 
	_tex->GetLevelDesc(0 /*level*/, &textureDesc);

	// make sure we got the requested format because our code 
	// that fills the texture is hard coded to a 32 bit pixel depth.
	if( textureDesc.Format != D3DFMT_X8R8G8B8 )
		return false;
		
	D3DLOCKED_RECT lockedRect;
	// 최상위 텍스처를 잠근다.
	_tex->LockRect(0/*lock top surface*/, &lockedRect, 
		0 /* lock entire tex*/, 0/*flags*/);         

	DWORD* imageData = (DWORD*)lockedRect.pBits;
	for(int i = 0; i < texHeight; i++)
	{
		for(int j = 0; j < texWidth; j++)
		{
			D3DXCOLOR c;

			// get height of upper left vertex of quad.
			// 하이트맵에서 하이트 정보를 가져온다.
			float height = (float)getHeightmapEntry(i, j) / _heightScale;

			// 정보에 따라서 색지정을 해준다.
			if( (height) < 42.5f ) 		 c = d3d::BEACH_SAND;
			else if( (height) < 85.0f )	 c = d3d::LIGHT_YELLOW_GREEN;
			else if( (height) < 127.5f ) c = d3d::PUREGREEN;
			else if( (height) < 170.0f ) c = d3d::DARK_YELLOW_GREEN;
			else if( (height) < 212.5f ) c = d3d::DARKBROWN;
			else	                     c = d3d::WHITE;

			// fill locked data, note we divide the pitch by four because the
			// pitch is given in bytes and there are 4 bytes per DWORD.
			// 잠근데이터를 채운다. 
			// 여기서 피치는 그 다음라인까지의 거리를 뜻한다.
			// 표면의 한열 바이트수
			// 피치는 바이트로 지우지며, DWORD는 4바이트로 이루어져 있다.
			
			imageData[i * lockedRect.Pitch / 4 + j] = (D3DCOLOR)c;
		}
	}

	_tex->UnlockRect(0);

	// 지형을 밝힌다.
	if(!lightTerrain(directionToLight))
	{
		::MessageBox(0, "lightTerrain() - FAILED", 0, 0);
		return false;
	}
	
	// 밉맵을 채운다.
	hr = D3DXFilterTexture(
		_tex,
		0, // default palette
		0, // use top level as source level
		D3DX_DEFAULT); // default filter

	if(FAILED(hr))
	{
		::MessageBox(0, "D3DXFilterTexture() - FAILED", 0, 0);
		return false;
	}

	return true;
}

bool Terrain::lightTerrain(D3DXVECTOR3* directionToLight)
{
	// 원래 라이트 계산은 Direct3D에게 작업을 맡길 수도 있다. 하지만 그렇게 하지 않은 이유는
	/*
	1. 버텍스 법선을 저장하지 않아도 되므로 메모리 절약
	2. 지형은 정적이며 조명을 움직이지 않을 것이기 때문에 조명을 미리 계산하여
	Direct3D가 지형에 실시간으로 조명을 적용하는데 따르는 처리의 부담을 덜 수 있다. // 미리 구워둔다.
	*/

	// 여기서 지형의 음영을 계산하는데 이용하는 조명 테크닉은 가장 기본적인 것으로 보통 난반사광이라 불린다.
	// 발산되는 빛이 방향을 결정해도 되지만, 계산에 있어서는 광원으로의 방향을 지정하는 것이 유리
	HRESULT hr = 0;

	D3DSURFACE_DESC textureDesc; 
	_tex->GetLevelDesc(0 /*level*/, &textureDesc);

	// make sure we got the requested format because our code that fills the
	// texture is hard coded to a 32 bit pixel depth.
	if( textureDesc.Format != D3DFMT_X8R8G8B8 )
		return false;
		
	D3DLOCKED_RECT lockedRect;
	_tex->LockRect(
		0,          // lock top surface level in mipmap chain
		&lockedRect,// pointer to receive locked data
		0,          // lock entire texture image
		0);         // no lock flags specified

	DWORD* imageData = (DWORD*)lockedRect.pBits;
	for(int i = 0; i < textureDesc.Height; i++)
	{
		for(int j = 0; j < textureDesc.Width; j++)
		{
			// index into texture, note we use the pitch and divide by 
			// four since the pitch is given in bytes and there are 
			// 4 bytes per DWORD.
			int index = i * lockedRect.Pitch / 4 + j;

			// get current color of quad
			D3DXCOLOR c( imageData[index] );

			// shade current quad
			// 셀에 음영을 적용한다. 0-1값
			c *= computeShade(i, j, directionToLight);;

			// save shaded color
			// 적용한 값을 칼라값으로 저장한다.
			imageData[index] = (D3DCOLOR)c;
		}
	}

	_tex->UnlockRect(0);

	return true;
}

float Terrain::computeShade(int cellRow, int cellCol, D3DXVECTOR3* directionToLight)
{
	// get heights of three vertices on the quad
	// 현재높이와 한행 증가한 높이 한열 증가한 높이를 각각 구해준다.
	// 이는 현재지점 기준으로 두 벡터를 계산하기 위함이다.
	float heightA = getHeightmapEntry(cellRow,   cellCol);
	float heightB = getHeightmapEntry(cellRow,   cellCol+1);
	float heightC = getHeightmapEntry(cellRow+1, cellCol);

	// build two vectors on the quad
	// 간격은 cellSpacing이다.
	// y값은 각높이를 구하였다.
	D3DXVECTOR3 u(_cellSpacing, heightB - heightA, 0.0f);

	// 열은 커질수록 -간격을 해주어야한다.
	D3DXVECTOR3 v(0.0f,         heightC - heightA, -_cellSpacing);

	// find the normal by taking the cross product of two
	// vectors on the quad.
	D3DXVECTOR3 n;
	// 외적으로 두 벡터로 만들어진 평면에 대한 법선벡터를 구해준다.
	D3DXVec3Cross(&n, &u, &v);
	// 정규화시킨다.
	D3DXVec3Normalize(&n, &n);

	// 각도와 내적해서 값을 받고
	float cosine = D3DXVec3Dot(&n, directionToLight);

	// 그값이 0이하이면 90도 이상이므로 그럴때는 0으로 초기화한다.
	if(cosine < 0.0f)
		cosine = 0.0f;

	return cosine;
}

bool Terrain::readRawFile(std::string fileName)
{
	// Restriction: RAW file dimensions must be >= to the
	// dimensions of the terrain.  That is a 128x128 RAW file
	// can only be used with a terrain constructed with at most
	// 128x128 vertices.

	// A height for each vertex

	// BYTE == unsigned char
	std::vector<BYTE> in( _numVertices );

	std::ifstream inFile(fileName.c_str(), std::ios_base::binary);

	if( !inFile )
		return false;

	// 임시로 만들어놨던 in변수를 통해 파일을 읽는다.
	inFile.read(
		(char*)&in[0], // buffer
		in.size());// number of bytes to read into buffer

	inFile.close();

	// copy BYTE vector to int vector
	// 하이트맵을 크기를 다시 조정하고
	_heightmap.resize( _numVertices );

	// 읽은 데이터를 다시 저장한다.
	for(int i = 0; i < in.size(); i++)
		_heightmap[i] = in[i];

	return true;
}

float Terrain::getHeight(float x, float z)
{
	// Translate on xz-plane by the transformation that takes
	// the terrain START point to the origin.
	// 원점을 기준으로 다시 매핑시켜준다.
	x = ((float)_width / 2.0f) + x;
	// 대신 여기서 z의 +방향을 반대로 정의했다.
	z = ((float)_depth / 2.0f) - z;

	// Scale down by the transformation that makes the 
	// cellspacing equal to one.  This is given by 
	// 1 / cellspacing since; cellspacing * 1 / cellspacing = 1.
	// 간격으로 나눠준다.
	// 하이트 맵의 행과 열을 알아보기 위해서
	x /= (float)_cellSpacing;
	z /= (float)_cellSpacing;

	// From now on, we will interpret our positive z-axis as
	// going in the 'down' direction, rather than the 'up' direction.
	// This allows to extract the row and column simply by 'flooring'
	// x and z:

	float col = ::floorf(x);
	float row = ::floorf(z);

	// get the heights of the quad we're in:
	// 
    //  A   B
    //  *---*
    //  | / |
    //  *---*  
    //  C   D

	// 앞에서 내림으로 처리했기 때문에
	// row , col 의 위치는 A가 되고 그 지점 기준으로 나머지 지점을 계산해준다.
	float A = getHeightmapEntry(row,   col);
	float B = getHeightmapEntry(row,   col+1);
	float C = getHeightmapEntry(row+1, col);
	float D = getHeightmapEntry(row+1, col+1);

	//
	// Find the triangle we are in:
	//

	// Translate by the transformation that takes the upper-left
	// corner of the cell we are in to the origin.  Recall that our 
	// cellspacing was nomalized to 1.  Thus we have a unit square
	// at the origin of our +x -> 'right' and +z -> 'down' system.

	// 크게 어떤 사각형 내부에 있는지 확인했다면
	// 그 사각형 중심으로 연산하여 어떤 삼각형내에 속해있는지 확인해야 한다

	// 큰것부터 차례대로 확인하는 방법이다.
	// 사각형 > 삼각형
	float dx = x - col;
	float dz = z - row;

	// Note the below compuations of u and v are unneccessary, we really
	// only need the height, but we compute the entire vector to emphasis
	// the books discussion.
	float height = 0.0f;
	// dz + dx < 1.0f
	// 두값을 더한값이 1보다 작으면 윗삼각형이다.
	if(dz < 1.0f - dx)  // upper triangle ABC
	{
		// 두벡터 계산하고
		float uy = B - A; // A->B
		float vy = C - A; // A->C

		// Linearly interpolate on each vector.  The height is the vertex
		// height the vectors u and v originate from {A}, plus the heights
		// found by interpolating on each vector u and v.

		// A기준으로 A->B에서의 높이차이와 A->C에서의 높이차이를 이용하여
		// 보간한 값을 구해준다.
		// 나머지 부분은 보간해서 더해준다
		// 현재 위치점이 안에있는 사각형을 원점기준으로 다시 매핑시켰기 때문에
		// 0 ~ 1 사이의 값인 dx와 dz를 사용하여 보간해주면 된다.
		height = A + d3d::Lerp(0.0f, uy, dx)/* 0 ~ uy 값을 0~1사이의 값인 dx으로 나누고*/
			+ d3d::Lerp(0.0f, vy, dz);/* 0 ~ vy 값을 0~1사이의 값인 dz으로 나눈다*/
	}
	else // lower triangle DCB
	{
		float uy = C - D; // D->C
		float vy = B - D; // D->B

		// Linearly interpolate on each vector.  The height is the vertex
		// height the vectors u and v originate from {D}, plus the heights
		// found by interpolating on each vector u and v.
		height = D + d3d::Lerp(0.0f, uy, 1.0f - dx) + d3d::Lerp(0.0f, vy, 1.0f - dz);
	}

	return height;
}

bool Terrain::draw(D3DXMATRIX* world, bool drawTris)
{
	HRESULT hr = 0;

	if( _device )
	{
		_device->SetTransform(D3DTS_WORLD, world);

		_device->SetStreamSource(0, _vb, 0, sizeof(TerrainVertex));
		_device->SetFVF(TerrainVertex::FVF);
		_device->SetIndices(_ib);
		
		_device->SetTexture(0, _tex);

		// turn off lighting since we're lighting it ourselves
		_device->SetRenderState(D3DRS_LIGHTING, false);

		hr =_device->DrawIndexedPrimitive(
			D3DPT_TRIANGLELIST,
			0,
			0,
			_numVertices,
			0,
			_numTriangles);

		_device->SetRenderState(D3DRS_LIGHTING, true);

		if( drawTris )
		{
			_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
			hr =_device->DrawIndexedPrimitive(
				D3DPT_TRIANGLELIST,
				0,
				0,
				_numVertices,
				0,
				_numTriangles);

			_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		}

		if(FAILED(hr))
			return false;
	}

	return true;
}

