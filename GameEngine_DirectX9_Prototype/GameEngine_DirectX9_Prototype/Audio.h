#ifndef AUDIO_H
#define AUDIO_H


// �ܺ� ���̺귯�� ��� FMOD
/*
- ��Ŭ��� ��ο� �߰�
- ���̺귯�� ��ο� �߰�
- �߰����Ӽ��� ����� lib�߰�
- dll ��� ����(��������Ʈ������) // ȯ�溯�� // dll�� �����ؼ� ����ص������� ��μ������� ���־���.
  // directX SDK(dll ����)�ʹ� �ٸ��� dll�� ����ϰ� �ֱ� ������ ������ �۾��� �߰������� ���־���.
*/

// cpp�� �ش����� �߰�
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