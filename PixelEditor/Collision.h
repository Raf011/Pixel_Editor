#pragma once
#include <vector>
#include "GameObject.h"

class Collision
{

private:
	Collision(); // Singleton design pattern with a privet constructor

	std::vector<GameObject*> ActorsInGame;
	int FindIndexOfAnActor(GameObject* ActorToFind);
	bool DoesObjectExistInTheList(GameObject* ActorToCheck);
	GameObject *GO1;
	GameObject *GO2;

public:
	~Collision();

	// Get the Instance of the Singleton Class
	static Collision* GetInstance();

	//int Value = 11;

	void RegisterNewCollision(GameObject* Actor, float Width, float Height);
	void RecalculateCollision(GameObject* ForActor, float NewWidth, float NewHeight);
	void RemoveObject(GameObject* ForActor);
	void Update();
	std::vector<GameObject*> GetAllActorsWithCollisionEnabled();
};