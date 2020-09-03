#pragma once
#include "olcPixelGameEngine.h"
#include "World.h"
#include "Grid.h"
#include "PixelWindow.h"

#ifdef _DEBUG
#define EditorOnCreate OnEditorCreate()
#define EditorOnUpdate(ElapsedTime) OnEditorTick(ElapsedTime)
#else
#define EditorOnCreate Button_Simulate_OnClicked(); OnCreate()
#define EditorOnUpdate(ElapsedTime) OnUpdate(ElapsedTime)
#endif // _DEBUG

#define mainCam World::Get()->GetMainCamera()
//#define world World::Get()

class PixelGameEditor : public olc::PixelGameEngine
{
public:
	PixelGameEditor();
	virtual ~PixelGameEditor() {};

private:
	unsigned int Frame;
	Grid* grid2D = new Grid();
	int32_t mouseX = 0;
	int32_t mouseY = 0;
	olc::Sprite* rotationSprite;
	olc::Decal*  rotationDecal;
	bool beganPlay = false;

	float initialCamSpeed = 0.0f;

	PixelWindow* ObjectInspectorWindow = new PixelWindow({ 0, 0 }, { 240, 360 }, { 23, 24, 25 , 255 }, "Object Inspector");
	PixelWindow* SimulateWindow;
	PixelWindow* WorldWindow;
	PixelWindow* EditableBlockWindow;

	PixelWindow::Image* ObjectInspectorImage = nullptr;
	PixelWindow::Text*  CameraDebugText = nullptr;
	PixelWindow::Text*  ObjectDebugText = nullptr;
	void UpdateDebugText();

public:
	bool OnEditorCreate();
	bool OnEditorTick(float &fElapsedTime);
	bool OnCreate();
	bool OnUpdate(float &fElapsedTime);

	//Fires once when simulation starts
	virtual bool OnBeginPlay() { return false; };
	//Fires each frame after simulation starts
	virtual bool OnTick() { return false; };

protected:
	unsigned int GetCurrentFrame() { return Frame; }
	inline void CenterCamera();

	bool isZoomingCamera = false;
	bool saveCamPos = true;
	float zoomCameraOffsetX = 0.0f;
	float zoomCameraOffsetY = 0.0f;
	float zoomCameraOffset = 0.0f;

	inline void Mouse_MoveCamera();
	float moveCameraOffsetX = 0.0f;
	float moveCameraOffsetY = 0.0f;
	bool saveCameraMoveOffset = true;

	bool CanDragCameraWithMouse = true;
	void CameraControl();
	void DrawGrid();
	void DrawGameObjects(float &fElapsedTime);
	void InitGameObjects();

	void TestGrid(float PosX, float PosY, bool DrawNeigbours);

	void OutlineGridNode(Grid::Node* node, olc::Pixel color, bool DrawADot = true);

	bool isObjectDraggedByMouse = false;
	GameObject* ObjectDraggedByMouse = nullptr;	
	PixelMath::Vec2D GrabOffset = {0,0};
	bool CanDrag = true;
	void MouseDragFunctionality();
	GameObject* SelectedGameObject = nullptr;
	olc::Pixel SelectedGameObjectTint = olc::WHITE;
	void ObjectSelectionFunctionality();
	void DeselectObject();
	void ObjectRotatorFunctionality();
	bool bRotatorToolVisible = true;
	bool bConstrainMousePointerToGridArea = false;
	float rotationSpeed = 0.1f;

	struct Rotator
	{
		PixelMath::Vec2D pos;
		PixelMath::Vec2D size;
		PixelMath::Vec2D offset;
		float middlePointY;
		int32_t r_mouseX;
		int32_t r_mouseY;

		bool isSelected() { return (r_mouseX > offset.X && r_mouseX < offset.X + size.X && r_mouseY > offset.Y && r_mouseY < offset.Y + size.Y); }
	};
	Rotator rotator;

	void Button1_OnClicked()
	{
		//pw1->SetWindowPosition(0, 0);
	}

	void Button_ResetObjectInspectorWindow_OnClicked()
	{
		ObjectInspectorWindow->SetWindowPosition(0, 0);
		ObjectInspectorWindow->SetWindowSize(240, 360);
	}

	void Button_CloseWindow1()
	{
		ObjectInspectorWindow->Hide();
	}

	void Button_Simulate_OnClicked()
	{
		world->setIsSimulating(true);
		mainCam->SetZoom(1.0f);
		mainCam->SetPosition({ 0.f, 0.f });

		for (auto i : world->GetGameObjectsSpawnedInTheWorld())
			i->OnBeginPlay();
	}

private:
	void SetPixelWindows();
	bool b_RMB_Event_Active = false;
	void RMB_Events();
	PixelWindow* RMB_Event_Window = nullptr;
	GameObject* RMB_Event_GO = nullptr;
	void SetRMBButton(PixelWindow::Button<PixelGameEditor>* button);
	void RMB_Event_Button_DestroyObject() { world->DestroyGameObject(RMB_Event_GO); }
	void RMB_Event_Button_Reset() { RMB_Event_GO->SetPosition(0, 0); }
	void RMB_Event_Button_ToggleCollision() { RMB_Event_GO->isDrawCollisionEnabled() ? RMB_Event_GO->EnableDrawCollision(false) : RMB_Event_GO->EnableDrawCollision(true); }
};