#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "Brick.h"

using namespace cocos2d;

enum
{
	Z_ORDER_ZERO,
	Z_ORDER_ONE,
	Z_ORDER_TWO,
	Z_ORDER_THREE,
	Z_ORDER_FOUR,
	Z_ORDER_FIVE,
	Z_ORDER_SIX,
	Z_ORDER_SEVEN,
	Z_ORDER_EIGHT,
	Z_ORDER_NINE,
	Z_ORDER_TEN,

	Z_ORDER_MAX,
};

inline int getRand(int start, int end)
{
	float i = CCRANDOM_0_1()*(end - start + 1) + start;  //产生一个从start到end间的随机数  
	return (int)i;
}

class HelloWorld : public cocos2d::Layer
{
public:
	~HelloWorld();
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);

	void brickFalling(float dt);
	void brickPushing(float dt);
private:
	Brick* brickCreate();

	float brickPush();
private:

	EventListenerTouchOneByOne* m_listener;
	Vector<Brick*> m_vecBrick;

	int m_posFlag;
};

#endif // __HELLOWORLD_SCENE_H__
