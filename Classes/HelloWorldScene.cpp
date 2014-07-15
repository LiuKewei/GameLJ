#include "HelloWorldScene.h"

USING_NS_CC;

const float c_brickFallSpeed = 0.5f;

const float c_brickIntervalSpace = 10.0f;

const int c_brickCount = 5;

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
	m_vecBrickLeft.clear();
	m_vecBrickMid.clear();
	m_vecBrickRight.clear();
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
		Rect rect = Rect(0, 0, s.width, s.height);

		if (rect.containsPoint(locationInNode))
		{
			//log("sprite began... x = %f, y = %f", locationInNode.x, locationInNode.y);

			//加分

			//删除对象
			this->removeChild(target);
			//删除，并添加元素
			if (m_vecBrickLeft.contains(target))
			{
				m_vecBrickLeft.eraseObject(target);
				//pushLeft(c_brickCount-1);
			}
			else if (m_vecBrickMid.contains(target))
			{
				m_vecBrickMid.eraseObject(target);
				//pushMid(c_brickCount-1);
			}
			else if (m_vecBrickRight.contains(target))
			{
				m_vecBrickRight.eraseObject(target);
				//pushRight(c_brickCount-1);
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

	pushLeft();
	pushMid();
	pushRight();
	this->schedule(schedule_selector(HelloWorld::brickPushingLeft));
	this->schedule(schedule_selector(HelloWorld::brickPushingMid));
	this->schedule(schedule_selector(HelloWorld::brickPushingRight));

	return true;
}

void HelloWorld::brickPushingLeft(float dt)
{
	for (int i = 0; i < m_vecBrickLeft.size(); ++i)
	{
		auto brick = m_vecBrickLeft.at(i);
		auto pos = brick->getPosition();
		if (pos.y <= -brick->getContentSize().height / 2)
		{
			this->removeChild(brick);
			m_vecBrickLeft.eraseObject(brick);
			pushLeft();
		}
		else
		{
			brick->setPosition(pos - Point(0, c_brickFallSpeed));
		}
	}
}
void HelloWorld::brickPushingMid(float dt)
{
	for (int i = 0; i < m_vecBrickMid.size(); ++i)
	{
		auto brick = m_vecBrickMid.at(i);
		auto pos = brick->getPosition();
		if (pos.y <= -brick->getContentSize().height / 2)
		{
			this->removeChild(brick);
			m_vecBrickMid.eraseObject(brick);
			pushMid();
		}
		else
		{
			brick->setPosition(pos - Point(0, c_brickFallSpeed));
		}
	}
}
void HelloWorld::brickPushingRight(float dt)
{

	for (int i = 0; i < m_vecBrickRight.size(); ++i)
	{
		auto brick = m_vecBrickRight.at(i);
		auto pos = brick->getPosition();
		if (pos.y <= -brick->getContentSize().height / 2)
		{
			this->removeChild(brick);
			m_vecBrickRight.eraseObject(brick);
			pushRight();
		}
		else
		{
			brick->setPosition(pos - Point(0, c_brickFallSpeed));
		}
	}
}



Brick* HelloWorld::brickCreate()
{
	int shape = getRand(SHAPE_TRIANGLE, SHAPE_CIRCLE);
	int color = getRand(COLOR_RED, COLOR_YELLOW);
	auto brick = Brick::create();
	brick->bindSprite(Sprite::create(c_brickName[c_brickNameIndex[shape][color]]));
	brick->setShape(shape);
	brick->setColor(color);
	brick->setLocalZOrder(Z_ORDER_MAX);
	this->addChild(brick);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(m_listener->clone(), brick);
	//CCLOG("brick->getAnchorPoint() .... %f, %f", brick->getAnchorPoint().x, brick->getAnchorPoint().y);
	return brick;
}

void HelloWorld::pushLeft()
{
	auto brick = this->brickCreate();
	brick->setPosition(Director::getInstance()->getVisibleOrigin() + Point(Director::getInstance()->getVisibleSize().width / 4, Director::getInstance()->getVisibleSize().height + brick->getContentSize().height / 2));
	m_vecBrickLeft.pushBack(brick);
}
void HelloWorld::pushMid()
{
	auto brick = this->brickCreate();
	brick->setPosition(Director::getInstance()->getVisibleOrigin() + Point(Director::getInstance()->getVisibleSize().width / 4 * 2, Director::getInstance()->getVisibleSize().height + brick->getContentSize().height / 2));
	m_vecBrickMid.pushBack(brick);
}
void HelloWorld::pushRight()
{
	auto brick = this->brickCreate();
	brick->setPosition(Director::getInstance()->getVisibleOrigin() + Point(Director::getInstance()->getVisibleSize().width / 4 * 3, Director::getInstance()->getVisibleSize().height + brick->getContentSize().height / 2));
	m_vecBrickRight.pushBack(brick);
}