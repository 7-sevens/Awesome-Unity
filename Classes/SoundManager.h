//////////////////////////////////////////////////////////////////////////
//  @class  SoundManager
//	@brief	sound manager
//	@author sevens
//	@date   2015-03-01
//	@blog   http://www.developer1024.com/
//////////////////////////////////////////////////////////////////////////
#ifndef SOUND_MANAGER_H_
#define SOUND_MANAGER_H_


enum SoundType
{
	SOUND_TYPE_GAMEWIN,
	SOUND_TYPE_GAMELOSE,
	SOUND_TYPE_MOVE,
	SOUND_TYPE_MERGE,
};

class SoundManager
{
public:
	static SoundManager* getInstance();

protected:
	SoundManager();
	~SoundManager();

public:
	// 音效开关控制
	void ChangeSoundSwitch(bool bSoundOn);

	// 播放音乐
	void PlayMusic(SoundType type);

	// 播放音效
	void PlayEffect(SoundType type);

private:
	bool _bSoundOn;
};

#endif // SOUND_MANAGER_H_