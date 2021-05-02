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
#include "Enemy.h"
#include "Collision.h"
#include "EditableBlock.h"
#include "TestEntity.h"
#include <cstdio> // printf
//#include "box2d/box2d.h"

#define collisionManager Collision::GetInstance()
#define zombieSprite "..//PixelEditor//Sprites//Zombie.png"

//#define stateChange

#define sec time_functions::s

class Demo : public PixelGameEditor
{
	GameObject* player = new Player();
	Agent* agent = new Agent();
	Enemy* enemy = new Enemy();
	GameObject* editableBlock = new EditableBlock({ 925.f, 75.f });
	GameObject* testEntity = new TestEntity();

	GameObject* box2dTestObject  = new GameObject({ 100.0f, 0.0f }, { 50.0f, 50.0f }, "..//PixelEditor//Sprites//Box.png");
	GameObject* box2dTestObject2 = new GameObject({ 100.0f, -100.0f }, { 50.0f, 50.0f }, "..//PixelEditor//Sprites//Box.png");
	GameObject* box2dTestObject3 = new GameObject({ 51.0f, -500.0f }, { 50.0f, 50.0f }, "..//PixelEditor//Sprites//Box.png");
	GameObject* FloorBlock = new GameObject({ 500.0f, 1005.0f }, { 1000.0f, 10.0f }, "..//PixelEditor//Sprites//Box.png");

	Enemy* enemy2 = new Enemy();

public:
	int i_FRAME = 0;

	enum AgentStates : unsigned char
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
		//agentSpawned->EnableDrawCollision(true);
		//agentSpawned->EnableDrawTaskFullDebug(true);
		//agentSpawned->Init(); // Not Needed as we are changing the sprite which inits the decal
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
		FloorBlock->bAllowEditorRotation = false;

		//GameWorld->GetGrid()->LoadATileMap("..//PixelEditor//Sprites//Background.png", { 0.0F, 0.0F });
		//GameWorld->GetGrid()->LoadATileMap("..//PixelEditor//Sprites//Background2.png", { 2000.0F, 0.0F });
		//GameWorld->GetGrid()->LoadATileMap("..//PixelEditor//Sprites//Background.png", { 0.0F, 1000.0F });
		setCanDrawTileMap(false);

		enemy->SetPosition(agent->GetPosition());
		enemy2->SetPosition(agent->GetPosition());

		//[0, -1]
		//[1,  0]

		float x = enemy2->GetPosition().X * 0.0f + enemy2->GetPosition().Y * -1.0f;
		float y = enemy2->GetPosition().X * 1.0f + enemy2->GetPosition().Y *  0.0f;

		enemy2->SetPosition(x, y);

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

		DrawString(5, 780, "Elapsed Time: " + std::to_string(fElapsedTime), olc::WHITE);
		DrawString(5, 790, "Frame: " + std::to_string(i_FRAME), olc::WHITE);

		//time_functions::delays::Delay(time_functions::s, 5);
		//time_functions::delays::Delay<Demo, void>(time_functions::s, 5, this, &Demo::test);
		//time_functions::delays::Delay<Demo, void, std::string>(time_functions::s, 5, this, &Demo::test2, "Delay with arguments");

		{
			//time_functions::benchmarking::Timer test("Clear");
			//Clear({ 35, 35, 35 });
		}

		if (GetKey(olc::Key::O).bPressed)
			DebugSpawnAgent();

		if (GetKey(olc::Key::HOME).bPressed && !GameWorld->isSimulating())
			SetCanDrawGrid(!canDrawGrid());

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

		//GameWorld->DestroyGameObject(testEntity);
		SetCanDrawGrid(false);

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

		box2dTestObject->SetCanBeDetroyed(false);
		//box2dTestObject2->SetCanBeDetroyed(false);
		box2dTestObject2->SetHealth(1000.0f);
		//box2dTestObject3->SetCanBeDetroyed(false);

		player->AddPhysicsComponent();
		player->OverridePhysicsBodyPosition(true);

		//box2dTestObject->Physics_ChangeDensity(0.02f);
	}

	int FUNCTYPETEST(const char** txt, int* number) { printf("\n #THIS IS A FUNC PTR TEST - %s \" = %i \"# \n", *txt, *number); return *number; }

	void DelayTest(long long delayInSec = 5) // To be used in thread
	{
#ifdef stateChange
		//func ptr that takes one argument
		time_functions::delays::Delay<Agent, void, GameObject*>(time_functions::s, delayInSec, agent, &Agent::FollowTarget, player);
#endif // stateChange

		//lambda example
		time_functions::delays::Delay<void>(sec, delayInSec, [delayInSec] { printf("This is a delay that waits %lld sec", delayInSec); });

		//function with packed arguments
		int a = time_functions::delays::Delay(sec, delayInSec, this, &Demo::FUNCTYPETEST, "HELLO", 11);
		printf("a = %i", a);
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

