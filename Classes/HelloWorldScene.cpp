#include "HelloWorldScene.h"

USING_NS_CC;

#define SCALE_RATIO 32.0

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
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Point origin = Director::getInstance()->getVisibleOrigin();

    //SET MOUSE LISTENER
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    
    listener->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(HelloWorld::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(HelloWorld::onTouchEnded, this);
    
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    //END MOUSE LISTENER
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("tunghung.plist");
	auto testBNote = SpriteBatchNode::create("tunghung.png");
	addChild(testBNote);
	testBNote->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	auto testSprite = Sprite::create("dot.png");
	/*auto animation = Animation::create();
	for (int i = 1; i < 9; i++)
	{
		std::stringstream ss;
		ss << i << ".png";
		animation->addSpriteFrame(SpriteFrameCache::getInstance()->spriteFrameByName(ss.str().c_str()));
	}
	animation->setDelayPerUnit(0.1f);
	//animation->setLoops(100);
	testSprite->runAction(CCRepeatForever::create(CCAnimate::create(animation))); */
	addChild(testSprite);
	//Create BOX2D World
    b2Vec2 gravity = b2Vec2(0.0f, -10.0f);
    world = new b2World(gravity);  
	world->SetAllowSleeping(true);
	world->SetContinuousPhysics(true);

    //CREATE A BALL
    dragOffsetStartX = 0;    
    dragOffsetEndX = 0;    
    dragOffsetStartY = 0;    
    dragOffsetEndY = 0;    
    existBall= false;    
    ballX = 500;
    ballY = 200;  
    powerMultiplier = 10;  
    ball =Sprite::create("ball.png");
    ball->setPosition(CCPoint(ballX,ballY));
    this->addChild(ball); 
	/*ball2 = Sprite::create("ball.png");
	ball2->setPosition(ball->getPosition());
	addChild(ball2); */
    //HelloWorl2d::defineBall();
	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(0.0f, -10.0f);
	b2Body* groundBody = world->CreateBody(&groundBodyDef);
	b2PolygonShape groundBox;
	groundBox.SetAsBox(50.0f, 10.0f);
	groundBody->CreateFixture(&groundBox, 0.0f);


    addWall(visibleSize.width ,10,(visibleSize.width / 2) ,0); //CEIL
    addWall(10 ,visibleSize.height ,0,(visibleSize.height / 2) ); //LEFT
    addWall(10 ,visibleSize.height ,visibleSize.width,(visibleSize.height / 2) ); //RIGHT

    for (int i = 1 ; i <= 99; i++){
        points[i] =CCSprite::create("dot.png");
        this->addChild(points[i]); 
    }
    scheduleUpdate();
    return true;
}

//Simulate Physics
void HelloWorld::update(float dt){
   int positionIterations = 10;  
   int velocityIterations = 10;
   
   deltaTime = dt;
   world->Step(dt, velocityIterations, positionIterations);  
   cocos2d::log("ball pos: %d   %d", ball->getPosition().x, ball->getPosition().y);
   for (b2Body *body = world->GetBodyList(); body != NULL; body = body->GetNext())   
     if (body->GetUserData()) 
     {  
       CCSprite *sprite = (CCSprite *) body->GetUserData();  
       sprite->setPosition(ccp(body->GetPosition().x * SCALE_RATIO,body->GetPosition().y * SCALE_RATIO));  
       sprite->setRotation(-1 * CC_RADIANS_TO_DEGREES(body->GetAngle())); 



     }  
    world->ClearForces();
    world->DrawDebugData();       
 
}  

void HelloWorld::addWall(float w,float h,float px,float py) {

    b2PolygonShape floorShape;

    floorShape.SetAsBox(w/ SCALE_RATIO,h/ SCALE_RATIO);
    b2FixtureDef floorFixture;
    floorFixture.density=5;
    floorFixture.friction=5;
    floorFixture.restitution=0.5;
    floorFixture.shape=&floorShape;
    b2BodyDef floorBodyDef;

    floorBodyDef.position.Set(px/ SCALE_RATIO,py/ SCALE_RATIO);
    b2Body *floorBody = world->CreateBody(&floorBodyDef);

    floorBody->CreateFixture(&floorFixture);

}

