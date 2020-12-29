#include "Agent.h"
#include "World.h"


#define fo_None    0b0000'0000
#define fo_GoTo    0b0000'0001
#define fo_Follow  0b0000'0010
#define fo_Flee    0b0000'0100 
// NOTE: 0b0000'0011 will have both fo_GoTo & fo_Follow flags on (fo_GoTo | fo_Follow)
// It would be better to use Enum here as we never combine flags together so bitwise
// approach is absolute in this case

//#define Face_None           0b0000'0001
//#define Face_MoveDirection  0b0000'0010
//#define Face_Target         0b0000'0100

Agent::Agent() : GameObject({ 0, 0 }, { 64, 64 }, "..//PixelEditor//Sprites//Agent.png")
{
	SetRotation(0.0f);
	Name = "Agent";
	//faceOption = Face_MoveDirection;
}


Agent::~Agent()
{
}

void Agent::OnBeginPlay()
{
	//target = { 900.0f, 900.0f };
	//bShowDebug = bDrawTasksFullDebug;
}

void Agent::Update(float &fElapsedTime)
{
	if(shouldMove && isAlive())
	{
		//target = world->GetGameObjectsSpawnedInTheWorld().at(0)->GetPosition(); // Player

		//force = Arrive(target, fElapsedTime);
		//force = Seek(target);
		//force = Flee(target);
		//force = Pursuit(target);

		SetForceBasedOnState(fElapsedTime);

		if(shouldMove)
		{
			ApplyForce(force);
			velocity += acceleration;

			//Wfloat rotation = 0.0f;

			//Rotation
			if(faceTheTarget)
			{
				//switch (faceOption)
				//{
				//	case Face_None: break;
				//	case Face_MoveDirection:  rotation = GetRotatonToFaceTarget(transform.Position, transform.Position + velocity) - GetRotationBasedOnAngle(-90.0f); SetRotation(rotation); break;
				//	case Face_Target: rotation = GetRotatonToFaceTarget(transform.Position, target) - GetRotationBasedOnAngle(-90.0f); SetRotation(rotation); break;
				//	default:break;
				//}
				
				float rotation = GetRotatonToFaceTarget(transform.Position, target) - GetRotationBasedOnAngle(-90.0f);
				SetRotation(rotation);
			}

			transform.Position = transform.Position + velocity;	
		}

		//Debug
		if (bDrawTasksFullDebug)
		{
			DrawDebugText();
			DrawDirectionalVectors();
			nextDebugLine = 5;
		}

		//Reset Values
		Heading = velocity;
		acceleration.ZeroOut();
		force.ZeroOut();
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

	engine->DrawString(10, GetNextDebugLine(), "panicDistanceSq(" + std::to_string(panicDistanceSq) +
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
		engine->DrawString(10, GetNextDebugLine(), "Speed = " + std::to_string(speed), olc::YELLOW);
		engine->DrawString(10, GetNextDebugLine(), "Velocity = X(" + std::to_string(desiredVelocity.X) +
			"), Y(" + std::to_string(desiredVelocity.Y) + ")", olc::YELLOW);

		engine->DrawString(10, GetNextDebugLine(), "Steer = X(" + std::to_string(steer.X) +
			"), Y(" + std::to_string(steer.Y) + ")", olc::YELLOW);
		engine->DrawString(10, GetNextDebugLine(), "Distance To Target = " + std::to_string(distance), olc::YELLOW);
	}

	return steer;
}

//PixelMath::Vec2D Agent::Pursuit(GameObject* target, float & timeDelta)
//{
//	PixelMath::Vec2D ToEvader = target->GetPosition() - GetPosition();
//
//	double RelativeHeading = (double)PixelMath::Functions::Dot(GetHeading(), target->GetHeading());
//	
//	return { 0.0f, 0.0f };
//}

void Agent::ApplyForce(PixelMath::Vec2D force)
{
	acceleration += force;
}

void Agent::SetForceBasedOnState(float &ElapsedTime)
{
	//if(currentMoveState == MoveState::eNone)
	if(moveState & fo_None)
	{
		shouldMove = false;
		return;
	}

	//if(currentMoveState == MoveState::eGoTo)
	if(moveState & fo_GoTo)
	{
		//are we already at the destination?
		if(GetDistanceToTarget() < 1.0f)
		{
			//currentMoveState = MoveState::eNone;
			moveState = fo_GoTo;
			shouldMove = false;
			return;
		}

		force = Arrive(target, ElapsedTime);
		return;
	}

	//if(currentMoveState == MoveState::eFollow)
	if(moveState & fo_Follow)
	{
		target = followedEntity->GetPosition();
		force = Arrive(target, ElapsedTime);
		return;
	}

	//if (currentMoveState == MoveState::eFlee)
	if (moveState & fo_Flee)
	{
		target = followedEntity->GetPosition();
		force = Flee(target);
		return;
	}
}

void Agent::MoveTo(PixelMath::Vec2D Target)
{
	target = Target;
	shouldMove = true;
	//currentMoveState = MoveState::eGoTo;
	moveState = fo_GoTo;
}

void Agent::MoveTo(GameObject* Target)
{
	target = Target->GetPosition();
	shouldMove = true;
	//currentMoveState = MoveState::eGoTo;
	moveState = fo_GoTo;
}

void Agent::FollowTarget(GameObject* Target)
{
	if(Target)
	{
		followedEntity = Target;
		target = Target->GetPosition();
		shouldMove = true;
		//currentMoveState = MoveState::eFollow;
		moveState = fo_Follow;
	}
	else
		moveState = fo_None;
		//currentMoveState = MoveState::eNone;
}

void Agent::FleeTarget(GameObject *Target)
{
	if (Target)
	{
		followedEntity = Target;
		target = Target->GetPosition();
		shouldMove = true;
		//currentMoveState = MoveState::eFlee;
		moveState = fo_Flee;
	}
	else
		moveState = fo_None;
		//currentMoveState = MoveState::eNone;
}

float Agent::GetDistanceToTarget()
{
	return PixelMath::Functions::Mag(target - transform.Position);
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
	engine->DrawString(10, GetNextDebugLine(), "Angle = " + std::to_string(angle) + " (" + std::to_string(angleInDegrees) + " degrees)", olc::YELLOW);
}

void Agent::DrawDebugText()
{
	engine->DrawString(10, GetNextDebugLine(), "Position = X: " + std::to_string(transform.Position.X) + "Y:" + std::to_string(transform.Position.Y), olc::RED);
	engine->DrawString(10, GetNextDebugLine(), "Acceleration = X(" + std::to_string(acceleration.X) +
		"), Y(" + std::to_string(acceleration.Y) + ")", olc::YELLOW);

	std::string stateTxt = "NOT_SET";
	int32_t textPos = 45;

	//switch (currentMoveState)
	//{
	//case Agent::eNone: stateTxt = "NONE";
	//	break;
	//case Agent::eGoTo: stateTxt = "MOVE TO"; textPos = 110;
	//	break;
	//case Agent::eFollow: stateTxt = "FOLLOW"; textPos = 110;
	//	break;
	//case Agent::eFlee: stateTxt = "FLEE";
	//	break;
	//default:
	//	break;
	//}

	switch (moveState)
	{
	case fo_None: stateTxt = "NONE";
		break;
	case fo_GoTo: stateTxt = "MOVE TO"; textPos = 110;
		break;
	case fo_Follow: stateTxt = "FOLLOW"; textPos = 110;
		break;
	case fo_Flee: stateTxt = "FLEE";
		break;
	default:
		break;
	}

	engine->DrawString(10, GetNextDebugLine(), "Current Task = " + stateTxt, olc::GREEN);

}
