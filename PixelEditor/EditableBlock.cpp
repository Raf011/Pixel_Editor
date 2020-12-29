#include "EditableBlock.h"
#include "World.h"
#include "PixelMath.h"
#include "Debug.h"

EditableBlock::EditableBlock(PixelMath::Vec2D position) : GameObject(position, { 32, 32 }, "..//PixelEditor//Sprites//Box.png", true)
{
	Name = "EditableBlock";
	//Tag = "EditableBlock";
	Block* tempBlock = AddNewBlock({ 32.0f, 0 }, { 32, 32 });
	tempBlock->SetTint(olc::CYAN);
	tempBlock->SetPosition(position);
	bAllowRMBEvents = false;
	bAllowEditorRotation = false;
	Visible = false;
	canBeDestroyed = false;
	// Disable collision
	SetCollisionEnabled(false);

	//bAllowDragging = false;
	//bAlwaysCenterThePosition = false;
}


EditableBlock::~EditableBlock()
{
}

void EditableBlock::drawDebugCells()
{
	if(selectedBlock)
	{
		PE_ASSERT(false); // test
		int offset = 10 * (int)(mainCam->GetZoom());
		int one = (int)(mainCam->GetZoom());
		int Sx = (int)selectedBlock->GetWorldTransform().PositionOffset.X - offset;
		Sx -= ((int)selectedBlock->GetWorldTransform().OffsetSize.X) / 2;
		int Sy = (int)selectedBlock->GetWorldTransform().PositionOffset.Y;
		Sy -= ((int)selectedBlock->GetWorldTransform().OffsetSize.Y) / 2;
		int Ex = (int)selectedBlock->GetWorldTransform().PositionOffset.X - one;
		Ex -= ((int)selectedBlock->GetWorldTransform().OffsetSize.X) / 2;
		int Ey = (int)selectedBlock->GetWorldTransform().PositionOffset.Y + (int)selectedBlock->GetWorldTransform().OffsetSize.Y;
		Ey -= ((int)selectedBlock->GetWorldTransform().OffsetSize.Y) / 2;
		engine->DrawRect(Sx, Sy, (Ex - Sx), (Ey - Sy));
	}
}

//void EditableBlock::OnBeginPlay()
//{
//}
//
//////
//void EditableBlock::Update(float & fElapsedTime)
//{
//
//}

void EditableBlock::OnEditorUpdate(float& fElapsedTime)
{
	if(bSeparatorActive)
		SplitLineFunctionality();

	if(engine->GetKey(olc::Key::T).bPressed)
		AddNewBlock({ 100.0f, 0 }, { 32, 32 });

	if (engine->GetKey(olc::Key::ENTER).bPressed && !blocksOptimized)
		optimizeBlocks();

	// prevent dragging and resizing at the same time.
	if(isBeingResized)
		GameWorld->setMouseDragDisabled(true);


	//////// DEBUG ONLY ////////
	if (selectedBlock && !isBeingResized && isBeingDragged)
	{
		UpdateLocations();
	}

	else
	{
		//engine->DrawString(800, 800, "None Selected");
		if (selectedBlock) selectedBlock_LastKnownLoc = selectedBlock->GetPosition();
	}



	DrawDebug(true);
	//drawDebugCells();

	////////////////////////////

	isBeingDragged = false;
}

EditableBlock::Block* EditableBlock::AddNewBlock(PixelMath::Vec2D vPos, PixelMath::Vec2D vS)
{
	Block* blockTemp = new Block(vPos, vS, this);
	blockTemp->Init();
	blockTemp->SetTint(olc::GetRandomColorInRange(255, 255, 255));
	blocks.emplace_back(blockTemp);
	blockTemp->Name = " >EB_Child_" + std::to_string(blocks.size());
	return blockTemp;
}

void EditableBlock::SplitAllBlocksWithinRegion(EditableBlock::e_CutDirection cut_axis, float splitPoint, Block* active_block)
{
	//copy list of blocks before adding new ones
	std::vector<Block*> availableBlocks = blocks;

	for(Block* block : availableBlocks)
		//if(block != active_block)
			switch (cut_axis)
			{
			case EditableBlock::X_Axis: 
				if (block->transform.PositionFromTopLeftCorner.X <= splitPoint && block->transform.PositionFromTopLeftCorner.X + block->GetSize().X >= splitPoint)
					CutOnXAxis(splitPoint, block);

			break;

			case EditableBlock::Y_Axis:
				if (block->transform.PositionFromTopLeftCorner.Y <= splitPoint && block->transform.PositionFromTopLeftCorner.Y + block->GetSize().Y >= splitPoint)
					CutOnYAxis(splitPoint, block);
				
			break;

			default: break;
			}

	//availableBlocks.erase(availableBlocks.begin(), availableBlocks.end());
}

