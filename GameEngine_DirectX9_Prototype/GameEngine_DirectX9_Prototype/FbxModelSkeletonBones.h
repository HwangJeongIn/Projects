#ifndef FBXMODELSKELETONBONES_H
#define FBXMODELSKELETONBONES_H

#include "FbxModelSkeletonBone.h"
#include <vector>
#include <iostream>
#include "Trace.h"
#include "Utility.h"

using namespace std;

// vector�� �ƴ� ���� map���� �����ߴٸ�? vector vs map
/*
** map // map�� �ؽ����̺�� ���������� 

1. key : �̸� / value : �ε����ΰ��

�̸����� ã���� �ð����⵵ : O(1) // �����δ� Ʈ�������̱� ������ O(logn)
�ε����� ã���� �ð����⵵ : O(n)
�̸� �ߺ����X

2. key : �ε��� / value : �̸��ΰ��

�ε����� ã���� �ð����⵵ : O(1) // �����δ� Ʈ�������̱� ������ O(logn)
�̸����� ã���� �ð����⵵ : O(n)
�ε��� �ߺ����X

** vector

���� : �ε��� = vector�� �ε���

�ε����� ã���� �ð����⵵ : O(1)
�̸����� ã���� �ð����⵵ : O(n) // ��ȸ�ϸ鼭 �̸��� ��ġ�ϸ� ����

�ᱹ �� �ڷᱸ�� ��� �ð����⵵ ���鿡�� ����ϴ�. // ���⼭�� ������ vector�� �����ϴ�. // lonN vs 1
�ٵ� ��ȸ�ϴ� �ӵ��� vector�� �޸𸮰� �������̱� ������ �ξ� �����ϴ�. �ᱹ vector�ڷᱸ���� ä���ߴ�
*/

class FbxModelSkeletonBones
{
private :
	
	// ���࿡ ���̴��� �Ἥ �÷��� �Ѵٸ� ���� ������ ���縦 ���� ���� �����Ҵ��� �ϴ°� ������?
	// ������ ���߿� ���̴��� ���� �����غ��ٰ� �޶����������� // ����� ���̴��� �ƴ� ���� cpu���� ����� �Ѵ�
	// ** �׿� ������� ���Ϳ� AddBone�Ҷ� ���簡 �Ͼ�� ������ ���縦 �������� �����Ҵ��� �ϴ°� ������ ���� // ���ο� ���� ���Ƽ� �����ϴµ� �ð��� �� �ɸ��Ͱ���
	

	// ���⼭ ���� �ε��� = ������ �ε��� �̴�.
	// �� �ֻ��� ���� �ε����� 0�̰� �ֻ������� �θ��� �ε����� -1�̴�.
	vector<FbxModelSkeletonBone *> bones;

public :
	FbxModelSkeletonBones()
		//:
	{}

	~FbxModelSkeletonBones()
	{
		// �����Ҵ��� ������ ��� ����
		for (auto it = bones.begin(); it != bones.end(); ++it)
		{
			if((*it) !=nullptr)
				delete (*it);
			(*it) = nullptr;
		}
		bones.clear();
	}

