#include "HelloWorldScene.h"

USING_NS_CC;



const float c_brickFallSpeed = 2.0f;

const float c_brickIntervalSpace = 20.0f;

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

#define FALLINTERVAL(x) ((c_brickIntervalSpace*(CCRANDOM_0_1()*1.5f+1.0f) + (x)) / c_brickFallSpeed / 60)

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
	m_score = 0;
	m_brickBaseScore = 100;
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Point origin = Director::getInstance()->getVisibleOrigin();

	//CCLOG("origin %f, %f", origin.x, origin.y);

	//CCLOG("visibleSize %f, %f", visibleSize.width, visibleSize.height);

	//waves = Waves3D::create(10, Size(15, 10), 18, 15);
	waves = Shaky3D::create(20, Size(20, 20), 2, false);

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
			//log("sprite began... x = %f, y = %f", locationInNode.x, locationInNode.y);

			//��Ϸ����

			auto b = static_cast<Brick*>(m_brick->getChildByTag(TAG_BRICK));
			auto targetbrick = static_cast<Brick*>(target->getChildByTag(TAG_BRICK));
			if (targetbrick->getShape() == b->getShape() && targetbrick->getColor() == b->getColor())
			{
				log("Game Over!!  onTouchBegan");
				gamestop();
				target->runAction(Sequence::create(Blink::create(2.0f, 3), CallFuncN::create(CC_CALLBACK_0(HelloWorld::showstand, NULL)), NULL));
			}
			else
			{
				//�ӷ�
				this->score();
				//ɾ������
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
	m_scoreLabel->setPosition(Director::getInstance()->getVisibleOrigin() + Point(Director::getInstance()->getVisibleSize().width / 2, Director::getInstance()->getVisibleSize().height - 50));
	m_scoreLabel->setScale(1.8f);
	this->addChild(m_scoreLabel, Z_ORDER_MAX);
	gamestart();

	return true;
}

void HelloWorld::brickPushingLeft(float dt)
{
	this->unschedule(schedule_selector(HelloWorld::brickPushingLeft));
	this->schedule(schedule_selector(HelloWorld::brickPushingLeft), FALLINTERVAL(brickPush()));
}

void HelloWorld::brickPushingMid(float dt)
{
	this->unschedule(schedule_selector(HelloWorld::brickPushingMid));
	this->schedule(schedule_selector(HelloWorld::brickPushingMid), FALLINTERVAL(brickPush()));
}

void HelloWorld::brickPushingRight(float dt)
{
	this->unschedule(schedule_selector(HelloWorld::brickPushingRight));
	this->schedule(schedule_selector(HelloWorld::brickPushingRight), FALLINTERVAL(brickPush()));
}

void HelloWorld::brickFalling(float dt)
{
	//m_vecBrick.at(0)->setPosition(m_vecBrick.at(0)->getPosition() - Point(0, c_brickFallSpeed));
	for (int i = 0; i < m_vecBrick.size(); ++i)
	{
		auto target = m_vecBrick.at(i);
		auto pos = target->getPosition();
		target->setPosition(pos - Point(0, c_brickFallSpeed));
		auto targetbrick = static_cast<Brick*>(target->getChildByTag(TAG_BRICK));
		if (pos.y < targetbrick->getContentSize().height / 2 * c_brickScale)
		{
			auto b = static_cast<Brick*>(m_brick->getChildByTag(TAG_BRICK));
			if (targetbrick->getColor() == b->getColor() && targetbrick->getShape() == b->getShape())
			{
				this->removeChild(target);
				m_vecBrick.eraseObject(target);
				continue;
			}
			log("Game Over!!  brickFalling");
			gamestop();
			target->runAction(Sequence::create(Blink::create(2.0f, 3), CallFuncN::create(CC_CALLBACK_0(HelloWorld::showstand, NULL)), NULL));
		}
	}
}


Node* HelloWorld::brickCreate()
{
	auto _bgNode = NodeGrid::create();
	_bgNode->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
	addChild(_bgNode);
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
	_bgNode->addChild(brick);
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
	brick->setPosition(Director::getInstance()->getVisibleOrigin() + Point(width - brick->getContentSize().width / 2 * c_brickScale, Director::getInstance()->getVisibleSize().height + brick->getContentSize().height / 2));
	m_vecBrick.pushBack(brick);
	++m_posFlag;

	
	brick->runAction(Sequence::create(waves, NULL));

	return brick->getContentSize().height * c_brickScale;
}



void HelloWorld::gamestart()
{
	m_brick = this->brickCreate();
	_eventDispatcher->removeEventListenersForTarget(m_brick);
	m_brick->retain();
	m_brick->setPosition(Director::getInstance()->getVisibleOrigin() + Point(Director::getInstance()->getVisibleSize().width / 2 - m_brick->getContentSize().width / 2 * c_brickScale, Director::getInstance()->getVisibleSize().height + m_brick->getContentSize().height / 2));
	m_vecBrick.insert(0, m_brick);
	m_brick->runAction(Sequence::create(waves, NULL));

	this->schedule(schedule_selector(HelloWorld::brickFalling));
	this->schedule(schedule_selector(HelloWorld::brickPushingLeft), (c_brickIntervalSpace + m_brick->getContentSize().height * c_brickScale * 5.0f) / c_brickFallSpeed / 60);
	this->schedule(schedule_selector(HelloWorld::brickPushingMid), (c_brickIntervalSpace + m_brick->getContentSize().height * c_brickScale * 5.0f) / c_brickFallSpeed / 60);
	this->schedule(schedule_selector(HelloWorld::brickPushingRight), (c_brickIntervalSpace + m_brick->getContentSize().height * c_brickScale * 5.0f) / c_brickFallSpeed / 60);
	m_listenerFlag = true;
}
void HelloWorld::gamestop()
{
	gamepause();
	//this->removeAllChildren();
	//m_vecBrick.clear();
	for (auto e : m_vecBrick)
	{
		e->stopAllActions();
	}
}
void HelloWorld::gamepause()
{
	m_listenerFlag = false;
	this->unschedule(schedule_selector(HelloWorld::brickFalling));
	this->unschedule(schedule_selector(HelloWorld::brickPushingLeft));
	this->unschedule(schedule_selector(HelloWorld::brickPushingMid));
	this->unschedule(schedule_selector(HelloWorld::brickPushingRight));
}


void HelloWorld::score()
{
	m_score += m_brickBaseScore;
	sprintf(m_scoreLabelstr, "%d", m_score);
	m_scoreLabel->setString(m_scoreLabelstr);
}

void HelloWorld::showstand()
{

}


