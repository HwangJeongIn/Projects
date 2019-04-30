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

// ����
/*
- AudioSource��� Ŭ���� ���� ���ο��� �������� ä�ΰ� ���� ��ü�� ������ �ִ�
- AudioSource�� ����ڰ� ������ �ൿ�� ���� ���ϰ� ���������� �۾����ְ� �ܺο� �������̽��� �����Ѵ�
  // ������ �ൿ : ���尴ü�� �����ʴ� �ٸ� ä�ο��� �÷����Ұ�츦 ������
- AduioSource���� �ܺο� ������ �Լ�
	void play();
	void pause();
	void stop();

- Audio������ ���带 ã�� �� �ִ� map����(getSounds�Լ��� ����ؼ� get)
- Audio���� ������ �Լ�
	void registerSound(const string & fileName, bool effectSound);
		// ��ϵǾ� ������ ������� �ʰ� �ȵ�ϵ� ��츸 ������ش�. // ����Ʈ �����̸� ���� ������ ���� �ʴ´�.
	bool getSounds(AudioSource & output, const string & fileName)
		// ��ϵǾ� ���� ���� ���� �������ͷ� �ʱ�ȭ �ϰ� false�� ��ȯ�Ѵ�.
*/

// cpp�� �ش����� �߰�

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

	// ������� ����
	AudioSource(AudioSource & other){}

	bool checkInit() const { return ((system) && (sound)); }

public:
	AudioSource(FMOD::System * system, const string & audioFileName, FMOD::Sound * sound)
		: system(system), audioFileName(audioFileName), sound(sound), channel(nullptr){}
	virtual ~AudioSource()
	{
		sound->release();
	}

	//ignore�÷��װ� ���������� ���带 ������ �ϳ� �����Ѵ�.
	// �ַ� ȿ���� ��ġ�� �Ҷ� ����ϸ� ������
	void play(bool ignore = false)
	{
		if (!checkInit()) return;

		// channel�� nullptr�϶��� ó�� �����ϴ� ���̱� ������ �÷������ش�.
		if (!channel )
		{
			system->playSound(sound, 0, false, &channel);
			return;
		}

		// channel�� �����Ǿ� �ִµ� stop�� ���ؼ� �����ִٸ�(isPlaying�� false) ���� �÷����Ѵ�.
		// �� ignore�÷��װ� ���������� ������ ���带 ���� �÷����Ѵ�.
		if (!isPlaying() || ignore)
		{
			system->playSound(sound, 0, false, &channel);
			return;
		}


		// �����÷����� true / �������� �ƴ� / ignore�÷��� �������� ����
		// unpause ���ش�.
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
		// ����غ� ��� ���񰪱������� �Ǵµ�
		// -���� ���� // �� �ʹ� ū���� ���� �ʰ� �ϱ� ���ؼ� ���⼭ �������� �ʿ䰡 ������
		// ���⼱ 15�� �����ߴ�
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

// ������� �������� ��ġ
// ������Ҷ� �Ҹ��� ���� �ʱ� ���� ��ġ
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
	//  ���߿� �α� ��� ������ �ٲܿ���
	//virtual void registerAudio(const string & audioFileName, bool effectSound = false) {}
	//virtual AudioSource * getAudioSource(const string & audioFileName) { return nullptr; }
	//	virtual void update() {}
};


#endif