	void setAllBonesAnimationMatrix(const string & animationName, unsigned int currentKeyFrame, unsigned int nextKeyFrame, float keyFrameFactor)
	{
		D3DXMATRIX parentAnimationMatrix;
		D3DXMATRIX animationMatrix;
		D3DXMATRIX inverseMatrix;

		// ���ʴ�� ���� �޾Ƽ�(�ֻ������� ��ȸ�ϰԵ�) ��Ʈ������ �������ش�.
		for (int i = 0; i < bones.size(); ++i)
		{
			D3DXMatrixIdentity(&parentAnimationMatrix);
			D3DXMatrixIdentity(&animationMatrix);
			D3DXMatrixIdentity(&inverseMatrix);


			const FbxMatrix & tempInverse = bones[i]->getClusterTransformInverseLinkMatrix();
			FbxDXConverter::ToD3DXMATRIX(inverseMatrix, tempInverse);


			FbxModelAnimationKeyFrames * animationKeyFrames = bones[i]->getAnimationKeyFrames(animationName);
			if (animationKeyFrames == nullptr)
			{
				// ���������϶�
				// �ִϸ��̼� ����� ��������̴�.
				bones[i]->setAnimationMatrix(animationMatrix * parentAnimationMatrix);
				
				// ���������϶�
				// ���� �ִϸ��̼� ����� T������ ����� * ��������̴�.
				bones[i]->setFinalAnimationMatrix(inverseMatrix * bones[i]->getAnimationMatrix());
				continue;
			}

			int parentIndex = bones[i]->getParentBoneIndex();

			// �θ��� �ε����� -1�� �ƴҶ��� ������Ŀ��� ����� �ȴ�.
			if (parentIndex != -1)
			{
				parentAnimationMatrix = bones[parentIndex]->getAnimationMatrix();
			}
			
		
			pair<D3DXMATRIX, CombinationOfQuaternionAndTranslation> * currentP = animationKeyFrames->getKeyFrameTransform(currentKeyFrame);
			pair<D3DXMATRIX, CombinationOfQuaternionAndTranslation> * nextP = animationKeyFrames->getKeyFrameTransform(nextKeyFrame);

			// �Ѵ� ���� �ƴҶ� �����۾��� ���ش�.
			if (currentP && nextP /*&& false*/)
			{
				const D3DXQUATERNION & currentQ = currentP->second.getQuaternion();
				const D3DXQUATERNION & nextQ = nextP->second.getQuaternion();
				D3DXQUATERNION q;
				D3DXQuaternionSlerp(&q, &currentQ, &nextQ, keyFrameFactor);
				D3DXMatrixRotationQuaternion(&animationMatrix,&q);


				const D3DXVECTOR3 & currentT = currentP->second.getTranslation();
				const D3DXVECTOR3 & nextT = nextP->second.getTranslation();
				D3DXVECTOR3 t;
				D3DXVec3Lerp(&t, &currentT, &nextT, keyFrameFactor);

				animationMatrix._41 = t.x;
				animationMatrix._42 = t.y;
				animationMatrix._43 = t.z;

			}
			//else
			//{
			//	animationMatrix = animationKeyFrames->getKeyFrameTransform(currentKeyFrame)->first;
			//}




			bones[i]->setAnimationMatrix( animationMatrix * parentAnimationMatrix);
			bones[i]->setFinalAnimationMatrix(inverseMatrix * bones[i]->getAnimationMatrix());
		}
	}


	// ���������� �ε����� �������� ������ ���� ���� ������ �ε����� bones.size()���̴�
	int getIndexOfBoneToInsert() const { return bones.size(); }

	// get�� �ƴ� find��� �̸��� ����� ���� : ��ã�� ���ɼ��� �ֱ� ������ �׻� get�� �������� ���Ѵ�.

	FbxModelSkeletonBone * findBoneWithBoneIndex(int index)
	{
		if (index < 0 || index >= bones.size())
			return nullptr;

		return bones[index];
	}

	const string * findBoneNameWithBoneIndex(int index)
	{
		if (index < 0 || index >= bones.size())
			return nullptr;

		return &bones[index]->getBoneName();
	}

	FbxModelSkeletonBone * findBoneWithBoneName(const string & name)
	{
		for (int i = 0; i < bones.size(); ++i)
		{
			if (bones[i]->getBoneName() == name) return bones[i];
		}
		// ��ã�������
		return nullptr;
	}

	int findBoneIndexWithBoneName(const string & name)
	{
		for (int i = 0; i < bones.size(); ++i)
		{
			if (bones[i]->getBoneName() == name) return i;
		}
		// ��ã�������
		return -1;
	}


	void AddBone(FbxModelSkeletonBone * bone)
	{
		if (!bone) return;
		bones.push_back(bone);
	}

	void AddBone(int parentBoneIndex, const string & parentBoneName, int boneIndex, const string & boneName)
	{
		// �θ��� -1�̸��� �ɼ� ���� �ڽĺ��� -1���ϰ� �� �� ����.
		if (parentBoneIndex < -1 || boneIndex <= -1) return;
		FbxModelSkeletonBone * bone = new FbxModelSkeletonBone(parentBoneIndex, parentBoneName, boneIndex, boneName);
		bones.push_back(bone);
	}

	// ������
	void printAllInfo() const
	{
		for (int i = 0; i < bones.size(); ++i)
		{
			Trace::Write("TAG_DEBUG", "Index", i);
			if (bones[i])
			{
				Trace::Write("TAG_DEBUG", "boneIndex", bones[i]->getBoneIndex());
				Trace::Write("TAG_DEBUG", "boneName", bones[i]->getBoneName());

				Trace::Write("TAG_DEBUG", "parentBoneIndex", bones[i]->getParentBoneIndex());
				Trace::Write("TAG_DEBUG", "parentBoneName", bones[i]->getParentBoneName());


			}
			Trace::Write("TAG_DEBUG", "", "");

		}
	}





};
#endif