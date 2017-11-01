#ifndef GAME_DEF_H_
#define GAME_DEF_H_

#define GRID_SPACE	5			// 网格间距
#define MAX_SIDE_GRID_COUNT	8	// 每一行(列)网格最大数

// 游戏模式
enum ENM_GAME_MODE
{
	GAME_MODE_NORMAL_4X4,		//经典模式4x4
	GAME_MODE_NORMAL_5X5,		//经典模式5x5
	GAME_MODE_NORMAL_6X6,		//经典模式6x6

	GAME_MODE_DEADLINE_5MIN,	//限时模式5MIN
	GAME_MODE_DEADLINE_10MIN,	//限时模式10MIN
	GAME_MODE_DEADLINE_15MIN,	//限时模式15MIN

	GAME_MODE_ENDLESS,			//无尽模式
};

#endif // GAME_DEF_H_