#pragma once
#include "Agent.h"
#include "StateMachine.h"


class Enemy : public Agent
{
public:
	Enemy();
	~Enemy() { delete FSM; }

private:
	StateMachine<Enemy>* FSM = new StateMachine<Enemy>(this);
	void OnAgentUpdate() override;

	class State_Global : public State<Enemy>
	{
	private:
		State_Global() {};
		~State_Global() {};

	public:
		static State_Global* Instance();
		void Enter(Enemy* owner) override;
		void Execute(Enemy* owner) override;
		void Exit(Enemy* owner) override;
	};

	class State_MoveToPlayer : public State<Enemy>
	{
	private:
		State_MoveToPlayer() {};
		~State_MoveToPlayer() {};

	public:
		static State_MoveToPlayer* Instance();
		void Enter(Enemy* owner) override;
		void Execute(Enemy* owner) override;
		void Exit(Enemy* owner) override;
	};
};

