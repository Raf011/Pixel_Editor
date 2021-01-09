#include "ProjectileManager.h"



ProjectileManager::ProjectileManager()
{
}


ProjectileManager::~ProjectileManager()
{
}

ProjectileManager * ProjectileManager::Get()
{
	static ProjectileManager instance;
	return &instance;
}

ProjectileManager::Projectile* ProjectileManager::SpawnProjectile(GameObject* owner, PixelMath::Vec2D position, PixelMath::Vec2D scale, PixelMath::Vec2D direction, float speed, float damage, int lifeSpan, bool stopAtTarget, olc::Pixel tint)
{
	if (speed < 0.001f) return nullptr; // Don't spawn if the speed is too low

	std::string pName = "Projectile(" + std::to_string(projectilesSpawned.size()) + ")";
	Projectile* tempProj = new Projectile(pName, owner, position, scale, direction, speed, damage, lifeSpan, stopAtTarget, tint);
	tempProj->Init();

	projectilesSpawned.emplace_back(tempProj);

	return tempProj;
}

void ProjectileManager::Update(float fDeltaTime)
{
	for(Projectile* projectile : projectilesSpawned)
	{
		if(projectile->isAlive())
		{
			projectile->iLifeSpan--;

			if (projectile->iLifeSpan > 0)
			{
				if (!projectile->bStopAtTarget)
					MoveProjectile(projectile, fDeltaTime);

				else
					ThrowProjectile(projectile, fDeltaTime);
			}

			else
				DestroyProjectile(projectile);
		}

		else
			DestroyProjectile(projectile);
	}
}

int ProjectileManager::GetIndexOfAProjectile(Projectile * object)
{
	int index = 0;

	for(Projectile* i : projectilesSpawned)
	{
		if (i == object)
			return index;

		index++;
	}

	//Index not found
	return -1;
}

void ProjectileManager::MoveProjectile(Projectile * p, float &timeDelta)
{	
	PixelMath::Vec2D vAcceleration = { 0.0f, 0.0f };

	// Don't update the desire location so the projectile never stops
	//p->vDesiredDirection = p->vDirection - p->GetPosition();

	float speed = p->fSpeed * (timeDelta * 100.0f);

	PixelMath::Functions::SetMag(p->vDesiredDirection, speed);
	PixelMath::Vec2D steer = p->vDesiredDirection - p->vVelocity;
	//Limit(steer, p->fSpeed);

	vAcceleration += steer;

	p->SetPosition(p->GetPosition() + p->vVelocity);
	p->vVelocity = p->vVelocity + vAcceleration;
}

void ProjectileManager::ThrowProjectile(Projectile *p, float &timeDelta)
{
	PixelMath::Vec2D vAcceleration = { 0.0f, 0.0f };

	// Update the desire location so the projectile stops
	p->vDesiredDirection = p->vDirection - p->GetPosition();

	float distance = PixelMath::Functions::Mag(p->vDesiredDirection);
	float speedMult = 1.0f;

	if(distance < 100.0f)
	{

		speedMult = distance / 100.0f;

		if (distance < 5.0f)
		return;
	}

	float speed = p->fSpeed * speedMult * (timeDelta * 100.0f);

	PixelMath::Functions::SetMag(p->vDesiredDirection, speed);
	PixelMath::Vec2D steer = p->vDesiredDirection - p->vVelocity;
	//Limit(steer, 0.1f);


	vAcceleration += steer;

	p->SetPosition(p->GetPosition() + p->vVelocity);
	p->vVelocity = p->vVelocity + vAcceleration;
}

void ProjectileManager::Limit(PixelMath::Vec2D & vec, float limit)
{
	if (vec.X > limit) vec.X = limit;
	if (vec.Y > limit) vec.Y = limit;
}

void ProjectileManager::DestroyProjectile(Projectile* object)
{
	if (!object)
	{
		std::cout << "Projectile Was Null!";
	}

	int index = GetIndexOfAProjectile(object);
	if(index > -1)
	{
		projectilesSpawned.erase(projectilesSpawned.begin() + index, projectilesSpawned.begin() + index + 1);
		object->Destroy();
	}
}

ProjectileManager::Projectile::Projectile(std::string name, GameObject* owner, PixelMath::Vec2D position, PixelMath::Vec2D scale, PixelMath::Vec2D direction, float speed, float damage,  int lifeSpan, bool stopAtTarget, olc::Pixel tint) : GameObject(position, scale, "..//PixelEditor//Sprites//Dot.png")
{
	vDirection = direction;
	fSpeed = speed;
	fDamage = damage;
	Owner = owner;
	iLifeSpan = lifeSpan;
	Name = name;
	bStopAtTarget = stopAtTarget;
	pTint = tint;

	//set the static desired velocity
	vDesiredDirection = vDirection - GetPosition();

	SetRotation(GetRotatonToFaceTarget(position, direction));

}

ProjectileManager::Projectile::~Projectile()
{
}

void ProjectileManager::Projectile::OnCollide(GameObject * CollidingWith)
{
	if (CollidingWith == lastObjectProjectileCollidedWith) { return;  }
	if (!CollidingWith || CollidingWith == this) { return; } //if colliding with itself;
	if (CollidingWith == dynamic_cast<Projectile*>(CollidingWith)) { return; } //if it's not another projectile

	if(CollidingWith != Owner && isAlive() && CollidingWith != this)
	{
		if(CollidingWith->GetCanBeDestroyed())
		{
			CollidingWith->Damage(fDamage);
		}

		if(bCanBounce)
		{
			vDesiredDirection = { -vDesiredDirection.X, -vDesiredDirection.Y };
			fSpeed -= 0.3f;
		}

		else
		{
			if(canBeDestroyed)
			{
				Get()->DestroyProjectile(this);
			}
		}
	}

	lastObjectProjectileCollidedWith = CollidingWith;
}
