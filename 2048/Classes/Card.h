//////////////////////////////////////////////////////////////////////////
//  @class  Card
//	@brief	card sprite
//	@author sevens
//	@date   2015-03-01
//	@blog   http://www.developer1024.com/
//////////////////////////////////////////////////////////////////////////
#ifndef CARD_H_
#define CARD_H_

#include "cocos2d.h"

USING_NS_CC;


class Card : public cocos2d::Sprite
{
public:
	static Card* createCard(int number, int cardWidth, int cardHeight, float px, float py);
	virtual bool init();
	CREATE_FUNC(Card);

public:
	void SetCardNumber(int number);
	int GetCardNumber();

	LayerColor* GetCardBGLayer();

private:
	void initCard(int number, int cardWidth, int cardHeight, float px, float py);

private:
	int				_cardNumber;
	LabelTTF*		_cardLabel;
	LayerColor*		_cardLayerColor;

};

#endif // CARD_H_