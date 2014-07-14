#include "HelloWorldScene.h"

USING_NS_CC;

HelloWorld::~HelloWorld()
{
	m_vecSpr.clear();
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
		auto target = static_cast<Sprite*>(event->getCurrentTarget());

		Point locationInNode = target->convertToNodeSpace(touch->getLocation());
		Size s = target->getContentSize();
		Rect rect = Rect(0, 0, s.width, s.height);

		if (rect.containsPoint(locationInNode))
		{
			log("sprite began... x = %f, y = %f", locationInNode.x, locationInNode.y);
			this->removeChild(target);
			m_vecSpr.eraseObject(target);



			return false;
		}
		return false;
	};

	m_listener->onTouchEnded = [=](Touch* touch, Event* event){
		auto target = static_cast<Sprite*>(event->getCurrentTarget());
		log("sprite onTouchesEnded.. ");
		target->setOpacity(255);
	};


	auto sprite = Sprite::create("example.png");
	sprite->setPosition(origin + Point(visibleSize.width / 2, visibleSize.height / 2));
	addChild(sprite);

	_eventDispatcher->addEventListenerWithSceneGraphPriority(m_listener, sprite);

	m_vecSpr.pushBack(sprite);

	this->schedule(schedule_selector(HelloWorld::spritePushing));

	return true;
}


void HelloWorld::spritePushing(float dt)
{
	if (m_vecSpr.size() == 0)
	{
		auto sprite = Sprite::create("example.png");
		sprite->setPosition(Director::getInstance()->getVisibleOrigin() + Point(0, Director::getInstance()->getVisibleSize().height) + Point(sprite->getContentSize().width / 2, sprite->getContentSize().height / 2));
		_eventDispatcher->addEventListenerWithSceneGraphPriority(m_listener, sprite);
		this->addChild(sprite);
		m_vecSpr.pushBack(sprite);
	}
	else
	{
		for (int i = 0; i < m_vecSpr.size(); ++i)
		{
			auto sprite = m_vecSpr.at(i);
			auto pos = sprite->getPosition();
			if (pos.y <= -sprite->getContentSize().height / 2)
			{
				this->removeChild(sprite);
				m_vecSpr.eraseObject(sprite);
			}
			else
			{
				sprite->setPosition(pos - Point(0, 2));
			}
		}
	}
}

