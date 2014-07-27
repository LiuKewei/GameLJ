#include "HelloWorldScene.h"

USING_NS_CC;



const float c_brickFallSpeed = 1.5f;

const float c_brickIntervalSpace = 20.0f;

const int c_brickCount = 5;

const float c_brickScale = 2.0f;

const float c_scale = 0.01f;

const char* c_brickName[9] = {
	"TRIANGLE_RED.png", "TRIANGLE_GREEN.png", "TRIANGLE_YELLOW.png",
	"SQUARE_RED.png", "SQUARE_GREEN.png", "SQUARE_YELLOW.png",
	"CIRCLE_RED.png", "CIRCLE_GREEN.png", "CIRCLE_YELLOW.png"
};

const int c_brickNameIndex[3][3] = {
	0, 1, 2,
	3, 4, 5,
	6, 7, 8
};

#define FALLINTERVAL(x) ((c_brickIntervalSpace*(CCRANDOM_0_1()*1.5f+1.0f) + (x)) / m_brickFallSpeed / 60.0f)

HelloWorld::~HelloWorld()
{
	m_vecBrick.clear();
	m_brick->release();
	m_listener->release();
	m_showstand->release();
	m_scoreLabel->release();
}

Scene* HelloWorld::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = HelloWorld::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}
	m_posFlag = 1;
	m_score = 0;
	m_brickBaseScore = 10;
	m_brickFallSpeed = c_brickFallSpeed;
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Point origin = Director::getInstance()->getVisibleOrigin();

	auto scale = ScaleBy::create(0.1f, 0.9f);
	waves = Sequence::create(scale, scale->reverse(), NULL);

	//CCLOG("origin %f, %f", origin.x, origin.y);
	//CCLOG("visibleSize %f, %f", visibleSize.width, visibleSize.height);

	m_showstand = Sprite::create("showstand.png");
	m_showstand->setPosition(origin + Point(visibleSize.width / 2, visibleSize.height / 2) - Point(0, visibleSize.height));
	m_showstand->setLocalZOrder(Z_ORDER_MAX);
	m_showstand->retain();

	m_listener = EventListenerTouchOneByOne::create();
	m_listener->setSwallowTouches(true);
	m_listener->retain();

	m_listener->onTouchBegan = [=](Touch* touch, Event* event){
		auto target = static_cast<Node*>(event->getCurrentTarget());
		Point locationInNode = target->convertToNodeSpace(touch->getLocation());
		Size s = target->getContentSize();
		Rect rect = Rect(0, 0, s.width*c_brickScale, s.height*c_brickScale);

		if (rect.containsPoint(locationInNode) && m_listenerFlag)
		{
			target->stopAllActions();
			//log("sprite began... x = %f, y = %f", locationInNode.x, locationInNode.y);

			//游戏结束

			auto b = static_cast<Brick*>(m_brick->getChildByTag(TAG_BRICK));
			auto targetbrick = static_cast<Brick*>(target->getChildByTag(TAG_BRICK));
			if (targetbrick->getShape() == b->getShape() && targetbrick->getColor() == b->getColor())
			{
				//log("Game Over!!  onTouchBegan");
				gamestop();
				target->runAction(Sequence::create(Blink::create(2.0f, 3), CallFuncN::create(CC_CALLBACK_0(HelloWorld::showstand, this)), NULL));
			}
			else
			{
				//加分
				this->score();
				//删除对象
				this->removeChild(target);
				m_vecBrick.eraseObject(target);
			}


			return false;
		}
		return false;
	};

	m_listener->onTouchEnded = [=](Touch* touch, Event* event){
		auto target = static_cast<Sprite*>(event->getCurrentTarget());
		//log("sprite onTouchesEnded.. ");
		target->setOpacity(255);
	};

	m_scoreLabel = Label::createWithBMFont("boundsTestFont.fnt", "");
	m_scoreLabel->setPosition(Director::getInstance()->getVisibleOrigin() + Point(Director::getInstance()->getVisibleSize().width / 2, Director::getInstance()->getVisibleSize().height - 50.0f));
	m_scoreLabel->setScale(1.8f);
	m_scoreLabel->retain();
	sprintf(m_scoreLabelstr, "0000", m_score);
	m_scoreLabel->setString(m_scoreLabelstr);
	this->addChild(m_scoreLabel, Z_ORDER_MAX - 1);
	gamestart();

	return true;
}

void HelloWorld::brickPushingLeft(float dt)
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
	if (dt < 0)log(" %4.0f", dt);
#endif
	this->unschedule(schedule_selector(HelloWorld::brickPushingLeft));
	this->schedule(schedule_selector(HelloWorld::brickPushingLeft), FALLINTERVAL(brickPush()));
}

