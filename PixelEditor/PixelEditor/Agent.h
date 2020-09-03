#pragma once
#include "GameObject.h"


class Agent : public GameObject
{
public:
	Agent();
	~Agent();

	//Moving this to use int
	//enum FaceOptions
	//{
	//	FaceNone, FaceMoveDirection, FaceTarget
	//};

private:
	int8_t nextDebugLine = 5;
	uint8_t moveState{ 0b0000'0000 };
	uint8_t faceOption{ 0b0000'0000 };

	/*
	enum MoveState
	{
		eNone, eGoTo, eFollow, eFlee
	};

	MoveState currentMoveState = eNone;
	*/
	PixelMath::Vec2D target;
	PixelMath::Vec2D force;
	PixelMath::Vec2D velocity;
	PixelMath::Vec2D acceleration;

	bool faceTheTarget = true;
	bool shouldMove = true;
	bool keepFollowing = false;

	float maxSpeed   = 1.0f;
	float maxForce   = 0.3f; //apply error factor so it's not perfect (lower the number, the less precise this is)
	float slowRadius = 100.0f;
	float stopAcceptanceRadious = 0.0f;
	virtual void OnBeginPlay() override;
	virtual void Update(float &fElapsedTime) override;
	void Limit(PixelMath::Vec2D& vec, float limit);
	GameObject* followedEntity = nullptr;
	//bool bShowDebug = false;

	// Debug
	void DrawDirectionalVectors();
	void DrawDebugText();

	PixelMath::Vec2D Arrive(PixelMath::Vec2D Target, float &timeDelta);
	PixelMath::Vec2D Seek(PixelMath::Vec2D Target);
	//PixelMath::Vec2D Pursuit(GameObject* target, float &timeDelta);
	PixelMath::Vec2D Flee(PixelMath::Vec2D TargetPos);
	void ApplyForce(PixelMath::Vec2D force);
	void SetForceBasedOnState(float &ElapsedTime);
	int32_t GetNextDebugLine() { nextDebugLine += 15; return nextDebugLine - 15; }

public:
	float GetDistanceToTarget();
	void MoveTo(PixelMath::Vec2D Target);
	void FollowTarget(GameObject* Target);
	void FleeTarget(GameObject* Target);
	//void ShowDebug(bool debugOn) { bShowDebug = debugOn; }
};

