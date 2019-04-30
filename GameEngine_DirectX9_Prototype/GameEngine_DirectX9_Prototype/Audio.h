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

// 설계
/*
- AudioSource라는 클래스 정의 내부에서 개별적인 채널과 사운드 객체를 가지고 있다
- AudioSource는 사용자가 엉뚱한 행동을 하지 못하게 내부적으로 작업해주고 외부에 인터페이스만 제공한다
  // 엉뚱한 행동 : 사운드객체와 맞지않는 다른 채널에서 플레이할경우를 막아줌
- AduioSource에서 외부에 제공할 함수
	void play();
	void pause();
	void stop();

- Audio에서는 사운드를 찾을 수 있는 map제공(getSounds함수를 사용해서 get)
- Audio에서 제공할 함수
	void registerSound(const string & fileName, bool effectSound);
		// 등록되어 있으면 등록하지 않고 안등록된 경우만 등록해준다. // 이펙트 사운드이면 루프 설정을 하지 않는다.
	bool getSounds(AudioSource & output, const string & fileName)
		// 등록되어 있지 않을 경우는 널포인터로 초기화 하고 false를 반환한다.
*/

// cpp용 해더파일 추가

#include <fmod.hpp>
#include <cassert>
#include <Windows.h>
#include <string>
#include <iostream>
#include <map>

using namespace std;


class Locator;

class AudioSource
{
private:
	FMOD::System *system;
	FMOD::Sound * sound;
	FMOD::Channel * channel;
	string audioFileName;

	// 복사생성 제한
	AudioSource(AudioSource & other){}

	bool checkInit() const { return ((system) && (sound)); }

public:
	AudioSource(FMOD::System * system, const string & audioFileName, FMOD::Sound * sound)
		: system(system), audioFileName(audioFileName), sound(sound), channel(nullptr){}
	virtual ~AudioSource()
	{
		sound->release();
	}

	//ignore플래그가 켜져있으면 사운드를 무조건 하나 생성한다.
	// 주로 효과음 겹치게 할때 사용하면 좋을듯
	void play(bool ignore = false)
	{
		if (!checkInit()) return;

		// channel이 nullptr일때는 처음 생성하는 것이기 때문에 플레이해준다.
		if (!channel )
		{
			system->playSound(sound, 0, false, &channel);
			return;
		}

		// channel이 생성되어 있는데 stop에 의해서 멈춰있다면(isPlaying이 false) 역시 플레이한다.
		// 또 ignore플래그가 켜져있을때 무조건 사운드를 새로 플레이한다.
		if (!isPlaying() || ignore)
		{
			system->playSound(sound, 0, false, &channel);
			return;
		}


		// 현재플레이중 true / 널포인터 아님 / ignore플래그 켜져있지 않음
		// unpause 해준다.
		unpause();

	}

	void pause()
	{
		if (!channel) return;
		channel->setPaused(true);
	}
	void unpause()
	{
		if (!channel) return;
		channel->setPaused(false);
	}

	void stop()
	{
		if (!channel) return;
		channel->stop();
	}

	bool isPlaying() const 
	{
		if (!channel) return false;
		bool flag = false;
		channel->isPlaying(&flag);
		return flag;
	}

	bool getPaused() const
	{
		if (!channel) return false;
		bool flag = false;
		channel->getPaused(&flag);
		return flag;
	}

	void setVolume(float value)
	{
		// 사용해본 결과 절댓값기준으로 되는듯
		// -값도 들어간다 // 단 너무 큰값이 들어가지 않게 하기 위해서 여기서 조정해줄 필요가 있을듯
		// 여기선 15로 제한했다
		if (!channel) return;

		if (abs(value) > 15) value = 15.0f;

		channel->setVolume(value);

	}
	void getMode(FMOD_MODE * mode)
	{
		sound->getMode(mode);
	}


};

class Audio
{
private :
	FMOD::System *system = NULL;

	map<string, AudioSource * > audioSources;
	static const string filePathToLoadSoundFiles;
	const string fileNameToLoadSoundFiles = "Sound";
	AudioSource * currentBackGroundSound;
	void init();
	void initFilePathToLoadSoundFiles();
	void initAudioSourcesWithBaseSoundFiles();
	void release();


	virtual AudioSource * getAudioSource(const string & audioFileName);
protected :

	Audio()
		: currentBackGroundSound(nullptr)
	{
		init();
	}
	virtual ~Audio() 
	{
		release();
	}

public :

	friend class Locator;

	virtual void registerAudio(const string & audioFileName, bool effectSound = false);
	virtual void update() { system->update(); }
	virtual void playBackGroundSound(const string & audioFileName);
	virtual void playEffectSound(const string & audioFileName, bool ignore = false);

};

// 오디오를 끄기위한 장치
// 디버깅할때 소리를 듣지 않기 위한 장치
class NullAudio : public Audio
{
private:
	NullAudio()
		: Audio() {}
	virtual ~NullAudio(){}

	virtual AudioSource * getAudioSource(const string & audioFileName) { return nullptr; }
public :
	friend class Locator;
	virtual void registerAudio(const string & audioFileName, bool effectSound = false) {}
	virtual void update() {}
	virtual void playBackGroundSound(const string & audioFileName) {}
	virtual void playEffectSound(const string & audioFileName, bool ignore = false) {}
};

class DebuggingAudio : public Audio
{

private:
	Audio & wrapped;

	DebuggingAudio(Audio & audio)
		: wrapped(audio) {}
	virtual ~DebuggingAudio() {}

	void log(const char * message)
	{
		if (!message) return;
		cout << message << endl;
	}

	Audio * getWrappedAudio() { return &wrapped; }

public:
	friend class Locator;
	//  나중에 로그 찍는 것으로 바꿀예정
	//virtual void registerAudio(const string & audioFileName, bool effectSound = false) {}
	//virtual AudioSource * getAudioSource(const string & audioFileName) { return nullptr; }
	//	virtual void update() {}
};


#endif