#include "PixelWindow.h"

PixelWindow::PixelWindow(PixelMath::Vec2D position, PixelMath::Vec2D size, olc::Pixel color, std::string WindowName, bool TitleBar, olc::Pixel TitleBarColor)
	: pw_position(position), pw_size(size), pw_back_color(color), windowName(WindowName), bTitleBar(TitleBar), pw_title_bar_color(TitleBarColor)
{
	pw_initial_size = size;

	if(TitleBar)
		AddTitleBar();

	sb_ScrollBar = new ScrollBar(&pw_position, &pw_size, &fGlobalOffset, &lastObjectOnY, &CanDrag, &fGlobalBarValue);
	//AddChild(test);
}

PixelWindow::~PixelWindow()
{
}

void PixelWindow::MouseDrag()
{
	if (engine->GetMouse(0).bHeld && CanDrag)
	{
		int mouseX = engine->GetMouseX();
		int mouseY = engine->GetMouseY();

		if (!isObjectDraggedByMouse)
		{

			if (mouseX > pw_position.X && mouseX < pw_position.X + pw_size.X &&
				mouseY > pw_position.Y && mouseY < pw_position.Y + pw_size.Y)
			{
				isObjectDraggedByMouse = true;
				world->setMouseDragDisabled(true);
				offset.X = mouseX - pw_position.X;
				offset.Y = mouseY - pw_position.Y;
			}


			//If none of the objects were in mouse range when LMB was clicked don't allow
			//mouse dragging to avoid the 'hoover/magnet effect'
			if (!isObjectDraggedByMouse)
			{
				CanDrag = false;
				world->setMouseDragDisabled(false);
			}
		}

		else
		{
			SetWindowPosition(mouseX - offset.X, mouseY - offset.Y);
		}
	}

	else
	{
		isObjectDraggedByMouse = false;

		if (engine->GetMouse(0).bReleased)
		{
			CanDrag = true;
			offset.X = 0;
			offset.Y = 0;
		}
	}

}

void PixelWindow::Resize()
{

	olc::Pixel resizeIconColor;
	PixelMath::Vec2D posL;
	posL.X = pw_position.X + pw_size.X - 11;
	posL.Y = pw_position.Y + pw_size.Y - 11;

	PixelMath::Vec2D posR;
	posR.X = pw_position.X + pw_size.X - 1;
	posR.Y = pw_position.Y + pw_size.Y - 1;

	//engine->DrawRect(posL.X, posL.Y, posR.X - posL.X, posR.Y - posL.Y, olc::RED);

	float mouseX = engine->GetMouseX();
	float mouseY = engine->GetMouseY();

	isBeingResized ? resizeIconColor = { 61, 255, 248, 255 } : resizeIconColor = olc::WHITE;

	// Draw triangle icon
	engine->FillTriangle(pw_position.X + pw_size.X - 1, pw_position.Y + pw_size.Y - 1,
		pw_position.X + pw_size.X - 1, pw_position.Y - 10 + pw_size.Y - 1,
		pw_position.X - 10 + pw_size.X - 1, pw_position.Y + pw_size.Y - 1, resizeIconColor);

	if(engine->GetMouse(0).bHeld && !isLMB_Pressed)
	{
		// Removes the magnet effect where LMB is held before hovering over the resize area
		isLMB_Pressed = true;

		if (mouseX > posL.X && mouseX < posR.X && mouseY > posL.Y && mouseY < posR.Y)
		{
			if (!isBeingResized)
				MouseInitialResizePos = { mouseX , mouseY };

			isBeingResized = true;
			CanDrag = false;
			isLMB_Pressed = false;
			fGlobalOffset = 0.0f; // Reset position of global offset
			fGlobalBarValue = 0.0f; // Reset bar value
		}
		
		if(isBeingResized)
		{
			if(pw_size.X > pw_initial_size.X - 1.0f && pw_size.Y > pw_initial_size.Y - 1.0f)
			{
				pw_size.X += mouseX - MouseInitialResizePos.X;
				pw_size.Y += mouseY - MouseInitialResizePos.Y;
				MouseInitialResizePos = { mouseX , mouseY };
				isLMB_Pressed = false;
			}
			
			else if(pw_size.X < pw_initial_size.X + 1.0f)
			{
				pw_size.X = pw_initial_size.X + 1.0f;
				isBeingResized = false;
			}

			else
			{
				pw_size.Y = pw_initial_size.Y + 1.0f;
				isBeingResized = false;
			}
				
		}

		else
			isBeingResized = false;
	}

	else
	{
		isBeingResized = false;
		//CanDrag = true;
	}

	if (engine->GetMouse(0).bReleased)
	{
		isLMB_Pressed = false;
		CanDrag = true;
	}
}

