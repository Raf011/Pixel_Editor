#include "Enemy.h"
#include "World.h"

#define Singleton(_namespace_) static _namespace_ instance; return &instance

Enemy::Enemy()
{
	FSM->SetGlobalState(State_Global::Instance());
	FSM->SetCurrentState(State_MoveToPlayer::Instance());
	FSM->ChangeState(State_MoveToPlayer::Instance());
}

void Enemy::OnAgentUpdate()
{
	FSM->Update();
}


//STATES:
//########## Global State ##########
Enemy::State_Global* Enemy::State_Global::Instance()
{
	Singleton(Enemy::State_Global);
}

void Enemy::State_Global::Enter(Enemy* owner)
{
}

void Enemy::State_Global::Execute(Enemy* owner)
{
}

void Enemy::State_Global::Exit(Enemy* owner)
{
}

//########## MoveToPlayer State ##########
Enemy::State_MoveToPlayer* Enemy::State_MoveToPlayer::Instance()
{
	Singleton(Enemy::State_MoveToPlayer);
}

void Enemy::State_MoveToPlayer::Enter(Enemy* owner)
{
	static GameObject* PlayerPtr = GameWorld->GetObjectByName("Player");

	if(PlayerPtr)
	{
		owner->CAN_TEST_PATH = false;
		owner->FollowTarget(PlayerPtr);
	}
}

void Enemy::State_MoveToPlayer::Execute(Enemy* owner)
{
}

void Enemy::State_MoveToPlayer::Exit(Enemy* owner)
{
}

//########## X State ##########
