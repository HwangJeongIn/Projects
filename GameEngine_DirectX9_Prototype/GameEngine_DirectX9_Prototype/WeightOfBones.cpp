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
	
	// 본들에 대한 가중치가 없는 경우 단위행렬이 반환된다.
	D3DXMATRIX result
	{
		0,0,0,0,
		0,0,0,0,
		0,0,0,0,
		0,0,0,0
	};


	// 1. 각본들을 인덱스로 받아오고 그 본들에대한 애니메이션 매트릭스를 받아온다
	// 2. 그 애니메이션매트릭스와 가중치를 곱해서 매트릭스를 최종 조합해서 result에 저장한다.
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
