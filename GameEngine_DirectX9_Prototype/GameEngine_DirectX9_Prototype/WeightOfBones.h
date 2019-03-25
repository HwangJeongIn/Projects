#ifndef	WEIGHTOFBONES_H
#define WEIGHTOFBONES_H

#include <iostream>
#include <vector>
#include <fbxsdk.h>
#include <d3dx9.h>
#include "Trace.h"

using namespace std;


// ���鿡 ���� ����ġ�� ������ �ִ�. �̴� �ϳ��� ���ؽ����� ����鿡 ���� ����ġ�� �ִ��� ��Ÿ����
// ���࿡ �ִϸ��̼� �Ҷ� �������
/*
���� ������ �ִϸ��̼��� Ư�� Ű������(������)������ ��Ʈ������ ��� ����Ѵ�.

ó�� �ʱ� T������ �����(ó�� ���� Ʈ�������� ĵ���ϰ� ó������ ����ϱ� ���ؼ�) * ������ �ִϸ��̼� ��Ʈ����
(�θ𺻺��� �����ͼ� ���� ��Ʈ���� �θ� �ִϸ��̼� ��Ʈ���� * �ڽĺ� �ִϸ��̼� ��Ʈ����)
//�ִϸ��̼� �� ������(���������ؼ� �� ���̰���)���� �����鿡�� T������ ����� * ������ �ִϸ��̼� ��Ʈ������ ����ؼ� ������ �־���Ѵ�.


�� ���ؽ����� ���� ����ġ�� ���� ������ ��Ʈ�������� �����ؼ� ���� ��Ʈ������ ����ؼ�
���ؽ� ���۸� ��� ��� �־��ش�. MyVertex���� position * �����ִϸ��̼� ���ؽ� ��Ʈ���� �ؼ� mesh��ü�� v����Ʈ�� �޾Ƽ� �־��ش�.

*/
/*
* ������ ����ġ���� ��� �����ұ�?
Ư�� ���� ����ġ�� ������ �ʿ䰡 ������? => X // ����Ҷ� ������� �޾ƿͼ� ��ü ����ġ ����� �ϸ�ȴ�.
�����ͱ����� Ʈ���� map���ٴ� �����Ͱ��� ���������� ����Ǵ� vector�� �����ҰͰ���.

* ���� �̸��� ������ ����ġ�� �����ؾ��ұ� �ƴϸ� ���� �ε����� ������ �־�� �ұ�?
�ΰ��� ������ ��� �����Ѵٸ� �� �Լ��� ������ �����ؾ� �Ұ��̴�. // �̸����� ���� ã�Ƽ� �ִϸ��̼� ��Ʈ������ �����ְų� �ε����� ã�ƾ� �Ұ��̴�.
�̸��� �ٸ� �ִϸ��̼� ���Ͽ��� ���� �̸��� �ٸ��� ���ǵ� ���� �ִ�.
�׷��� ������ �����ε����� �����ϱ���Ѵ�. 

* ���� �ε����� ����ġ�� ���� ��� vector�� �־��ֱ�� �ߴ�. ���� ����� pair������ ����Ѵ�.

* ���߿� ���� ���� �ְ� ���� �۾��� �Ҷ�

*/
class FbxModelSkeletonBones;

class WeightOfBones
{
private :
	// ����ġ�� ���� �� �ִ� �ִ� �� ������ 4���� �����ߴ�.
	static const unsigned int maxNumberOfBones; // 4

	// ������ ����ġ�� �����ϰ� �ִ� vector
	vector<pair<int, float>> weightOfBones;

	float sumOfWeights;
public :
	WeightOfBones()
		: sumOfWeights(0) 
	{}
	void addWeightOfBone(int boneIndex, float weight)
	{
		// ���� �ε����� 0������ �����Ѵ�.
		// �ֻ��� ���� �ε����� 0�̰� �� �θ��� ���� ������ -1�� �����Ѵ�.
		// �� ���� ����ġ�� 0~1���̿� ���ٸ� �����Ѵ�.
		if (boneIndex < 0 || weight < 0 || weight > 1) return;
		
		// ���� �ִ� ���� �������� ���ų� ��ü ����ġ ���� 1�̳� 1�� �Ѿ��� ��� �����Ѵ�.
		if (weightOfBones.size() > 4 || sumOfWeights >= 1.0f) return;

		// ���� ���� ����ġ�� ���� ����ġ ���� 1�� ������ 1�� ���缭 weight�� �����Ѵ�.
		if (sumOfWeights + weight > 1.0f)
			weight = 1.0f - sumOfWeights;

		// ����ġ �տ� ���� ���� ����ġ�� �����ش�.
		sumOfWeights += weight;
		
		weightOfBones.push_back({ boneIndex, weight });
	}

	D3DXMATRIX getCombinedBonesMatrixWithWeight(FbxModelSkeletonBones * skeletonBones) const;


	// ������
	vector<pair<int, float>> & getAllInfo(){ return weightOfBones; }

};

#endif