#ifndef FBXMODELSKELETONBONE_H
#define FBXMODELSKELETONBONE_H

#include <d3dx9.h>
#include <string>
#include <iostream>

/*
VertexAtTimeT = TransformationOfPoseAtTimeT * InverseOfGlobalBindPoseMatrix * VertexAtBindingTime 
transformLinkMatrix.Inverse() * transformMatrix * geometryTransform

// ���� ��ǥ��� ���ư��� > ��ü �޽� ��ȯ ���(freeze transformation �����ߴٸ� �������) > ������Ʈ�� ���

ã�ƺ� ��� �̷������� ���ָ� ���ú�ȯ�� �Žð����� �������� ����� �ʿ䰡 ���ٰ� �Ѵ�.
�ٵ� ���� ������Ʈ�� ����̳� transform ����� �����ִ°� ����� ���ϴ��� �߸𸣱� ������
������Ĵ�� ���� �������鼭 ��Ʈ������ ���ʴ�� �����ִ� ����� ä���Ѵ�.

// ���߿� ���� ������ ã�ƺ� ����


*/

using namespace std;

class FbxModelSkeletonBone
{
private :

	int parentBoneIndex;
	string parentBoneName;

	int boneIndex;
	string boneName;

	// The transformation of the mesh at binding time
	// ���ε� Ÿ�ӿ� ��ü �޽��� ��ȯ����̴�. // ��� Ŭ�����Ϳ��� ��� ����
	FbxMatrix clusterTransformMatrix;

	// ���� �߿��� ����̴�.
	// The transformation of the cluster(joint) at binding time from joint space to world space
	// ���ε� Ÿ�ӿ� ��������(���⿣������ ������)���� ����������� ��ȯ�ϴ� ����̴�.
	// �� �ʱ� ��� ���� ���庯ȯ ����̶�� ���� �ȴ�. ���� �ִϸ��̼��� �����Ҷ� 
	// �ʱ� ����(���� T������ �Ѵ�)�� ���� ������� ���ؼ� ������������ �ٽ� ���ư��� ������ ����� ���ʷ� ���ؼ� ���� ����� ������.
	FbxMatrix clusterTransformLinkMatrix; // ���⼭ cluster's link = joint(���⼭�� bone)�� �ǹ��Ѵ�

	FbxMatrix clusterTransformInverseMatrix;
	FbxMatrix clusterTransformInverseLinkMatrix;

	//map<string, KeyFrames> animation;
	// �ִϸ��̼� �̸� + Ű�����ӵ�
public :
	FbxModelSkeletonBone(int parentBoneIndex, const string & parentBoneName, int boneIndex, const string & boneName)
		: parentBoneIndex(parentBoneIndex), parentBoneName(parentBoneName), boneIndex(boneIndex), boneName(boneName)
	{
		clusterTransformMatrix.SetIdentity();
		clusterTransformLinkMatrix.SetIdentity();

		clusterTransformInverseMatrix.SetIdentity();
		clusterTransformInverseLinkMatrix.SetIdentity();
	}

	void setClusterTransformMatrix(const FbxMatrix & matrix)
	{
		// �������ٶ� ����ı��� ���� �������ش�.
		// Ŭ������ �������� �޾ƿ� �� �ִ� ����̴�.
		clusterTransformMatrix = matrix;
		clusterTransformInverseMatrix = clusterTransformMatrix.Inverse();
	}

	void setClusterTransformLinkMatrix(const FbxMatrix & matrix)
	{
		// �������ٶ� ����ı��� ���� �������ش�.
		// Ŭ������ �������� �޾ƿ� �� �ִ� ����̴�.
		clusterTransformLinkMatrix = matrix;
		clusterTransformInverseLinkMatrix = clusterTransformMatrix.Inverse();
	}

	const FbxMatrix & getClusterTransformMatrix() const { return clusterTransformMatrix; }
	const FbxMatrix & getClusterTransformInverseMatrix() const { return clusterTransformInverseMatrix; }
	const FbxMatrix & getClusterTransformLinkMatrix() const { return clusterTransformLinkMatrix; }
	const FbxMatrix & getClusterTransformInverseLinkMatrix() const { return clusterTransformInverseLinkMatrix; }

	const string & getParentBoneName() const { return parentBoneName; }
	int getParentBoneIndex() const { return parentBoneIndex; }
	const string & getBoneName() const { return boneName; }
	int getBoneIndex() const { return boneIndex; }

};

#endif