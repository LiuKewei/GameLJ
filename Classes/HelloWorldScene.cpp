#include "HelloWorldScene.h"

USING_NS_CC;

const float c_brickFallSpeed = 1.0f;

const float c_brickIntervalSpace = 15.0f;

const int c_brickCount = 5;

const int c_brickScale = 2.0f;

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

HelloWorld::~HelloWorld()
{
	m_vecBrick.clear();
	m_brick->release();
	m_listener->release();
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
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Point origin = Director::getInstance()->getVisibleOrigin();

	CCLOG("origin %f, %f", origin.x, origin.y);

	CCLOG("visibleSize %f, %f", visibleSize.width, visibleSize.height);


	m_listener = EventListenerTouchOneByOne::create();
	m_listener->setSwallowTouches(true);
	m_listener->retain();

	m_listener->onTouchBegan = [=](Touch* touch, Event* event){
		auto target = static_cast<Brick*>(event->getCurrentTarget());

		Point locationInNode = target->convertToNodeSpace(touch->getLocation());
		Size s = target->getContentSize();
		Rect rect = Rect(0, 0, s.width*c_brickScale, s.height*c_brickScale);

		if (rect.containsPoint(locationInNode))
		{
			//log("sprite began... x = %f, y = %f", locationInNode.x, locationInNode.y);

			//游戏结束

			if (target->getShape() == m_brick->getShape() && target->getColor() == m_brick->getColor())
			{
				log("Game Over!!");
				gamestop();
			}
			else
			{
				//加分

				//删除对象
				this->removeChild(target);
				m_vecBrick.eraseObject(target);
				this->removeChild(m_brick);
				m_vecBrick.eraseObject(m_brick);
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

	gamestart();

	return true;
}

void HelloWorld::brickPushing(float dt)
{
	this->unschedule(schedule_selector(HelloWorld::brickPushing));
	float x = brickPush();
	float y = brickPush();
	float z = brickPush();
	x = (x > y ? x : y);
	x = x > z ? x : z;
	this->schedule(schedule_selector(HelloWorld::brickPushing), (c_brickIntervalSpace + x) / c_brickFallSpeed / 60);
}

void HelloWorld::brickFalling(float dt)
{
	m_vecBrick.at(0)->setPosition(m_vecBrick.at(0)->getPosition() - Point(0, c_brickFallSpeed));
	for (int i = 1; i < m_vecBrick.size(); ++i)
	{
		auto target = m_vecBrick.at(i);
		auto pos = target->getPosition();
		if (pos.y < -target->getContentSize().height * c_brickScale - 10.0f)
		{
			gamestop();
		}
		else
		{
			target->setPosition(pos - Point(0, c_brickFallSpeed));
		}
	}
}


Brick* HelloWorld::brickCreate()
{
	int shape = getRand(SHAPE_TRIANGLE, SHAPE_CIRCLE);
	int color = getRand(COLOR_RED, COLOR_YELLOW);
	auto brick = Brick::create();
	auto sprite = Sprite::create(c_brickName[c_brickNameIndex[shape][color]]);
	sprite->setScale(c_brickScale);
	//log("sprite size %f, %f", sprite->getContentSize().width, sprite->getContentSize().height);
	brick->bindSprite(sprite);
	//log("brick size %f, %f", brick->getContentSize().width, brick->getContentSize().height);
	brick->setShape(shape);
	brick->setColor(color);
	brick->setLocalZOrder(Z_ORDER_MAX);
	this->addChild(brick);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(m_listener->clone(), brick);
	//CCLOG("brick->getAnchorPoint() .... %f, %f", brick->getAnchorPoint().x, brick->getAnchorPoint().y);
	return brick;
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
	brick->setPosition(Director::getInstance()->getVisibleOrigin() + Point(width - brick->getContentSize().width / 2 * c_brickScale, Director::getInstance()->getVisibleSize().height + brick->getContentSize().height / 2));
	m_vecBrick.pushBack(brick);
	++m_posFlag;

	return brick->getContentSize().height * c_brickScale;
}



void HelloWorld::gamestart()
{
	m_brick = this->brickCreate();
	_eventDispatcher->removeEventListenersForTarget(m_brick);
	m_brick->retain();
	m_brick->setPosition(Director::getInstance()->getVisibleOrigin() + Point(Director::getInstance()->getVisibleSize().width / 2 - m_brick->getContentSize().width / 2 * c_brickScale, Director::getInstance()->getVisibleSize().height + m_brick->getContentSize().height / 2));
	m_vecBrick.insert(0, m_brick);

	this->schedule(schedule_selector(HelloWorld::brickFalling));
	this->schedule(schedule_selector(HelloWorld::brickPushing), (c_brickIntervalSpace + m_brick->getContentSize().height * c_brickScale * 5.0f) / c_brickFallSpeed / 60);
}
void HelloWorld::gamestop()
{
	gamepause();
	this->removeAllChildren();
	m_vecBrick.clear();
}
void HelloWorld::gamepause()
{
	this->unschedule(schedule_selector(HelloWorld::brickFalling));
	this->unschedule(schedule_selector(HelloWorld::brickPushing));
}