void EditableBlock::SplitLineFunctionality()
{
	float mouseX = engine->GetMouseX() / mainCam->GetZoom() + mainCam->GetPosition().X;
	float mouseY = engine->GetMouseY() / mainCam->GetZoom() + mainCam->GetPosition().Y;
	int width;
	int height;
	PixelMath::Vec2D posL;
	PixelMath::Vec2D posR;
	PixelMath::Vec2D DrawPos;

	if (engine->GetMouse(1).bHeld)
	{
		for (Block* child : blocks)
		{
			PixelMath::Vec2D sizeHalfed = child->transform.Size / 2.0f;
			posL = child->transform.Position - sizeHalfed;
			posR = child->transform.Position + sizeHalfed;
			width  = child->transform.Size.X;
			height = child->transform.Size.Y;
			DrawPos = child->transform.PositionOffset - (child->transform.Size / 2.0f);

			if (mouseX > posL.X && mouseX < posR.X && mouseY > posL.Y && mouseY < posR.Y)
			{
				//engine->DrawLine(engine->GetMouseX(), DrawPos.Y, engine->GetMouseX(), DrawPos.Y + child->transform.OffsetSize.Y, olc::RED);
				activeBlock = child;
				GameWorld->setMouseDragDisabled(true);
				bCanCut = true;

				float scaleX = child->transform.Size.X + 10.0f;
				float scaleY = child->transform.Size.Y + 10.0f;

				switch (GetSplitLineDirection(mouseX, mouseY))
				{
					case EditableBlock::X_Axis: engine->DrawDecal({ (float)engine->GetMouseX(), DrawPos.Y - 10.0f }, GetDecal(), { 1.0f, scaleY + 10.0f }, olc::RED); break;
					case EditableBlock::Y_Axis: engine->DrawDecal({ DrawPos.X - 10.0f, (float)engine->GetMouseY() }, GetDecal(), { scaleX + 10.0f, 1.0f }, olc::RED);	break;
					default: break;
				}

				break;
			}

			else
				bCanCut = false;
		}
	}

	//Cut geometry
	if(engine->GetMouse(1).bReleased && bSeparatorActive && bCanCut)
	{
		selectedBlock = nullptr;

		// mirror cut
		if(b_EB_AllowMirrorCut)
		{
			switch (GetSplitLineDirection(mouseX, mouseY))
			{
				case EditableBlock::X_Axis: 
					SplitAllBlocksWithinRegion(X_Axis, mouseX, activeBlock); // cut all blocks on this line except for the active block
					//CutOnXAxis(mouseX, activeBlock); // cut the active block
					break;
				case EditableBlock::Y_Axis: 
					SplitAllBlocksWithinRegion(Y_Axis, mouseY, activeBlock); // cut all blocks on this line except for the active block
					//CutOnYAxis(mouseY, activeBlock); // cut the active block
					break;
				default: break;
			}
		}

		// cut only the selected block
		else
		{
			switch (GetSplitLineDirection(mouseX, mouseY))
			{
				case EditableBlock::X_Axis:
					CutOnXAxis(mouseX, activeBlock); // cut the active block
					break;
				case EditableBlock::Y_Axis:
					CutOnYAxis(mouseY, activeBlock); // cut the active block
					break;
				default: break;
			}
		}

		activeBlock = nullptr;
	}

}

