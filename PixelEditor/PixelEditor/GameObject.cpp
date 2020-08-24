#include "GameObject.h"
#include "World.h"
#include "Collision.h"
#include <cmath>
//#include "PixelGameEditor.h"

// used to copy class name
#define quote(x) #x

GameObject::GameObject(PixelMath::Vec2D position, PixelMath::Vec2D scale, std::string SpritePath, bool animate, int animSpeed_ms)
{
	transform.Position = position;
	transform.Size = scale;

	sprite = new olc::Sprite(SpritePath);
	isAnimated = animate;
	animationSpeed_ms = animSpeed_ms;

	if (animate) 
		SetupSpriteSheet();

	World::Get()->RegisterNewGameObject(this);
	Name = quote(GameObject) + std::to_string(ID);

	if (b_CanCollide)
	{
		CollsionSize = transform.Size;
		Collision::GetInstance()->RegisterNewCollision(this, CollsionSize.X, CollsionSize.Y);
	}
}

GameObject::~GameObject()
{
	Collision::GetInstance()->RemoveObject(this);
}

void GameObject::Update(float &fElapsedTime)
{
}

void GameObject::OnEditorUpdate(float & fElapsedTime)
{
}

void GameObject::OnBeginPlay()
{
}

void GameObject::Tick(float &elapsedTime)
{
	if(checkIfObjectIsAlive())
	{

		// Center the collision
		CollsionPosition = transform.Position - PixelMath::Vec2D{ transform.Size.X / 2, transform.Size.Y / 2 };
		transform.PositionFromTopLeftCorner = CollsionPosition;

		// Draw only if within mainCam bounds
		if (mainCam && !despawned && Visible)
		{
			//Clamp Rotation
			//PixelMath::Functions::Clamp(transform.fRotation, 6.2f, -6.2f);
			if (transform.fRotation > 6.2f) transform.fRotation = 0;
			if (transform.fRotation < -6.2f) transform.fRotation = 0;

			//Calculate transform
			transform.OffsetSize.X  = (transform.Size.X * mainCam->GetZoom()) / sprite->width;
			transform.OffsetSize.Y  = (transform.Size.Y * mainCam->GetZoom())  / sprite->height;
			transform.PositionOffset.X = (transform.Position.X - mainCam->GetPosition().X) * mainCam->GetZoom();
			transform.PositionOffset.Y = (transform.Position.Y - mainCam->GetPosition().Y) * mainCam->GetZoom();
			//transform.PositionFromTopLeftCornerWithOffset = transform.PositionOffset - (transform.Size / 2.0f);

			//Update collision's position
			PixelMath::Vec2D ColPos = { (CollsionPosition.X - mainCam->GetPosition().X) * mainCam->GetZoom(), (CollsionPosition.Y - mainCam->GetPosition().Y) * mainCam->GetZoom() };

			if (isAnimated)
			{
				olc::vf2d pos = { spriteSheet.CurrentDrawPosition.X, spriteSheet.CurrentDrawPosition.Y };
				olc::vf2d size = { spriteSheet.SingleSpriteSize.X, spriteSheet.SingleSpriteSize.Y };
				olc::vf2d scale = { mainCam->GetZoom(), mainCam->GetZoom() };

				engine->DrawPartialRotatedDecal({ transform.PositionOffset.X, transform.PositionOffset.Y }, decal, pos, size, scale, transform.fRotation, { size.x / 2, size.y / 2 }, pTint);

				// Debug draw collision
				if (b_CanCollide && bDrawCollision)
					engine->DrawRect(ColPos.X, ColPos.Y, CollsionSize.X * mainCam->GetZoom(), CollsionSize.Y * mainCam->GetZoom(), olc::RED);
				
				UpdateSpriteSheet();
			}

			else
			{
				engine->DrawRotatedDecal({ transform.PositionOffset.X, transform.PositionOffset.Y }, decal, transform.fRotation, { transform.Size.X / 2, transform.Size.Y / 2 }, { transform.OffsetSize.X, transform.OffsetSize.Y }, pTint);

				// Debug draw collision
				if (b_CanCollide && bDrawCollision)
					engine->DrawRect(ColPos.X, ColPos.Y, CollsionSize.X * mainCam->GetZoom(), CollsionSize.Y * mainCam->GetZoom(), olc::RED);
			}
		}

		
		//Call update function when game starts
		if (world->isSimulating())
			Update(elapsedTime);

		else
			OnEditorUpdate(elapsedTime);
	}
}

