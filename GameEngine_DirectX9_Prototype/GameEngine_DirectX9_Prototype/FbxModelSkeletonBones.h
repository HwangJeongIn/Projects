#ifndef FBXMODELSKELETONBONES_H
#define FBXMODELSKELETONBONES_H

#include "FbxModelSkeletonBone.h"
#include <vector>
#include <iostream>

using namespace std;

class FbxModelSkeletonBones
{
private :
	vector<FbxModelSkeletonBone * > bones;
public :
	~FbxModelSkeletonBones()
	{
		// 본정보 모두 삭제
		for (auto it = bones.begin(); it != bones.end(); ++it)
		{
			if((*it) !=nullptr)
				delete (*it);
			(*it) = nullptr;
		}
		bones.clear();
	}
};
#endif