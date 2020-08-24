#pragma once
#include "PixelMath.h"
#include <vector>

class Grid
{
public:
	Grid();
	~Grid();
	bool bCanUpdateOffsiedPosition = true; //recalculates position and size based on camera position and stores the value in the grid class.

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
		bool isOccupied() { return Occupied; }
	};

private:
	void SetNeighbours();
	float NodeSize = 50.0f;
	float NodeSizeOffset = 50.0f;

public:
	float GetNodeSize() { return NodeSize; }
	PixelMath::Vec2D GridSize;
	PixelMath::Vec2D GridTotalCellCount = { 25, 20 };
	Node Grid2D[20][25];
	std::vector<Node*> OccupiedNodes;
	std::vector<Node*> DebugAllNodes;

	// Used to store the last calculated position against the camera zoom and position
	PixelMath::Vec2D DrawPoint = { 0, 0 };

	//auto G2D;


	bool GridInit();
	Node* GetNodeAt(PixelMath::Vec2D Location);
	Node* GetNodeAt(int x, int y);
	std::vector<Node*> GetNodesInTheArea(PixelMath::Vec2D AreaLocation, PixelMath::Vec2D AreaSize);
	void UpdateGrid();
	void UpdateOffsetPosition();

};

