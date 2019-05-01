#include "Audio.h"

const string Audio::filePathToLoadSoundFiles = "../Audio/Sound/";

void Audio::init()
{
	FMOD_RESULT result;

	result = FMOD::System_Create(&system);
	if (result != FMOD_OK)
		assert(false);

	result = system->init(512, FMOD_INIT_NORMAL, 0);
	if (result != FMOD_OK)
		assert(false);

	initFilePathToLoadSoundFiles();
	initAudioSourcesWithBaseSoundFiles();
}

void Audio::initFilePathToLoadSoundFiles()
{
	/*
	char tempPath[512];
	GetModuleFileName(NULL, tempPath, 512);

	filePathToLoadSoundFiles = tempPath;

	int count = 0;
	int i = filePathToLoadSoundFiles.size();
	for (; i >= 0; --i)
	{
		if (filePathToLoadSoundFiles[i] == '\\') ++count;

		if (count == 2) break;

	}

	// 2���� ���������� ������ ���ߴ�. // ���� �߻�
	if (i == -1)
		return;

	// 2���� ���������� ������.
	filePathToLoadSoundFiles = filePathToLoadSoundFiles.substr(0, i + 1);
	filePathToLoadSoundFiles += fileNameToLoadSoundFiles + '\\';
	*/

	
	
	// �������͸��� �����Ҷ� ���� ��ȣ / �� \\ �� ����.
	//filePathToLoadSoundFiles = "C:/Users/user/Documents/GitHub/Projects/GameEngine_DirectX9_Prototype/Sound/";
	//filePathToLoadSoundFiles = "C:\\Users\\user\\Documents\\GitHub/Projects\\GameEngine_DirectX9_Prototype\\Sound\\";


	// ����η� �ϴ°� �����ҵ��ϴ�.
	/*
	../ �� ���� �ҽ������� �ִ� ���͸��� ���� ���͸��� �����ϰ�  
	C:\Users\user\Documents\GitHub\Projects\GameEngine_DirectX9_Prototype\GameEngine_DirectX9_Prototype�� ������
	C:\Users\user\Documents\GitHub\Projects\GameEngine_DirectX9_Prototype �̴�
	���⼭ fileNameToLoadSoundFiles(Sound)�� ����
	���� ��δ� C:\Users\user\Documents\GitHub\Projects\GameEngine_DirectX9_Prototype\Sound
	*/
	//filePathToLoadSoundFiles = "../Audio/Sound/"; //"../Sound/";

}

void Audio::initAudioSourcesWithBaseSoundFiles()
{
	// �̸� ��� ����ص���
	// ���Լ��� ��ΰ� �ʱ�ȭ �� ���Ŀ� ȣ���ؾ� �Ѵ�.
	registerAudio("BS_BackGround_BossMonster.mp3", false);
	registerAudio("BS_BackGround_EndScene.mp3", false);
	registerAudio("BS_BackGround_MainScene.mp3", false);
	registerAudio("BS_BackGround_Monster.mp3", false);
	registerAudio("BS_BackGround_StartScene.mp3", false);

	registerAudio("BS_Effect_Arrow.mp3", true);
	registerAudio("BS_Effect_Bird1.mp3", true);
	registerAudio("BS_Effect_Bird2.mp3", true);
	registerAudio("BS_Effect_Bird3.mp3", true);
	registerAudio("BS_Effect_Bird4.mp3", true);
	registerAudio("BS_Effect_BossMonsterAttack.mp3", true);
	registerAudio("BS_Effect_BossMonsterDamaged.mp3", true);
	registerAudio("BS_Effect_Button.mp3", true);
	registerAudio("BS_Effect_FootStep.mp3", true);
	registerAudio("BS_Effect_FootStepSingle.mp3", true);
	registerAudio("BS_Effect_MainButton.mp3", true);
	registerAudio("BS_Effect_MonsterAttack.mp3", true);
	registerAudio("BS_Effect_MonsterDamaged.mp3", true);
	registerAudio("BS_Effect_Wind.mp3", true);
	/*
	BS_BackGround_BossMonster.mp3
	BS_BackGround_EndScene.mp3
	BS_BackGround_MainScene.mp3
	BS_BackGround_Monster.mp3
	BS_BackGround_StartScene.mp3
	BS_Effect_Arrow.mp3
	BS_Effect_Bird1.mp3
	BS_Effect_Bird2.mp3
	BS_Effect_Bird3.mp3
	BS_Effect_Bird4.mp3
	BS_Effect_BossMonsterAttack.mp3
	BS_Effect_BossMonsterDamaged.mp3
	BS_Effect_Button.mp3
	BS_Effect_FootStep.mp3
	BS_Effect_FootStepSingle.mp3
	BS_Effect_MainButton.mp3
	BS_Effect_MonsterAttack.mp3
	BS_Effect_MonsterDamaged.mp3
	BS_Effect_Wind.mp3
	*/
}

void Audio::release()
{
	auto it = audioSources.begin();
	for (; it != audioSources.end(); ++it)
	{
		// �������Ͷ�� �������� �Ѿ��
		if ((*it).second == nullptr) continue;

		// �����Ҵ��� ����� �ҽ����� ��� �����Ѵ�.
		delete (*it).second;
		(*it).second = nullptr;
	}
	audioSources.clear();

	// ���������� close�� ���ְ� �����Ƿ� release�� ȣ��
	system->release();
}

void Audio::registerAudio(const string & audioFileName, bool effectSound)
{
	// ���࿡ ����� �Ǿ��ִ� ��Ȳ�̶�� �����Ѵ�. // �ߺ��ؼ� ������� �ʴ´�. (���Ѵ�)
	if (audioSources.find(audioFileName) != audioSources.end())
		return;

	// ������� AudioSource���ο��� ���־ ������ ���ϸ����� ���带 �����غ���
	// �������Ͱ� ������ ������� �ʱ� ���ؼ� ���⼭ �ʱ�ȭ�غ���.

	FMOD::Sound * sound = nullptr;
	if (effectSound)
		system->createSound((filePathToLoadSoundFiles + audioFileName).c_str(), FMOD_DEFAULT, 0, &sound);
	else
		system->createSound((filePathToLoadSoundFiles + audioFileName).c_str(), FMOD_LOOP_NORMAL, 0, &sound);
	
	// ���࿡ ������ �ȵǾ��ٸ� ���� // ������� �ʴ´�.
	if (!sound) return;

	audioSources[audioFileName] = new AudioSource(system, audioFileName, sound);
}

AudioSource * Audio::getAudioSource(const string & audioFileName)

{
	// ����� �Ǿ����� �ʴ� ��Ȳ�̶�� �������� ����
	if (audioSources.find(audioFileName) == audioSources.end())
		return nullptr;

	return audioSources[audioFileName];
}

void Audio::playBackGroundSound(const string & audioFileName)
{
	if (currentBackGroundSound)
	{
		currentBackGroundSound->pause();
		currentBackGroundSound->stop();
	}


	auto it = audioSources.find(audioFileName);
	if (it == audioSources.end()) return;
	currentBackGroundSound = it->second;
	it->second->play();
	it->second->setVolume(6.0f);
}

void Audio::playEffectSound(const string & audioFileName, bool ignore)
{
	auto it = audioSources.find(audioFileName);
	if (it == audioSources.end()) return;
	FMOD_MODE mode = 0;
	it->second->getMode(&mode);
	// �⺻�����̾ƴϸ� ������ ����ɼ��ִ�.
	if (mode == FMOD_LOOP_NORMAL) return;
	it->second->play(ignore);
	it->second->setVolume(2.0f);
}


