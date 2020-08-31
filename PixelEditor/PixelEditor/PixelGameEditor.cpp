#include "PixelGameEditor.h"
#include "Collision.h"
#include "EditableBlock.h"

bool PixelGameEditor::OnEditorCreate()
{
	OnCreate();

	if (!world->isSimulating())
	{
		initialCamSpeed = mainCam->GetSpeed();;
		SetPixelWindows();
	}

	return true;
}

bool PixelGameEditor::OnEditorTick(float & fElapsedTime)
{
	OnUpdate(fElapsedTime);

	//Close the editor
	if (GetKey(olc::Key::ESCAPE).bPressed)
		olc_Terminate();

	if(!world->isSimulating())
	{
		CameraControl();

		//Update Mouse Position in the world
		mouseX = GetMouseX() / mainCam->GetZoom() + mainCam->GetPosition().X;
		mouseY = GetMouseY() / mainCam->GetZoom() + mainCam->GetPosition().Y;

		//Constrain mouse position to the grid
		if(bConstrainMousePointerToGridArea)
		{
			if (mouseX > grid2D->GridSize.X) mouseX = grid2D->GridSize.X - grid2D->GetNodeSize();
			if (mouseY > grid2D->GridSize.Y) mouseY = grid2D->GridSize.Y - grid2D->GetNodeSize();
			if (mouseX < 0) mouseX = 0;
			if (mouseY < 0) mouseY = 0;
		}


		UpdateDebugText();
		MouseDragFunctionality();
		ObjectSelectionFunctionality();
		RMB_Events();

		if (GetKey(olc::Key::G).bHeld)
			TestGrid(mouseX, mouseY, true);

		if(GetKey(olc::Key::R).bPressed)
		{
			if (bRotatorToolVisible)
				bRotatorToolVisible = false;

			else
				bRotatorToolVisible = true;
		}

		if (GetKey(olc::Key::Z).bHeld && rotationSpeed < 0.3f)
			rotationSpeed += 0.001f;

		if (GetKey(olc::Key::X).bHeld && rotationSpeed > 0.004f)
			rotationSpeed -= 0.001f;

		//Show/Hide Object Inspector
		if (GetKey(olc::Key::I).bPressed)
			ObjectInspectorWindow->isVisible() ? ObjectInspectorWindow->Hide() : ObjectInspectorWindow->Show();

		// Delete a GameObject
		if (GetKey(olc::Key::DEL).bPressed && SelectedGameObject)
		{
			world->DestroyGameObject(SelectedGameObject);
			SelectedGameObject = nullptr;
		}

		//Pixel Windows
		ObjectInspectorWindow->Update();
		SimulateWindow->Update();
		WorldWindow->Update();
		EditableBlockWindow->Update();
		//////////

		//UpdateGrid
		grid2D->UpdateGrid();
	}

	//Is Simulating
	else
	{
		//On Simulate
		if (!beganPlay)
		{
			OnBeginPlay();
			beganPlay = true;
		}

		OnTick();
	}

	return true;
}

bool PixelGameEditor::OnCreate()
{
	grid2D->GridInit();
	InitGameObjects();
	//Rotation tool
	rotationSprite = new olc::Sprite("..//PixelEditor//Sprites//Arrow.png");
	rotationDecal = new olc::Decal(rotationSprite);
	return true;
}

bool PixelGameEditor::OnUpdate(float & fElapsedTime)
{
	Frame++;
	Clear({ 35, 35, 35 });
	DrawGrid();
	DrawGameObjects(fElapsedTime);
	Collision::GetInstance()->Update();
	return true;
}


