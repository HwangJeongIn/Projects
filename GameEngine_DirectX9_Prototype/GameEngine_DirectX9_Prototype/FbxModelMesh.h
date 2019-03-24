#ifndef FBXMODELMESH_H
#define FBXMODELMESH_H

#include <fbxsdk.h>
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include "Trace.h"
#include "WeightOfBones.h"

using namespace std;

class FbxModelMesh;
class FbxModelSkeletonBones;

class MyFbxVertex
{
private:
	FbxDouble3 position;
	FbxDouble2 uv;
	FbxDouble3 normal;
	FbxDouble3 tangent;
	FbxDouble3 binormal;

	// for animation
	WeightOfBones weightOfBones;
	FbxMatrix animationMatrix;
public:
	MyFbxVertex(const FbxDouble3 & position, const FbxDouble3 & normal,
		const FbxDouble2 & uv, const FbxDouble3 & binormal, const FbxDouble3 & tangent, const WeightOfBones & weightOfBones)
		: position(position), normal(normal), uv(uv), binormal(binormal), tangent(tangent), weightOfBones(weightOfBones)
	{
		animationMatrix.SetIdentity();
	}
	friend class FbxModelMesh;
	const FbxDouble3 & getPosition() const { return position; }
	FbxDouble3 getPositionWithAnimation()
	{
		calcAnimationMatrix();

		FbxVector4 temp { position.mData[0], position.mData[0], position.mData[0], 1 };
		// 나중에 테스트 해볼예정 // FbxVector4가 아닌 FbxDouble이 들어가도되는지
		temp = animationMatrix.MultNormalize(temp);
		return{ temp.mData[0], temp.mData[1], temp.mData[2] };
	}

	void calcAnimationMatrix()
	{
		// 애니메이션이 진행될때마다 여기서 새롭게 계산되어 animationMatrix에 저장된다.
		// 이 animationMatrix는 getPositionWithAnimation함수를 통해 사용되며 
		// 이 함수의 반환값을 통해서 실제 렌더링할 메쉬 위치를 이동한다.
		animationMatrix = weightOfBones.getCombinedBonesMatrixWithWeight();
	}

	const FbxDouble3 & getNormal()const { return normal; }
	const FbxDouble2 & getUV()const { return uv; }
	const FbxDouble3 & getBinormal()const { return binormal; }
	const FbxDouble3 & getTangent() const { return tangent; }
	const FbxMatrix & getAnimationMatrix() const { return animationMatrix; }

	void setPosition(const FbxDouble3 & other) { position = other; }
	void setNormal(const FbxDouble3 & other) { normal = other; }
	void setUv(const FbxDouble2 & other) { uv = other; }
	void setBinormal(const FbxDouble3 & other) { binormal = other; }
	void setTangent(const FbxDouble3 & other) { tangent = other; }
	void setAnimationMatrix(const FbxMatrix & other) { animationMatrix = other; }


	bool operator==(const MyFbxVertex & other) const
	{
		return (position == other.getPosition() && uv == other.getUV() && normal == other.getNormal()
			&& tangent == other.getTangent() && binormal == other.getBinormal());
	}

	bool operator<(const MyFbxVertex & other) const
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
	vector<MyFbxVertex> vertices;
	vector<unsigned int> indices;

	// 나중에 사용
	map<MyFbxVertex, unsigned int> vertexTable;

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

	MyFbxVertex * getVertex(int index)
	{
		// 만약에 사이즈보다 인덱스가 크거나 0보다 작으면 nullptr리턴
		if (vertices.size() <= index || index <0)
			return nullptr;

		return &vertices[index];
	}

	//FbxDouble3 getVertexPositionWithAnimation(int index)
	//{
	//	// 만약에 사이즈보다 인덱스가 크거나 0보다 작으면 000 리턴
	//	if (vertices.size() <= index || index <0)
	//		return{0,0,0};
	//
	//	FbxVector4 temp { vertices[index].position.mData[0], vertices[index].position.mData[0], vertices[index].position.mData[0], 1 };

	//	temp = vertices[index].animationMatrix.MultNormalize(temp);
	//	return{ temp.mData[0], temp.mData[1], temp.mData[2] };
	//}

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

	void processVertices(FbxModelSkeletonBones * skeletonBones);
	

	void insertVertex(const FbxDouble3& position, const FbxDouble3& normal, const FbxDouble2& uv, const FbxDouble3& binormal, const FbxDouble3& tangent, const WeightOfBones & weightOfBones)
	{
		MyFbxVertex vertex { position, normal, uv, binormal, tangent, weightOfBones };

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
		map<MyFbxVertex, unsigned int>::iterator lookup = vertexTable.find(vertex);
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