void PixelWindow::Update()
{
	if(bVisible)
	{
		engine->FillRect(pw_position.X, pw_position.Y, pw_size.X, pw_size.Y, pw_back_color);

		// Draw triangle icon
		//engine->FillTriangle(pw_position.X + pw_size.X - 1, pw_position.Y + pw_size.Y - 1,
		//	pw_position.X + pw_size.X - 1, pw_position.Y - 10 + pw_size.Y - 1,
		//	pw_position.X - 10 + pw_size.X - 1, pw_position.Y + pw_size.Y - 1, olc::WHITE);
		
		NeedsAScrollBar = false;
		//lastObjectOnY = 0.0f;
		//lastObjectOnYSize = 0.0f;
		for (PWChild* i : children)
		{
			AnchorChild(i);
			i->PreUpdate();

			// Only draw if in bound of the window
			if (i->position.X + i->size.X < pw_position.X + pw_size.X + 1.0f && i->position.Y + i->size.Y < pw_position.Y + pw_size.Y + 1.0f && i->position.Y >= pw_position.Y)
				i->Update();

			//Update info about the lowest object so the scroll bar knows how far it can scroll
			else
			{
				NeedsAScrollBar = true;
				
				float GO_Positive = fGlobalOffset * -1.0f;

				if(lastObjectOnY < i->GetPosition().Y && i->GetPosition().Y > pw_position.Y)
				{
					lastObjectOnY = ((i->GetPosition().Y + i->GetSize().Y) - (pw_position.Y + pw_size.Y)) + GO_Positive;
					//lastObjectOnYSize = i->GetSize().Y;
					//lastObjectOnY = 150.0f;
				}
			}
		}


		if(NeedsAScrollBar || fGlobalOffset < 0.0f || ForceAScrollBar) sb_ScrollBar->Update();
		ResetAnchorValues();
		if(bCanResize) Resize();
		if (!LockDragging) MouseDrag();

	}
}

void PixelWindow::AddChild(PWChild * child)
{
	children.emplace_back(child);
	NextAvailableID++;
}

void PixelWindow::AddTitleBar()
{
	//bTitleBar = true;
	titleBarBackground = AddSeparator(0.0f, pw_title_bar_color);
	titleBarBackground->size.Y = 10.0f;
	titleBarBackground->IgnoreScrolling(true);
	AddText({ 2.0f, 0.0f }, windowName)->IgnoreScrolling(true);
}

//void PixelWindow::CollapseAndExpandWindow()
//{
//	if(bIsCollapsed)
//	{
//		bIsCollapsed = false;
//	}
//
//	else
//	{
//		bIsCollapsed = true;
//	}
//}

PixelWindow::TickBox * PixelWindow::AddTickBox(bool * bValue, PixelWindow * ParentWindow, PixelMath::Vec2D v2d_position)
{
	PixelWindow::TickBox* tickBoxTemp = new TickBox(bValue, ParentWindow, v2d_position);
	AddChild(tickBoxTemp);
	return tickBoxTemp;
}

PixelWindow::Separator* PixelWindow::AddSeparator(float offsetY, olc::Pixel color)
{
	PixelWindow::Separator* sepTemp = new Separator(&pw_position, &pw_size, offsetY, 2.0f, color);
	sepTemp->child_offset = sepTemp->position - pw_position;
	AddChild(sepTemp);
	return sepTemp;
}

PixelWindow::Image * PixelWindow::AddImage(PixelMath::Vec2D imagePosition, PixelMath::Vec2D imageSize, olc::Sprite * Image)
{
	PixelWindow::Image* tempImage = new PixelWindow::Image(pw_position + imagePosition, imagePosition, imageSize, Image);
	AddChild(tempImage);
	return tempImage;
}

PixelWindow::Text * PixelWindow::AddText(PixelMath::Vec2D textPosition, std::string text, olc::Pixel textColor)
{
	PixelWindow::Text* textTemp = new Text(pw_position + textPosition, textPosition, text, textColor);
	AddChild(textTemp);
	return textTemp;
}

PixelWindow::Slider * PixelWindow::AddSlider(PixelMath::Vec2D position, PixelMath::Vec2D scale, float & value, float min, float max)
{
	Slider* tempSlider = new Slider(position, position - pw_position, scale, value, min, max);
	tempSlider->LMB_Pressed = &isLMB_Pressed;
	tempSlider->allowWindowDrag = &CanDrag;
	tempSlider->isBeingResized = &isBeingResized;
	AddChild(tempSlider);
	return tempSlider;
}

void PixelWindow::SetWindowPosition(int x, int y)
{
	pw_position = { (float)x, (float)y };

	for (PWChild* i : children)
		i->SetPosition(pw_position + i->GetOffset());
}

void PixelWindow::SetWindowPosition(PixelMath::Vec2D position)
{
	pw_position = position;

	for (PWChild* i : children)
		i->SetPosition(pw_position + i->GetOffset());
}

