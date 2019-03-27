#ifndef FBXMODELANIMATIONS_H 
#define FBXMODELANIMATIONS_H

#include "FbxModelAnimation.h"
#include <map>
#include "Utility.h"
// 기본적인 구조
/*
여기서는 애니메이션의 이름과 그 프레임 카운트 프레임 레이트만 저장하고 있다.
나머지 계산은 본에 접근해서 그 애니메이션이 등록되어있으면 행렬을 계산한다.
*/


// 애니메이션 저장 형식
/*
먼저 애니메이션을 열기 위해서 애니메이션 파일 이름이 필요하다.
그 파일이름을 가지고 애니메이션 이름과 매칭하고 그 애니메이션 이름으로 animationTable에서 찾아서 플레이한다.

조건 : 

	1. 애니메이션 파일 이름 중복 허용 X // 어차피 같은 애니메이션폴더에 있어야 하기 때문에 중복되어 있을수 없다.
	2. 내부 애니메이션 이름 중복 허용 X // 좀더 고려해볼생각

설계 방법
	애니메이션 파일이름 > 애니메이션 이름 > 애니메이션 객체의 과정으로 설계했다가
	현재는 애니메이션 파일이름 + 애니메이션 이름으로 하려고했는데 결국 애니메이션 파일이름이 겹쳤을때 바로 리턴하고 싶은데
	map<pair<string, string> , FbxModelAnimation *> 으로 설계할 경우 반복자를 통해서 순회하면서 파일이름으 등록되어있는것인지 찾아야한다.

	그런데 여기서 또 문제점은 다른 파일이라도 애니메이션의 이름이 같다면 등록을 못한다는 점이다.
	
	해결책

	1. 시간복잡도를 줄이기 위해서 animationFileNameToNameTable 정의해서 파일이름으로 한번에 찾아서 존재여부 확인
	   // 그렇지 않으면 같은 이름의 파일이 등록되어있는지 순회하면서 찾아야 한다.

	2. 파일이름과 애니메이션 이름
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
		// 만약에 등록되어 있지 않다면 현재 애니메이션을 nullptr로 하고 리턴
		if (it == animationTable.end())
		{
			setCurrentAnimation(nullptr);
		}

		// 등록되어있는 상황
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
	애니메이션을 추가하는데 3가지 정보가 필요하다
	- animationName;
	- keyFrameCount;
	- frameRate;
	*/
	void addAnimation(FbxModelAnimation * animation)
	{
		if (!animation) return;

		const string & animationFileName = animation->getAnimationFileName();
		const string & animationName = animation->getAnimationName();

		// 먼저 파일이름으로 애니메이션 파일이 등록되어있는지 확인한다. // 등록이 안되어있다면 등록해준다.
		map<string, string>::iterator fileNameToName = animationFileNameToNameTable.find(animationFileName);
		if (fileNameToName == animationFileNameToNameTable.end())
		{
			// 등록이 안되어있다면 등록
			animationFileNameToNameTable[animationFileName] = animation->getAnimationName();
		}

		// 애니메이션 리스트에 이미 등록되어있는지 확인하고 등록되어 있다면 리턴
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
		
		// 먼저 파일이름과 애니메이션 이름의 테이블에 등록해준다.
		animationFileNameToNameTable[animationFileName] = animationName;

		// 최종 애니메이션 테이블에 등록한다.
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
			// 만약에 파일이름 테이블에 애니메이션 이름이 등록되어 있지 않다면 리턴.
			setCurrentAnimation(nullptr);
			return;
		}

		// 그게아니라면 받은 애니메이션 이름으로 다시 찾아본다.
		play(animationFileName, animationFileNameToNameTable[animationFileName]);

	}



	void stop()
	{
		setCurrentAnimation(nullptr);
	}

};


#endif