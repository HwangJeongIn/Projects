#include "WeightOfBones.h"
#include "FbxModelSkeletonBones.h"



const unsigned int WeightOfBones::maxNumberOfBones = 4;


D3DXMATRIX WeightOfBones::getCombinedBonesMatrixWithWeight(FbxModelSkeletonBones * skeletonBones) const
{
	if (!skeletonBones)
		return {
				1,0,0,0,
				0,1,0,0,
				0,0,1,0,
				0,0,0,1
			    };
	
	// ���鿡 ���� ����ġ�� ���� ��� ��������� ��ȯ�ȴ�.
	D3DXMATRIX result
	{
		0,0,0,0,
		0,0,0,0,
		0,0,0,0,
		0,0,0,0
	};


	// 1. �������� �ε����� �޾ƿ��� �� ���鿡���� �ִϸ��̼� ��Ʈ������ �޾ƿ´�
	// 2. �� �ִϸ��̼Ǹ�Ʈ������ ����ġ�� ���ؼ� ��Ʈ������ ���� �����ؼ� result�� �����Ѵ�.
	FbxModelSkeletonBone * bone = nullptr;

	//if (weightOfBones.size() >= 1)
	//{
	//	bone = skeletonBones->findBoneWithBoneIndex(weightOfBones[0].first);
	//	if (bone)
	//	{
	//		result = bone->getFinalAnimationMatrix();
	//	}
	//}
	//return result;

	for (int i = 0; i < weightOfBones.size(); ++i)
	{
		bone = skeletonBones->findBoneWithBoneIndex(weightOfBones[i].first);
		if (bone)
		{
			result += weightOfBones[i].second * bone->getFinalAnimationMatrix();
			//Trace::Write("TAG_DEBUG", "value", weightOfBones[i].second);
		}
	}
	//Trace::Write("TAG_DEBUG", "");

	return result;
}
