#include "GameMainScene.h"
#include "SoundManager.h"
#include "GameManager.h"

USING_NS_CC;


cocos2d::Scene* GameMainScene::createScene()
{
	auto scene = Scene::create();
	auto layer = GameMainScene::create();
	scene->addChild(layer);

	return scene;
}

bool GameMainScene::init()
{
	if (!Layer::init())
		return false;

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	// bg 
	auto bg = Sprite::create("bg.png");
	//auto bg = LayerColor::create(Color4B(234, 249, 249, 255));
	//bg->setPosition(Vec2(visibleSize.width / 4.0, visibleSize.height / 4.0));
	this->addChild(bg,-1);
	
	//标题
	auto title = LabelTTF::create("2048", "Arial", 50);
	title->setColor(Color3B(255, 255, 153));
	title->setPosition(Point(visibleSize.width / 4, visibleSize.height - 30));
	this->addChild(title);
	
	//auto scoreLayerColor = LayerColor::create(Color4B(217, 217, 217, 255), visibleSize.width / 4.0, visibleSize.width / 6.0);
	//scoreLayerColor->setPosition(Point(visibleSize.width / 2.0, visibleSize.height - visibleSize.width / 6.0 - 30));
	//this->addChild(scoreLayerColor);
	/*
	// logo
	auto logo = Sprite::create("logo.png");
	Size logoSize = logo->getContentSize();
	logo->setPosition(Vec2(logoSize.width/2.0 + 30, visibleSize.height - 20));
	this->addChild(logo);

	auto gameScore = Sprite::create("game_score.png");
	gameScore->setPosition(Vec2(visibleSize.width/2.0, visibleSize.height - 20));
	this->addChild(gameScore);

	auto bestScore = Sprite::create("best_score.png");
	bestScore->setPosition(Vec2(visibleSize.width - logoSize.width / 2.0 - 30, visibleSize.height - 20));
	this->addChild(bestScore);
	*/
	

	ENM_GAME_MODE game_mode = GameManager::getInstance()->GetGameMode();
	if (game_mode == GAME_MODE_DEADLINE_5MIN || game_mode == GAME_MODE_DEADLINE_10MIN || game_mode == GAME_MODE_DEADLINE_15MIN)
	{
		_deadlineLabel = LabelTTF::create("00:00", "Arial", 30);
		_deadlineLabel->setColor(Color3B(255, 0, 0));
		_deadlineLabel->setPosition(Point(visibleSize.width / 2, visibleSize.height - 70));
		this->addChild(_deadlineLabel);

		schedule(schedule_selector(GameMainScene::updateDeadlineTime), 1.0f, kRepeatForever, 1.0f);
	}

	_iCardSideLength = 0;
	_iSideGridCount = 4;
	matchGameModeData();

	// 创建卡片矩阵
	CreateGrid();
	RandomCard();
	RandomCard();

	// 创建Touch监听
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = CC_CALLBACK_2(GameMainScene::onTouchBegan, this);
	listener->onTouchEnded = CC_CALLBACK_2(GameMainScene::onTouchEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);


	return true;
}

bool GameMainScene::onTouchBegan(cocos2d::Touch *pTouch, cocos2d::Event *pEvent)
{
	gettimeofday(&_timeval, NULL);

	_touchBeginPos = pTouch->getLocation();

	return true;
}

void GameMainScene::onTouchEnded(cocos2d::Touch *pTouch, cocos2d::Event *pEvent)
{
	timeval tv_end;
	gettimeofday(&tv_end, NULL);
	if (tv_end.tv_sec - _timeval.tv_sec > 3)
	{
		//For Test
	}

	auto touchEndPos = pTouch->getLocation();
	int offsetX = touchEndPos.x - _touchBeginPos.x;
	int offsetY = touchEndPos.y - _touchBeginPos.y;

	bool bTouch = false;
	if (abs(offsetX) > abs(offsetY)) //方向判断
	{
		if (offsetX < -5)
			bTouch = MoveLeft();
		else if (offsetX > 5)
			bTouch = MoveRight();
	}
	else
	{
		if (offsetY > 5)
			bTouch = MoveDown();
		else if (offsetY < -5)
			bTouch = MoveUp();
	}

	//判断处理
	if (bTouch)
	{
		RandomCard();
	}
}

