#include "Card.h"


Card* Card::createCard(int number, int cardWidth, int cardHeight, float px, float py)
{
	Card *card = new Card();
	if (!card->init())
	{
		CC_SAFE_RELEASE(card);
		return NULL;
	}

	card->autorelease();
	card->initCard(number, cardWidth, cardHeight, px, py);

	return card;
}

bool Card::init()
{
	if (!Sprite::init())
		return false;

	return true;
}

void Card::SetCardNumber(int number)
{
	_cardNumber = number;
	//根据不同的数字范围显示不同的字体大小及背景颜色
	if (number >= 0 && number < 16)
		_cardLabel->setFontSize(50);

	if (number >= 16 && number < 128)
		_cardLabel->setFontSize(40);

	if (number >= 128 && number < 1024)
		_cardLabel->setFontSize(30);

	if (number >= 1024)
		_cardLabel->setFontSize(20);

	if (number == 0)
		_cardLayerColor->setColor(Color3B(217, 217, 217));

	if (number == 2)
		_cardLayerColor->setColor(Color3B(240, 230, 220));

	if (number == 4)
		_cardLayerColor->setColor(Color3B(51, 153, 51));

	if (number == 8)
		_cardLayerColor->setColor(Color3B(255, 153, 102));

	if (number == 16)
		_cardLayerColor->setColor(Color3B(153, 204, 153));

	if (number == 32)
		_cardLayerColor->setColor(Color3B(153, 204, 255));

	if (number == 64)
		_cardLayerColor->setColor(Color3B(255, 204, 204));

	if (number == 128)
		_cardLayerColor->setColor(Color3B(204, 102, 0));

	if (number == 256)
		_cardLayerColor->setColor(Color3B(153, 204, 51));

	if (number == 512)
		_cardLayerColor->setColor(Color3B(255, 102, 102));

	if (number == 1024)
		_cardLayerColor->setColor(Color3B(204, 204, 255));

	if (number == 2048)
		_cardLayerColor->setColor(Color3B(255, 204, 0));

	if (number > 0)
		_cardLabel->setString(String::createWithFormat("%d", number)->getCString());
	else
		_cardLabel->setString("");
}

int Card::GetCardNumber()
{
	return _cardNumber;
}

LayerColor* Card::GetCardBGLayer()
{
	return _cardLayerColor;
}

void Card::initCard(int number, int cardWidth, int cardHeight, float px, float py)
{
	_cardNumber = number;

	_cardLayerColor = LayerColor::create(Color4B(217, 217, 217, 255), cardWidth - 5, cardHeight - 5);
	_cardLayerColor->setPosition(Point(px, py));
	if (number > 0)	//数字卡片
		_cardLabel = LabelTTF::create(String::createWithFormat("%d", number)->getCString(), "Arial", 50);
	else //空白卡片
		_cardLabel = LabelTTF::create("", "Arial", 50);
		
	_cardLabel->setPosition(_cardLayerColor->getContentSize().width / 2.0, _cardLayerColor->getContentSize().height / 2.0);
	_cardLayerColor->addChild(_cardLabel);

	this->addChild(_cardLayerColor);
}