void EditableBlock::CutOnXAxis(float &f_mouseX, Block* target_block)
{
	GameWorld->setMouseDragDisabled(true);
	bCanCut = false;
	float cutAtX = f_mouseX;
	float OldBlock_PositionXFromLeftCorner = (target_block->GetPosition().X - (target_block->GetSize().X / 2.0f));
	float oldBlockWidth = cutAtX - OldBlock_PositionXFromLeftCorner;
	float newBlockWidth = target_block->GetSize().X - oldBlockWidth;

	//New block's position and size 
	PixelMath::Vec2D newBlockPos = { cutAtX + newBlockWidth / 2.0f, target_block->GetPosition().Y };
	PixelMath::Vec2D newBlockSize = { newBlockWidth, target_block->GetSize().Y };

	//Old block's position and size 
	PixelMath::Vec2D oldBlockPos = { OldBlock_PositionXFromLeftCorner + (oldBlockWidth / 2.0f), target_block->GetPosition().Y };
	PixelMath::Vec2D oldBlockSize = { oldBlockWidth, target_block->GetSize().Y };

	target_block->SetSize(oldBlockSize);
	target_block->SetPosition(oldBlockPos);

	Block* tempNewBlock = AddNewBlock(newBlockPos, newBlockSize);

	//////////////// Set neighbors ////////////////
	tempNewBlock->AddNeigbour(target_block, e_FacingDirection::Left);

	//if already has a neighbor on the right transfer it to the new block
	tempNewBlock->AddNeigbour(target_block->myNeighbors.Right, e_FacingDirection::Right);
	tempNewBlock->AddNeigbour(target_block->myNeighbors.Up, e_FacingDirection::Up);
	tempNewBlock->AddNeigbour(target_block->myNeighbors.Down, e_FacingDirection::Down);

	target_block->AddNeigbour(tempNewBlock, e_FacingDirection::Right);
	//////////////////////////////////////////////

	target_block->RefreshHandles();

	return;
}

void EditableBlock::CutOnYAxis(float &f_mouseY, Block* target_block)
{
	GameWorld->setMouseDragDisabled(true);
	bCanCut = false;
	float cutAtY = f_mouseY;
	float OldBlock_PositionYFromLeftCorner = (target_block->GetPosition().Y - (target_block->GetSize().Y / 2.0f));
	float oldBlockHeight = cutAtY - OldBlock_PositionYFromLeftCorner;
	float newBlockHeight = target_block->GetSize().Y - oldBlockHeight;

	//New block's position and size 
	PixelMath::Vec2D newBlockPos = { target_block->GetPosition().X, cutAtY + newBlockHeight / 2.0f };
	PixelMath::Vec2D newBlockSize = { target_block->GetSize().X, newBlockHeight };

	//Old block's position and size 
	PixelMath::Vec2D oldBlockPos = { target_block->GetPosition().X, OldBlock_PositionYFromLeftCorner + (oldBlockHeight / 2.0f) };
	PixelMath::Vec2D oldBlockSize = { target_block->GetSize().X, oldBlockHeight };

	target_block->SetSize(oldBlockSize);
	target_block->SetPosition(oldBlockPos);

	Block* tempNewBlock = AddNewBlock(newBlockPos, newBlockSize);

	//////////////// Set neighbors ////////////////
	tempNewBlock->AddNeigbour(target_block, e_FacingDirection::Up);

	//if already has a neighbor on the right transfer it to the new block
	tempNewBlock->AddNeigbour(target_block->myNeighbors.Down, e_FacingDirection::Down);
	tempNewBlock->AddNeigbour(target_block->myNeighbors.Right, e_FacingDirection::Right);
	tempNewBlock->AddNeigbour(target_block->myNeighbors.Left, e_FacingDirection::Left);
	target_block->AddNeigbour(tempNewBlock, e_FacingDirection::Down);
	//////////////////////////////////////////////

	target_block->RefreshHandles();

	return;
}

EditableBlock::e_CutDirection EditableBlock::GetSplitLineDirection(float &f_mouseX, float &f_mouseY)
{
	float sizeX = activeBlock->GetWorldTransform().Size.X;
	float posX = activeBlock->GetWorldTransform().Position.X - (sizeX / 2.0f);
	float sizeY = activeBlock->GetWorldTransform().Size.Y;
	float posY = activeBlock->GetWorldTransform().Position.Y - (sizeY / 2.0f);
	
	///if closer to left or right cut in half all of the blocks in the row
	if((f_mouseX > posX && f_mouseX < posX + 10.0f) || (f_mouseX < posX + sizeX && f_mouseX > posX + sizeX - 10.0f))
		return e_CutDirection::Y_Axis;

	if((f_mouseY > posY && f_mouseY < posY + 10.0f) || (f_mouseY < posY + sizeY && f_mouseY > posY + sizeY - 10.0f))
		return e_CutDirection::X_Axis;
}