void GameMainScene::CreateGrid()
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	int iSideGridCount = _iSideGridCount;

	//创建卡片矩阵
	for (int i = 0; i < iSideGridCount; i++)
	{
		for (int j = 0; j < iSideGridCount; j++)
		{
			//左右边距留白15，最下面留白size.height/8
			//坐标从左下角算起，右为正，上为正
			Card* card = Card::createCard(0, _iCardSideLength, _iCardSideLength, _iCardSideLength*i + 15 + GRID_SPACE/2.0, _iCardSideLength*j + 15 - GRID_SPACE/2.0 + visibleSize.height / 8);
			this->addChild(card);
			_cardArray[i][j] = card;

			Card* actionCard = Card::createCard(0, _iCardSideLength, _iCardSideLength, _iCardSideLength*i + 15 + GRID_SPACE/2.0, _iCardSideLength*j + 15 - GRID_SPACE/2.0 + visibleSize.height / 8);
			this->addChild(actionCard);
			_cardActionArray[i][j] = actionCard;

			auto hide = Hide::create();
			_cardActionArray[i][j]->GetCardBGLayer()->runAction(hide);
		}
	}

	auto gridBGLayerColor = LayerColor::create(Color4B(200, 200, 200, 255), _GridBGSideLength, _GridBGSideLength);
	gridBGLayerColor->setPosition(Point(10, visibleSize.height / 8.0 + GRID_SPACE));
	this->addChild(gridBGLayerColor, -1);

	/*
	//创建临时卡片矩阵，用于动画，每个动画卡片对应一个实际卡片的动画.
	//这是个技巧,并且动画层在卡片层之上，所以后加入，也可以设置addchild层次
	for (int i = 0; i < iSideGridCount; i++)
	{
		for (int j = 0; j < iSideGridCount; j++)
		{
			//最左边留白12，最下面留白size.height/6
			//坐标从左下角算起，右为正，上为正
			Card* card = Card::createCard(0, _iCardSideLength, _iCardSideLength, _iCardSideLength*i + 12, _iCardSideLength*j + 12 + visibleSize.height / 6);
			this->addChild(card);
			_cardActionArray[i][j] = card;

			auto hide = Hide::create();
			_cardActionArray[i][j]->GetCardBGLayer()->runAction(hide);
		}
	}
	*/
}

void GameMainScene::RandomCard()
{
	int iSideGridCount = _iSideGridCount;
	int row = CCRANDOM_0_1() * iSideGridCount;
	int col = CCRANDOM_0_1() * iSideGridCount;

	if (_cardArray[row][col]->GetCardNumber() > 0)
	{
		RandomCard();
	}
	else
	{
		_cardArray[row][col]->SetCardNumber(CCRANDOM_0_1() * 10 < 1 ? 4 : 2);
		auto action = Sequence::createWithTwoActions(ScaleTo::create(0, 0), ScaleTo::create(0.3f, 1));
		_cardArray[row][col]->GetCardBGLayer()->runAction(action);
	}
}

bool GameMainScene::MoveLeft()
{
	bool bMoved = false;

	Size visibleSize = Director::getInstance()->getVisibleSize();
	//int iCardSize = (visibleSize.width - 4 * 5) / 4;

	for (int row = 0; row < _iSideGridCount; row++)
	{
		for (int col = 0; col < _iSideGridCount; col++)
		{	//内部类似冒泡排序
			for (int tempCol = col + 1; tempCol < _iSideGridCount; tempCol++)
			{
				if (_cardArray[tempCol][row]->GetCardNumber() > 0) //col右边的卡片有数字才移动
				{
					if (_cardArray[col][row]->GetCardNumber() == 0)
					{
						auto place = Place::create(Point(_iCardSideLength*tempCol + 12, _iCardSideLength*row + 12 + visibleSize.height / 6.0));
						_cardActionArray[tempCol][row]->SetCardNumber(_cardArray[tempCol][row]->GetCardNumber());
						auto show = Show::create();
						auto move = MoveBy::create(0.1f, Point(-_iCardSideLength*(tempCol - col), 0));
						auto hide = Hide::create();
						_cardActionArray[tempCol][row]->GetCardBGLayer()->runAction(Sequence::create(place, show, move, hide, NULL));
						
						//如果col位置是空卡片，就把tempCol卡片移到col处，tempCol处变成空卡片
						_cardArray[col][row]->SetCardNumber(_cardArray[tempCol][row]->GetCardNumber());
						_cardArray[tempCol][row]->SetCardNumber(0);
						SoundManager::getInstance()->PlayEffect(SOUND_TYPE_MOVE);

						col--; //再扫描一遍，确保所有结果正确
						bMoved = true;
					}
					else if (_cardArray[col][row]->GetCardNumber() == _cardArray[tempCol][row]->GetCardNumber())
					{
						auto place = Place::create(Point(_iCardSideLength*tempCol + 12, _iCardSideLength*row + 12 + visibleSize.height / 6.0));
						_cardActionArray[tempCol][row]->SetCardNumber(_cardArray[tempCol][row]->GetCardNumber());
						auto show = Show::create();
						auto move = MoveBy::create(0.1f, Point(-_iCardSideLength*(tempCol - col), 0));
						auto hide = Hide::create();
						_cardActionArray[tempCol][row]->GetCardBGLayer()->runAction(Sequence::create(place, show, move, hide, NULL));

						//如果col位置非空，且与tempCol位置的数字相同，则乘2
						_cardArray[col][row]->SetCardNumber(_cardArray[col][row]->GetCardNumber() * 2);
						_cardArray[tempCol][row]->SetCardNumber(0);
						SoundManager::getInstance()->PlayEffect(SOUND_TYPE_MERGE);

						//数字合并动画
						auto merge = Sequence::create(ScaleTo::create(0.1f, 1.2f), ScaleTo::create(0.1f, 1.0f), NULL);
						_cardArray[col][row]->GetCardBGLayer()->runAction(merge);

						bMoved = true;
					}

					break; //此处break防止出现连续乘2的bug
				}
			}
		}
	}

	return bMoved;
}