inline void PixelGameEditor::CenterCamera()
{
	isZoomingCamera = true;

	float screenW = (float)(ScreenWidth() / 2);
	float screenH = (float)(ScreenHeight() / 2);
	float posX = 0.0f;
	float posY = 0.0f;

	//focus on the selected entity
	if(SelectedGameObject)
	{
		posX = SelectedGameObject->GetPosition().X - (screenW / mainCam->GetZoom());
		posY = SelectedGameObject->GetPosition().Y - (screenH / mainCam->GetZoom());
		//DeselectObject(); //option to deselected objects
	}

	//focus on the middle of the screen
	else
	{
		if(saveCamPos)
		{
			zoomCameraOffsetX = mainCam->GetPosition().X;
			zoomCameraOffsetY = mainCam->GetPosition().Y;
			zoomCameraOffset = mainCam->GetZoom();
		
			posX = zoomCameraOffsetX ;
			posY = zoomCameraOffsetY;
		
			saveCamPos = false;
		}
		else
		{
			posX = (zoomCameraOffsetX + screenW / zoomCameraOffset) - screenW / mainCam->GetZoom();
			posY = (zoomCameraOffsetY + screenH / zoomCameraOffset) - screenH / mainCam->GetZoom();
		}
	}

	mainCam->SetPosition({ posX, posY });
}

inline void PixelGameEditor::Mouse_MoveCamera()
{
	if(!isObjectDraggedByMouse && !b_RMB_Event_Active && CanDragCameraWithMouse)
	{

		if (GetMouseX() < 10.0f || GetMouseX() > ScreenWidth() - 10.0f)
		{
			CanDragCameraWithMouse = false; // can no longer drag the camera as the mouse cursor left the window bounds.
			return;		
		}

		if (GetMouseY() < 10.0f || GetMouseY() > ScreenHeight() - 10.0f)
		{
			CanDragCameraWithMouse = false;
			return;
		}

		if(saveCameraMoveOffset)
		{
			moveCameraOffsetX = (float)GetMouseX();
			moveCameraOffsetY = (float)GetMouseY();
			saveCameraMoveOffset = false;
		}


		float offsetX = (float)GetMouseX() - moveCameraOffsetX;
		float offsetY = (float)GetMouseY() - moveCameraOffsetY;

		mainCam->SetPosition({ mainCam->GetPosition().X - offsetX, mainCam->GetPosition().Y - offsetY });
		moveCameraOffsetX = (float)GetMouseX();
		moveCameraOffsetY = (float)GetMouseY();
	}
}

void PixelGameEditor::CameraControl()
{
	isZoomingCamera = false;
	float CameraZoom = mainCam->GetZoom();
	float CameraSpeed = mainCam->GetSpeed();
	float CameraZoomSpeed = mainCam->GetZoomSpeed();
	PixelMath::Vec2D CameraPosition = mainCam->GetPosition();

	if (GetKey(olc::Key::SHIFT).bHeld)
		mainCam->SetSpeed(initialCamSpeed * 6.0f);

	if (GetKey(olc::Key::SHIFT).bReleased)
		mainCam->SetSpeed(initialCamSpeed);

	if (GetKey(olc::Key::W).bHeld || GetKey(olc::Key::UP).bHeld)
	{	mainCam->SetPositionY(CameraPosition.Y - CameraSpeed / CameraZoom); b_RMB_Event_Active = false; }

	if (GetKey(olc::Key::S).bHeld || GetKey(olc::Key::DOWN).bHeld)
	{	mainCam->SetPositionY(CameraPosition.Y + CameraSpeed / CameraZoom); b_RMB_Event_Active = false; }

	if (GetKey(olc::Key::A).bHeld || GetKey(olc::Key::LEFT).bHeld)
	{	mainCam->SetPositionX(CameraPosition.X - CameraSpeed / CameraZoom); b_RMB_Event_Active = false; }

	if (GetKey(olc::Key::D).bHeld || GetKey(olc::Key::RIGHT).bHeld)
	{ mainCam->SetPositionX(CameraPosition.X + CameraSpeed / CameraZoom); b_RMB_Event_Active = false; }
	
	if ((GetKey(olc::Key::E).bHeld || GetMouseWheel() > 1) && CameraZoom < 10.0f)
	{ mainCam->SetZoom(CameraZoom + CameraZoomSpeed); CenterCamera(); b_RMB_Event_Active = false;	}

	if ((GetKey(olc::Key::Q).bHeld || GetMouseWheel() < 0) && CameraZoom > 0.3f)
	{ mainCam->SetZoom(CameraZoom - CameraZoomSpeed); CenterCamera(); b_RMB_Event_Active = false; }

	if (GetKey(olc::Key::F).bPressed)
	{ b_RMB_Event_Active = false; CenterCamera(); /*mainCam->SetPosition({ 0.0f, 0.0f });*/ }

	if (!isZoomingCamera) // we are no longer zooming in as CenterCamera() was not trigger anywhere above
		saveCamPos = true;

	if (GetKey(olc::Key::R).bPressed)
	{ mainCam->SetZoom(1.0f); mainCam->SetPosition({ 0.0f, 0.0f });	b_RMB_Event_Active = false; }
	
	if (GetMouse(2).bHeld)
	{ Mouse_MoveCamera(); }

	if (GetMouse(2).bReleased)
	{
		saveCameraMoveOffset = true;
		CanDragCameraWithMouse = true;
	}
}