//update to the offset of the selected block
void EditableBlock::UpdateLocations()
{
	PixelMath::Vec2D offset = selectedBlock->GetPosition() - selectedBlock_LastKnownLoc;

	for(Block* block : blocks)
	{
		if(block != selectedBlock)
			block->SetPosition(block->GetPosition() + offset);
	}

	//Remember the current blocks position
	selectedBlock_LastKnownLoc = selectedBlock->GetPosition();
}

void EditableBlock::DrawDebug(bool status)
{
	//exit if debug is disabled
	if (!status) return;
	
	//toggle debug with the button
	if(engine->GetKey(olc::Key::B).bPressed)
	{
		enableDebugButtonActive ? enableDebugButtonActive = false : enableDebugButtonActive = true;
	}

	//exit if debug rendering is disabled by the user
	if (!enableDebugButtonActive) return;

	//Debug functionality
	int PositionY = 5;
	int PositionYSpacing = 10;

	if (activeBlock) { engine->DrawString(10, PositionY, "Active Block: " + activeBlock->GetName(), olc::YELLOW); PositionY += PositionYSpacing; }
	if (activeBlock) { engine->DrawString(10, PositionY, "Active Block's Position = " + activeBlock->GetPosition().AsString(), olc::GREEN); PositionY += PositionYSpacing; }
	if (activeBlock) { engine->DrawString(10, PositionY, "Active Block's Left Top Corner Position = " + activeBlock->transform.PositionFromTopLeftCorner.AsString(), olc::GREEN); PositionY += PositionYSpacing; }
	if (activeBlock) { engine->DrawString(10, PositionY, "Active Block's Size =  " + activeBlock->GetSize().AsString(), olc::GREEN); PositionY += PositionYSpacing; }
	if (selectedBlock) { engine->DrawString(10, PositionY, "Selected Block: " + selectedBlock->GetName(), olc::YELLOW); PositionY += PositionYSpacing; }
	if (!blocks.empty()) { engine->DrawString(10, PositionY, "Number of Blocks = " + std::to_string(blocks.size()), olc::YELLOW); PositionY += PositionYSpacing; }
		
}

//TODO: add optimization feature to automatically reduce the block count and add it as an option in Block's settings
void EditableBlock::optimizeBlocks()
{
	Debug::Log("EditableBlock.cpp - optimizeBlocks() binded to Enter key is empty", Debug::Flags::Error);
}

EditableBlock::Block* EditableBlock::GetBlockAt(PixelMath::Vec2D location)
{
	float bX = 0.0f;
	float bY = 0.0f;
	float bWidth = 0.0f;
	float bHeight = 0.0f;

	for(Block* block : blocks)
	{
		bX = block->GetWorldTransform().PositionFromTopLeftCorner.X;
		bY = block->GetWorldTransform().PositionFromTopLeftCorner.Y;
		bWidth = block->GetWorldTransform().Size.X;
		bHeight = block->GetWorldTransform().Size.Y;

		if(bX <= location.X && bX + bWidth  >= location.X &&
		   bY <= location.Y && bY + bHeight >= location.Y)
		{
			return block;
		}
	}

	return nullptr;
}

void EditableBlock::ReevaluateHandles(Block * targetBlock)
{
	targetBlock->RemoveHandles();

	bool hLeft = false;
	bool hRight = false;
	bool hUp = false;
	bool hDown = false;
	float offset = 10.0f; // Don't add a handle if the gap is smaller than this number

	float posX   = targetBlock->GetWorldTransform().PositionFromTopLeftCorner.X;
	float posY   = targetBlock->GetWorldTransform().PositionFromTopLeftCorner.Y;
	float Width  = targetBlock->GetSize().X;
	float Height = targetBlock->GetSize().Y;

	hLeft = isAreaOccupiedByABlock({ posX - offset, posY + 1.0f }, { posX - 1.0f, posY + Height - 1.0f }, targetBlock);
	hRight = isAreaOccupiedByABlock({ posX + Width + 1.0f, posY + 1.0f }, { posX + Width + offset, posY + Height - 1.0f }, targetBlock);
	hUp = isAreaOccupiedByABlock({ posX + 1.0f, posY - offset }, { posX + Width - 1.0f, posY - 1.0f}, targetBlock);
	hDown = isAreaOccupiedByABlock({ posX + 1.0f, posY + Height + 1.0f }, { posX + Width - 1.0f, posY + Height + offset }, targetBlock);
	
	if (!hLeft) targetBlock->AddHandle(EditableBlock::e_FacingDirection::Left);
	if (!hRight) targetBlock->AddHandle(EditableBlock::e_FacingDirection::Right);
	if (!hUp) targetBlock->AddHandle(EditableBlock::e_FacingDirection::Up);
	if (!hDown) targetBlock->AddHandle(EditableBlock::e_FacingDirection::Down);

	targetBlock->RefreshHandles();
}

