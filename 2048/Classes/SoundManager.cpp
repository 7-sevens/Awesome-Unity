#include "SoundManager.h"
#include "SimpleAudioEngine.h"


static SoundManager *s_pInstance = nullptr;

SoundManager* SoundManager::getInstance()
{
	if (!s_pInstance)
	{
		s_pInstance = new SoundManager();
	}

	return s_pInstance;
}

SoundManager::SoundManager()
{
	_bSoundOn = true;
}

SoundManager::~SoundManager()
{

}

void SoundManager::ChangeSoundSwitch(bool bSoundOn)
{
	_bSoundOn = bSoundOn;
}

void SoundManager::PlayMusic(SoundType type)
{
	switch (type)
	{
	case SOUND_TYPE_GAMEWIN:
		break;
	case SOUND_TYPE_GAMELOSE:
		break;
	default:
		break;
	}
}

void SoundManager::PlayEffect(SoundType type)
{
	switch (type)
	{
	case SOUND_TYPE_MOVE:
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("move.wav", false);
		break;
	case SOUND_TYPE_MERGE:
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("merge.wav", false);
		break;
	default:
		break;
	}
}
