#pragma once
#include "GameObject.h"


class EditableBlock : public GameObject
{
public:
	EditableBlock(PixelMath::Vec2D position);
	~EditableBlock();

	void drawDebugCells();

	struct s_FacingDirection
	{
		bool Left  = false;
		bool Right = false;
		bool Up    = false;
		bool Down  = false;
	};

	enum e_FacingDirection
	{
		Left, Right, Up, Down
	};

	enum e_CutDirection
	{
		X_Axis, Y_Axis
	};

private:
	bool blocksOptimized = false;
	bool bSeparatorActive = true;
	//virtual void OnBeginPlay() override;
	//virtual void Update(float &fElapsedTime) override;
	virtual void OnEditorUpdate(float &fElapsedTime) override;

	class Block : public GameObject
	{
		class Handle
		{
		public:
			Handle(Block* Owner, PixelMath::Vec2D vPosition, e_FacingDirection h_direction)
			{
				owner = Owner;
				position = vPosition;
				spr = new olc::Sprite("..//PixelEditor//Sprites/Dot.png");
				dec = new olc::Decal(spr);
				hDirection = h_direction;
				offsetPosition = Owner->GetPosition() - vPosition;

			}
			~Handle() {};


		private:	
			Block* owner = nullptr;
			PixelMath::Vec2D position = { 0.0f, 0.0f };
			PixelMath::Vec2D offsetPosition = { 0.0f, 0.0f };
			PixelMath::Vec2D DrawPosition = { 0.0f, 0.0f };
			olc::Sprite* spr;
			olc::Decal*  dec;
			PixelMath::Vec2D savedPos = { 0.0f, 0.0f };
			e_FacingDirection hDirection;


		public:
			PixelMath::Vec2D GetPosition() { return position; }
			PixelMath::Vec2D GetDrawPosition() { return DrawPosition; }
			olc::Decal* GetDecal() { return dec; }
			void RememberPosition() { savedPos = position; }
			PixelMath::Vec2D GetPositionFromMemory() { return savedPos; }
			float offset = 0.0f;
			e_FacingDirection getDirection() { return hDirection; }
	
			void InitHandle()
			{
				dec->sprite = spr;
			}

			// update handle draw position
			void Update()
			{
				PixelMath::Vec2D h_pos = owner->transform.Position;
				if (hDirection == e_FacingDirection::Left) h_pos.X -= (owner->transform.Size.X / 2.0f) + offsetPosition.X + offset + 20.0f;
				if (hDirection == e_FacingDirection::Right) h_pos.X += (owner->transform.Size.X / 2.0f) + offsetPosition.X + offset + 10.0f;
				if (hDirection == e_FacingDirection::Up) h_pos.Y -= (owner->transform.Size.Y / 2.0f) + offsetPosition.Y + offset + 20.0f;
				if (hDirection == e_FacingDirection::Down)   h_pos.Y += (owner->transform.Size.Y / 2.0f) + offsetPosition.Y + offset + 10.0f;
				position = h_pos;

				PixelMath::Vec2D h_draw_pos = owner->transform.PositionOffset;
				if (hDirection == e_FacingDirection::Left) h_draw_pos.X -= (owner->transform.OffsetSize.X / 2.0f) + offsetPosition.X + offset + 20.0f;
				if (hDirection == e_FacingDirection::Right) h_draw_pos.X += (owner->transform.OffsetSize.X / 2.0f) + offsetPosition.X + offset + 10.0f;
				if (hDirection == e_FacingDirection::Up) h_draw_pos.Y -= (owner->transform.OffsetSize.Y / 2.0f) + offsetPosition.Y + offset + 20.0f;
				if (hDirection == e_FacingDirection::Down)   h_draw_pos.Y += (owner->transform.OffsetSize.Y / 2.0f) + offsetPosition.Y + offset + 10.0f;
				DrawPosition = h_draw_pos;
			}
		};
		//////////////////////////////////////

		struct s_Neighbors
		{
			Block* Left = nullptr;
			Block* Right = nullptr;
			Block* Up = nullptr;
			Block* Down = nullptr;
		};

		friend class EditableBlock;
	public:
		Block(PixelMath::Vec2D vPosition, PixelMath::Vec2D vSize, EditableBlock* eb_parent);
		~Block() {};
		EditableBlock* parent = nullptr;
		void RefreshHandles();
		void RemoveHandles();
		void SetDirections(bool left, bool right, bool up, bool down);
		void AddNeigbour(Block* neigbour, e_FacingDirection side);


	private:
		virtual void OnEditorUpdate(float &fElapsedTime) override;
		virtual void onSelected() override;
		virtual void onDeselected() override;
		virtual void onDragged() override;
		Handle* AddHandle(e_FacingDirection direction);
		void HandleFunctionality();
		void ClearAllHandles();


	private:
		std::vector<Handle*> handles;
		Handle* ActiveHandle = nullptr;
		bool bResizing = false;
		PixelMath::Vec2D vOldSize = { 0.0f, 0.0f };
		PixelMath::Vec2D vOldPosition = { 0.0f, 0.0f };
		float blockMinSize = 2.0f;
		s_FacingDirection Directions;
		s_Neighbors myNeighbors;
	};


	std::vector<Block*> blocks;

	Block* AddNewBlock(PixelMath::Vec2D vPos, PixelMath::Vec2D vS);
	void SplitAllBlocksWithinRegion(EditableBlock::e_CutDirection cut_axis, float splitPoint, Block* active_block);
	void SplitLineFunctionality();
	void CutOnXAxis(float &f_mouseX, Block* target_block);
	void CutOnYAxis(float &f_mouseY, Block* target_block);
	e_CutDirection GetSplitLineDirection(float &f_mouseX, float &f_mouseY);
	Block* activeBlock = nullptr;
	Block* selectedBlock = nullptr;
	bool bCanCut = false;
	void UpdateLocations();
	PixelMath::Vec2D selectedBlock_LastKnownLoc;
	bool isBeingResized = false;
	bool isBeingDragged = false;
	void DrawDebug(bool status);
	bool enableDebugButtonActive = false;
	void optimizeBlocks(); //optimize 

	Block* GetBlockAt(PixelMath::Vec2D location);
	void ReevaluateHandles(Block* targetBlock);
	bool isAreaOccupiedByABlock(PixelMath::Vec2D StartPos, PixelMath::Vec2D EndPos);
	bool isAreaOccupiedByABlock(PixelMath::Vec2D StartPos, PixelMath::Vec2D EndPos, Block* IgnoredBlock);

	bool willCollide(Block* target, PixelMath::Vec2D desiredPosition, PixelMath::Vec2D desiredSize, e_FacingDirection dir, float offset);

public:
	bool b_EB_DrawCollision = true;
	bool b_EB_AllowMirrorCut = true;
	void EB_EnableDrawCollision();
	void EB_DisableDrawCollision();
};

