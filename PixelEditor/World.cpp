#include "World.h"

World::World()
{

}

World::~World()
{

}

void World::InitPhysics()
{ 
	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(0.0f, -1010.0f);//Ground position

	groundBody = box_World->CreateBody(&groundBodyDef);

	b2PolygonShape groundBox;
	groundBox.SetAsBox(1000.0f, 10.0f);
	
	groundBody->CreateFixture(&groundBox, 0.0f);
}

World * World::Get()
{
	static World instance;
	return &instance;
}

void World::Init(olc::PixelGameEngine* enginePtr, Grid* gridPtr)
{
	PGE = enginePtr;
	_Grid = gridPtr;

	InitPhysics();
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

GameObject* World::GetObjectByIndex(int index)
{
	return ListOfGameObjects[index];
}

GameObject* World::GetObjectByName(std::string name)
{
	for(GameObject* obj : ListOfGameObjects)
	{
		if (obj->GetName() == name)
			return obj;
	}

	return nullptr;
}

PixelMath::Vec2D World::GetPointInWorld(PixelMath::Vec2D point)
{
	point.X = (point.X - mainCam->GetPosition().X) * mainCam->GetZoom();
	point.Y = (point.Y - mainCam->GetPosition().Y) * mainCam->GetZoom();

	return point;
}
