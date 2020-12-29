#define OLC_PGE_APPLICATION
#include "PixelGameEditor.h"
#include "World.h"
#include <iostream>
#include <string>
#include <thread>
#include "FunctionHelpers.h"
#include "GameObject.h"
#include "Player.h"
#include "Agent.h"
#include "Collision.h"
#include "EditableBlock.h"
#include "TestEntity.h"
//#include "box2d/box2d.h"

#define collisionManager Collision::GetInstance()
#define zombieSprite "..//PixelEditor//Sprites//Zombie.png"

class Demo : public PixelGameEditor
{
	GameObject* player = new Player();
	Agent* agent = new Agent();
	GameObject* editableBlock = new EditableBlock({ 925.f, 75.f });
	GameObject* testEntity = new TestEntity();

	GameObject* box2dTestObject  = new GameObject({ 100.0f, 0.0f }, { 50.0f, 50.0f }, "..//PixelEditor//Sprites//Box.png");
	GameObject* box2dTestObject2 = new GameObject({ 100.0f, -100.0f }, { 50.0f, 50.0f }, "..//PixelEditor//Sprites//Box.png");
	GameObject* box2dTestObject3 = new GameObject({ 51.0f, -500.0f }, { 50.0f, 50.0f }, "..//PixelEditor//Sprites//Box.png");
	GameObject* FloorBlock = new GameObject({ 500.0f, 1005.0f }, { 1000.0f, 10.0f }, "..//PixelEditor//Sprites//Box.png");

public:
	int i_FRAME = 0;

	enum AgentStates
	{
		Flee, Follow, GoTo
	};

	//////////// GAME ////////////////
public:
	Demo()
	{
		sAppName = "Demo";
	}

	void DebugSpawnAgent()
	{
		Agent* agentSpawned = new Agent();
		agentSpawned->SetPosition(0, 0);
		agentSpawned->EnableDrawCollision(true);
		agentSpawned->EnableDrawTaskFullDebug(true);
		agentSpawned->Init();
		agentSpawned->FollowTarget(player);

		//Cosmetic Options
		agentSpawned->ChangeSprite(zombieSprite, PixelMath::GetRotationBasedOnAngle(270.0f), true, 150.0f);

		//std::thread delayThread(&Demo::DelayStateChange, this, agentSpawned, player, AgentStates::Follow, 5);
		//delayThread.detach();
	}

public:
	// Called once at the start of the editor
	bool OnUserCreate() override
	{
		EditorOnCreate;

		agent->EnableDrawCollision(true);
		agent->EnableDrawTaskFullDebug(true);
		agent->SetPosition( 800.f, 500.f );
		//agent->GetSprite()->LoadFromFile("..//PixelEditor//Sprites//Zombie.png");
		//agent->GetDecal()->sprite->LoadFromFile("..//PixelEditor//Sprites//Zombie.png");
		agent->ChangeSprite(zombieSprite, PixelMath::GetRotationBasedOnAngle(270.0f), true, 150.0f);

		//Make agent move to the target here
		//agent->MoveTo(player->GetPosition());
		RunAPhysicsTest();
		
		FloorBlock->SetName("Floor");
		FloorBlock->SetTint(olc::DARK_GREY);
		FloorBlock->SetSpriteSizeToObjectSize();
		FloorBlock->SetCollisionEnabled(false);
		FloorBlock->bAllowDragging = false;

		return true;
	}

	//typedef void(*funcP)();
	void test() { std::cout << "DELAY TEST\n"; }
	void test2(std::string txt) { std::cout << txt << "\n"; }

	template <class t>
	void testDelay(void(t::*thisIsAFunctionPointer)())
	{
		(this->*thisIsAFunctionPointer)();
	}

	void t()
	{
		testDelay<Demo>(&Demo::test);
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		EditorOnUpdate(fElapsedTime);

		i_FRAME++;

		//time_functions::delays::Delay(time_functions::s, 5);
		//time_functions::delays::Delay<Demo, void>(time_functions::s, 5, this, &Demo::test);
		//time_functions::delays::Delay<Demo, void, std::string>(time_functions::s, 5, this, &Demo::test2, "Delay with arguments");

		{
			//time_functions::benchmarking::Timer test("Clear");
			//Clear({ 35, 35, 35 });
		}

		if (GetKey(olc::Key::O).bPressed)
			DebugSpawnAgent();

		return true;
	}

	bool OnBeginPlay() override
	{
		//Make agent move to the target here
		//agent->MoveTo(player->GetPosition());
		agent->FleeTarget(player);

		std::thread testThread(&Demo::DelayTest, this, 10); // Start following player after 10 sec
		testThread.detach();
		//std::thread([this] {DelayTest(10); }).detach(); //also works (version 2)
		//std::thread([this] { time_functions::delays::Delay<Agent, void, GameObject*>(time_functions::s, 10, agent, &Agent::FollowTarget, player); }).detach(); //also works (version 3)

		GameWorld->DestroyGameObject(testEntity);

		return true;
	}

	bool OnTick() override
	{
		return true;
	}

	void RunAPhysicsTest()
	{
		box2dTestObject->GetSprite()->width = 50.0f; box2dTestObject->GetSprite()->height = 50.0f;
		box2dTestObject2->GetSprite()->width = 50.0f; box2dTestObject2->GetSprite()->height = 50.0f;
		box2dTestObject3->GetSprite()->width = 50.0f; box2dTestObject3->GetSprite()->height = 50.0f;

		box2dTestObject->AddPhysicsComponent(0.1f);
		box2dTestObject2->AddPhysicsComponent(0.1f);
		box2dTestObject3->AddPhysicsComponent(2.0f);
		player->AddPhysicsComponent();
		player->OverridePhysicsBodyPosition(true);

		//box2dTestObject->Physics_ChangeDensity(0.02f);
	}

	void DelayTest(long long delayInSec = 5) // To be used in thread
	{
		time_functions::delays::Delay<Agent, void, GameObject*>(time_functions::s, delayInSec, agent, &Agent::FollowTarget, player);
	}

	void DelayStateChange(Agent* agentPtr, GameObject* Target, AgentStates newState, long long delay)
	{
		switch (newState)
		{
		case Demo::Flee: time_functions::delays::Delay<Agent, void, GameObject*>(time_functions::s, delay, agentPtr, &Agent::FleeTarget, Target);
			break;
		case Demo::Follow: time_functions::delays::Delay<Agent, void, GameObject*>(time_functions::s, delay, agentPtr, &Agent::FollowTarget, Target);
			break;
		case Demo::GoTo: time_functions::delays::Delay<Agent, void, GameObject*>(time_functions::s, delay, agentPtr, &Agent::MoveTo, Target);
			break;
		default:
			break;
		}
	}
	/////////////////////////////////////////////////////////////////////////
};


int main()
{
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	Demo demo;
	if (demo.Construct(1280, 800, 1, 1, false))
		demo.Start();

	//demo.testDelay(demo.test);

	return 0;
}

