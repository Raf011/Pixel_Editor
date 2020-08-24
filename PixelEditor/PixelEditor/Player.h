#pragma once
#include "GameObject.h"
#include "World.h"

class Player :	public GameObject
{
public:
	Player();
	~Player();

private:
	Camera2D* playerCamera = nullptr;
	void Update(float &fElapsedTime) override;
	void OnBeginPlay() override;
	void CameraControls(float &elapsedTime);
	float fSpeed = 100.0f;
	GameObject* TempAgentRef = nullptr;
	float mouseX;
	float mouseY;
};