void PixelWindow::AnchorChild(PWChild * child)
{
	float offset = 0.0f;
	float globalOffset = fGlobalOffset;

	if (child->bIgnoreScrolling) globalOffset = 0.0f;

	switch (child->anchor)
	{
	case PixelWindow::TopLeft:
		child->position.X = pw_position.X;
		child->position.Y = pw_position.Y + availableAnchorOffsets.TopLeft;
		child->position.X += child->child_offset.X;
		child->position.Y += child->child_offset.Y + globalOffset;

		offset = child->child_offset.Y;
		availableAnchorOffsets.TopLeft += child->size.Y + anchorOffset + offset;
		break;

	case PixelWindow::TopCenter:
		child->position.X = pw_position.X + (pw_size.X / 2) - (child->size.X / 2);
		child->position.Y = pw_position.Y + availableAnchorOffsets.TopCenter;
		child->position.X += child->child_offset.X;
		child->position.Y += child->child_offset.Y + globalOffset;

		//child->position.Y += availableAnchorOffsets.TopCenter;
		offset = child->child_offset.Y;
		availableAnchorOffsets.TopCenter += child->size.Y + anchorOffset + offset;
		break;

	case PixelWindow::TopRight:
		child->position.X = pw_position.X + pw_size.X - child->size.X;
		child->position.Y = pw_position.Y + availableAnchorOffsets.TopRight;
		child->position.X += child->child_offset.X;
		child->position.Y += child->child_offset.Y + globalOffset;

		//child->position.Y += availableAnchorOffsets.TopRight;
		offset = child->child_offset.Y;
		availableAnchorOffsets.TopRight += child->size.Y + anchorOffset + offset;
		break;

	case PixelWindow::MiddleLeft:
		child->position.X = pw_position.X;
		child->position.Y = pw_position.Y + (pw_size.Y / 2) - (child->size.Y / 2);
		child->position.X += child->child_offset.X;
		child->position.Y += child->child_offset.Y + globalOffset;

		//child->position.Y += availableAnchorOffsets.MiddleLeft;
		offset = child->child_offset.Y;
		availableAnchorOffsets.MiddleLeft += child->size.Y + anchorOffset + offset;
		break;

	case PixelWindow::Center:
		child->position.X = pw_position.X + (pw_size.X / 2) - (child->size.X / 2);
		child->position.Y = pw_position.Y + (pw_size.Y / 2) - (child->size.Y / 2);
		child->position.X += child->child_offset.X;
		child->position.Y += child->child_offset.Y + globalOffset;

		//child->position.Y += availableAnchorOffsets.Center;
		offset = child->child_offset.Y;
		availableAnchorOffsets.Center += child->size.Y + anchorOffset + offset;
		//availableAnchorOffsets.Center += child->size.Y + anchorOffset + child->child_offset.Y;
		break;

	case PixelWindow::MiddleRight:
		child->position.X = pw_position.X + pw_size.X - child->size.X;
		child->position.Y = pw_position.Y + (pw_size.Y / 2) - (child->size.Y / 2);
		child->position.X += child->child_offset.X;
		child->position.Y += child->child_offset.Y + globalOffset;

		//child->position.Y += availableAnchorOffsets.MiddleRight;
		offset = child->child_offset.Y;
		availableAnchorOffsets.MiddleRight += child->size.Y + anchorOffset + offset;
		break;

	case PixelWindow::BottomLeft:
		child->position.X = pw_position.X;
		child->position.Y = pw_position.Y + pw_size.Y - (child->size.Y);
		child->position.X += child->child_offset.X;
		child->position.Y += child->child_offset.Y + globalOffset;

		//child->position.Y += availableAnchorOffsets.BottomLeft;
		offset = child->child_offset.Y;
		availableAnchorOffsets.BottomLeft += child->size.Y + anchorOffset + offset;
		break;

	case PixelWindow::BottomCenter:
		child->position.X = pw_position.X + (pw_size.X / 2) - (child->size.X / 2);
		child->position.Y = pw_position.Y + pw_size.Y - (child->size.Y);
		child->position.X += child->child_offset.X;
		child->position.Y += child->child_offset.Y + globalOffset;

		//child->position.Y += availableAnchorOffsets.BottomCenter;
		offset = child->child_offset.Y;
		availableAnchorOffsets.BottomCenter += child->size.Y + anchorOffset + offset;
		break;

	case PixelWindow::BottomRight:
		child->position.X = pw_position.X + pw_size.X - child->size.X;
		child->position.Y = pw_position.Y + pw_size.Y - (child->size.Y);
		child->position.X += child->child_offset.X;
		child->position.Y += child->child_offset.Y + globalOffset;

		//child->position.Y += availableAnchorOffsets.BottomRight;
		offset = child->child_offset.Y;
		availableAnchorOffsets.BottomRight += child->size.Y + anchorOffset + offset;
		break;
	case PixelWindow::None:	
		child->position = pw_position + child->child_offset;
		child->position.Y += globalOffset;
		break;
	default:
		break;
	}
}