void GameObject::SetupSpriteSheet()
{
	spriteSheet.TotalSize = { (float)sprite->width, (float)sprite->height };
	spriteSheet.SingleSpriteSize = transform.Size;
	spriteSheet.NumberOfColumnsAndRowns.X = spriteSheet.TotalSize.X / transform.Size.X;
	spriteSheet.NumberOfColumnsAndRowns.Y = spriteSheet.TotalSize.Y / transform.Size.Y;
	spriteSheet.CurrentDrawPosition = { 0,0 };
	spriteSheet.CurrentInstance = { 0,0 };
	spriteSheet.nextUpdateAt = std::chrono::high_resolution_clock::now();
}

void GameObject::UpdateSpriteSheet()
{
	if(spriteSheet.nextUpdateAt < std::chrono::high_resolution_clock::now())
	{
		if (spriteSheet.CurrentInstance.X < spriteSheet.NumberOfColumnsAndRowns.X)
		{
			spriteSheet.CurrentDrawPosition.X += spriteSheet.SingleSpriteSize.X;
			spriteSheet.CurrentInstance.X++;
		}

		else if(spriteSheet.CurrentInstance.Y < spriteSheet.NumberOfColumnsAndRowns.Y)
		{
			spriteSheet.CurrentDrawPosition.Y += spriteSheet.SingleSpriteSize.Y;
			spriteSheet.CurrentDrawPosition.X = 0;
			spriteSheet.CurrentInstance.Y++;
		}

		// reset
		else
		{
			spriteSheet.CurrentInstance.X = 0;
			spriteSheet.CurrentInstance.Y = 0;
			spriteSheet.CurrentDrawPosition.X = 0;
			spriteSheet.CurrentDrawPosition.Y = 0;
		}

		spriteSheet.nextUpdateAt = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(animationSpeed_ms);
	}
}

bool GameObject::checkIfObjectIsAlive()
{
	if (canBeDestroyed && Health < 0.0001f)
	{
		bIsAlive = false;
		world->DestroyGameObject(this);
		return false;
	}


	// rterun isAlive instead of true just in case if something else called the destroy function
	return bIsAlive;
}

void GameObject::SetPosition(PixelMath::Vec2D newPosition)
{
	transform.Position = newPosition;
}

void GameObject::SetPosition(int newPositionX, int newPositionY)
{
	transform.Position.X = newPositionX;
	transform.Position.Y = newPositionY;
}

void GameObject::SetSize(PixelMath::Vec2D newSize)
{
	transform.Size = newSize;
	spriteSheet.SingleSpriteSize = newSize;
}

void GameObject::SetSize(int newWidth, int newHeight)
{
	transform.Size.X = newWidth;
	transform.Size.Y = newHeight;
	spriteSheet.SingleSpriteSize.X = newWidth;
	spriteSheet.SingleSpriteSize.Y = newHeight;
}

void GameObject::Init()
{
	decal = new olc::Decal(sprite);
}

int GameObject::GetID()
{
	return ID;
}

void GameObject::OnCollide(GameObject* CollidingWith)
{

}

std::string GameObject::GetName()
{
	return Name;
}

std::string GameObject::GetTag()
{
	return Tag;
}

PixelMath::Vec2D GameObject::GetSize()
{
	return transform.Size;
}

PixelMath::Vec2D GameObject::GetPosition()
{
	return transform.Position;
}

PixelMath::Vec2D GameObject::GetCollisionSize()
{
	return CollsionSize;
}

//void GameObject::SetTag(std::string tag)
//{
//	Tag = tag;
//}

GameObject * GameObject::GetParentObject()
{
	if (isAChildObject && ParentObject)
		return ParentObject;

	else
		return nullptr;
}

void GameObject::SetAsChildObject(GameObject * parent)
{
	if(parent)
	{
		isAChildObject = true;
		ParentObject = parent;
	}
}

void GameObject::SetCollisionSize(float Width, float Height)
{
	CollsionSize = { Width, Height };
}

void GameObject::SetCollisionSize(PixelMath::Vec2D Size)
{
	CollsionSize = Size;
}

void GameObject::SetCollisionEnabled(bool flag)
{
	// don't change anything if we already have the flag set by default
	if(flag != b_CanCollide)
	{
		b_CanCollide = flag;

		if(flag)
		{
			Collision::GetInstance()->RegisterNewCollision(this, CollsionSize.X, CollsionSize.Y);
			return;
		}
		else
		{
			Collision::GetInstance()->RemoveObject(this);
		}
	}
}

float GameObject::GetRotatonToFaceTarget(PixelMath::Vec2D position, PixelMath::Vec2D target)
{
	float a = target.Y - position.Y;
	float b = target.X - position.X;

	float angle = atan2(a, b);
	return angle;
}

float GameObject::ConvertAngleToDegrees(float angle)
{
	float result = angle * 180 / PI;
	return result;
}

void GameObject::Destroy()
{
	// Setting health below 0 will automatically call the world's destroy function
	Health = -1.0f;
}
