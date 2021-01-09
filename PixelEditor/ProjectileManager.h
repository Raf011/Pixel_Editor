#pragma once
#include "GameObject.h"

#ifndef ProjectileManager
#define projectileManager ProjectileManager::Get()
#endif // !ProjectileManager


class ProjectileManager
{
private:
	ProjectileManager();
	~ProjectileManager();

public:
	static ProjectileManager* Get();
	void Update(float fDeltaTime);

private:
	class Projectile : public GameObject
	{
	public:
		Projectile(std::string name, GameObject* owner, PixelMath::Vec2D position, PixelMath::Vec2D scale, PixelMath::Vec2D direction, float speed, float damage, int lifeSpan, bool stopAtTarget, olc::Pixel tint);
		~Projectile();
		
		GameObject* Owner = nullptr;
		virtual void OnCollide(GameObject* CollidingWith) override;

	public:
		float fDamage = 1.0f;	
		float fSpeed  = 1.0f;
		int iLifeSpan = 100;
		bool bStopAtTarget = false;
		PixelMath::Vec2D vVelocity;
		PixelMath::Vec2D vDirection;
		PixelMath::Vec2D vDesiredDirection;

	private:
		bool bCanBounce = false;
		GameObject* lastObjectProjectileCollidedWith = nullptr;

	public:
		void SetCanBounce(bool canBounce) { bCanBounce = canBounce; }
	};

	std::vector<Projectile*> projectilesSpawned;

	int GetIndexOfAProjectile(Projectile* object);
	void MoveProjectile(Projectile* p, float &timeDelta);
	void ThrowProjectile(Projectile* p, float &timeDelta);
	void Limit(PixelMath::Vec2D& vec, float limit);

public:
	void DestroyProjectile(Projectile* object);
	Projectile* SpawnProjectile(GameObject* owner, PixelMath::Vec2D position, PixelMath::Vec2D scale, PixelMath::Vec2D direction = { 1.0f, 0.0f }, float speed = 1.0f, float damage = 1.0f, int lifeSpan = 500, bool stopAtTarget = false, olc::Pixel tint = olc::WHITE);
};