void PixelWindow::ResetAnchorValues()
{
	float TitleBarOffset = 0.0f;
	if(bTitleBar) TitleBarOffset = 15.0f;

	availableAnchorOffsets.TopLeft = TitleBarOffset;
	availableAnchorOffsets.TopCenter = TitleBarOffset;
	availableAnchorOffsets.TopRight = 0.0f;
	availableAnchorOffsets.MiddleLeft = 0.0f;
	availableAnchorOffsets.Center = 0.0f;
	availableAnchorOffsets.MiddleRight = 0.0f;
	availableAnchorOffsets.BottomLeft = 0.0f;
	availableAnchorOffsets.BottomCenter = 0.0f;
	availableAnchorOffsets.BottomRight = 0.0f;
}

void PixelWindow::Clear()
{
	for(auto i : children)
	{
		delete i;
	}

	//for (int i = children.size() - 1; i > -1; i--)
	//{
	//	delete (children.at(i));
	//}

	children.clear();
	NextAvailableID = 0;
	ResetAnchorValues();

	if(bTitleBar)
		AddTitleBar();
}

void PixelWindow::Slider::SliderFunctionality()
{
	float mouseX = (float)engine->GetMouseX();
	float mouseY = (float)engine->GetMouseY();
	PixelMath::Vec2D posL = SliderPosition;
	PixelMath::Vec2D posR = { posL.X + 5.0f * size.X, posL .Y + 10.0f * size.Y };
	//PixelMath::Vec2D pw_size = { 5.0f, 10.0f };

	if (engine->GetMouse(0).bHeld)
	{
		// Removes the magnet effect where LMB is held before hovering over the resize area
		//*LMB_Pressed = true;
		float MAX = size.X * 100.0f;

		if (mouseX > posL.X && mouseX < posR.X && mouseY > posL.Y && mouseY < posR.Y && (!*LMB_Pressed))
		{
			if (!isBeingDragged)
				MouseInitialPos = { mouseX , mouseY };

			isBeingDragged = true;
			//CanDrag = false;
			*LMB_Pressed = true; // Let the resize function know we are using the LMB
			*allowWindowDrag = false;
		}

		if (isBeingDragged)
		{
			// Clamp slider between 0 and MAX
			if (SliderPosition.X >= position.X && SliderPosition.X <= position.X + MAX)
			{
				float mouseOffsetX = mouseX - SliderPosition.X;
				float mouseOffsetY = mouseY - SliderPosition.Y;
				//Makes sure the mouse pointer is not too far from the slider
				if (mouseOffsetX < MousePointerOutOfBoundsValue && mouseOffsetX > -MousePointerOutOfBoundsValue &&
					mouseOffsetY < MousePointerOutOfBoundsValue && mouseOffsetY > -MousePointerOutOfBoundsValue)
				{
					// Move the slider left
					if (mouseOffsetX < 0.0f && SliderX > 0.0f)
						SliderX -= -(1.0f * mouseOffsetX);

					// Move the slider right
					else if (mouseOffsetX > 0.0f && SliderX < MAX)
						SliderX += 1.0f * mouseOffsetX;
				}
				//Mouse pinter is too far. Stop dragging.
				else
					isBeingDragged = false;


				MouseInitialPos = { mouseX , mouseY };
				*LMB_Pressed = true; // Let the resize function know we are using the LMB
				*allowWindowDrag = false;
			}
			
			if (SliderX > MAX) SliderX = MAX;
			if (SliderX < 0.0f)   SliderX = 0.0f;

			float newMax = SliderMax - SliderMin;
			*SliderValue = (newMax * SliderX) / MAX;
			*SliderValue += SliderMin;
		}
	}

	if (engine->GetMouse(0).bReleased)
	{
		//*LMB_Pressed = false;
		*allowWindowDrag = true;
		isBeingDragged = false;
		//CanDrag = true;
	}
}

PixelWindow::TickBox::TickBox(bool *value, PixelWindow* parentWindow, PixelMath::Vec2D box_position)
{
	isTicked = value;
	myParentWindow = parentWindow;
	position = box_position;
	//child_offset = box_position;
	std::string temp;
	*value ? temp = "T" : temp = "F";
	myButton = parentWindow->AddButton<PixelWindow::TickBox>(this, &PixelWindow::TickBox::onClicked, position, { 10.0f, 10.0f }, temp, olc::WHITE);
	myButton->SetTextColor(olc::BLACK);
	myButton->SetHoveredOverColor({ 201, 255, 246 });
}
