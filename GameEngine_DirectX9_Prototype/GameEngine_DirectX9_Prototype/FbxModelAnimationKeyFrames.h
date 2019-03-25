#ifndef FBXMODELANIMATIONKEYFRAMES_H
#define FBXMODELANIMATIONKEYFRAMES_H

#include <iostream>
#include <vector>
#include <string>
#include <d3dx9.h>

using namespace std;

class CombinationOfQuaternionAndTranslation
{
private :
	D3DXQUATERNION quaternion;
	D3DXVECTOR3 translation;

public :
	CombinationOfQuaternionAndTranslation(const D3DXMATRIX & matrix)
	{
		// transform matrix를 quaternion으로 변환
		D3DXQuaternionRotationMatrix(&quaternion, &matrix);
		translation = D3DXVECTOR3(matrix._41,matrix._42,matrix._43);
	}

	const D3DXQUATERNION & getQuaternion() const { return quaternion; }
	const D3DXVECTOR3 & getTranslation() const { return translation; }
};

class FbxModelAnimationKeyFrames
{
private :
	string animationName;
	vector<pair <D3DXMATRIX, CombinationOfQuaternionAndTranslation>> keyFrames;

public :
	FbxModelAnimationKeyFrames(const string & animationName)
		: animationName(animationName) 
	{}

	void addKeyFrame(const D3DXMATRIX & matrix)
	{
		CombinationOfQuaternionAndTranslation cQAT(matrix);
		keyFrames.push_back({ matrix, cQAT });
	}

	pair<D3DXMATRIX, CombinationOfQuaternionAndTranslation> * getKeyFrameTransform(unsigned int frameIndex)
	{
		if (frameIndex >= keyFrames.size() || frameIndex < 0) return nullptr;
		return &keyFrames[frameIndex];
	}

	unsigned int getKeyFrameCount() const{ return keyFrames.size(); }
	const string & getAnimationName() const { return animationName; }

	
};


#endif