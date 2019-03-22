#ifndef FBXMODELMESH_H
#define FBXMODELMESH_H

#include <fbxsdk.h>
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include "Trace.h"

using namespace std;


class MyVertex
{
private:
	FbxDouble3 position;
	FbxDouble2 uv;
	FbxDouble3 normal;
	FbxDouble3 tangent;
	FbxDouble3 binormal;

public:
	MyVertex(const FbxDouble3 & position, const FbxDouble3 & normal,
		const FbxDouble2 & uv, const FbxDouble3 & binormal, const FbxDouble3 & tangent)
		: position(position), normal(normal), uv(uv), binormal(binormal), tangent(tangent)
	{

	}

	const FbxDouble3 & getPosition() const { return position; }
	const FbxDouble3 & getNormal()const { return normal; }
	const FbxDouble2 & getUV()const { return uv; }
	const FbxDouble3 & getBinormal()const { return binormal; }
	const FbxDouble3 & getTangent() const { return tangent; }

	void setPosition(const FbxDouble3 & other) { position = other; }
	void setNormal(const FbxDouble3 & other) { normal = other; }
	void setUv(const FbxDouble2 & other) { uv = other; }
	void setBinormal(const FbxDouble3 & other) { binormal = other; }
	void setTangent(const FbxDouble3 & other) { tangent = other; }

	bool operator==(const MyVertex & other) const
	{
		return (position == other.getPosition() && uv == other.getUV() && normal == other.getNormal()
			&& tangent == other.getTangent() && binormal == other.getBinormal());
	}

	bool operator<(const MyVertex & other) const
	{
		return (position[0] * position[0] + position[1] * position[1] + position[2] * position[2]
					< other.getPosition()[0] * other.getPosition()[0] + other.getPosition()[1] * other.getPosition()[1] + other.getPosition()[2] * other.getPosition()[2]);
	}

	// 수정해야함
	//bool operator<(const MyVertex & other) const
	//{
	//	return  (position[0] * position[0] + position[1] * position[1] + position[2] * position[2]
	//		< other.getPosition()[0] * other.getPosition()[0] + other.getPosition()[1] * other.getPosition()[1] + other.getPosition()[2] * other.getPosition()[2]);
	//}
	//bool operator>(const MyVertex & other) const
	//{
	//	return (position[0] > other.getPosition()[0]);
	//}
};

class FbxModelMesh
{
private:
	string fileName;

	FbxMesh * mesh;
	FbxNode * node;

	vector<FbxDouble3> controlPoints;
	vector<MyVertex> vertices;
	vector<unsigned int> indices;

	// 나중에 사용
	map<MyVertex , unsigned int> vertexTable;

public:
	FbxModelMesh()
		: mesh(nullptr), node(nullptr)  /*, scene(nullptr)*/
	{

	}

	~FbxModelMesh()
	{

	}

	// Scene을 로드 하는 것은 외부에서 해준다. 
	// 씬포인터를 더블포인터로 받아서 여기포인터가 가리키는 값을 변경
	//FbxScene ** getFbxScene() { return &scene; }

	int getIndex(int index)
	{
		// 만약에 사이즈보다 인덱스가 크거나 0보다 작으면 -1리턴
		if (indices.size() <= index || index < 0)
			return -1;

		return indices[index];
	}

	MyVertex * getVertex(int index)
	{
		// 만약에 사이즈보다 인덱스가 크거나 0보다 작으면 nullptr리턴
		if (vertices.size() <= index || index <0)
			return nullptr;

		return &vertices[index];
	}

	//bool getIndex(int index, unsigned int * output)
	//{
	//	// 만약에 사이즈보다 인덱스가 크거나 0보다 작거나 넣어줘야하는 포인터가 nullptr라면 리턴
	//	if (indices.size() <= index || index < 0 || !output)
	//		return false;

	//	output = &indices[index];
	//	return true;
	//}

	//bool getVertex(int index, MyVertex * output)
	//{
	//	// 만약에 사이즈보다 인덱스가 크거나 0보다 작거나 넣어줘야하는 포인터가 nullptr라면 리턴
	//	if (vertices.size() <= index || index <0 || !output)
	//		return false;

	//	output = &vertices[index];
	//	return true;
	//}
	int getVertexCount() const
	{ 
		if (!mesh) return 0;
		return vertices.size();
	}
	int getIndexCount() const 
	{ 
		if (!mesh) return 0;
		return indices.size(); 
	}
	int getTriCount() const 
	{
		if (!mesh) return 0;
		return indices.size() / 3;
	}

	void setNode(FbxNode * node)
	{
		this->node = node;
		if(node)
			mesh = node->GetMesh();
	}

	FbxNode * getNode()
	{
		return node;
	}

