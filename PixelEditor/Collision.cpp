#include "Collision.h"



Collision::Collision()
{
}

int Collision::FindIndexOfAnActor(GameObject* ActorToFind)
{
	int i_index = 0;
	for (auto i : ActorsInGame)
	{
		if (i->GetID() == ActorToFind->GetID())
		{
			return i_index;
		}
		i_index++;
	}
	return -1;
}

bool Collision::DoesObjectExistInTheList(GameObject* ActorToCheck)
{
	for (auto i : ActorsInGame)
	{
		if (i == ActorToCheck)
			return true;
	}

	return false;
}


Collision::~Collision()
{
}


Collision* Collision::GetInstance()
{
	static Collision instance;
	return &instance;
}

void Collision::RegisterNewCollision(GameObject *Actor, float Width, float Height)
{
	Actor->SetCollisionSize(Width, Height);
	ActorsInGame.push_back(Actor);
}

void Collision::RecalculateCollision(GameObject *ForActor, float NewWidth, float NewHeight)
{
	int index = FindIndexOfAnActor(ForActor);
	if (index != -1)
	{
		ActorsInGame.at(index)->SetCollisionSize(NewWidth, NewHeight);
	}
}

void Collision::RemoveObject(GameObject * ForActor)
{
	// remove from the list
	if (DoesObjectExistInTheList(ForActor))
		ActorsInGame.erase(ActorsInGame.begin() + FindIndexOfAnActor(ForActor));
}

void Collision::Update()
{
	if (ActorsInGame.size() > 1)
	{
		for (int i = 0; i < ActorsInGame.size(); i++)
		{
			for (int i2 = (i + 1); i2 < ActorsInGame.size(); i2++)
			{
				if (
					((ActorsInGame.at(i)->GetColliderPosition().X + ActorsInGame.at(i)->GetCollisionSize().X) >= (ActorsInGame.at(i2)->GetColliderPosition().X)) &&
					((ActorsInGame.at(i2)->GetColliderPosition().X + ActorsInGame.at(i2)->GetCollisionSize().X) >= (ActorsInGame.at(i)->GetColliderPosition().X)) &&
					((ActorsInGame.at(i)->GetColliderPosition().Y + ActorsInGame.at(i)->GetCollisionSize().Y) >= (ActorsInGame.at(i2)->GetColliderPosition().Y)) &&
					((ActorsInGame.at(i2)->GetColliderPosition().Y + ActorsInGame.at(i2)->GetCollisionSize().Y) >= (ActorsInGame.at(i)->GetColliderPosition().Y))
					)
				{
					GO1 = ActorsInGame.at(i);
					GO2 = ActorsInGame.at(i2);



					GO1->OnCollide(GO2);
					GO2->OnCollide(GO1);

				}
			}
		}
	}
}

std::vector<GameObject*> Collision::GetAllActorsWithCollisionEnabled()
{
	return ActorsInGame;
}

//void Collision::Physics(GameObject * objectA, GameObject * objectB)
//{
//	float forceA = objectA->GetMass() * objectA->GetAcceleration();
//	float forceB = objectB->GetMass() * objectB->GetAcceleration();
//}

// if (ActorsInGame.at(i)->MyLocation.X == ActorsInGame.at(i2)->MyLocation.X && ActorsInGame.at(i)->MyLocation.Y == ActorsInGame.at(i2)->MyLocation.Y)

