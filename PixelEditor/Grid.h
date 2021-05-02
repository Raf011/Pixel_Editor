#pragma once
#include "PixelMath.h"
#include <vector>
#include "GameObject.h"
#include "olcPixelGameEngine.h"

class Grid
{
public:
	Grid();
	~Grid();
	bool bCanUpdateOffsetPosition = true; //recalculates position and size based on camera position and stores the value in the grid class.

public:
	class Node
	{

	friend class Grid;

	public:
		Node();
		~Node();

	private:
		PixelMath::Vec2D Position;
		PixelMath::Vec2D PositionOffset;
		bool Occupied = false;
		bool Visible = true;
		std::vector<GameObject*> vecOccupiedBy;
		olc::Decal* _decal = nullptr;

		struct Neighbours
		{
			Node* TopLeft = nullptr;
			Node* Top = nullptr;
			Node* TopRight = nullptr;
			Node* Left = nullptr;
			Node* Right = nullptr;
			Node* BottomLeft = nullptr;
			Node* Bottom = nullptr;
			Node* BottomRight = nullptr;
		};

		Neighbours neighbours;

	public:
		PixelMath::Vec2D GetPosition();
		PixelMath::Vec2D GetPositionOffset(); // for now this is updated in the PixelGameEditor.cpp in DrawGrid().
		void SetPositionOffset(PixelMath::Vec2D pos) { PositionOffset = pos; } // for now this is updated in the PixelGameEditor.cpp in DrawGrid().
		Neighbours GetNeighbours();
		std::vector<Grid::Node*> GetNeighboursInVec();
		olc::Decal* GetDecal() const { return _decal; }
		bool isOccupied() { return Occupied; }
		std::vector<GameObject*> GetOccupiedBy() { return vecOccupiedBy; }

		bool operator == (const Node& anotherNode) { return (this->Position == anotherNode.Position); }
		bool operator != (const Node& anotherNode) { return (this->Position != anotherNode.Position); }
	};

private:
	void SetNeighbours();
	float NodeSize = 50.0f;
	float NodeSizeOffset = 50.0f;
	bool _bHighlightOccupiedCells = true;

public:
	float GetNodeSize() { return NodeSize; }
	PixelMath::Vec2D GridSize;
	PixelMath::Vec2D GridTotalCellCount = { 55, 50 }; //{ 25, 20 }
	Node Grid2D[50][55]; //[20][25]
	std::vector<Node*> OccupiedNodes;
	std::vector<Node*> DebugAllNodes;
	void HighlightOccupiedCells(bool highlight = true) { _bHighlightOccupiedCells = highlight; }

	// Used to store the last calculated position against the camera zoom and position
	PixelMath::Vec2D DrawPoint = { 0, 0 };

	//auto G2D;


	bool GridInit();
	Node* GetNodeAt(PixelMath::Vec2D Location);
	Node* GetNodeAt(int x, int y);
	std::vector<Node*> GetNodesInTheArea(PixelMath::Vec2D AreaLocation, PixelMath::Vec2D AreaSize);
	void UpdateGrid();
	void UpdateOffsetPosition();
	unsigned const int NodeCount() { return (int)(GridSize.X + GridSize.Y); }
	void LoadATileMap(std::string imagePath, PixelMath::Vec2D gridNodeOrigin);

};