bool GameMainScene::MoveRight()
{
	bool bMoved = false;

	Size visibleSize = Director::getInstance()->getVisibleSize();
	//int iCardSize = (visibleSize.width - 4 * 5) / 4;

	for (int row = 0; row < _iSideGridCount; row++)
	{
		for (int col = _iSideGridCount-1; col >= 0; col--)
		{	//内部类似冒泡排序
			for (int tempCol = col - 1; tempCol >= 0; tempCol--)
			{
				if (_cardArray[tempCol][row]->GetCardNumber() > 0) //col右边的卡片有数字才移动
				{
					if (_cardArray[col][row]->GetCardNumber() == 0)
					{
						auto place = Place::create(Point(_iCardSideLength*tempCol + 12, _iCardSideLength*row + 12 + visibleSize.height / 6.0));
						_cardActionArray[tempCol][row]->SetCardNumber(_cardArray[tempCol][row]->GetCardNumber());
						auto show = Show::create();
						auto move = MoveBy::create(0.1f, Point(-_iCardSideLength*(tempCol - col), 0));
						auto hide = Hide::create();
						_cardActionArray[tempCol][row]->GetCardBGLayer()->runAction(Sequence::create(place, show, move, hide, NULL));
						
						//如果col位置是空卡片，就把tempCol卡片移到col处，tempCol处变成空卡片
						_cardArray[col][row]->SetCardNumber(_cardArray[tempCol][row]->GetCardNumber());
						_cardArray[tempCol][row]->SetCardNumber(0);
						SoundManager::getInstance()->PlayEffect(SOUND_TYPE_MOVE);

						col++; //再扫描一遍，确保所有结果正确
						bMoved = true;
					}
					else if (_cardArray[col][row]->GetCardNumber() == _cardArray[tempCol][row]->GetCardNumber())
					{
						auto place = Place::create(Point(_iCardSideLength*tempCol + 12, _iCardSideLength*row + 12 + visibleSize.height / 6.0));
						_cardActionArray[tempCol][row]->SetCardNumber(_cardArray[tempCol][row]->GetCardNumber());
						auto show = Show::create();
						auto move = MoveBy::create(0.1f, Point(-_iCardSideLength*(tempCol - col), 0));
						auto hide = Hide::create();
						_cardActionArray[tempCol][row]->GetCardBGLayer()->runAction(Sequence::create(place, show, move, hide, NULL));

						//如果col位置非空，且与tempCol位置的数字相同，则乘2
						_cardArray[col][row]->SetCardNumber(_cardArray[col][row]->GetCardNumber() * 2);
						_cardArray[tempCol][row]->SetCardNumber(0);
						SoundManager::getInstance()->PlayEffect(SOUND_TYPE_MERGE);

						//数字合并动画
						auto merge = Sequence::create(ScaleTo::create(0.1f, 1.2f), ScaleTo::create(0.1f, 1.0f), NULL);
						_cardArray[col][row]->GetCardBGLayer()->runAction(merge);

						bMoved = true;
					}

					break; //此处break防止出现连续乘2的bug
				}
			}
		}
	}

	return bMoved;
}