void HelloWorld::defineBall(){
    ballShape.m_radius = 45 / SCALE_RATIO;
    b2FixtureDef ballFixture;
    ballFixture.density=10;
    ballFixture.friction=0.8;
    ballFixture.restitution=0.6;
    ballFixture.shape=&ballShape;

    ballBodyDef.type= b2_dynamicBody;
    ballBodyDef.userData=ball;
	ballBodyDef.bullet = true;
    ballBodyDef.position.Set(ball->getPosition().x/SCALE_RATIO,ball->getPosition().y/SCALE_RATIO);

    ballBody = world->CreateBody(&ballBodyDef);
    ballBody->CreateFixture(&ballFixture);
    ballBody->SetGravityScale(10);
	///BALL 2
	/*ball2BodyDef.type = b2_dynamicBody;

	ball2BodyDef.position.Set(ball2->getPosition().x / SCALE_RATIO, ball2->getPosition().y / SCALE_RATIO);
	ball2BodyDef.userData = ball2;
	ball2Body = world->CreateBody(&ball2BodyDef);
	ball2Body->CreateFixture(&ballFixture);
	ball2Body->SetGravityScale(5);*/
}

bool HelloWorld::onTouchBegan(Touch* touch, Event* event)
{
    dragOffsetStartX = touch->getLocation().x;
    dragOffsetStartY = touch->getLocation().y;

    
    CCPoint touchLocation = touch->getLocation();

    ballX = touchLocation.x;
    ballY = touchLocation.y;

    if (existBall){        
        world->DestroyBody(ballBody);
		//world->DestroyBody(ball2Body);
    }

    ball->setPosition(ccp(ballX ,ballY));
	//ball2->setPosition(Vec2(200, 200));
    return true;
}

void HelloWorld::onTouchMoved(Touch* touch, Event* event)
{
    CCPoint touchLocation = touch->getLocation();

    dragOffsetEndX = touchLocation.x;
    dragOffsetEndY = touchLocation.y;

    float dragDistanceX = dragOffsetStartX - dragOffsetEndX;
    float dragDistanceY = dragOffsetStartY - dragOffsetEndY;

    //HelloWorld::simulateTrajectory(b2Vec2((dragDistanceX )/SCALE_RATIO,(dragDistanceY )/SCALE_RATIO));
    HelloWorld::simulateTrajectory(b2Vec2((dragDistanceX * powerMultiplier)/SCALE_RATIO,(dragDistanceY * powerMultiplier)/SCALE_RATIO));

}

void HelloWorld::onTouchEnded(Touch* touch, Event* event)
{
    existBall = true;

    HelloWorld::defineBall();

    CCPoint touchLocation = touch->getLocation();

    dragOffsetEndX = touchLocation.x;
    dragOffsetEndY = touchLocation.y;

    float dragDistanceX = dragOffsetStartX - dragOffsetEndX;
    float dragDistanceY = dragOffsetStartY - dragOffsetEndY;

    //ballBody->SetLinearVelocity(b2Vec2((dragDistanceX)/SCALE_RATIO,(dragDistanceY)/SCALE_RATIO));
    ballBody->SetLinearVelocity(b2Vec2((dragDistanceX * powerMultiplier)/SCALE_RATIO,(dragDistanceY * powerMultiplier)/SCALE_RATIO));
} 

void HelloWorld::simulateTrajectory(b2Vec2 coord){

    //define ball physicis
    HelloWorld::defineBall();

    ballBody->SetLinearVelocity(b2Vec2(coord.x,coord.y));
    for (int i = 1; i <= 99; i++){
		world->Step(deltaTime, 10, 10);
        points[i]->setPosition(CCPoint(ballBody->GetPosition().x*SCALE_RATIO,ballBody->GetPosition().y*SCALE_RATIO));
        world->ClearForces();
            
    }

    world->DestroyBody(ballBody);
}

void HelloWorld::menuCloseCallback(Ref* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
    return;
#endif

    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
