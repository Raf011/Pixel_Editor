#pragma once
#include "olcPixelGameEngine.h"
#include "Camera.h"
#include "GameObject.h"
#include <vector>
#include "box2d/box2d.h"
#include "Grid.h"

#ifndef GameWorld
#define GameWorld World::Get()
#endif // !GameWorld

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
	Grid* _Grid = nullptr;
	std::vector<GameObject*> ListOfGameObjects;
	bool mouseDragDisabled = false;
	bool b_simulating = false;
	unsigned int NextAvailableUniqueID = 0;
	void InitPhysics();

	//Physics
	b2Vec2* box_gravity = new b2Vec2(0.0f, -10.0f);
	b2World* box_World = new b2World(*box_gravity);
	b2Body* groundBody = nullptr;

public:
	static World* Get();
	void Init(olc::PixelGameEngine* enginePtr, Grid* gridPtr);
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
	PixelMath::Vec2D GetPointInWorld(PixelMath::Vec2D point);
	GameObject* GetObjectByIndex(int index);
	Grid* GetGrid() { return _Grid; }

	b2World* GetPhysicsWorld() { return  box_World; }
	void SetGravity(float x, float y) { box_gravity->Set(x, y); }
};

