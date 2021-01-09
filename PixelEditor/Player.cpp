#include "Player.h"
#include "ProjectileManager.h"


Player::Player() : GameObject({ 500, 500 }, { 64, 64 }, "..//PixelEditor//Sprites//Player.png", true, 200)
{
	playerCamera = GameWorld->GetMainCamera();
	Name = "Player";
	//fRotation = -1.5f;
	//transform.fRotation = GetRotationBasedOnAngle(30);
}


Player::~Player()
{
}

///////////// BEGIN PLAY /////////////
void Player::OnBeginPlay()
{
	//playerCamera->SetZoom(0.7f);
	playerCamera->SetZoom(1.0f);
	playerCamera->SetSpeed(1.5f);

	PixelMath::Vec2D NewPos = playerCamera->GetPosition();
	NewPos.X += ((engine->ScreenWidth() / 2) - (GetSize().X / 2))  / playerCamera->GetZoom();
	NewPos.Y += ((engine->ScreenHeight() / 2) - (GetSize().Y / 2)) / playerCamera->GetZoom();

	playerCamera->SetPosition(GetPosition() - GetSize());

	TempAgentRef = GameWorld->GetGameObjectsSpawnedInTheWorld().at(1);
}

///////////// TICK /////////////
void Player::Update(float &fElapsedTime)
{
	mouseX = engine->GetMouseX() / mainCam->GetZoom() + mainCam->GetPosition().X;
	mouseY = engine->GetMouseY() / mainCam->GetZoom() + mainCam->GetPosition().Y;

	projectileManager->Update(fElapsedTime);
	CameraControls(fElapsedTime);

	if (engine->GetKey(olc::Key::R).bPressed)
		TempAgentRef->SetPosition(100, 100);

	if (engine->GetMouse(0).bPressed)
	{		
		PixelMath::Vec2D dir = { mouseX, mouseY };
		PixelMath::Vec2D pos = { transform.Position.X + 15.0f, transform.Position.Y - 20.0f };
		projectileManager->SpawnProjectile(this, pos, { 8.0f, 2.0f }, dir, 6.0f, 20.0f)->SetCanBounce(true);
	}

	if (engine->GetKey(olc::Key::G).bPressed)
	{
		PixelMath::Vec2D dir = { mouseX, mouseY };
		float error = float(std::rand() % 50);
		dir += error;
		PixelMath::Vec2D pos = { transform.Position.X + 15.0f, transform.Position.Y - 20.0f };
		projectileManager->SpawnProjectile(this, pos, { 8.0f, 8.0f }, dir, 6.0f, 100.0f, 500, true, olc::DARK_GREEN);
	}

	if(engine->GetKey(olc::Key::SHIFT).bPressed)
	{
		fSpeed = 200.0f;
	}
	else if(engine->GetKey(olc::Key::SHIFT).bReleased)
	{
		fSpeed = 100.0f;
	}

	PixelMath::Vec2D dir = { mouseX, mouseY };
	//dir -= Position;
	//engine->DrawString(600, 10, "Mouse Position = X(" + std::to_string(dir.X) +
	//	"), Y(" + std::to_string(dir.Y) + ")", olc::WHITE);
}

void Player::CameraControls(float &elapsedTime)
{
	float CameraZoom = playerCamera->GetZoom();
	float  Speed = fSpeed * elapsedTime;
	float  CameraZoomSpeed = playerCamera->GetZoomSpeed();
	PixelMath::Vec2D CameraPosition = playerCamera->GetPosition();

	if (engine->GetKey(olc::Key::W).bHeld || engine->GetKey(olc::Key::UP).bHeld)
	{
		transform.Position.Y -= Speed / CameraZoom;
		//fRotation = -1.5f;
	}

	if (engine->GetKey(olc::Key::S).bHeld || engine->GetKey(olc::Key::DOWN).bHeld)
	{
		transform.Position.Y += Speed / CameraZoom;
		//fRotation = 1.5f;
	}

	if (engine->GetKey(olc::Key::A).bHeld || engine->GetKey(olc::Key::LEFT).bHeld)
	{
		transform.Position.X -= Speed / CameraZoom;
		//fRotation = 3.15f;
			//6.3f - 360;
	}

	if (engine->GetKey(olc::Key::D).bHeld || engine->GetKey(olc::Key::RIGHT).bHeld)
	{
		transform.Position.X += Speed / CameraZoom;
		//fRotation = 0.0f;
	}

	//Camera follows the player
	PixelMath::Vec2D NewPos = transform.Position;
	NewPos.X -= ((engine->ScreenWidth()  / 2) - (transform.Size.X / 2)) / CameraZoom;
	NewPos.Y -= ((engine->ScreenHeight() / 2) - (transform.Size.Y / 2)) / CameraZoom;
	playerCamera->SetPosition(NewPos);

	//Face the mouse pointer	
	//Convert Mouse Position X and Y to World position
	SetRotation(GetRotatonToFaceTarget(transform.Position, { mouseX, mouseY }));

}

//void Player::OnCollide(GameObject * CollidingWith)
//{
//
//}
