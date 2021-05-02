#pragma once
#include "PixelMath.h"
#include "Grid.h"

class PathFinding
{
public:
	PathFinding();
	~PathFinding();

	//This search will ignore the object that occupies the target node
	std::vector<Grid::Node*> FindPath_Grid(PixelMath::Vec2D startPos, PixelMath::Vec2D targetPos);
	//This search will find the closest non-occupied node from the target node
	std::vector<Grid::Node*> FindPath2(PixelMath::Vec2D startPos, PixelMath::Vec2D targetPos);
	std::vector<PixelMath::Vec2D> FindPath_Vec(PixelMath::Vec2D startPos, PixelMath::Vec2D targetPos);

private:
	class mNode
	{
	public:
		mNode(Grid::Node* GridNode) : gridNode(GridNode) {};
		~mNode() {};

		Grid::Node* gridNode = nullptr;

		int gCost = -1;
		int hCost = -1;
		int fCost() { return gCost + hCost; }		
		
		bool operator == (mNode otherNode) { return this->gridNode == otherNode.gridNode; }
		bool operator != (mNode otherNode) { return this->gridNode != otherNode.gridNode; }

		mNode* parent = nullptr;
	};

	bool bStillSearching = false;
	std::vector<mNode*> openSet;
	std::vector<mNode*> closedSet;
	mNode* startNode = nullptr;
	mNode* targetNode = nullptr;
	const int MaxIterations = 100;

	void removeNodeFromVector(mNode *remove, std::vector<mNode*> &from);
	bool vectorContainsNodeAtThatPos(PixelMath::Vec2D pos, std::vector<mNode*> &list);
	int GetDistance(Grid::Node* nodeA, Grid::Node* nodeB);
	std::vector<Grid::Node*> RetracePathGrid(mNode* StartNode, mNode* EndNode);
	std::vector<PixelMath::Vec2D> RetracePathVec(mNode* StartNode, mNode* EndNode);
	std::vector<PathFinding::mNode*> CreateNeigbourNodes(mNode* node, std::vector<mNode*> &OpenSet, std::vector<mNode*> &ClosedSet);

	void ClearSets(std::vector<PathFinding::mNode*>& OpenSet, std::vector<PathFinding::mNode*>& ClosedSet);

	inline bool CheckIfNodeIsOccupied(mNode* node, mNode* targetNode);
	inline void GetClosestFreeNode(mNode* node);
	inline void CheckIfValuesAreInBounds(PixelMath::Vec2D* Start, PixelMath::Vec2D* Ende);
};

