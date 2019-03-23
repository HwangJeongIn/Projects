#ifndef FBXMODELSKELETONBONE_H
#define FBXMODELSKELETONBONE_H

#include <d3dx9.h>
#include <string>
#include <iostream>

/*
VertexAtTimeT = TransformationOfPoseAtTimeT * InverseOfGlobalBindPoseMatrix * VertexAtBindingTime 
transformLinkMatrix.Inverse() * transformMatrix * geometryTransform

// 월드 좌표계로 돌아가고 > 전체 메쉬 변환 행렬(freeze transformation 수행했다면 단위행렬) > 지오메트리 행렬

찾아본 결과 이런식으로 해주면 로컬변환을 매시간마다 수동으로 계산할 필요가 없다고 한다.
근데 아직 지오메트리 행렬이나 transform 행렬을 곱해주는게 어떤것을 뜻하는지 잘모르기 때문에
기존방식대로 본을 내려오면서 매트릭스를 차례대로 구해주는 방식을 채택한다.

// 나중에 좀더 정보를 찾아볼 예정


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
	// 바인딩 타임에 전체 메쉬의 변환행렬이다. // 모든 클러스터에서 모두 같다
	FbxMatrix clusterTransformMatrix;

	// 가장 중요한 행렬이다.
	// The transformation of the cluster(joint) at binding time from joint space to world space
	// 바인딩 타임에 관절공간(여기엔진에선 본공간)에서 월드공간으로 변환하는 행렬이다.
	// 즉 초기 포즈에 대한 월드변환 행렬이라고 보면 된다. 본에 애니메이션을 적용할때 
	// 초기 포즈(흔히 T포즈라고 한다)에 대한 역행렬을 곱해서 관절공간으로 다시 돌아가고 나머지 행렬을 차례로 곱해서 최종 행렬을 만들어낸다.
	FbxMatrix clusterTransformLinkMatrix; // 여기서 cluster's link = joint(여기서는 bone)를 의미한다

	FbxMatrix clusterTransformInverseMatrix;
	FbxMatrix clusterTransformInverseLinkMatrix;

	//map<string, KeyFrames> animation;
	// 애니메이션 이름 + 키프레임들
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
		// 설정해줄때 역행렬까지 같이 설정해준다.
		// 클러스터 정보에서 받아올 수 있는 행렬이다.
		clusterTransformMatrix = matrix;
		clusterTransformInverseMatrix = clusterTransformMatrix.Inverse();
	}

	void setClusterTransformLinkMatrix(const FbxMatrix & matrix)
	{
		// 설정해줄때 역행렬까지 같이 설정해준다.
		// 클러스터 정보에서 받아올 수 있는 행렬이다.
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