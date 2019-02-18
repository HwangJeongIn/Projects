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

	// 2개의 역슬래쉬를 만나지 못했다. // 오류 발생
	if (i == -1)
		return;

	// 2개의 역슬래쉬를 만났다.
	filePathToLoadSoundFiles = filePathToLoadSoundFiles.substr(0, i + 1);
	filePathToLoadSoundFiles += fileNameToLoadSoundFiles + '\\';
	
}

void Audio::initAudioSourcesWithBaseSoundFiles()
{
	// 미리 몇개는 등록해두자
	// 이함수는 경로가 초기화 된 이후에 호출해야 한다.
	registerAudio("BS_BackGround_1.mp3", false);
	registerAudio("BS_BackGround_2.mp3", false);
	registerAudio("BS_Effect_1.mp3", true);

}

void Audio::release()
{
	auto it = audioSources.begin();
	for (; it != audioSources.end(); ++it)
	{
		// 널포인터라면 다음으로 넘어간다
		if ((*it).second == nullptr) continue;

		// 동적할당한 오디오 소스들을 모두 삭제한다.
		delete (*it).second;
		(*it).second = nullptr;
	}
	audioSources.clear();

	// 내부적으로 close를 해주고 있으므로 release만 호출
	system->release();
}

void Audio::registerAudio(const string & audioFileName, bool effectSound)
{
	// 만약에 등록이 되어있는 상황이라면 리턴한다. // 중복해서 등록하지 않는다. (못한다)
	if (audioSources.find(audioFileName) != audioSources.end())
		return;

	// 원래라면 AudioSource내부에서 해주어도 되지만 파일명으로 사운드를 생성해보고
	// 널포인터가 나오면 등록하지 않기 위해서 여기서 초기화해본다.

	FMOD::Sound * sound = nullptr;
	if (effectSound)
		system->createSound((filePathToLoadSoundFiles + audioFileName).c_str(), FMOD_DEFAULT, 0, &sound);
	else
		system->createSound((filePathToLoadSoundFiles + audioFileName).c_str(), FMOD_LOOP_NORMAL, 0, &sound);

	// 만약에 생성이 안되었다면 리턴 // 등록하지 않는다.
	if (!sound) return;

	audioSources[audioFileName] = new AudioSource(system, audioFileName, sound);
}

AudioSource * Audio::getAudioSource(const string & audioFileName)

{
	// 등록이 되어있지 않는 상황이라면 널포인터 리턴
	if (audioSources.find(audioFileName) == audioSources.end())
		return nullptr;

	return audioSources[audioFileName];
}