void HelloWorld::brickPushingMid(float dt)
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
	if (dt < 0)log(" %3.0f", dt);
#endif
	this->unschedule(schedule_selector(HelloWorld::brickPushingMid));
	this->schedule(schedule_selector(HelloWorld::brickPushingMid), FALLINTERVAL(brickPush()));
}

void HelloWorld::brickPushingRight(float dt)
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
	if (dt < 0)log(" %2.0f", dt);
#endif
	this->unschedule(schedule_selector(HelloWorld::brickPushingRight));
	this->schedule(schedule_selector(HelloWorld::brickPushingRight), FALLINTERVAL(brickPush()));
}


void HelloWorld::brickScaling(float dt)
{
	for (auto ele : m_vecBrick)
	{
		float scale = ele->getScale();
		auto targetbrick = static_cast<Brick*>(ele->getChildByTag(TAG_BRICK));

		if (scale <= 0.9f)
		{
			targetbrick->m_scaleFlag = true;
		}
		else if (scale > 1.0f)
		{
			targetbrick->m_scaleFlag = false;
		}

		if (targetbrick->m_scaleFlag)
		{
			ele->setScale(scale + c_scale);
		}
		else
		{
			ele->setScale(scale - c_scale);
		}
	}
}


void HelloWorld::brickFalling(float dt)
{
	//m_vecBrick.at(0)->setPosition(m_vecBrick.at(0)->getPosition() - Point(0, m_brickFallSpeed));
	for (int i = 0; i < m_vecBrick.size(); ++i)
	{
		auto target = m_vecBrick.at(i);
		auto pos = target->getPosition();
		target->setPosition(pos - Point(0, m_brickFallSpeed));
		auto targetbrick = static_cast<Brick*>(target->getChildByTag(TAG_BRICK));
		if (pos.y < targetbrick->getContentSize().height / 2 * c_brickScale)
		{
			target->stopAllActions();
			auto b = static_cast<Brick*>(m_brick->getChildByTag(TAG_BRICK));
			if (targetbrick->getColor() == b->getColor() && targetbrick->getShape() == b->getShape())
			{
				this->removeChild(target);
				m_vecBrick.eraseObject(target);
				continue;
			}
			//log("Game Over!!  brickFalling");
			gamestop();
			target->runAction(Sequence::create(Blink::create(2.0f, 3), CallFuncN::create(CC_CALLBACK_0(HelloWorld::showstand, this)), NULL));
		}
	}
}


Node* HelloWorld::brickCreate()
{
	auto _bgNode = NodeGrid::create();
	_bgNode->setAnchorPoint(Point::ANCHOR_MIDDLE);
	this->addChild(_bgNode, Z_ORDER_MAX);
	int shape = getRand(SHAPE_TRIANGLE, SHAPE_CIRCLE);
	int color = getRand(COLOR_RED, COLOR_YELLOW);
	auto brick = Brick::create();
	auto sprite = Sprite::create(c_brickName[c_brickNameIndex[shape][color]]);
	sprite->setScale(c_brickScale);
	//log("sprite size %f, %f", sprite->getContentSize().width, sprite->getContentSize().height);
	brick->bindSprite(sprite);
	//log("brick size %f, %f", brick->getContentSize().width, brick->getContentSize().height);
	_bgNode->setContentSize(brick->getContentSize());
	brick->setShape(shape);
	brick->setColor(color);
	brick->setLocalZOrder(Z_ORDER_MAX);
	brick->setTag(TAG_BRICK);

	_bgNode->addChild(brick, Z_ORDER_MAX);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(m_listener->clone(), _bgNode);
	//CCLOG("brick->getAnchorPoint() .... %f, %f", brick->getAnchorPoint().x, brick->getAnchorPoint().y);
	return _bgNode;
}

float HelloWorld::brickPush()
{
	auto brick = this->brickCreate();
	float width = 0.0;
	if (m_posFlag % 3 == 1)
	{
		width = Director::getInstance()->getVisibleSize().width / 4 - c_brickIntervalSpace;
	}
	else if (m_posFlag % 3 == 2)
	{
		width = Director::getInstance()->getVisibleSize().width / 2;
	}
	else if (m_posFlag % 3 == 0)
	{
		width = Director::getInstance()->getVisibleSize().width / 4 * 3 + c_brickIntervalSpace;
		m_posFlag = 0;
	}
	brick->setPosition(Director::getInstance()->getVisibleOrigin() + Point(width - brick->getContentSize().width / 4 * c_brickScale, Director::getInstance()->getVisibleSize().height + brick->getContentSize().height / 2));
	m_vecBrick.pushBack(brick);
	++m_posFlag;

	return brick->getContentSize().height * c_brickScale;
}



