#pragma once
#include "olcPixelGameEngine.h"
#include "Camera.h"
#include "GameObject.h"
#include <vector>

#ifndef world
#define world World::Get()
#endif // !world

#ifndef engine
#define engine World::Get()->GetEngine()
#endif // !engine

#ifndef mainCam
#define mainCam World::Get()->GetMainCamera()
#endif // !mainCam

class World
{
private:
	World();
	~World();
	olc::PixelGameEngine* PGE = nullptr;
	Camera2D* Camera = nullptr;
	std::vector<GameObject*> ListOfGameObjects;
	bool mouseDragDisabled = false;
	bool b_simulating = false;
	unsigned int NextAvailableUniqueID = 0;

public:
	static World* Get();
	void Init(olc::PixelGameEngine* engineRef);
	void AddMainCamera(Camera2D* CCamera);
	Camera2D* GetMainCamera();
	olc::PixelGameEngine* GetEngine();
	void RegisterNewGameObject(GameObject* object);
	void DestroyGameObject(GameObject* object);
	int GetIndexOfAnObject(GameObject* object);
	std::vector<GameObject*> GetGameObjectsSpawnedInTheWorld();
	bool isMouseDragDisabled() { return mouseDragDisabled; }
	void setMouseDragDisabled(bool disabled) { mouseDragDisabled = disabled; }
	bool isSimulating() { return b_simulating; }
	void setIsSimulating(bool simulating) { b_simulating = simulating; }
	int debug_retunNextAvailableID() { return NextAvailableUniqueID; }

};

