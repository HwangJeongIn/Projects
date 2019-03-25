#ifndef FBXMODELSKELETONBONES_H
#define FBXMODELSKELETONBONES_H

#include "FbxModelSkeletonBone.h"
#include <vector>
#include <iostream>
#include "Trace.h"
#include "Utility.h"

using namespace std;

// vector가 아닌 만약 map으로 설계했다면? vector vs map
/*
** map // map을 해쉬테이블로 가정했을때 

1. key : 이름 / value : 인덱스인경우

이름으로 찾을때 시간복잡도 : O(1) // 실제로는 트리구조이기 때문에 O(logn)
인덱스로 찾을때 시간복잡도 : O(n)
이름 중복허용X

2. key : 인덱스 / value : 이름인경우

인덱스로 찾을때 시간복잡도 : O(1) // 실제로는 트리구조이기 때문에 O(logn)
이름으로 찾을때 시간복잡도 : O(n)
인덱스 중복허용X

** vector

보장 : 인덱스 = vector의 인덱스

인덱스로 찾을때 시간복잡도 : O(1)
이름으로 찾을때 시간복잡도 : O(n) // 순회하면서 이름이 일치하면 리턴

결국 두 자료구조 모두 시간복잡도 측면에서 비슷하다. // 여기서도 솔직히 vector가 유리하다. // lonN vs 1
근데 순회하는 속도는 vector의 메모리가 연속적이기 때문에 훨씬 유리하다. 결국 vector자료구조를 채택했다
*/

class FbxModelSkeletonBones
{
private :
	
	// 만약에 쉐이더를 써서 올려야 한다면 내부 값들의 복사를 막기 위해 동적할당을 하는게 맞을까?
	// 구조는 나중에 쉐이더를 좀더 공부해보다가 달라질수도있음 // 현재는 쉐이더가 아닌 직접 cpu에서 계산을 한다
	// ** 그와 상관없이 벡터에 AddBone할때 복사가 일어나기 때문에 복사를 막기위해 동적할당을 하는게 나을것 같다 // 내부에 값이 많아서 복사하는데 시간이 좀 걸릴것같다
	

	// 여기서 본의 인덱스 = 벡터의 인덱스 이다.
	// 즉 최상위 본은 인덱스가 0이고 최상위본의 부모본은 인덱스가 -1이다.
	vector<FbxModelSkeletonBone *> bones;

public :
	FbxModelSkeletonBones()
		//:
	{}

	~FbxModelSkeletonBones()
	{
		// 동적할당한 본정보 모두 삭제
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

		// 차례대로 본을 받아서(최상위부터 순회하게됨) 매트릭스를 세팅해준다.
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
				// 널포인터일때
				// 애니메이션 행렬은 단위행렬이다.
				bones[i]->setAnimationMatrix(animationMatrix * parentAnimationMatrix);
				
				// 널포인터일때
				// 최종 애니메이션 행렬은 T포즈의 역행렬 * 단위행렬이다.
				bones[i]->setFinalAnimationMatrix(inverseMatrix * bones[i]->getAnimationMatrix());
				continue;
			}

			int parentIndex = bones[i]->getParentBoneIndex();

			// 부모본의 인덱스가 -1이 아닐때만 단위행렬에서 벗어나게 된다.
			if (parentIndex != -1)
			{
				parentAnimationMatrix = bones[parentIndex]->getAnimationMatrix();
			}
			
		
			pair<D3DXMATRIX, CombinationOfQuaternionAndTranslation> * currentP = animationKeyFrames->getKeyFrameTransform(currentKeyFrame);
			pair<D3DXMATRIX, CombinationOfQuaternionAndTranslation> * nextP = animationKeyFrames->getKeyFrameTransform(nextKeyFrame);

			// 둘다 널이 아닐때 보간작업을 해준다.
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


	// 순차적으로 인덱스가 정해지기 때문에 현재 본을 넣을때 인덱스는 bones.size()값이다
	int getIndexOfBoneToInsert() const { return bones.size(); }

	// get이 아닌 find라는 이름을 사용한 이유 : 못찾을 가능성도 있기 때문에 항상 get을 보장하지 못한다.

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
		// 못찾았을경우
		return nullptr;
	}

	int findBoneIndexWithBoneName(const string & name)
	{
		for (int i = 0; i < bones.size(); ++i)
		{
			if (bones[i]->getBoneName() == name) return i;
		}
		// 못찾았을경우
		return -1;
	}


	void AddBone(FbxModelSkeletonBone * bone)
	{
		if (!bone) return;
		bones.push_back(bone);
	}

	void AddBone(int parentBoneIndex, const string & parentBoneName, int boneIndex, const string & boneName)
	{
		// 부모본은 -1미만이 될수 없고 자식본은 -1이하가 될 수 없다.
		if (parentBoneIndex < -1 || boneIndex <= -1) return;
		FbxModelSkeletonBone * bone = new FbxModelSkeletonBone(parentBoneIndex, parentBoneName, boneIndex, boneName);
		bones.push_back(bone);
	}

	// 디버깅용
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