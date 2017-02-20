/** @class  GameMainScene
  * @brief  游戏主场景
  * @author sevens
  * @date   2015/03/01
  * @blog   http://www.developer1024.com/
  */

#ifndef __CARD_SPRITE_H__
#define __CARD_SPRITE_H__

#include "cocos2d.h"

USING_NS_CC;


class CardSprite :public Sprite
{
public:
	//创建三部曲
	static CardSprite* createCard(int num, int cardWidth, int cardHeight, float px, float py);
	virtual bool init();
	CREATE_FUNC(CardSprite);

	void setNumber(int num);
	int getNumber();
	LayerColor* getCardLayer();
private:
	int number;
	void cardInit(int num, int cardWidth, int cardHeight, float px, float py);
	LabelTTF *cardLabel;
	LayerColor *cardBgColour;
};

#endif // __CARD_SPRITE_H__