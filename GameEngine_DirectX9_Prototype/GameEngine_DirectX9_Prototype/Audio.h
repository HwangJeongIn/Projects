#ifndef AUDIO_H
#define AUDIO_H


// 외부 라이브러리 사용 FMOD
/*
- 인클루드 경로에 추가
- 라이브러리 경로에 추가
- 추가종속성에 사용할 lib추가
- dll 경로 설정(이프로젝트에서만) // 환경변수 // dll을 복사해서 사용해도되지만 경로설정으로 해주었다.
  // directX SDK(dll 없음)와는 다르게 dll를 사용하고 있기 때문에 마지막 작업을 추가적으로 해주었다.
*/

// cpp용 해더파일 추가
#include <fmod.hpp>
#include <iostream>
#include <vector>

using namespace std;


class Locator;

class Audio
{
private :
	FMOD::System *system = NULL;

	vector<FMOD::Sound *> effectSounds;
	vector<FMOD::Sound *> backGroundSounds;

protected:

	Audio(){}
	virtual ~Audio() {}

public :

	friend class Locator;
	void init()
	{
		FMOD_RESULT result;
		

		result = FMOD::System_Create(&system);      // Create the main system object.
		if (result != FMOD_OK)
		{
			//printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
			exit(-1);
		}

		result = system->init(512, FMOD_INIT_NORMAL, 0);    // Initialize FMOD.
		if (result != FMOD_OK)
		{
			//printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
			exit(-1);
		}
	}

	void registerSounds(const string & audioFileName, bool effectSound = false)
	{
		FMOD::Sound * sound = nullptr;
		system->createSound(audioFileName.c_str(), FMOD_DEFAULT, 0, &sound);

		if (effectSound)
		{
			effectSounds.push_back(sound); return;
		}

		backGroundSounds.push_back(sound);
	}

	void release()
	{
		FMOD::Sound::release();
		FMOD::System::close(system);

	}
};

#endif