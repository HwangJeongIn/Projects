#ifndef FBXMODELSKELETONBONES_H
#define FBXMODELSKELETONBONES_H

#include "FbxModelSkeletonBone.h"
#include <vector>
#include <iostream>
#include "Trace.h"

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