void HelloWorld::gamestart()
{
	m_brick = this->brickCreate();
	_eventDispatcher->removeEventListenersForTarget(m_brick);
	m_brick->retain();
	m_brick->setPosition(Director::getInstance()->getVisibleOrigin() + Point(Director::getInstance()->getVisibleSize().width / 2 - m_brick->getContentSize().width / 4 * c_brickScale, Director::getInstance()->getVisibleSize().height + m_brick->getContentSize().height / 2));
	m_vecBrick.insert(0, m_brick);

	float inteval = (c_brickIntervalSpace + m_brick->getContentSize().height * c_brickScale * 5.0f) / m_brickFallSpeed / 60.0f;
	//log("inteval %f", inteval);
	this->schedule(schedule_selector(HelloWorld::brickFalling));
	this->schedule(schedule_selector(HelloWorld::brickScaling));
	this->schedule(schedule_selector(HelloWorld::brickPushingLeft), inteval);
	this->schedule(schedule_selector(HelloWorld::brickPushingMid), inteval);
	this->schedule(schedule_selector(HelloWorld::brickPushingRight), inteval);
	m_listenerFlag = true;
}
void HelloWorld::gamestop()
{
	gamepause();
	for (auto e : m_vecBrick)
	{
		e->stopAllActions();
	}
}
void HelloWorld::gamepause()
{
	m_listenerFlag = false;
	this->unschedule(schedule_selector(HelloWorld::brickScaling));
	this->unschedule(schedule_selector(HelloWorld::brickFalling));
	this->unschedule(schedule_selector(HelloWorld::brickPushingLeft));
	this->unschedule(schedule_selector(HelloWorld::brickPushingMid));
	this->unschedule(schedule_selector(HelloWorld::brickPushingRight));
}


void HelloWorld::score()
{
	m_score += m_brickBaseScore;
	if (m_score % 500 == 0)
	{
		m_brickFallSpeed += m_brickFallSpeed*0.1f;
		//log("m_brickFallSpeed    %f", m_brickFallSpeed);
		m_brickBaseScore = m_brickBaseScore >= 100 ? 100 : m_brickBaseScore + 10;
	}
	sprintf(m_scoreLabelstr, "%d", m_score);
	m_scoreLabel->setString(m_scoreLabelstr);
}

void HelloWorld::showstand()
{
	this->removeAllChildren();
	m_vecBrick.clear();
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Point origin = Director::getInstance()->getVisibleOrigin();
	this->addChild(m_showstand);



	auto gameRestartItem = MenuItemImage::create(
		"restart.png",
		"restart.png",
		CC_CALLBACK_1(HelloWorld::restartGame, this));

	gameRestartItem->setPosition(Point(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height / 2 + gameRestartItem->getContentSize().height / 2));

	// create menu, it's an autorelease object
	auto gameRestartMenu = Menu::create(gameRestartItem, NULL);
	gameRestartMenu->setPosition(Point::ANCHOR_MIDDLE);
	m_showstand->addChild(gameRestartMenu, Z_ORDER_MAX);


	// add a "close" icon to exit the progress. it's an autorelease object
	auto closeItem = MenuItemImage::create(
		"CloseNormal.png",
		"CloseSelected.png",
		CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));

	closeItem->setPosition(Point(origin.x + visibleSize.width - closeItem->getContentSize().width / 2,
		origin.y + closeItem->getContentSize().height / 2));

	// create menu, it's an autorelease object
	auto menu = Menu::create(closeItem, NULL);
	menu->setPosition(Point::ZERO);
	m_showstand->addChild(menu, Z_ORDER_MAX);

	m_showstand->addChild(m_scoreLabel, Z_ORDER_MAX);
	m_scoreLabel->setPosition(origin + Point(visibleSize.width / 2, visibleSize.height * 4 / 5));
	m_showstand->runAction(MoveBy::create(0.5f, Point(0, visibleSize.height)));
}




void HelloWorld::menuCloseCallback(Ref* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.", "Alert");
	return;
#endif

	Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}

void HelloWorld::restartGame(Ref* pSender)
{
	this->removeAllChildren();
	m_brick->release();
	m_showstand->removeAllChildren();
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Point origin = Director::getInstance()->getVisibleOrigin();
	m_showstand->setPosition(origin + Point(visibleSize.width / 2, visibleSize.height / 2) - Point(0, visibleSize.height));
	m_score = 0;
	m_brickBaseScore = 10;
	m_brickFallSpeed = c_brickFallSpeed;
	m_scoreLabel->setString("0000");
	this->addChild(m_scoreLabel, Z_ORDER_MAX - 1);

	this->gamestart();
}