	void loadMeshFromNodes(FbxNode * node)
	{
		if (!node) return;

		FbxNodeAttribute * nodeAtrribute = node->GetNodeAttribute();

		// 속성값이 nullptr값이면 리턴해준다.
		if (nodeAtrribute)
		{
			// 메쉬일때 초기화
			if (nodeAtrribute->GetAttributeType() == FbxNodeAttribute::eMesh)
			{
				mesh = node->GetMesh();
			}
		}
	}

	void processControlPoints()
	{
		if (!mesh) return;
		// 제어점의 개수를 가져온다.
		unsigned int count = mesh->GetControlPointsCount();
		//int tt = mesh->GetPolygonCount();
		controlPoints.resize(count);
		FbxDouble3 controlPoint{ 0,0,0 };
		for (unsigned int i = 0; i < count; ++i)
		{
			// 제어점을 가져오려면 GetControlPointAt(int index) 멤버 함수를 이용한다.
			// x좌표 
			controlPoint[0] = static_cast<float>(mesh->GetControlPointAt(i).mData[0]);
			// y좌표 
			controlPoint[1] = static_cast<float>(mesh->GetControlPointAt(i).mData[1]);
			// z좌표
			controlPoint[2] = static_cast<float>(mesh->GetControlPointAt(i).mData[2]);

			// 인덱스를 명확하게 하기 위해서 push_back이 아닌
			// 인덱스로 접근해서 안에 값을 넣어준다.
			// controlPoints.push_back(controlPoint);
			controlPoints[i] = controlPoint;
		}
	}

	void processVertices()
	{
		if (!mesh) return;

		unsigned int polygonCount = mesh->GetPolygonCount();
		unsigned int vertexCounter = 0;

		FbxDouble3 position{ 0,0,0 };
		FbxDouble3 normal{ 0,0,0 };
		FbxDouble3 binormal{ 0,0,0 };
		FbxDouble3 tangent{ 0,0,0 };
		FbxDouble2 uv{ 0,0 };


		// 폴리곤
		// 원래는 폴리곤 사이즈가 3/4였는데 converter에 의해서 3개로 모두 변경되었다.
		// directX에서 작업하기 편하다.
		for (unsigned int i = 0; i < polygonCount; ++i)
		{
			//for (unsigned int j = 0; j < mesh->GetPolygonSize(i); ++j)
			//{
				int polygonSize = mesh->GetPolygonSize(i);

				// 현재는 모두 삼각형으로 분리시켰기 때문에 모두 삼각형기준으로 작업된다.
				if (polygonSize == 3)
				{
					for (unsigned int j = 0; j < 3; ++j)
					{
						int controlPointIndex = mesh->GetPolygonVertex(i, j);


						// 각각의 정보를 초기화 시켜준다.
						// 만약에 내부에 정보가 없으면 모두 0으로 들어간다.
						position = { 0,0,0 };
						normal = { 0,0,0 };
						binormal = { 0,0,0 };
						tangent = { 0,0,0 };
						uv = { 0,0 };

						// 내부 레퍼런스 모드에 따라서 버텍스 카운터가 필요할 수도 있고
						// 컨트롤 포인트 인덱스가 필요할 수도 있다.
						position = controlPoints[controlPointIndex];
						readNormal(mesh, controlPointIndex, vertexCounter, normal);
						readTangent(mesh, controlPointIndex, vertexCounter, tangent);
						readBinormal(mesh, controlPointIndex, vertexCounter, binormal);
						readUV(mesh, controlPointIndex, mesh->GetTextureUVIndex(i, j,fbxsdk::FbxLayerElement::eTextureDiffuse)/*vertexCounter*/, uv);

						// DirectX기준으로 UV좌표 맞춰줌
					    // if (uv[0] == 0.0f && uv[1] == 0.0f) 
						{
							uv[0] = uv[0];
							uv[1] = 1.0 - uv[1];
						}
						//if (vertexCounter == 0 || vertexCounter ==1)
						//{
						//	Trace::Write("TAG_DEBUG", "u", uv[0]);
						//	Trace::Write("TAG_DEBUG", "v", uv[1]);

						//}

						//vertices.push_back(vertex);
						insertVertex(position, normal, uv, binormal, tangent);
						++vertexCounter;
					}
				}
				//else if (polygonSize == 4)
				//{
				//	// 사각형을 그려줘야 한다.
				//	// 먼저 삼각형하나를 그려주고
				//	for (unsigned int j = 0; j < 3; ++j)
				//	{
				//		int controlPointIndex = mesh->GetPolygonVertex(i, j);
				//
				//		// 각각의 정보를 초기화 시켜준다.
				//		// 만약에 내부에 정보가 없으면 모두 0으로 들어간다.
				//		position = { 0,0,0 };
				//		normal = { 0,0,0 };
				//		binormal = { 0,0,0 };
				//		tangent = { 0,0,0 };
				//		uv = { 0,0 };
				//
				//		// 내부 레퍼런스 모드에 따라서 버텍스 카운터가 필요할 수도 있고
				//		// 컨트롤 포인트 인덱스가 필요할 수도 있다.
				//		position = controlPoints[controlPointIndex];
				//		readNormal(mesh, controlPointIndex, vertexCounter, normal);
				//		readTangent(mesh, controlPointIndex, vertexCounter, tangent);
				//		readBinormal(mesh, controlPointIndex, vertexCounter, binormal);
				//		readUV(mesh, controlPointIndex, mesh->GetTextureUVIndex(i, j), uv);
				//
				//		//vertices.push_back(vertex);
				//		insertVertex(position, normal, uv, binormal, tangent);
				//		++vertexCounter;
				//	}
				//
				//	// 일단 4개기준으로 vertexCounter초기화 해준다
				//	++vertexCounter;
				//
				//	// 나머지 삼각형하나를 그려준다.
				//	for (unsigned int j = 2; j != 1; (++j)%=4)
				//	{
				//		int controlPointIndex = mesh->GetPolygonVertex(i, j);
				//
				//		// 각각의 정보를 초기화 시켜준다.
				//		// 만약에 내부에 정보가 없으면 모두 0으로 들어간다.
				//		position = { 0,0,0 };
				//		normal = { 0,0,0 };
				//		binormal = { 0,0,0 };
				//		tangent = { 0,0,0 };
				//		uv = { 0,0 };
				//
				//		vertexCounter += - 4 + (j);
				//		// 내부 레퍼런스 모드에 따라서 버텍스 카운터가 필요할 수도 있고
				//		// 컨트롤 포인트 인덱스가 필요할 수도 있다.
				//		position = controlPoints[controlPointIndex];
				//		readNormal(mesh, controlPointIndex, vertexCounter, normal);
				//		readTangent(mesh, controlPointIndex, vertexCounter, tangent);
				//		readBinormal(mesh, controlPointIndex, vertexCounter, binormal);
				//		readUV(mesh, controlPointIndex, mesh->GetTextureUVIndex(i, j), uv);
				//		vertexCounter +=  4 - (j);
				//
				//
				//		//vertices.push_back(vertex);
				//		insertVertex(position, normal, uv, binormal, tangent);
				//
				//	}
				//}
				//
			//}


		}
	}