void PixelGameEditor::DrawGrid()
{
	PixelMath::Vec2D CellPosition;
	float NodeScale = grid2D->GetNodeSize() * mainCam->GetZoom();
	PixelMath::Vec2D vGridSize = grid2D->GridTotalCellCount;
	float DEBUG = 0;

	////////////////////////// COULUMNS //////////////////////////
	for (int column = 0; column < vGridSize.Y; column++)
	{
		CellPosition = grid2D->Grid2D[column][0].GetPosition();
		float x = (CellPosition.X - mainCam->GetPosition().X) * mainCam->GetZoom();
		float y = (CellPosition.Y - mainCam->GetPosition().Y) * mainCam->GetZoom();

		float xDrawDistance = x + (vGridSize.X * NodeScale);
		if (xDrawDistance > ScreenWidth())
			xDrawDistance = ScreenWidth();

		if (x < DEBUG) x = DEBUG;

		if (y < ScreenHeight() - DEBUG)
			DrawLine(x, y, xDrawDistance - DEBUG, y, olc::GREY);

		else
			break;

		// Draw last line
		if (column + 1 == vGridSize.Y)
			DrawLine(x, y + NodeScale, xDrawDistance, y + NodeScale, olc::GREY);
	}

	////////////////////////// ROWS //////////////////////////
	for (int row = 0; row < vGridSize.X; row++)
	{
		CellPosition = grid2D->Grid2D[0][row].GetPosition();
		float x = (CellPosition.X - mainCam->GetPosition().X) * mainCam->GetZoom();
		float y = (CellPosition.Y - mainCam->GetPosition().Y) * mainCam->GetZoom();

		float yDrawDistance = y + (vGridSize.Y * NodeScale);
		if (yDrawDistance > ScreenHeight()) yDrawDistance = ScreenHeight();

		if (y < DEBUG) y = DEBUG;

		if (x < ScreenWidth() - DEBUG)
			DrawLine(x, y, x, yDrawDistance - DEBUG, olc::GREY);

		else
			break;

		// Draw last line
		if (row + 1 == vGridSize.X)
			DrawLine(x + NodeScale, y, x + NodeScale, yDrawDistance, olc::GREY);
	}
}

void PixelGameEditor::DrawGameObjects(float &fElapsedTime)
{
	for (auto i : world->GetGameObjectsSpawnedInTheWorld())
	{
		i->Tick(fElapsedTime);
	}
}

void PixelGameEditor::InitGameObjects()
{
	for (auto i : world->GetGameObjectsSpawnedInTheWorld())
		i->Init();
}

void PixelGameEditor::TestGrid(float PosX, float PosY, bool DrawNeigbours)
{
	Grid::Node* node = grid2D->GetNodeAt({ PosX, PosY });

	if (DrawNeigbours)
	{
		if (node->GetNeighbours().Bottom) OutlineGridNode(node->GetNeighbours().Bottom, olc::GREEN);
		if (node->GetNeighbours().BottomLeft) OutlineGridNode(node->GetNeighbours().BottomLeft, olc::GREEN);
		if (node->GetNeighbours().BottomRight) OutlineGridNode(node->GetNeighbours().BottomRight, olc::GREEN);
		if (node->GetNeighbours().Left) OutlineGridNode(node->GetNeighbours().Left, olc::GREEN);
		if (node->GetNeighbours().Right) OutlineGridNode(node->GetNeighbours().Right, olc::GREEN);
		if (node->GetNeighbours().Top) OutlineGridNode(node->GetNeighbours().Top, olc::GREEN);
		if (node->GetNeighbours().TopLeft) OutlineGridNode(node->GetNeighbours().TopLeft, olc::GREEN);
		if (node->GetNeighbours().TopRight) OutlineGridNode(node->GetNeighbours().TopRight, olc::GREEN);
	}

	OutlineGridNode(node, { 82, 229, 195 }, false);

	if (node->isOccupied())
		FillRect((int)node->GetPositionOffset().X, (int)node->GetPositionOffset().Y, 50 * mainCam->GetZoom(), 50 * mainCam->GetZoom(), olc::RED);

}

