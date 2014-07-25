#include "AppDelegate.h"
#include "HelloWorldScene.h"

USING_NS_CC;


AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
}

bool AppDelegate::applicationDidFinishLaunching() {
	timeval psv;
	gettimeofday(&psv, NULL);
	unsigned long int rand_seed = psv.tv_sec * 1000 + psv.tv_usec / 1000;//设置时间随机因子
	srand(rand_seed);

    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
        glview = GLView::create("My Game");
        director->setOpenGLView(glview);
	}
	Size designSize = Size(320, 480);
	Size resourceSize = Size(640, 960);
	director->setContentScaleFactor(resourceSize.height / designSize.height);
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
	glview->setFrameSize(designSize.width, designSize.height);
#endif

#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
	glview->setDesignResolutionSize(designSize.width, designSize.height, ResolutionPolicy::SHOW_ALL);
#else
	glview->setDesignResolutionSize(designSize.width, designSize.height, ResolutionPolicy::SHOW_ALL);
#endif

    // turn on display FPS
    director->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);

	addResPath();
    // create a scene. it's an autorelease object
    auto scene = HelloWorld::createScene();

    // run
    director->runWithScene(scene);

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}


void AppDelegate::addResPath() {
	std::vector<std::string> searchPaths = FileUtils::getInstance()->getSearchPaths();
	searchPaths.insert(searchPaths.begin(), "./fonts");
	FileUtils::getInstance()->setSearchPaths(searchPaths);
}