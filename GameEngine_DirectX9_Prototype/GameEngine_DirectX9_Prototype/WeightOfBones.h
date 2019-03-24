#ifndef	WEIGHTOFBONES_H
#define WEIGHTOFBONES_H

#include <iostream>
#include <vector>
#include <fbxsdk.h>

using namespace std;


// 뼈들에 대한 가중치를 가지고 있다. 이는 하나의 버텍스마다 어떤본들에 대한 가중치가 있는지 나타낸다
// 만약에 애니메이션 할때 예상순서
/*
먼저 본들의 애니메이션의 특정 키프레임(보간된)에서의 매트릭스를 모두 계산한다.

처음 초기 T포즈의 역행렬(처음 포즈 트랜스폼을 캔슬하고 처음부터 계산하기 위해서) * 각본의 애니메이션 매트릭스
(부모본부터 내려와서 계산된 매트릭스 부모본 애니메이션 매트릭스 * 자식본 애니메이션 매트릭스)
//애니메이션 각 프레임(보간에의해서 그 사이값도)마다 각본들에서 T포즈의 역행렬 * 각본의 애니메이션 매트릭스를 계산해서 가지고 있어야한다.


이 버텍스들은 먼저 가중치를 받은 본들의 매트릭스들을 조합해서 최종 매트릭스를 계산해서
버텍스 버퍼를 열어서 모두 넣어준다. MyVertex에서 position * 최종애니메이션 버텍스 매트릭스 해서 mesh객체의 v포인트를 받아서 넣어준다.

*/
/*
* 본들의 가중치들은 어떻게 저장할까?
특정 본의 가중치를 가져올 필요가 있을까? => X // 계산할때 순서대로 받아와서 전체 가중치 계산을 하면된다.
데이터구조가 트리인 map보다는 데이터값이 연속적으로 저장되는 vector가 적절할것같다.

* 본의 이름을 가지고 가중치를 저장해야할까 아니면 본의 인덱스를 가지고 있어야 할까?
두가지 정보를 모두 제공한다면 또 함수를 나눠서 생각해야 할것이다. // 이름으로 본을 찾아서 애니메이션 매트릭스를 곱해주거나 인덱스로 찾아야 할것이다.
이름은 다른 애니메이션 파일에서 본의 이름이 다르게 정의될 수도 있다.
그렇기 때문에 본의인덱스로 접근하기로한다. 

* 본의 인덱스와 가중치를 같이 묶어서 vector로 넣어주기로 했다. 묶는 방법은 pair형식을 사용한다.

* 나중에 블렌드 본을 넣고 블렌드 작업을 할때

*/
class WeightOfBones
{
private :
	// 가중치를 받을 수 있는 최대 본 개수를 4개로 지정했다.
	static const unsigned int maxNumberOfBones; // 4

	// 본들의 가중치를 저장하고 있는 vector
	vector<pair<int, float>> weightOfBones;

	float sumOfWeights;
public :
	WeightOfBones()
		: sumOfWeights(0) 
	{}
	void addWeightOfBone(int boneIndex, float weight)
	{
		// 본의 인덱스는 0번부터 시작한다.
		// 최상위 본의 인덱스는 0이고 그 부모본은 없기 때문에 -1로 정의한다.
		// 또 들어온 가중치가 0~1사이에 없다면 리턴한다.
		if (boneIndex < 0 || weight < 0 || weight > 1) return;
		
		// 만약 최대 본의 개수보다 많거나 전체 가중치 합이 1이나 1을 넘었을 경우 리턴한다.
		if (weightOfBones.size() > 4 || sumOfWeights >= 1.0f) return;

		// 만약 들어온 가중치와 현재 가중치 합이 1을 넘으면 1에 맞춰서 weight를 지정한다.
		if (sumOfWeights + weight > 1.0f)
			weight = 1.0f - sumOfWeights;

		// 가중치 합에 현재 들어온 가중치를 더해준다.
		sumOfWeights += weight;
		
		weightOfBones.push_back({ boneIndex, weight });
	}

	FbxMatrix getCombinedBonesMatrixWithWeight() const
	{
		// 본들에 대한 가중치가 없는 경우 단위행렬이 반환된다.
		FbxMatrix result;
		result.SetIdentity();

		// 1. 각본들을 인덱스로 받아오고 그 본들에대한 애니메이션 매트릭스를 받아온다
		// 2. 그 애니메이션매트릭스와 가중치를 곱해서 매트릭스를 최종 조합해서 result에 저장한다.

		return result;
	}

	// 디버깅용
	vector<pair<int, float>> & getAllInfo(){ return weightOfBones; }

};

#endif