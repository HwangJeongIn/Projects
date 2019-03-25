#ifndef FBXMODELSKELETONBONE_H
#define FBXMODELSKELETONBONE_H

#include <d3dx9.h>
#include <string>
#include <map>
#include <iostream>
#include "FbxModelAnimationKeyFrames.h"

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

	// ���� �ִϸ��̼ǿ� ����� ���
	D3DXMATRIX finalAnimationMatrix;


	// ��������� ���� �ִϸ��̼� ��Ʈ������ ���ϱ� ���� �ʿ��� ��Ʈ����
	D3DXMATRIX animationMatrix;



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

	// �ִϸ��̼� �̸� + Ű�����ӵ�
	map<string, FbxModelAnimationKeyFrames *> animationKeyFramesTable;

public :
	FbxModelSkeletonBone(int parentBoneIndex, const string & parentBoneName, int boneIndex, const string & boneName)
		: parentBoneIndex(parentBoneIndex), parentBoneName(parentBoneName), boneIndex(boneIndex), boneName(boneName)
	{
		D3DXMatrixIdentity(&animationMatrix);
		D3DXMatrixIdentity(&finalAnimationMatrix);

		clusterTransformMatrix.SetIdentity();
		clusterTransformLinkMatrix.SetIdentity();

		clusterTransformInverseMatrix.SetIdentity();
		clusterTransformInverseLinkMatrix.SetIdentity();
	}

	~FbxModelSkeletonBone()
	{
		for (auto it = animationKeyFramesTable.begin(); it != animationKeyFramesTable.end(); ++it)
		{
			if ((*it).second)
			{
				delete (*it).second;
				(*it).second = nullptr;
			}
		}
		animationKeyFramesTable.clear();
	}

	const D3DXMATRIX & getFinalAnimationMatrix() const { return finalAnimationMatrix; }
	const D3DXMATRIX & getAnimationMatrix() const { return animationMatrix; }
	void setFinalAnimationMatrix(const D3DXMATRIX & matrix) { finalAnimationMatrix = matrix; }
	void setAnimationMatrix(const D3DXMATRIX & matrix) { animationMatrix = matrix; }


	void addAnimationKeyFrames(FbxModelAnimationKeyFrames * animationKeyFrames)
	{
		if (!animationKeyFrames) return;
		const string & animationName = animationKeyFrames->getAnimationName();

		map<string, FbxModelAnimationKeyFrames *>::iterator it = animationKeyFramesTable.find(animationName);
		// ���� ��ϵǾ� �ִٸ� �����Ѵ�.
		if (it != animationKeyFramesTable.end()) return;

		// ����� �ȵǾ� �ִٸ� ������ش�
		animationKeyFramesTable[animationName] = animationKeyFrames;
	}

	FbxModelAnimationKeyFrames * getAnimationKeyFrames(const string & animationName)
	{
		map<string, FbxModelAnimationKeyFrames *>::iterator it = animationKeyFramesTable.find(animationName);
		// ���� ��ϵǾ� ���� �ʴٸ� �������� ����
		if (it == animationKeyFramesTable.end()) return nullptr;

		// ��ϵǾ� �ִٸ� ����
		return (*it).second;
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
		clusterTransformInverseLinkMatrix = clusterTransformLinkMatrix.Inverse();
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