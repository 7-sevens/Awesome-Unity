//////////////////////////////////////////////////////////////////////////
//  @class  GameManager
//	@brief	game manager
//	@author sevens
//	@date   2015-03-02
//	@blog   http://www.developer1024.com/
//////////////////////////////////////////////////////////////////////////
#ifndef GAME_MANAGER_H_
#define GAME_MANAGER_H_

#include "GameDef.h"


class GameManager
{
public:
	static GameManager* getInstance();
protected:
	GameManager();
	~GameManager();

public:
	void SetGameMode(ENM_GAME_MODE enmGameMode);
	ENM_GAME_MODE GetGameMode();

private:
	ENM_GAME_MODE	_enmGameMode;

};

#endif // GAME_MANAGER_H_