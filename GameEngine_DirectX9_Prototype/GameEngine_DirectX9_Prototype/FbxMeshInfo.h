#ifndef FBXMESHINFO_H
#define FBXMESHINFO_H

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

	bool operator<(const MyVertex & other) const
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

class FbxMeshInfo
{
private:
	string fileName;

	FbxMesh * mesh;

	vector<FbxDouble3> controlPoints;
	vector<MyVertex> vertices;
	vector<unsigned int> indices;
	map<MyVertex , unsigned int> vertexTable;

public:
	FbxMeshInfo()
		: mesh(nullptr)/*, scene(nullptr)*/
	{

	}

	~FbxMeshInfo()
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

	MyVertex * getVertex(int index)
	{
		// ���࿡ ������� �ε����� ũ�ų� 0���� ������ nullptr����
		if (vertices.size() <= index || index <0)
			return nullptr;

		return &vertices[index];
	}

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
		return mesh->GetPolygonCount(); 
	}

	void setMesh(FbxMesh * mesh)
	{
		this->mesh = mesh;
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

				// ������ ������ �ʱ�ȭ �����ش�.
				// ���࿡ ���ο� ������ ������ ��� 0���� ����.
				position = { 0,0,0 };
				normal = { 0,0,0 };
				binormal = { 0,0,0 };
				tangent = { 0,0,0 };
				uv = { 0,0 };

				// ���� ���۷��� ��忡 ���� ���ؽ� ī���Ͱ� �ʿ��� ���� �ְ�
				// ��Ʈ�� ����Ʈ �ε����� �ʿ��� ���� �ִ�.
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
		MyVertex vertex { position, normal, uv, binormal, tangent };

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
		map<MyVertex, unsigned int>::iterator lookup = vertexTable.find(vertex);
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