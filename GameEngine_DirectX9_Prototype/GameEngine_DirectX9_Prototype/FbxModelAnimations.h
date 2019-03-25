#ifndef FBXMODELANIMATIONS_H 
#define FBXMODELANIMATIONS_H

#include "FbxModelAnimation.h"
#include <map>
#include "Utility.h"
/*
���⼭�� �ִϸ��̼��� �̸��� �� ������ ī��Ʈ ������ ����Ʈ�� �����ϰ� �ִ�.
������ ����� ���� �����ؼ� �� �ִϸ��̼��� ��ϵǾ������� ����� ����Ѵ�.
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
	�ִϸ��̼��� �߰��ϴµ� 3���� ������ �ʿ��ϴ�
	- animationName;
	- keyFrameCount;
	- frameRate;
	*/
	void addAnimation(FbxModelAnimation * animation)
	{
		if (!animation) return;
		// �̹� ��ϵǾ��ִ��� Ȯ���ϰ� ��ϵǾ� �ִٸ� ����
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
		// ���࿡ ��ϵǾ� ���� �ʴٸ� ���� �ִϸ��̼��� nullptr�� �ϰ� ����
		if (it == animationTable.end())
		{
			setCurrentAnimation(nullptr);
		}

		// ��ϵǾ��ִ� ��Ȳ
		setCurrentAnimation((*it).second);

	}

	void stop()
	{
		setCurrentAnimation(nullptr);
	}

};


#endif