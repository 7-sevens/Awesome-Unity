#include "LoadingScene.h"
#include "SimpleAudioEngine.h"
#include "GameMainScene.h"

USING_NS_CC;
using namespace CocosDenshion;


cocos2d::Scene* LoadingScene::createScene()
{
	auto scene = Scene::create();
	auto layer = LoadingScene::create();
	scene->addChild(layer);

	return scene;
}

bool LoadingScene::init()
{
	if (!Layer::init())
		return false;

	// background
	LayerColor *bg = LayerColor::create(Color4B(0, 255, 255, 255));
	this->addChild(bg);

	// schedule
	this->scheduleOnce(schedule_selector(LoadingScene::onLoadingCallBack), 0);

	return true;
}

void LoadingScene::onLoadingCallBack(float dt)
{
	//SimpleAudioEngine::getInstance()->preloadEffect("");

	auto gameScene = GameMainScene::createScene();
	TransitionScene *transition = TransitionProgressOutIn::create(0.5, gameScene);
	Director::getInstance()->replaceScene(transition);
}
