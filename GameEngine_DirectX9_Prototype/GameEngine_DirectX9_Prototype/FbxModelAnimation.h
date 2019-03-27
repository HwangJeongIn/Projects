#ifndef FBXMODELANIMATION_H 
#define FBXMODELANIMATION_H

#include <iostream>
#include <string>

using namespace std;

class FbxModelAnimation
{
private :
	string animationFileName;
	string animationName;
	int keyFrameCount;
	float frameRate;

public :
	FbxModelAnimation(const string & animationFileName, const string & animationName, int keyFrameCount, float frameRate)
		: animationFileName(animationFileName), animationName(animationName), keyFrameCount(keyFrameCount), frameRate(frameRate)
	{

	}

	const string & getAnimationFileName() const { return animationFileName; }
	const string & getAnimationName() const { return animationName; }
	int getKeyFrameCount() const { return keyFrameCount; }
	float getFrameRate() const { return frameRate; }


};


#endif