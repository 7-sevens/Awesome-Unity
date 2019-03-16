//////////////////////////////////////////////////////////////////////////
//  @class  GameMainScene
//	@brief	game main scene
//	@author sevens
//	@date   2015-03-01
//	@blog   http://www.developer1024.com/
//////////////////////////////////////////////////////////////////////////
#ifndef GAME_MAIN_SCENE_H_
#define GAME_MAIN_SCENE_H_

#include "cocos2d.h"
#include "Card.h"
#include "GameDef.h"

class GameMainScene : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();
	virtual bool init();
	CREATE_FUNC(GameMainScene);

	virtual bool onTouchBegan(cocos2d::Touch *pTouch, cocos2d::Event *pEvent);
	virtual void onTouchEnded(cocos2d::Touch *pTouch, cocos2d::Event *pEvent);

public:
	// 创建游戏卡片网格矩阵
	void CreateGrid();

	void RandomCard();

	bool MoveLeft();
	bool MoveRight();
	bool MoveUp();
	bool MoveDown();

private:
	// 根据游戏模式计算游戏中相关适配数据
	void matchGameModeData();

	void updateDeadlineTime(float dt);

	void checkGameResult();

private:
	Card*	_cardArray[MAX_SIDE_GRID_COUNT][MAX_SIDE_GRID_COUNT];			//卡片矩阵
	Card*	_cardActionArray[MAX_SIDE_GRID_COUNT][MAX_SIDE_GRID_COUNT];	//卡片动画矩阵

	LabelTTF* _deadlineLabel;

	timeval	_timeval;

	Point	_touchBeginPos;

	double	_iCardSideLength;			//卡片边长
	int		_iSideGridCount;			//每一行（列）方格数目

	double	_GridBGSideLength;			//方格背景边长

	int		_iGameTime;					//限时模式游戏时长（second）
};

#endif // GAME_MAIN_SCENE_H_