	void insertVertex(const FbxDouble3& position, const FbxDouble3& normal, const FbxDouble2& uv, const FbxDouble3& binormal, const FbxDouble3& tangent)
	{
		MyVertex vertex { position, normal, uv, binormal, tangent };

		// 일단 맵에서 중복되는 버텍스를 재사용안하고 인덱스와 1:1매칭시켰다.
		// operator == 와 operator < 가 자체 클래스객체에서 구현되어야 하는 이유는 이 두가지 기준으로 여러가지 연산을 하기 때문이다.
		// 이는 맵이 해쉬테이블이 아닌 트리구조로 되어있다는 것을 암시하기도 한다.

		// 중복된 데이터가 많지만 일단 그냥해주었다... 나중에 수정할 예정
		unsigned int index = vertices.size();
		//vertexTable[vertex] = index;
		indices.push_back(index);
		vertices.push_back(vertex);
		return;


		// 나중에 여기서 수정
		map<MyVertex, unsigned int>::iterator lookup = vertexTable.find(vertex);
		if (lookup != vertexTable.end())
		{
			// 실제로 존재하는 값이라면(등록되어 있는 버텍스 값이라면) 그 값의 인덱스 값으로 넣어준다.
			indices.push_back(lookup->second);
		}
		else
		{
			// 존재하지 않는 값이라면 버텍스 테이블에 등록 
			// 각각의 리스트에 추가해준다.
			// 먼저 인덱스의 값은 맨마지막에 추가하는 것이기 때문에 size()값을 받아서 초기화 해준다.
			unsigned int index = vertices.size();
			vertexTable[vertex] = index;
			indices.push_back(index);
			vertices.push_back(vertex);
		}
	}

	void readNormal(FbxMesh* mesh, int controlPointIndex, int vertexCounter, FbxDouble3& outNormal);
	void readTangent(FbxMesh* mesh, int controlPointIndex, int vertexCounter, FbxDouble3& outTangent);
	void readBinormal(FbxMesh* mesh, int controlPointIndex, int vertexCounter, FbxDouble3& outbinormal);
	void readUV(FbxMesh* mesh, int controlPointIndex, int vertexCounter, FbxDouble2& outUV);
};

#endif