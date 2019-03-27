#ifndef FBXMODELANIMATIONS_H 
#define FBXMODELANIMATIONS_H

#include "FbxModelAnimation.h"
#include <map>
#include "Utility.h"
// �⺻���� ����
/*
���⼭�� �ִϸ��̼��� �̸��� �� ������ ī��Ʈ ������ ����Ʈ�� �����ϰ� �ִ�.
������ ����� ���� �����ؼ� �� �ִϸ��̼��� ��ϵǾ������� ����� ����Ѵ�.
*/


// �ִϸ��̼� ���� ����
/*
���� �ִϸ��̼��� ���� ���ؼ� �ִϸ��̼� ���� �̸��� �ʿ��ϴ�.
�� �����̸��� ������ �ִϸ��̼� �̸��� ��Ī�ϰ� �� �ִϸ��̼� �̸����� animationTable���� ã�Ƽ� �÷����Ѵ�.

���� : 

	1. �ִϸ��̼� ���� �̸� �ߺ� ��� X // ������ ���� �ִϸ��̼������� �־�� �ϱ� ������ �ߺ��Ǿ� ������ ����.
	2. ���� �ִϸ��̼� �̸� �ߺ� ��� X // ���� ����غ�����

���� ���
	�ִϸ��̼� �����̸� > �ִϸ��̼� �̸� > �ִϸ��̼� ��ü�� �������� �����ߴٰ�
	����� �ִϸ��̼� �����̸� + �ִϸ��̼� �̸����� �Ϸ����ߴµ� �ᱹ �ִϸ��̼� �����̸��� �������� �ٷ� �����ϰ� ������
	map<pair<string, string> , FbxModelAnimation *> ���� ������ ��� �ݺ��ڸ� ���ؼ� ��ȸ�ϸ鼭 �����̸��� ��ϵǾ��ִ°����� ã�ƾ��Ѵ�.

	�׷��� ���⼭ �� �������� �ٸ� �����̶� �ִϸ��̼��� �̸��� ���ٸ� ����� ���Ѵٴ� ���̴�.
	
	�ذ�å

	1. �ð����⵵�� ���̱� ���ؼ� animationFileNameToNameTable �����ؼ� �����̸����� �ѹ��� ã�Ƽ� ���翩�� Ȯ��
	   // �׷��� ������ ���� �̸��� ������ ��ϵǾ��ִ��� ��ȸ�ϸ鼭 ã�ƾ� �Ѵ�.

	2. �����̸��� �ִϸ��̼� �̸�
*/


class FbxModelAnimations
{
private:
	
	map<pair<string,string>, FbxModelAnimation *> animationTable;
	FbxModelAnimation * currentAnimation;

	map<string, string> animationFileNameToNameTable;

	float elapsedTime;
	float keyFrameFactor;
	unsigned int currentKeyFrame;
	unsigned int nextKeyFrame;

	void play(const string & animationFileName, const string & animationName)
	{
		auto it = animationTable.find(pair<string, string>(animationFileName, animationName));
		// ���࿡ ��ϵǾ� ���� �ʴٸ� ���� �ִϸ��̼��� nullptr�� �ϰ� ����
		if (it == animationTable.end())
		{
			setCurrentAnimation(nullptr);
		}

		// ��ϵǾ��ִ� ��Ȳ
		setCurrentAnimation((*it).second);

	}

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

		animationFileNameToNameTable.clear();
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

		const string & animationFileName = animation->getAnimationFileName();
		const string & animationName = animation->getAnimationName();

		// ���� �����̸����� �ִϸ��̼� ������ ��ϵǾ��ִ��� Ȯ���Ѵ�. // ����� �ȵǾ��ִٸ� ������ش�.
		map<string, string>::iterator fileNameToName = animationFileNameToNameTable.find(animationFileName);
		if (fileNameToName == animationFileNameToNameTable.end())
		{
			// ����� �ȵǾ��ִٸ� ���
			animationFileNameToNameTable[animationFileName] = animation->getAnimationName();
		}

		// �ִϸ��̼� ����Ʈ�� �̹� ��ϵǾ��ִ��� Ȯ���ϰ� ��ϵǾ� �ִٸ� ����
		for (auto it = animationTable.begin(); it != animationTable.end(); ++it)
		{
			if ((*it).second == animation)
				return;
		}
		animationTable[pair<string,string>(animationFileName, animationName)] = animation;
	}

	void addAnimation(const string animationFileName,const string & animationName, int keyFrameCount, float frameRate)
	{
		FbxModelAnimation * animation = new FbxModelAnimation(animationFileName, animationName, keyFrameCount, frameRate);
		
		// ���� �����̸��� �ִϸ��̼� �̸��� ���̺� ������ش�.
		animationFileNameToNameTable[animationFileName] = animationName;

		// ���� �ִϸ��̼� ���̺� ����Ѵ�.
		animationTable[pair<string, string>(animationFileName, animationName)] = animation;
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

	void playWithFileName(const string & animationFileName)
	{
		auto it = animationFileNameToNameTable.find(animationFileName);
		if (it == animationFileNameToNameTable.end())
		{
			// ���࿡ �����̸� ���̺� �ִϸ��̼� �̸��� ��ϵǾ� ���� �ʴٸ� ����.
			setCurrentAnimation(nullptr);
			return;
		}

		// �װԾƴ϶�� ���� �ִϸ��̼� �̸����� �ٽ� ã�ƺ���.
		play(animationFileName, animationFileNameToNameTable[animationFileName]);

	}



	void stop()
	{
		setCurrentAnimation(nullptr);
	}

};


#endif