#pragma once
#include "GameObject.h"


class Agent : public GameObject
{
public:
	Agent();
	~Agent();

private:
	PixelMath::Vec2D target;
	PixelMath::Vec2D velocity;
	PixelMath::Vec2D acceleration;
	float maxSpeed   = 1.0f;
	float maxForce   = 0.3f; //apply error factor so it's not perfect (lower the number, the less precise this is)
	float slowRadius = 100.0f;
	float stopAcceptanceRadious = 0.0f;
	virtual void OnBeginPlay() override;
	virtual void Update(float &fElapsedTime) override;
	void Limit(PixelMath::Vec2D& vec, float limit);
	//bool bShowDebug = false;

	// Debug
	void DrawDirectionalVectors();

public:
	PixelMath::Vec2D Seek(PixelMath::Vec2D Target);
	PixelMath::Vec2D Flee(PixelMath::Vec2D TargetPos);
	PixelMath::Vec2D Arrive(PixelMath::Vec2D Target, float &timeDelta);
	PixelMath::Vec2D Pursuit(GameObject* target, float &timeDelta);
	void ApplyForce(PixelMath::Vec2D force);
	//void ShowDebug(bool debugOn) { bShowDebug = debugOn; }
};

