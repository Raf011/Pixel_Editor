#include "World.h"

World::World()
{

}

World::~World()
{

}

World * World::Get()
{
	static World instance;
	return &instance;
}

void World::Init(olc::PixelGameEngine * engineRef)
{
	PGE = engineRef;
}

void World::AddMainCamera(Camera2D * CCamera)
{
	Camera = CCamera;
}

Camera2D * World::GetMainCamera()
{
	return Camera;
}

olc::PixelGameEngine * World::GetEngine()
{
	return PGE;
}

void World::RegisterNewGameObject(GameObject * object)
{
	object->ID = NextAvailableUniqueID;
	ListOfGameObjects.push_back(object);
	NextAvailableUniqueID++;
}

void World::DestroyGameObject(GameObject * object)
{
	//std::vector<GameObject*> ListOfGameObjectsCopy = ListOfGameObjects;
	int index = GetIndexOfAnObject(object);
	ListOfGameObjects.erase(ListOfGameObjects.begin() + index, ListOfGameObjects.begin() + index + 1);
	object->despawned = true;
	delete object;
}

int World::GetIndexOfAnObject(GameObject * object)
{
	for (int index = 0; index < ListOfGameObjects.size(); index++)
		if (ListOfGameObjects.at(index) == object)
			return index;

	return -1;
}

std::vector<GameObject*> World::GetGameObjectsSpawnedInTheWorld()
{
	return ListOfGameObjects;
}

