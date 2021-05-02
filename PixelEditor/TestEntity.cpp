#include "TestEntity.h"
#include "World.h"
#include <cmath>

TestEntity::TestEntity() : GameObject({0.0f, 0.0f }, { 6.0f, 6.0f }, "..//PixelEditor//Sprites//Dot.png")
{
	Assassin = new GameObject({ 600.0f, 300.0f }, { 64.0f, 64.0f }, "..//PixelEditor//Sprites//Agent.png");
	Guard = new GameObject({ 600.0f, 300.0f }, { 64.0f, 64.0f }, "..//PixelEditor//Sprites//Agent.png");

	Assassin->SetTint(olc::GREEN);
	Guard->SetTint(olc::RED);

	Assassin->EnableDrawCollision(false);
	Guard->EnableDrawCollision(false);

	Assassin->SetRotation(PixelMath::GetRotationBasedOnAngle(90.0f));
	Guard->SetRotation(PixelMath::GetRotationBasedOnAngle(90.0f));

	Assassin->SetCollisionEnabled(false);
	Guard->SetCollisionEnabled(false);
}


TestEntity::~TestEntity()
{
	delete Assassin;
	delete Guard;
}

void TestEntity::OnEditorUpdate(float & fElapsedTime)
{
	PixelMath::Vec2D debugDrawPos = { 0, 20 };
	debugDrawPos = transform.PositionOffset - debugDrawPos;

	PixelMath::Vec2D tempVec = GameWorld->GetPointInWorld({ 0.0f, 0.0f }); //world->GetObjectByIndex(0)->GetWorldTransform().PositionOffset;
	engine->DrawLine((int)transform.PositionOffset.X, (int)transform.PositionOffset.Y, (int)tempVec.X, (int)tempVec.Y, olc::RED);

	PixelMath::Vec2D tempVec2 = transform.Position; //copy the position of the object as we need to modify the values.
	tempVec2.Normalize();//normalize the vector
	float deg = 0.0f;//float to store the degrees
	deg = atan2(tempVec2.X, tempVec2.Y);//calculate the rotation in radians
	deg = PixelMath::Functions::Rad2Deg(deg);//convert radians to degrees using pixel math library
	std::string tStr = std::to_string(deg); //convert float to string  so we can print it :)

	engine->DrawString((int)debugDrawPos.X, (int)debugDrawPos.Y, tStr, olc::YELLOW); //print position of the object
	engine->DrawString((int)debugDrawPos.X - 50.0f, (int)debugDrawPos.Y - 20.0f, " X = " + std::to_string(transform.Position.X) + " Y = " + std::to_string(transform.Position.Y), olc::YELLOW);//print rotation

	if(Assassin && Guard)
	{
		auto Pos1 = Assassin->GetPosition();
		auto Pos2 = Guard->GetPosition();
		Pos1.Normalize();
		Pos2.Normalize();
		//auto dot1 = Pos1.X * Pos2.X + Pos1.Y * Pos2.Y;
		//auto dot2 = (Pos1.X * Pos2.X + Pos1.Y * Pos2.Y) * (Pos1.X * Pos1.Y);

		auto Pos3 = Guard->GetPosition() - Assassin->GetPosition();
		Pos3.Normalize();
		auto dot3 = Pos3.X; // Facing vector is = 1.0, 0.0 (Pos3.X * 1.0f + Pos3.Y * 0.0f;)

		auto Pos4 = Assassin->GetPosition() - Guard->GetPosition();
		Pos4.Normalize();
		auto dot4 = Pos4.X; // Facing vector is = 1.0, 0.0 ( Pos4.X * 1.0f + Pos4.Y * 0.0f;)

		auto GuardPosOffset    = Guard->GetWorldTransform().PositionOffset;
		auto AssassinPosOffset = Assassin->GetWorldTransform().PositionOffset;
		
		//Triangles
		engine->DrawTriangle(AssassinPosOffset.X, AssassinPosOffset.Y, Guard->GetWorldTransform().PositionOffset.X, Guard->GetWorldTransform().PositionOffset.Y, GameWorld->GetPointInWorld({ 0.0f, 0.0f }).X, GameWorld->GetPointInWorld({ 0.0f, 0.0f }).Y, olc::RED);
		engine->DrawTriangle(GameWorld->GetPointInWorld(Pos1 * 100.0f).X, GameWorld->GetPointInWorld(Pos1 * 100.0f).Y, GameWorld->GetPointInWorld(Pos2 * 100.0f).X, GameWorld->GetPointInWorld(Pos2 * 100.0f).Y, GameWorld->GetPointInWorld({ 0.0f, 0.0f }).X, GameWorld->GetPointInWorld({ 0.0f, 0.0f }).Y, olc::YELLOW);

		//Green
		engine->DrawString(AssassinPosOffset.X - 50, AssassinPosOffset.Y - 90.0f, "PosNor = " + Pos1.AsString());
		engine->DrawString(AssassinPosOffset.X - 50, AssassinPosOffset.Y - 80.0f, "Dot = " + std::to_string(dot3));

		//Red
		engine->DrawString(GuardPosOffset.X - 50, GuardPosOffset.Y - 90.0f, "PosNor = " + Pos2.AsString());
		engine->DrawString(GuardPosOffset.X - 50, GuardPosOffset.Y - 80.0f, "Dot = " + std::to_string(dot4));

		float MinDot = 0.8f; //0.8f;

		//Debug "sensors"
		std::string txtGreen = dot3 > MinDot ? "Green can see Red :)" : "Green can't see Red :(";
		std::string txtRed   = dot4 > MinDot ? "Red can see Green :)" : "Red can't see Green :(";
		olc::Pixel colGreen  = dot3 > MinDot ? olc::GREEN : olc::RED;
		olc::Pixel colRed    = dot4 > MinDot ? olc::GREEN : olc::RED;


		engine->DrawString(AssassinPosOffset.X - 50, AssassinPosOffset.Y - 110.0f, txtGreen, colGreen);
		engine->DrawString(GuardPosOffset.X    - 50, GuardPosOffset.Y    - 110.0f, txtRed,   colRed);
	
		//vision
		//engine->DrawTriangle(AssassinPosOffset.X, AssassinPosOffset.Y, Guard->GetWorldTransform().PositionOffset.X, Guard->GetWorldTransform().PositionOffset.Y, GameWorld->GetPointInWorld({ 0.0f, 0.0f }).X, GameWorld->GetPointInWorld({ 0.0f, 0.0f }).Y, olc::RED);
		
		//
		if (TempVec.X == 0.0f && TempVec.Y == 0.0f)
			TempVec = Pos2;

		auto VisibilityConeU = GameWorld->GetPointInWorld(Guard->GetPosition() + (TempVec + MinDot) * 100.0f);
		auto VisibilityConeD = GameWorld->GetPointInWorld(Guard->GetPosition() - (TempVec + MinDot) * 100.0f);

		engine->DrawLine(VisibilityConeU.X, VisibilityConeU.Y, GuardPosOffset.X, GuardPosOffset.Y, olc::GREY, 0xF1F1F1F1);
		engine->DrawLine(VisibilityConeU.X, VisibilityConeD.Y, GuardPosOffset.X, GuardPosOffset.Y, olc::GREY, 0xF1F1F1F1);
	}
}