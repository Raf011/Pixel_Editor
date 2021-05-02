#pragma once
#include "GameObject.h"


class TestEntity : public GameObject
{
public:
	TestEntity();
	~TestEntity();

private:
	void OnEditorUpdate(float &fElapsedTime) override;

	GameObject* Assassin = nullptr;
	GameObject* Guard = nullptr;
	PixelMath::Vec2D TempVec = PixelMath::Vec2D(0.0f, 0.0f);
};