void PixelGameEditor::OutlineGridNode(Grid::Node * node, olc::Pixel color, bool DrawADot)
{
	float x = (node->GetPosition().X - mainCam->GetPosition().X) * mainCam->GetZoom();
	float y = (node->GetPosition().Y - mainCam->GetPosition().Y) * mainCam->GetZoom();
	float size = grid2D->GetNodeSize() * mainCam->GetZoom();
	float Cx = (node->GetPosition().X + (grid2D->GetNodeSize() / 2) - mainCam->GetPosition().X) * mainCam->GetZoom();
	float Cy = (node->GetPosition().Y + (grid2D->GetNodeSize() / 2) - mainCam->GetPosition().Y) * mainCam->GetZoom();

	DrawRect(x, y, size, size, color);

	if (DrawADot)
		FillCircle(Cx, Cy, size / 20, olc::YELLOW);
}

void PixelGameEditor::MouseDragFunctionality()
{
	if (GetMouse(0).bHeld && CanDrag && !world->isMouseDragDisabled())
	{
		if (!isObjectDraggedByMouse)
		{
			for (auto obj : world->GetGameObjectsSpawnedInTheWorld())
			{
				PixelMath::Vec2D loc = obj->GetColliderPosition();
				PixelMath::Vec2D size = obj->GetSize();
				if (mouseX > loc.X && mouseX < loc.X + size.X && mouseY > loc.Y && mouseY < loc.Y + size.Y && obj->bAllowDragging)
				{
					isObjectDraggedByMouse = true;
					ObjectDraggedByMouse = obj;

					GrabOffset.X = mouseX - obj->GetPosition().X;
					GrabOffset.Y = mouseY - obj->GetPosition().Y;

					//Object selection
					DeselectObject();
					SelectedGameObject   = obj;
					SelectedGameObjectTint = SelectedGameObject->GetTint();
					obj->bIsSelected = true;
					obj->onSelected();
					break;
				}
			}

			//If none of the objects were in mouse range when LMB was clicked don't allow
			//mouse dragging to avoid the 'hoover' effect
			if (!isObjectDraggedByMouse)
				CanDrag = false;
		}

		else
		{			
			ObjectDraggedByMouse->SetPosition(mouseX - GrabOffset.X, mouseY - GrabOffset.Y);
			ObjectDraggedByMouse->onDragged();
		}
	}

	else
	{
		isObjectDraggedByMouse = false;
		ObjectDraggedByMouse = nullptr;

		if (GetMouse(0).bReleased)
			CanDrag = true;
	}

	//De-select an object
	if (GetMouse(0).bPressed && SelectedGameObject && !isObjectDraggedByMouse && !rotator.isSelected())
	{
		DeselectObject();
	}
}

void PixelGameEditor::ObjectSelectionFunctionality()
{
	if(SelectedGameObject)
	{
		SelectedGameObject->SetTint({200, 152, 152, 255});

		
		//Rotator
		if(bRotatorToolVisible && SelectedGameObject->bAllowEditorRotation)
			ObjectRotatorFunctionality();
	}
}

void PixelGameEditor::DeselectObject()
{
	// If an object was selected set the tint back to white
	if(SelectedGameObject)
	{
		SelectedGameObject->SetTint(SelectedGameObjectTint);
		SelectedGameObject->bIsSelected = false;
		SelectedGameObject->onDeselected();
	}

	SelectedGameObjectTint = olc::WHITE;
	SelectedGameObject = nullptr;
}

