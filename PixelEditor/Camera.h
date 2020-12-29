#pragma once
#include "PixelMath.h"

class Camera2D
{
public:
	Camera2D();
	~Camera2D();

private:
	PixelMath::Vec2D Position;
	PixelMath::Vec2D Bounds;
	float CameraSpeed = 2.5f;
	float CameraZoom = 1.0f;
	float CameraZoomSpeed = 0.01f;

public:
	PixelMath::Vec2D GetPosition() { return Position; }
	float GetSpeed() { return CameraSpeed; }
	float GetZoom() { return CameraZoom; }
	float GetZoomSpeed() { return CameraZoomSpeed; }
	void SetPosition(PixelMath::Vec2D pos) { Position = pos; }
	void SetPositionX(float posX) { Position.X = posX; }
	void SetPositionY(float posY) { Position.Y = posY; }
	void SetSpeed(float speed) { CameraSpeed = speed; }
	void SetZoom(float zoom) { CameraZoom = zoom; }
	void SetZoomSpeed(float speed) { CameraZoomSpeed = speed; }

};