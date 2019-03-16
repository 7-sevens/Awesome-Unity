#include "GameManager.h"


static GameManager *s_pInstance = nullptr;

GameManager* GameManager::getInstance()
{
	if (!s_pInstance)
	{
		s_pInstance = new GameManager();
	}

	return s_pInstance;
}

GameManager::GameManager()
:_enmGameMode(GAME_MODE_DEADLINE_5MIN)
{

}

GameManager::~GameManager()
{

}

void GameManager::SetGameMode(ENM_GAME_MODE enmGameMode)
{
	_enmGameMode = enmGameMode;
}

ENM_GAME_MODE GameManager::GetGameMode()
{
	return _enmGameMode;
}