void PixelGameEditor::ObjectRotatorFunctionality()
{

	float scaleX = (rotationSprite->width * mainCam->GetZoom()) / rotationSprite->width;
	float scaleY = (rotationSprite->height * mainCam->GetZoom()) / rotationSprite->height;

	float sizeX = rotationSprite->width;
	float sizeY = rotationSprite->height;

	rotator.offset.X = SelectedGameObject->GetPosition().X + (SelectedGameObject->GetSize().X / 2) + 50;
	rotator.offset.Y = SelectedGameObject->GetPosition().Y - (sizeY / 2);

	rotator.r_mouseX = mouseX;
	rotator.r_mouseY = mouseY;


	float x = (rotator.offset.X - mainCam->GetPosition().X) * mainCam->GetZoom();
	float y = (rotator.offset.Y - mainCam->GetPosition().Y) * mainCam->GetZoom();

	rotator.pos = { x,  y };
	rotator.size = { sizeX, sizeY };

	rotator.middlePointY = SelectedGameObject->GetPosition().Y;

	DrawDecal({ x, y }, rotationDecal, { scaleX , scaleY });

	//////////////DEBUG//////////////
	//std::string text = "x = " + std::to_string(rotator.pos.X) + " y = " + std::to_string(rotator.pos.Y) +
	//	" Width = " + std::to_string(scaleX) + " Height = " + std::to_string(scaleY) +
	//	" || MouseX = " + std::to_string(rotator.r_mouseX) + " MouseY = " + std::to_string(rotator.r_mouseY) +
	//	" || Hovered Over? = " + std::to_string(rotator.isSelected()) +
	//	"Offset X = " + std::to_string(rotator.offset.X) + " Offset Y = " + std::to_string(rotator.offset.Y);

	//DrawString(10, ScreenHeight() - 10, text, olc::YELLOW);
	////////////////////////////

	// User Rotate
	if (GetMouse(0).bHeld && rotator.isSelected())
	{
		if(mouseY > rotator.middlePointY)
		{
			SelectedGameObject->SetRotation(SelectedGameObject->GetRotation() + rotationSpeed);
		}
		else
			SelectedGameObject->SetRotation(SelectedGameObject->GetRotation() - rotationSpeed);
	}
}

void PixelGameEditor::RMB_Events()
{
	if (GetMouse(1).bPressed)
	{
		b_RMB_Event_Active = false;
		
		for (auto obj : world->GetGameObjectsSpawnedInTheWorld())
		{		
			PixelMath::Vec2D loc = obj->GetColliderPosition();
			PixelMath::Vec2D size = obj->GetSize();
			if (mouseX > loc.X && mouseX < loc.X + size.X && mouseY > loc.Y && mouseY < loc.Y + size.Y && obj->bAllowRMBEvents)
			{
				//All conditions satisfied, allow RMB event.
				b_RMB_Event_Active = true;
				DeselectObject();
				RMB_Event_GO = obj;
				float offset = obj->GetWorldTransform().OffsetSize.X * obj->GetSprite()->width;
				float pos = obj->GetWorldTransform().PositionOffset.X;
				if (pos > (float)ScreenWidth() - 200.0f) offset = -(offset * 2.0);
				PixelMath::Vec2D mousePos = { pos + offset, (float)GetMouseY() };
				RMB_Event_Window->SetWindowPosition(mousePos);
				RMB_Event_Window->Clear();

				SetRMBButton( RMB_Event_Window->AddButton(this, &PixelGameEditor::RMB_Event_Button_ToggleCollision, { 0, 0 }, { 150, 20 }, "Draw Collision", { 23, 24, 25 }) );
				SetRMBButton( RMB_Event_Window->AddButton(this, &PixelGameEditor::RMB_Event_Button_Reset, { 0, -9.0f }, { 150, 20 }, "Reset", { 23, 24, 25 }) );
				SetRMBButton(RMB_Event_Window->AddButton(this, &PixelGameEditor::RMB_Event_Button_DestroyObject, { 0, -9.0f }, { 150, 20 }, "Delete", { 23, 24, 25 }));

				break;
			}
		}
		

	}

	if(b_RMB_Event_Active)
	{
		RMB_Event_Window->Update();
	}

	if (GetMouse(0).bPressed && b_RMB_Event_Active)
	{
		b_RMB_Event_Active = false;
		RMB_Event_GO = nullptr;
	}
 }

