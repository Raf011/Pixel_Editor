#include "TestEntity.h"
#include "World.h"
#include <cmath>

TestEntity::TestEntity() : GameObject({0.0f, 0.0f }, { 6.0f, 6.0f }, "..//PixelEditor//Sprites//Dot.png")
{
}


TestEntity::~TestEntity()
{
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
}