bool GameMainScene::MoveUp()
{
	bool bMoved = false;

	Size visibleSize = Director::getInstance()->getVisibleSize();
	//int iCardSize = (visibleSize.width - 4 * 5) / 4;

	for (int col = 0; col < _iSideGridCount; col++)
	{
		for (int row = 0; row < _iSideGridCount; row++)
		{	//内部类似冒泡排序
			for (int tempRow = row + 1; tempRow < _iSideGridCount; tempRow++)
			{
				if (_cardArray[col][tempRow]->GetCardNumber() > 0) //col右边的卡片有数字才移动
				{
					if (_cardArray[col][row]->GetCardNumber() == 0)
					{
						auto place = Place::create(Point(_iCardSideLength*col + 12, _iCardSideLength*tempRow + 12 + visibleSize.height / 6.0));
						_cardActionArray[col][tempRow]->SetCardNumber(_cardArray[col][tempRow]->GetCardNumber());
						auto show = Show::create();
						auto move = MoveBy::create(0.1f, Point(0,-_iCardSideLength*(tempRow - row)));
						auto hide = Hide::create();
						_cardActionArray[col][tempRow]->GetCardBGLayer()->runAction(Sequence::create(place, show, move, hide, NULL));
						
						//如果col位置是空卡片，就把tempCol卡片移到col处，tempCol处变成空卡片
						_cardArray[col][row]->SetCardNumber(_cardArray[col][tempRow]->GetCardNumber());
						_cardArray[col][tempRow]->SetCardNumber(0);
						SoundManager::getInstance()->PlayEffect(SOUND_TYPE_MOVE);

						row--; //再扫描一遍，确保所有结果正确
						bMoved = true;
					}
					else if (_cardArray[col][row]->GetCardNumber() == _cardArray[col][tempRow]->GetCardNumber())
					{
						auto place = Place::create(Point(_iCardSideLength*col + 12, _iCardSideLength*tempRow + 12 + visibleSize.height / 6.0));
						_cardActionArray[col][tempRow]->SetCardNumber(_cardArray[col][tempRow]->GetCardNumber());
						auto show = Show::create();
						auto move = MoveBy::create(0.1f, Point(0,-_iCardSideLength*(tempRow - row)));
						auto hide = Hide::create();
						_cardActionArray[col][tempRow]->GetCardBGLayer()->runAction(Sequence::create(place, show, move, hide, NULL));

						//如果col位置非空，且与tempCol位置的数字相同，则乘2
						_cardArray[col][row]->SetCardNumber(_cardArray[col][row]->GetCardNumber() * 2);
						_cardArray[col][tempRow]->SetCardNumber(0);
						SoundManager::getInstance()->PlayEffect(SOUND_TYPE_MERGE);

						//数字合并动画
						auto merge = Sequence::create(ScaleTo::create(0.1f, 1.2f), ScaleTo::create(0.1f, 1.0f), NULL);
						_cardArray[col][row]->GetCardBGLayer()->runAction(merge);

						bMoved = true;
					}

					break; //此处break防止出现连续乘2的bug
				}
			}
		}
	}

	return bMoved;
}