void PixelGameEditor::SetRMBButton(PixelWindow::Button<PixelGameEditor>* button)
{
	button->SetAnchor(PixelWindow::AnchorOptions::TopLeft);
	button->SetTextColor(olc::WHITE);
	button->SetHoveredOverColor({ 204, 0, 78 });
}

void PixelGameEditor::SetPixelWindows()
{
	PixelWindow::Button<PixelGameEditor>* TempButton;

	//...........RMB Event Window...........//
	RMB_Event_Window = new PixelWindow({ 0.0f, 0.0f }, { 150.0f, 100.0f }, { 23, 24, 25 }, "RMB", false);
	RMB_Event_Window->DisableDragging(true);
	RMB_Event_Window->CanBeResized(false);
	//########### RMB Event Window END ###########//

	//...........Object Inspector...........//
	TempButton = ObjectInspectorWindow->AddButton<PixelGameEditor>(this, &PixelGameEditor::Button_CloseWindow1, { 0, 0 }, { 10, 10 }, "X", { 65,65,65 });
	TempButton->SetTextColor({ 252, 211, 0, 255 });
	TempButton->SetAnchor(PixelWindow::AnchorOptions::TopRight);
	TempButton->IgnoreScrolling(true);

	TempButton = ObjectInspectorWindow->AddButton<PixelGameEditor>(this, &PixelGameEditor::Button_ResetObjectInspectorWindow_OnClicked, { 0, 0 }, { 40, 20 }, "Reset", { 51, 51, 51, 255 });
	TempButton->SetTextColor({ 252, 211, 0, 255 });
	TempButton->IgnoreScrolling(true);
	TempButton->SetAnchor(PixelWindow::AnchorOptions::BottomLeft);

	CameraDebugText = ObjectInspectorWindow->AddText({ 5.0f, 0.0f }, "CAMERA_DEBUG");
	CameraDebugText->SetAnchor(PixelWindow::AnchorOptions::TopLeft);

	PixelWindow::Slider* slider = ObjectInspectorWindow->AddSlider({ 10.0f, 75.0f }, { 2.0f, 1.0f }, rotationSpeed, 0.003f, 0.3f);
	slider->SetAnchor(PixelWindow::AnchorOptions::TopLeft);

	ObjectInspectorWindow->AddSeparator(0.0f, olc::VERY_DARK_GREY)->SetAnchor(PixelWindow::AnchorOptions::TopLeft);

	ObjectInspectorImage = ObjectInspectorWindow->AddImage({ 20.0f, 0.0f }, { 64.0f, 64.0f }, new olc::Sprite("..//PixelEditor//Sprites//GameObject.png"));
	ObjectInspectorImage->SetAnchor(PixelWindow::AnchorOptions::TopLeft);
	
	ObjectDebugText = ObjectInspectorWindow->AddText({ 5.0f, 5.0f }, "OBJECT_DEBUG");;
	ObjectDebugText->SetAnchor(PixelWindow::AnchorOptions::TopLeft);


	
	ObjectInspectorWindow->AddSeparator(35.0f, olc::VERY_DARK_GREY)->SetAnchor(PixelWindow::AnchorOptions::TopLeft);
	ObjectInspectorWindow->AddText({ 5.0f, 300.0f }, "TEST");;
	PixelWindow::Text* temp = ObjectInspectorWindow->AddText({ 5.0f, 500.0f }, "TEST2");;
	//########### Object inspector END ###########//

	//...........Simulate Button...........//
	SimulateWindow = new PixelWindow({ (float)ScreenWidth() / 2.f, 6 }, { 160, 40 });
	TempButton = SimulateWindow->AddButton<PixelGameEditor>(this, &PixelGameEditor::Button_Simulate_OnClicked, { 0, 0 }, { 160, 40 }, "SIMULATE", { 23, 24, 25 , 255 });
	TempButton->SetTextScale(2.0f);
	TempButton->SetTextColor({ 77, 234, 205, 255 });
	TempButton->SetHoveredOverColor({ 11, 11, 11 });
	TempButton->IgnoreScrolling(true);
	SimulateWindow->DisableDragging(true);
	SimulateWindow->CanBeResized(false);
	//########### Simulate Button END ###########//

	//...........Objects Spawned Window...........//
	WorldWindow = new PixelWindow({ (float)ScreenWidth() - 200.0f, 0.0f }, { 200.0f, 100.0f }, { 23, 24, 25 }, "Game Objects");
	WorldWindow->SetTitleBarColor({ 204, 0, 78 });
	//###########Object Spawned Window END###########//

	//...........Editable Block Window...........//
	EditableBlockWindow = new PixelWindow({ 200.0f, 500.0f }, { 210.0f, 100.0f }, { 23, 24, 25 }, "Editable Block's Options");
	EditableBlockWindow->Hide();
	//########### Editable Block END ###########//

	TempButton = nullptr;
}

