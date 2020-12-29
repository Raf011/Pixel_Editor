#pragma once
#include "GameObject.h"


class TestEntity : public GameObject
{
public:
	TestEntity();
	~TestEntity();

private:
	void OnEditorUpdate(float &fElapsedTime) override;
};
