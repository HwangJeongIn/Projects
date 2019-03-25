#ifndef FBXMODELANIMATIONS_H 
#define FBXMODELANIMATIONS_H

#include "FbxModelAnimation.h"
#include <map>
#include "Utility.h"
/*
여기서는 애니메이션의 이름과 그 프레임 카운트 프레임 레이트만 저장하고 있다.
나머지 계산은 본에 접근해서 그 애니메이션이 등록되어있으면 행렬을 계산한다.
*/

class FbxModelAnimations
{
private:
	map<string, FbxModelAnimation *> animationTable;
	FbxModelAnimation * currentAnimation;

	float elapsedTime;
	float keyFrameFactor;
	unsigned int currentKeyFrame;
	unsigned int nextKeyFrame;

public:
	FbxModelAnimations()
		: currentAnimation(nullptr), currentKeyFrame(0), nextKeyFrame(1), 
		elapsedTime(0), keyFrameFactor(0.0f)
	{}

	~FbxModelAnimations()
	{
		for (auto it = animationTable.begin(); it != animationTable.end(); ++it)
		{
			if ((*it).second)
			{
				delete (*it).second;
				(*it).second = nullptr;
			}
		}
		animationTable.clear();
	}

	bool getIsAnimating() const { return currentAnimation!=nullptr; }
	float getKeyFrameFactor() const { return keyFrameFactor; }
	unsigned int getCurrentKeyFrame() const { return currentKeyFrame; }
	unsigned int getNextKeyFrame() const { return nextKeyFrame; }
	FbxModelAnimation * getCurrentAnimation() { return currentAnimation; }

	void update()
	{
		if (!currentAnimation) return;

		elapsedTime += (float)FrameTime::getDeltaTime()/1000.0f;
		//float frameRate = currentAnimation->getFrameRate();
		float oneFrame = 1.0f / currentAnimation->getFrameRate();
		int frameCount = currentAnimation->getKeyFrameCount();
		while (elapsedTime > oneFrame)
		{
			currentKeyFrame = (currentKeyFrame + 1) % frameCount;
			nextKeyFrame = (nextKeyFrame + 1) % frameCount;

			elapsedTime -= oneFrame;
		}

		keyFrameFactor = elapsedTime / oneFrame;

	}

	/*
	애니메이션을 추가하는데 3가지 정보가 필요하다
	- animationName;
	- keyFrameCount;
	- frameRate;
	*/
	void addAnimation(FbxModelAnimation * animation)
	{
		if (!animation) return;
		// 이미 등록되어있는지 확인하고 등록되어 있다면 리턴
		for (auto it = animationTable.begin(); it != animationTable.end(); ++it)
		{
			if ((*it).second == animation)
				return;
		}
		animationTable[animation->getAnimationName()] = animation;
	}

	void addAnimation(const string & animationName, int keyFrameCount, float frameRate)
	{
		FbxModelAnimation * animation = new FbxModelAnimation(animationName, keyFrameCount, frameRate);
		animationTable[animationName] = animation;
	}

	void resetTimeInfos()
	{
		currentKeyFrame = 0;
		nextKeyFrame = 1;
		elapsedTime = 0.0f;
		keyFrameFactor = 0.0f;
	}

	void setCurrentAnimation(FbxModelAnimation * animation)
	{
		if (!animation)
		{
			resetTimeInfos();
			currentAnimation = nullptr;
			return;
		}

		resetTimeInfos();
		currentAnimation = animation;
	}

	void play(const string & animationName)
	{
		auto it = animationTable.find(animationName);
		// 만약에 등록되어 있지 않다면 현재 애니메이션을 nullptr로 하고 리턴
		if (it == animationTable.end())
		{
			setCurrentAnimation(nullptr);
		}

		// 등록되어있는 상황
		setCurrentAnimation((*it).second);

	}

	void stop()
	{
		setCurrentAnimation(nullptr);
	}

};


#endif