void PixelGameEditor::UpdateDebugText()
{
	std::string tempText;
	int rSpeed = floor(rotationSpeed * 1000.0f);

	tempText = "Camera:\n\nx(" + std::to_string(mainCam->GetPosition().X) + ")\n\ny(" + std::to_string(mainCam->GetPosition().Y) +
		")\n\nZoom: " + std::to_string(mainCam->GetZoom()) +
		"\n\nObject Rotation Speed: " + std::to_string(rSpeed);
	CameraDebugText->SetText(tempText);


	if(SelectedGameObject)
	{
	//...........Game Objects Window...........//
		ObjectInspectorImage->SetImage(SelectedGameObject->GetDecal());

		int posX = floor(SelectedGameObject->GetPosition().X);
		int posY = floor(SelectedGameObject->GetPosition().Y);
		int Width  = floor(SelectedGameObject->GetSize().X);
		int Height = floor(SelectedGameObject->GetSize().Y);

		tempText = "Name: " + SelectedGameObject->GetName() +
			"\n\nPosition: X(" + std::to_string(posX) +
			") Y(" + std::to_string(posY) + ")" +
			"\n\nSize: Width(" + std::to_string(Width) +
			") Height(" + std::to_string(Height) + ")" +
			"\n\nRotation: " + std::to_string(SelectedGameObject->GetRotation());
		ObjectDebugText->SetText(tempText);

	//########### Game Objects Window END ###########//

	//...........Editable Block Window...........//
	if (SelectedGameObject->GetTag() == "EditableBlock")
	{		
		EditableBlock* selectedBlock = dynamic_cast<EditableBlock*>(SelectedGameObject->GetParentObject());
		if(selectedBlock)
		{
			EditableBlockWindow->Show();
			EditableBlockWindow->Clear();
			EditableBlockWindow->AddText({ 5.0f, 20.0f }, "Cut All Blocks On Axis: ");
			EditableBlockWindow->AddTickBox(&selectedBlock->b_EB_AllowMirrorCut, EditableBlockWindow, { 190.0f, 18.0f });
		}
	}
	else
		EditableBlockWindow->Hide();
	//########### Editable Block END ###########//
	}

	WorldWindow->Clear();
	
	int pos = 1;
	for (auto i : World::Get()->GetGameObjectsSpawnedInTheWorld())
	{
		int id = i->GetID();
		tempText = std::to_string(pos) + ". " + i->GetName() + " (Id:" + std::to_string(id) + ")";
	
		WorldWindow->AddText({ 0.0f, 0.0f }, tempText)->SetAnchor(PixelWindow::AnchorOptions::TopLeft);	
	
		pos++;
	}

	//engine->DrawString(0, 500, "World ID: " + std::to_string(world->debug_retunNextAvailableID()), olc::RED, 2);
	//engine->DrawString(0, 550, "WorldWindow ID: " + std::to_string(WorldWindow->NextAvailableID), olc::RED, 2);
	//engine->DrawString(0, 600, "Spawned object Count: " + std::to_string(World::Get()->GetGameObjectsSpawnedInTheWorld().size()),  olc::RED, 2);
}
