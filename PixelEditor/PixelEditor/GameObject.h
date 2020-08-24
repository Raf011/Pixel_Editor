#pragma once
#include "PixelMath.h"
#include "olcPixelGameEngine.h"
#include <chrono>

class GameObject
{
public:
	GameObject(PixelMath::Vec2D position, PixelMath::Vec2D scale, std::string SpritePath = "..//PixelEditor//Sprites//GameObject.png", bool animate = false, int animSpeed_ms = 100);

	virtual ~GameObject();

	virtual void Update(float &fElapsedTime);
	virtual void OnEditorUpdate(float &fElapsedTime);
	void Tick(float &elapsedTime);
	virtual void OnBeginPlay();
	virtual void OnDeath() {};

	struct Transform
	{
		PixelMath::Vec2D PositionOffset;
		PixelMath::Vec2D OffsetSize;
		PixelMath::Vec2D Size;
		PixelMath::Vec2D Position;
		PixelMath::Vec2D PositionFromTopLeftCorner;
		float fRotation = 0.0f;
	};

protected:
	friend class World;
	unsigned int ID;
	std::string Name = "nameNotSet";
	std::string Tag = "NotSet";
	Transform transform;

	bool isDecalInitialised = false;
	PixelMath::Vec2D CollsionSize;
	PixelMath::Vec2D CollsionPosition;
	PixelMath::Vec2D Heading = { 0.0f, 0.0f };
	float Health = 100.0f;
	bool Static = true;
	bool Visible = true;
	bool b_CanCollide = true;
	bool isAnimated = false;
	int animationSpeed_ms = 100;
	bool despawned = false;
	bool canBeDestroyed = true;
	olc::Pixel pTint = olc::WHITE;
	bool bDrawCollision = false;
	bool bDrawTasksFullDebug = false;

	struct SpriteSheet
	{
		PixelMath::Vec2D TotalSize;
		PixelMath::Vec2D SingleSpriteSize;
		PixelMath::Vec2D NumberOfColumnsAndRowns;
		PixelMath::Vec2D CurrentDrawPosition;
		PixelMath::Vec2D CurrentInstance;
		std::chrono::time_point<std::chrono::steady_clock> nextUpdateAt;
	};

	SpriteSheet spriteSheet;
	
private:
	olc::Sprite* sprite;
	olc::Decal* decal;
	void SetupSpriteSheet();
	void UpdateSpriteSheet();
	bool bIsAlive = true;
	bool checkIfObjectIsAlive();
	bool isAChildObject = false;
	GameObject* ParentObject = nullptr;

public:
	void SetPosition(PixelMath::Vec2D newPosition);
	void SetPosition(int newPositionX, int newPositionY);
	void SetSize(PixelMath::Vec2D newSize);
	void SetSize(int newWidth, int newHeight);
	void Init();
	int GetID();
	virtual void OnCollide(GameObject* CollidingWith);
	std::string GetName();
	std::string GetTag();
	PixelMath::Vec2D GetSize();
	PixelMath::Vec2D GetPosition();
	PixelMath::Vec2D GetCollisionSize();
	PixelMath::Vec2D GetColliderPosition() { return CollsionPosition; }
	PixelMath::Vec2D GetHeading() { return Heading; }
	bool isChildObject() { return isAChildObject; }
	GameObject* GetParentObject();
	void SetAsChildObject(GameObject* parent);
	void SetCollisionSize(float Width, float Height);
	void SetCollisionSize(PixelMath::Vec2D Size);
	bool CanCollide() { return b_CanCollide; }
	void SetCollisionEnabled(bool flag);
	float GetRotation() { return transform.fRotation; }
	void SetRotation(float rotation) { transform.fRotation = rotation; }
	float GetRotationBasedOnAngle(float angle) { return angle * 0.0175f; }
	Transform GetWorldTransform() { return transform; }
	float GetRotatonToFaceTarget(PixelMath::Vec2D position, PixelMath::Vec2D target);
	float ConvertAngleToDegrees(float angle);
	float GetHealth() { return Health; }
	void SetHealth(float health) { Health = health; }
	void Damage(float damageAmount) { Health -= damageAmount; }
	void Heal(float healAmount) { Health += healAmount; }
	void Destroy();
	bool isAlive() { return bIsAlive; }
	void EnableDrawCollision(bool draw) { bDrawCollision = draw; }
	bool isDrawCollisionEnabled() { return bDrawCollision; }
	void EnableDrawTaskFullDebug(bool draw) { bDrawTasksFullDebug = draw; }
	olc::Pixel GetTint() { return pTint; }
	void SetTint(olc::Pixel tint) { pTint = tint; }
	olc::Sprite* GetSprite() { return sprite; }
	olc::Decal*  GetDecal()  { return decal; }

	//Editor only flags
	bool bAllowRMBEvents = true;
	bool bAllowDragging = true;
	bool bAllowEditorRotation = true;
	bool bIsSelected = false;
	virtual void onSelected() {};
	virtual void onDeselected() {};
	virtual void onDragged() {};

};
