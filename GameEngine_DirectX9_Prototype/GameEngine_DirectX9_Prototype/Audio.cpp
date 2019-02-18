#include "Audio.h"

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
	
}

void Audio::initAudioSourcesWithBaseSoundFiles()
{
	// �̸� ��� ����ص���
	// ���Լ��� ��ΰ� �ʱ�ȭ �� ���Ŀ� ȣ���ؾ� �Ѵ�.
	registerAudio("BS_BackGround_1.mp3", false);
	registerAudio("BS_BackGround_2.mp3", false);
	registerAudio("BS_Effect_1.mp3", true);

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