bool EditableBlock::isAreaOccupiedByABlock(PixelMath::Vec2D StartPos, PixelMath::Vec2D EndPos)
{
	for(float x = StartPos.X; x <= EndPos.X; x++)
	{
		for(float y = StartPos.Y; y <= EndPos.Y; y++)
		{
			if (GetBlockAt({ x, y }))
				return true;
		}
	}

	return false;
}

bool EditableBlock::isAreaOccupiedByABlock(PixelMath::Vec2D StartPos, PixelMath::Vec2D EndPos, Block* IgnoredBlock)
{
	Block* blockFound = nullptr;
	for (float x = StartPos.X; x <= EndPos.X; x++)
	{
		for (float y = StartPos.Y; y <= EndPos.Y; y++)
		{
			blockFound = GetBlockAt({ x, y });
			if (blockFound && blockFound != IgnoredBlock)
				return true;
		}
	}

	return false;
}

bool EditableBlock::willCollide(Block* target, PixelMath::Vec2D desiredPosition, PixelMath::Vec2D desiredSize, e_FacingDirection dir, float offset)
{
	PixelMath::Vec2D endPos = desiredPosition;

	if (dir == e_FacingDirection::Left)
	{
		endPos.X = desiredPosition.X + desiredSize.X;
		desiredPosition.X -= offset;
	}

	if(dir == e_FacingDirection::Right)
	{
		endPos.X = desiredPosition.X + desiredSize.X - offset + 2.0f;
	}

	if (dir == e_FacingDirection::Up)
	{
		endPos.Y = desiredPosition.Y + desiredSize.Y + offset;
		desiredPosition.Y -= offset + 2.0f;
	}

	if (dir == e_FacingDirection::Down)
	{
		endPos.Y = desiredPosition.Y + desiredSize.Y - offset + 2.0f;
	}

	if (isAreaOccupiedByABlock(desiredPosition, endPos, target))
		return true;

	return false;
}

///// Editor Only Functions /////
void EditableBlock::EB_EnableDrawCollision()
{
	for (Block* block : blocks)
		block->bDrawCollision = true;
}

void EditableBlock::EB_DisableDrawCollision()
{
	for (Block* block : blocks)
		block->bDrawCollision = false;
}
///// Editor count Functions END /////

EditableBlock::Block::Block(PixelMath::Vec2D vPosition, PixelMath::Vec2D vSize, EditableBlock* eb_parent) : GameObject({ vPosition.X, vPosition.Y }, { vSize.X, vSize.Y }, "..//PixelEditor//Sprites//Box.png", true)
{
	Tag = "EditableBlock";
	SetAsChildObject(eb_parent);
	bAllowRMBEvents = false;
	bAllowEditorRotation = false;
	parent = eb_parent;
	canBeDestroyed = false;
	//bAllowDragging = false;
}

void EditableBlock::Block::RefreshHandles()
{
	for (Handle* h : handles)
		h->Update();
}

void EditableBlock::Block::RemoveHandles()
{
	for (Block::Handle* h : handles)
		delete h;

	handles.clear();
}

void EditableBlock::Block::SetDirections(bool left, bool right, bool up, bool down)
{
	Directions.Left  = left;
	Directions.Right = right;
	Directions.Up    = up;
	Directions.Down  = down;

	//Clear existing handles
	ClearAllHandles();

	//Add new handles
	if (left) AddHandle(e_FacingDirection::Left);
	if (right) AddHandle(e_FacingDirection::Right);
	if (up) AddHandle(e_FacingDirection::Up);
	if (down) AddHandle(e_FacingDirection::Down);
}

