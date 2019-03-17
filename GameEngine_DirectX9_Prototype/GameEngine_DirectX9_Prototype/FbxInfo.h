#ifndef FBXINFO_H
#define FBXINFO_H

#include <fbxsdk.h>
#include <string>
#include <iostream>
#include <vector>
#include <map>

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

	// 수정해야함
	bool operator<(const MyVertex & other) const
	{
		return true; (position[0] < other.getPosition()[0]);
	}
	bool operator>(const MyVertex & other) const
	{
		return (position[0] > other.getPosition()[0]);
	}

};

class FbxInfo
{
private:
	string fileName;

	FbxScene * scene;
	FbxMesh * mesh;

	vector<FbxDouble3> controlPoints;
	vector<MyVertex> vertices;
	vector<unsigned int> indices;
	map<MyVertex, unsigned int> vertexTable;

public:
	FbxInfo()
		: mesh(nullptr), scene(nullptr)
	{

	}

	// Scene을 로드 하는 것은 외부에서 해준다. 
	// 씬포인터를 더블포인터로 받아서 여기포인터가 가리키는 값을 변경
	FbxScene ** getFbxScene() { return &scene; }

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
		return mesh->GetPolygonCount(); 
	}

	void loadMeshFromNodes(FbxNode * node)
	{
		if (!node) return;

		FbxNodeAttribute * nodeAtrribute = node->GetNodeAttribute();

		// 속성값이 nullptr값이면 리턴해준다.
		// 노드를 돌면서 메쉬노드를 찾아준다.
		// 메쉬를 찾으면 메쉬 변수를 초기화 시키고 리턴한다.
		if (nodeAtrribute)
		{
			if (nodeAtrribute->GetAttributeType() == FbxNodeAttribute::eMesh)
			{
				mesh = node->GetMesh();
				return;
			}
		}

		// 메쉬를 찾지 못한다면 자식들을 돌면서 메쉬를 찾는다.
		const int childCount = node->GetChildCount();
		for (unsigned int i = 0; i < childCount; ++i)
		{
			loadMeshFromNodes(node->GetChild(i));
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

		unsigned int triCount = mesh->GetPolygonCount();
		unsigned int vertexCounter = 0;

		FbxDouble3 position{ 0,0,0 };
		FbxDouble3 normal{ 0,0,0 };
		FbxDouble3 binormal{ 0,0,0 };
		FbxDouble3 tangent{ 0,0,0 };
		FbxDouble2 uv{ 0,0 };


		for (unsigned int i = 0; i < triCount; ++i)
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
				readUV(mesh, controlPointIndex, mesh->GetTextureUVIndex(i, j), uv);

				//vertices.push_back(vertex);
				insertVertex(position, normal, uv, binormal, tangent);
				++vertexCounter;
			}
		}
	}

	void insertVertex(const FbxDouble3& position, const FbxDouble3& normal, const FbxDouble2& uv, const FbxDouble3& binormal, const FbxDouble3& tangent)
	{
		MyVertex vertex = { position, normal, uv, binormal, tangent };

		unsigned int index = vertices.size();
		//vertexTable[vertex] = index;
		indices.push_back(index);
		vertices.push_back(vertex);
		return;

		auto lookup = vertexTable.find(vertex);
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