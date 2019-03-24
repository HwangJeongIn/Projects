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
		// ���߿� �׽�Ʈ �غ����� // FbxVector4�� �ƴ� FbxDouble�� �����Ǵ���
		temp = animationMatrix.MultNormalize(temp);
		return{ temp.mData[0], temp.mData[1], temp.mData[2] };
	}

	void calcAnimationMatrix()
	{
		// �ִϸ��̼��� ����ɶ����� ���⼭ ���Ӱ� ���Ǿ� animationMatrix�� ����ȴ�.
		// �� animationMatrix�� getPositionWithAnimation�Լ��� ���� ���Ǹ� 
		// �� �Լ��� ��ȯ���� ���ؼ� ���� �������� �޽� ��ġ�� �̵��Ѵ�.
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

	// �����ؾ���
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

	// ���߿� ���
	map<MyFbxVertex, unsigned int> vertexTable;

public:
	FbxModelMesh()
		: mesh(nullptr), node(nullptr)  /*, scene(nullptr)*/
	{

	}

	~FbxModelMesh()
	{

	}

	// Scene�� �ε� �ϴ� ���� �ܺο��� ���ش�. 
	// �������͸� ���������ͷ� �޾Ƽ� ���������Ͱ� ����Ű�� ���� ����
	//FbxScene ** getFbxScene() { return &scene; }

	int getIndex(int index)
	{
		// ���࿡ ������� �ε����� ũ�ų� 0���� ������ -1����
		if (indices.size() <= index || index < 0)
			return -1;

		return indices[index];
	}

	MyFbxVertex * getVertex(int index)
	{
		// ���࿡ ������� �ε����� ũ�ų� 0���� ������ nullptr����
		if (vertices.size() <= index || index <0)
			return nullptr;

		return &vertices[index];
	}

	//FbxDouble3 getVertexPositionWithAnimation(int index)
	//{
	//	// ���࿡ ������� �ε����� ũ�ų� 0���� ������ 000 ����
	//	if (vertices.size() <= index || index <0)
	//		return{0,0,0};
	//
	//	FbxVector4 temp { vertices[index].position.mData[0], vertices[index].position.mData[0], vertices[index].position.mData[0], 1 };

	//	temp = vertices[index].animationMatrix.MultNormalize(temp);
	//	return{ temp.mData[0], temp.mData[1], temp.mData[2] };
	//}

	//bool getIndex(int index, unsigned int * output)
	//{
	//	// ���࿡ ������� �ε����� ũ�ų� 0���� �۰ų� �־�����ϴ� �����Ͱ� nullptr��� ����
	//	if (indices.size() <= index || index < 0 || !output)
	//		return false;

	//	output = &indices[index];
	//	return true;
	//}

	//bool getVertex(int index, MyVertex * output)
	//{
	//	// ���࿡ ������� �ε����� ũ�ų� 0���� �۰ų� �־�����ϴ� �����Ͱ� nullptr��� ����
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

		// �Ӽ����� nullptr���̸� �������ش�.
		if (nodeAtrribute)
		{
			// �޽��϶� �ʱ�ȭ
			if (nodeAtrribute->GetAttributeType() == FbxNodeAttribute::eMesh)
			{
				mesh = node->GetMesh();
			}
		}
	}

	void processControlPoints()
	{
		if (!mesh) return;
		// �������� ������ �����´�.
		unsigned int count = mesh->GetControlPointsCount();
		//int tt = mesh->GetPolygonCount();
		controlPoints.resize(count);
		FbxDouble3 controlPoint{ 0,0,0 };
		for (unsigned int i = 0; i < count; ++i)
		{
			// �������� ���������� GetControlPointAt(int index) ��� �Լ��� �̿��Ѵ�.
			// x��ǥ 
			controlPoint[0] = static_cast<float>(mesh->GetControlPointAt(i).mData[0]);
			// y��ǥ 
			controlPoint[1] = static_cast<float>(mesh->GetControlPointAt(i).mData[1]);
			// z��ǥ
			controlPoint[2] = static_cast<float>(mesh->GetControlPointAt(i).mData[2]);

			// �ε����� ��Ȯ�ϰ� �ϱ� ���ؼ� push_back�� �ƴ�
			// �ε����� �����ؼ� �ȿ� ���� �־��ش�.
			// controlPoints.push_back(controlPoint);
			controlPoints[i] = controlPoint;
		}
	}

	void processVertices(FbxModelSkeletonBones * skeletonBones);
	

	void insertVertex(const FbxDouble3& position, const FbxDouble3& normal, const FbxDouble2& uv, const FbxDouble3& binormal, const FbxDouble3& tangent, const WeightOfBones & weightOfBones)
	{
		MyFbxVertex vertex { position, normal, uv, binormal, tangent, weightOfBones };

		// �ϴ� �ʿ��� �ߺ��Ǵ� ���ؽ��� ������ϰ� �ε����� 1:1��Ī���״�.
		// operator == �� operator < �� ��ü Ŭ������ü���� �����Ǿ�� �ϴ� ������ �� �ΰ��� �������� �������� ������ �ϱ� �����̴�.
		// �̴� ���� �ؽ����̺��� �ƴ� Ʈ�������� �Ǿ��ִٴ� ���� �Ͻ��ϱ⵵ �Ѵ�.

		// �ߺ��� �����Ͱ� ������ �ϴ� �׳����־���... ���߿� ������ ����
		unsigned int index = vertices.size();
		//vertexTable[vertex] = index;
		indices.push_back(index);
		vertices.push_back(vertex);
		return;


		// ���߿� ���⼭ ����
		map<MyFbxVertex, unsigned int>::iterator lookup = vertexTable.find(vertex);
		if (lookup != vertexTable.end())
		{
			// ������ �����ϴ� ���̶��(��ϵǾ� �ִ� ���ؽ� ���̶��) �� ���� �ε��� ������ �־��ش�.
			indices.push_back(lookup->second);
		}
		else
		{
			// �������� �ʴ� ���̶�� ���ؽ� ���̺� ��� 
			// ������ ����Ʈ�� �߰����ش�.
			// ���� �ε����� ���� �Ǹ������� �߰��ϴ� ���̱� ������ size()���� �޾Ƽ� �ʱ�ȭ ���ش�.
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