void EditableBlock::Block::AddNeigbour(Block * neigbour, e_FacingDirection side)
{
	switch (side)
	{
		case EditableBlock::Left: myNeighbors.Left = neigbour; break;
		case EditableBlock::Right: myNeighbors.Right = neigbour; break;
		case EditableBlock::Up: myNeighbors.Up = neigbour; break;
		case EditableBlock::Down: myNeighbors.Down = neigbour; break;
		default: break;
	}
}

void EditableBlock::Block::OnEditorUpdate(float & fElapsedTime)
{
	HandleFunctionality();
}

void EditableBlock::Block::onSelected()
{
	parent->selectedBlock = this;
	parent->selectedBlock_LastKnownLoc = GetPosition();
	parent->ReevaluateHandles(this);
}

void EditableBlock::Block::onDeselected()
{
	parent->selectedBlock = nullptr;
}

void EditableBlock::Block::onDragged()
{
	parent->isBeingDragged = true;
}

EditableBlock::Block::Handle* EditableBlock::Block::AddHandle(e_FacingDirection direction)
{
	PixelMath::Vec2D h_pos = { 0.0f, 0.0f };

	switch (direction)
	{
	case EditableBlock::Left: h_pos = GetPosition(); h_pos.X - 10.0f;
		break;
	case EditableBlock::Right: h_pos = GetPosition(); h_pos.X + 10.0f;
		break;
	case EditableBlock::Up: h_pos = GetPosition(); h_pos.Y - 10.0f;
		break;
	case EditableBlock::Down: h_pos = GetPosition(); h_pos.Y + 10.0f;
		break;
	default:
		break;
	}

	Handle* TempHandle = new Handle(this, h_pos, direction);
	handles.emplace_back(TempHandle);	
	return TempHandle;
}

