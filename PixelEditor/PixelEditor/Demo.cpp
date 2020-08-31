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

#define collisionManager Collision::GetInstance()

class Demo : public PixelGameEditor
{
	GameObject* player = new Player();
	Agent* agent = new Agent();
	GameObject* editableBlock = new EditableBlock({ 925.f, 75.f });

public:
	int i_FRAME = 0;

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
	}

public:
	// Called once at the start of the editor
	bool OnUserCreate() override
	{
		EditorOnCreate;

		agent->EnableDrawCollision(true);
		agent->EnableDrawTaskFullDebug(true);
		agent->SetPosition( 800.f, 500.f );
		//Make agent move to the target here
		//agent->MoveTo(player->GetPosition());

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
		return true;
	}

	//bool OnTick() override
	//{
	//return true;
	//}

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

