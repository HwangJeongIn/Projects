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

	// 2개의 역슬래쉬를 만나지 못했다. // 오류 발생
	if (i == -1)
		return;

	// 2개의 역슬래쉬를 만났다.
	filePathToLoadSoundFiles = filePathToLoadSoundFiles.substr(0, i + 1);
	filePathToLoadSoundFiles += fileNameToLoadSoundFiles + '\\';
	*/

	
	
	// 하위디렉터리로 접근할때 쓰는 기호 / 와 \\ 는 같다.
	//filePathToLoadSoundFiles = "C:/Users/user/Documents/GitHub/Projects/GameEngine_DirectX9_Prototype/Sound/";
	//filePathToLoadSoundFiles = "C:\\Users\\user\\Documents\\GitHub/Projects\\GameEngine_DirectX9_Prototype\\Sound\\";


	// 상대경로로 하는게 더편할듯하다.
	/*
	../ 로 현재 소스파일이 있는 디렉터리의 상위 디렉터리에 접근하고  
	C:\Users\user\Documents\GitHub\Projects\GameEngine_DirectX9_Prototype\GameEngine_DirectX9_Prototype의 상위는
	C:\Users\user\Documents\GitHub\Projects\GameEngine_DirectX9_Prototype 이다
	여기서 fileNameToLoadSoundFiles(Sound)에 접근
	최종 경로는 C:\Users\user\Documents\GitHub\Projects\GameEngine_DirectX9_Prototype\Sound
	*/
	//filePathToLoadSoundFiles = "../Audio/Sound/"; //"../Sound/";

}

void Audio::initAudioSourcesWithBaseSoundFiles()
{
	// 미리 몇개는 등록해두자
	// 이함수는 경로가 초기화 된 이후에 호출해야 한다.
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
	// 기본형식이아니면 무한히 재생될수있다.
	if (mode == FMOD_LOOP_NORMAL) return;
	it->second->play(ignore);
	it->second->setVolume(2.0f);
}