bool GameMainScene::MoveDown()
{
	bool bMoved = false;

	Size visibleSize = Director::getInstance()->getVisibleSize();
	//int iCardSize = (visibleSize.width - 4 * 5) / 4;

	for (int col = 0; col < _iSideGridCount; col++)
	{
		for (int row = _iSideGridCount-1; row >= 0; row--)
		{	//内部类似冒泡排序
			for (int tempRow = row - 1; tempRow >= 0; tempRow--)
			{
				if (_cardArray[col][tempRow]->GetCardNumber() > 0) //col右边的卡片有数字才移动
				{
					if (_cardArray[col][row]->GetCardNumber() == 0)
					{
						auto place = Place::create(Point(_iCardSideLength*col + 12, _iCardSideLength*tempRow + 12 + visibleSize.height / 6.0));
						_cardActionArray[col][tempRow]->SetCardNumber(_cardArray[col][tempRow]->GetCardNumber());
						auto show = Show::create();
						auto move = MoveBy::create(0.1f, Point(0,-_iCardSideLength*(tempRow - row)));
						auto hide = Hide::create();
						_cardActionArray[col][tempRow]->GetCardBGLayer()->runAction(Sequence::create(place, show, move, hide, NULL));
						
						//如果col位置是空卡片，就把tempCol卡片移到col处，tempCol处变成空卡片
						_cardArray[col][row]->SetCardNumber(_cardArray[col][tempRow]->GetCardNumber());
						_cardArray[col][tempRow]->SetCardNumber(0);
						SoundManager::getInstance()->PlayEffect(SOUND_TYPE_MOVE);

						row++; //再扫描一遍，确保所有结果正确
						bMoved = true;
					}
					else if (_cardArray[col][row]->GetCardNumber() == _cardArray[col][tempRow]->GetCardNumber())
					{
						auto place = Place::create(Point(_iCardSideLength*col + 12, _iCardSideLength*tempRow + 12 + visibleSize.height / 6.0));
						_cardActionArray[col][tempRow]->SetCardNumber(_cardArray[col][tempRow]->GetCardNumber());
						auto show = Show::create();
						auto move = MoveBy::create(0.1f, Point(0,-_iCardSideLength*(tempRow - row)));
						auto hide = Hide::create();
						_cardActionArray[col][tempRow]->GetCardBGLayer()->runAction(Sequence::create(place, show, move, hide, NULL));

						//如果col位置非空，且与tempCol位置的数字相同，则乘2
						_cardArray[col][row]->SetCardNumber(_cardArray[col][row]->GetCardNumber() * 2);
						_cardArray[col][tempRow]->SetCardNumber(0);
						SoundManager::getInstance()->PlayEffect(SOUND_TYPE_MERGE);

						//数字合并动画
						auto merge = Sequence::create(ScaleTo::create(0.1f, 1.2f), ScaleTo::create(0.1f, 1.0f), NULL);
						_cardArray[col][row]->GetCardBGLayer()->runAction(merge);

						bMoved = true;
					}

					break; //此处break防止出现连续乘2的bug
				}
			}
		}
	}

	return bMoved;
}


void GameMainScene::matchGameModeData()
{
	Size visibleSize = Director::getInstance()->getVisibleSize();

	ENM_GAME_MODE game_mode = GameManager::getInstance()->GetGameMode();
	switch (game_mode)
	{
	case GAME_MODE_NORMAL_4X4:
		{
			_iSideGridCount = 4;
			_iCardSideLength = (visibleSize.width - 15 * 2 /*- 3 * GRID_SPACE*/) / 4.0;
		}
		break;
	case GAME_MODE_NORMAL_5X5:
		{
			_iSideGridCount = 5;
			_iCardSideLength = (visibleSize.width - 15 * 2 /* - 4 * GRID_SPACE*/) / 5.0;
		}
		break;
	case GAME_MODE_NORMAL_6X6:
		{
			_iSideGridCount = 6;
			_iCardSideLength = (visibleSize.width - 15 * 2  /*- 5 * GRID_SPACE*/) / 6.0;
		}
		break;
	case GAME_MODE_DEADLINE_5MIN:
		{
			_iSideGridCount = 4;
			_iCardSideLength = (visibleSize.width - 15 * 2 /*- 3 * GRID_SPACE*/) / 4.0;

			_iGameTime = 5 * 60;
		}
		break;
	case GAME_MODE_DEADLINE_10MIN:
		{
			_iSideGridCount = 4;
			_iCardSideLength = (visibleSize.width - 15 * 2 /*- 3 * GRID_SPACE*/) / 4.0;

			_iGameTime = 10 * 60;
		}
		break;
	case GAME_MODE_DEADLINE_15MIN:
		{
			_iSideGridCount = 4;
			_iCardSideLength = (visibleSize.width - 15 * 2 /*- 3 * GRID_SPACE*/) / 4.0;

			_iGameTime = 15 * 60;
		}
		break;
	case GAME_MODE_ENDLESS:
		{

		}
		break;
	default:
		break;
	}

	_GridBGSideLength = visibleSize.width - 2 * 10;
}

void GameMainScene::checkGameResult()
{

}

void GameMainScene::updateDeadlineTime(float dt)
{
	if (_iGameTime <= 0)
	{
		unschedule(schedule_selector(GameMainScene::updateDeadlineTime));
	}
	std::string sTime = std::to_string(_iGameTime);
	_deadlineLabel->setString(sTime);

	_iGameTime--;
}