void EditableBlock::Block::HandleFunctionality()
{
	if(bIsSelected || bResizing)
	{
		float mouseX = engine->GetMouseX() / mainCam->GetZoom() + mainCam->GetPosition().X;
		float mouseY = engine->GetMouseY() / mainCam->GetZoom() + mainCam->GetPosition().Y;
		parent->isBeingResized = false;

		for (Handle* handle : handles)
		{
			handle->Update();

			engine->DrawDecal({ handle->GetDrawPosition().X, handle->GetDrawPosition().Y }, handle->GetDecal(), { mainCam->GetZoom() ,  mainCam->GetZoom() });

			//engine->DrawString(10, 100, "Draw Position X(" + std::to_string(handle->GetDrawPosition().X) + "), Y(" + std::to_string(handle->GetDrawPosition().Y) +
				//"\n\nposition X(" + std::to_string(handle->GetPosition().X) + "), Y(" + std::to_string(handle->GetPosition().Y) + ")", olc::CYAN);


			if (!bResizing && engine->GetMouse(0).bHeld)
			{
				PixelMath::Vec2D posL = handle->GetPosition();
				PixelMath::Vec2D size = { handle->GetDecal()->sprite->width + handle->GetDecal()->sprite->width / mainCam->GetZoom(),  handle->GetDecal()->sprite->height + handle->GetDecal()->sprite->height / mainCam->GetZoom() };
				PixelMath::Vec2D posR = handle->GetPosition() + size;

				if (mouseX > posL.X && mouseX < posR.X && mouseY > posL.Y && mouseY < posR.Y)
				{
					ActiveHandle = handle;
					handle->RememberPosition();
					bResizing = true;
					vOldSize = GetSize();
					vOldPosition = GetPosition();
					GameWorld->setMouseDragDisabled(true);
				}
			}

			// Apply the new size
			else if (bResizing && engine->GetMouse(0).bReleased)
			{
				bResizing = false;
				ActiveHandle->offset = 0.0f;

				PixelMath::Vec2D newSize = vOldSize;
				e_FacingDirection dir = ActiveHandle->getDirection();
				PixelMath::Vec2D newPos = GetPosition();

				switch (dir)
				{
				case EditableBlock::Left: newPos.X = mouseX + vOldSize.X / 2.0f + 15.0f; newSize.X = vOldSize.X + (vOldPosition.X - newPos.X);
					break;
				case EditableBlock::Right: newSize.X = mouseX - vOldPosition.X + (vOldSize.X / 2.0f) - 15.0f;
					break;
				case EditableBlock::Up: newPos.Y = mouseY + vOldSize.Y / 2.0f + 15.0f; newSize.Y = vOldSize.Y + (vOldPosition.Y - newPos.Y);
					break;
				case EditableBlock::Down: newSize.Y = mouseY - vOldPosition.Y + (vOldSize.Y / 2.0f) - 15.0f;
					break;
				default:
					break;
				}

				if (newSize.X < blockMinSize)
				{
					newSize.X = blockMinSize;

					if (dir == Left)
						newPos.X = vOldPosition.X + vOldSize.X - blockMinSize;

					else
						newPos.X = vOldPosition.X;
				}

				if (newSize.Y < blockMinSize)
				{
					newSize.Y = blockMinSize;

					if (dir == Up)
						newPos.Y = vOldPosition.Y + vOldSize.Y - blockMinSize;

					else
						newPos.Y = vOldPosition.Y;
				}

				// make sure we don't collide with other blocks
				//if (!parent->willCollide(this, newPos, newSize, ActiveHandle->getDirection(), 10.0f))
				{
					SetSize(newSize);

					newPos.X += (GetSize().X / 2.0f) - (vOldSize.X / 2.0f);
					newPos.Y += (GetSize().Y / 2.0f) - (vOldSize.Y / 2.0f);
					SetPosition(newPos);
					GameWorld->setMouseDragDisabled(false);
					ActiveHandle = nullptr;
				}
			}

			// Editor Drawing
			if (bResizing )
			{
				e_FacingDirection dir = ActiveHandle->getDirection();
				PixelMath::Vec2D newSize = vOldSize;
				PixelMath::Vec2D newPos = transform.PositionOffset;

				switch (dir)
				{
				case EditableBlock::Left: newPos.X = mouseX + vOldSize.X / 2.0f + 15.0f; newSize.X = vOldSize.X + (vOldPosition.X - newPos.X);
					break;
				case EditableBlock::Right: newSize.X = mouseX - vOldPosition.X + (vOldSize.X / 2.0f) - 15.0f;
					break;
				case EditableBlock::Up: newPos.Y = mouseY + vOldSize.Y / 2.0f + 15.0f; newSize.Y = vOldSize.Y + (vOldPosition.Y - newPos.Y);
					break;
				case EditableBlock::Down: newSize.Y = mouseY - vOldPosition.Y + (vOldSize.Y / 2.0f) - 15.0f;
					break;
				default:
					break;
				}

				if (newSize.X < blockMinSize)
				{
					newSize.X = blockMinSize;

					if (dir == Left)
						newPos.X = vOldPosition.X + vOldSize.X - blockMinSize;

					else
						newPos.X = vOldPosition.X;
				}

				if (newSize.Y < blockMinSize)
				{
					newSize.Y = blockMinSize;

					if (dir == Up)
						newPos.Y = vOldPosition.Y + vOldSize.Y - blockMinSize;

					else
						newPos.Y = vOldPosition.Y;
				}

				// make sure we don't collide with other blocks
				//if (!parent->willCollide(this, newPos, newSize, ActiveHandle->getDirection(), 10.0f))
				{
					PixelMath::Vec2D offset = vOldSize;
					offset = newSize - offset;

					int width = newSize.X;
					int height = newSize.Y;
					PixelMath::Vec2D DrawPos;// = transform.PositionOffset - (transform.OffsetSize / 2.0f);
					DrawPos = newPos - (transform.Size / 2.0f);

					if (dir == Left)// add camera offset on X axis if we are modifing the left edge
					{
						DrawPos.X -= GameWorld->GetMainCamera()->GetPosition().X;
					}
					else if(dir == Up)// add camera offset on Y axis if we are modifing the top edge
					{
						DrawPos.Y -= GameWorld->GetMainCamera()->GetPosition().Y;
					}

					//DrawPos *= GameWorld->GetMainCamera()->GetZoom();
					//DrawPos = DrawPos -(transform.Size / 2.0f);
	
					ActiveHandle->offset = offset.X;
					engine->DrawRect(DrawPos.X, DrawPos.Y, width, height, olc::GREEN);
				}
			}

			// set only if true
			if (bResizing)
				parent->isBeingResized = true;
		}
	}
}

void EditableBlock::Block::ClearAllHandles()
{
	//Clear existing handles
	for (Handle* handle : handles)
	{
		delete handle;
	}

	handles.clear();
}

