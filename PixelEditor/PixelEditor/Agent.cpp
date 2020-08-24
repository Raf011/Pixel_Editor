#include "Agent.h"
#include "World.h"


Agent::Agent() : GameObject({ 0, 0 }, { 64, 64 }, "..//PixelEditor//Sprites//Agent.png")
{
	SetRotation(0.0f);
	Name = "Agent";
}


Agent::~Agent()
{
}

void Agent::OnBeginPlay()
{
	//target = { 900.0f, 900.0f };
	//bShowDebug = bDrawTasksFullDebug;
}

void Agent::Update(float & fElapsedTime)
{
	if(isAlive())
	{
		target = world->GetGameObjectsSpawnedInTheWorld().at(0)->GetPosition(); // Player

		PixelMath::Vec2D arrive = Arrive(target, fElapsedTime);
		//arrive = Seek(target);
		ApplyForce(arrive);

		float rotation = GetRotatonToFaceTarget(transform.Position, target) - GetRotationBasedOnAngle(-90.0f);
		SetRotation(rotation);

		velocity += acceleration;
		transform.Position = transform.Position + velocity;

		if (bDrawTasksFullDebug)
		{
			engine->DrawString(10, 5, "Position = X: " + std::to_string(transform.Position.X) + "Y:" + std::to_string(transform.Position.Y), olc::RED);
			engine->DrawString(10, 35, "Acceleration = X(" + std::to_string(acceleration.X) +
				"), Y(" + std::to_string(acceleration.Y) + ")", olc::YELLOW);
						
			DrawDirectionalVectors();
		}

		Heading = velocity;
		acceleration = {0.0f, 0.0f};
	}
}

PixelMath::Vec2D Agent::Seek(PixelMath::Vec2D Target)
{
	PixelMath::Vec2D desiredVelocity = PixelMath::Functions::Normalize(Target - transform.Position) * maxSpeed;
	return (desiredVelocity - velocity);
}

PixelMath::Vec2D Agent::Flee(PixelMath::Vec2D TargetPos)
{
	const double panicDistanceSq = 200.0 * 200.0;
	const double distanceSq = (double)PixelMath::Functions::MagSqr(GetPosition() - target);

	engine->DrawString(10, 131, "panicDistanceSq(" + std::to_string(panicDistanceSq) +
		"), distanceSq(" + std::to_string(distanceSq) + ")", olc::GREEN);
	
	if(distanceSq > panicDistanceSq)
	{
		velocity = { 0.0f, 0.0f };
		return { 0.0f, 0.0f };
	}


	PixelMath::Vec2D DesiredVel = PixelMath::Functions::Normalize((GetPosition() - TargetPos) * maxSpeed);

	return (DesiredVel - velocity);
}

PixelMath::Vec2D Agent::Arrive(PixelMath::Vec2D Target, float &timeDelta)
{
	PixelMath::Vec2D desiredVelocity = Target - transform.Position;

	float distance = PixelMath::Functions::Mag(desiredVelocity);
	float speed = maxSpeed * (timeDelta * 100.0f);

	if(distance < slowRadius)
	{
		if (distance < stopAcceptanceRadious)
		{
			speed = 0.0f;
		}


		else
		{
			speed = maxSpeed * (distance / slowRadius);
		}
	}

	PixelMath::Functions::SetMag(desiredVelocity, speed);
	PixelMath::Vec2D steer = desiredVelocity - velocity;
	Limit(steer, maxForce);

	if(bDrawTasksFullDebug)
	{
		engine->DrawString(10, 20, "Speed = " + std::to_string(speed), olc::YELLOW);
		engine->DrawString(10, 50, "Velocity = X(" + std::to_string(desiredVelocity.X) +
			"), Y(" + std::to_string(desiredVelocity.Y) + ")", olc::YELLOW);

		engine->DrawString(10, 65, "Steer = X(" + std::to_string(steer.X) +
			"), Y(" + std::to_string(steer.Y) + ")", olc::YELLOW);
		engine->DrawString(10, 80, "Distance To Target = " + std::to_string(distance), olc::YELLOW);
	}

	return steer;
}

PixelMath::Vec2D Agent::Pursuit(GameObject* target, float & timeDelta)
{
	PixelMath::Vec2D ToEvader = target->GetPosition() - GetPosition();

	double RelativeHeading = (double)PixelMath::Functions::Dot(GetHeading(), target->GetHeading());
	
	return { 0.0f, 0.0f };
}

void Agent::ApplyForce(PixelMath::Vec2D force)
{
	acceleration += force;
}

void Agent::Limit(PixelMath::Vec2D &vec, float limit)
{
	if (vec.X > limit) vec.X = limit;
	if (vec.Y > limit) vec.Y = limit;
}

//Debug
void Agent::DrawDirectionalVectors()
{
	float StartX = (transform.Position.X - mainCam->GetPosition().X) * mainCam->GetZoom();
	float StartY = (transform.Position.Y - mainCam->GetPosition().Y) * mainCam->GetZoom();
	float EndX = (target.X   - mainCam->GetPosition().X) * mainCam->GetZoom();
	float EndY = (target.Y   - mainCam->GetPosition().Y) * mainCam->GetZoom();
	//engine->DrawLine(StartX, StartY, EndX, EndY, olc::CYAN);

	EndX = (transform.Position.X + velocity.X * 100.0f - mainCam->GetPosition().X) * mainCam->GetZoom();
	EndY = (transform.Position.Y + velocity.Y * 100.0f - mainCam->GetPosition().Y) * mainCam->GetZoom();
	engine->DrawLine(StartX, StartY, EndX, EndY, olc::YELLOW);

	float angle = GetRotatonToFaceTarget(transform.Position, target) - GetRotationBasedOnAngle(-90.0f);
	int angleInDegrees = std::floor(ConvertAngleToDegrees(angle));
	engine->DrawString(10, 95, "Angle = " + std::to_string(angle) + " (" + std::to_string(angleInDegrees) + " degrees)", olc::YELLOW);
}
