#include "PathFinding.h"
#include "World.h"
#include "Debug.h"

//#define DebugLog

PathFinding::PathFinding()
{
}

PathFinding::~PathFinding()
{
}

std::vector<Grid::Node*> PathFinding::FindPath_Grid(PixelMath::Vec2D startPos, PixelMath::Vec2D targetPos)
{
	if(!bStillSearching)
	{
		CheckIfValuesAreInBounds(&startPos, &targetPos);

		startNode  = new mNode(GameWorld->GetGrid()->GetNodeAt(startPos));
		targetNode = new mNode(GameWorld->GetGrid()->GetNodeAt(targetPos));

#ifdef DebugLog
		Debug::Logger("\n\n\n\n\nNEW SEARCH\n\n");
		Debug::Logger("startNode = "  + startNode->gridNode->GetPosition().AsString()  + ".\n");
		Debug::Logger("targetNode = " + targetNode->gridNode->GetPosition().AsString() + ".\n");
#endif // DebugLog

		//std::vector<mNode*> openSet;
		//std::vector<mNode*> closedSet;

		openSet.push_back(startNode);

	}//!bStillSearching
	
	int currentIteration = 0;

	while (openSet.size() > 0)
	{

		if (openSet.size() > GameWorld->GetGrid()->NodeCount() || closedSet.size() > GameWorld->GetGrid()->NodeCount())
			return std::vector<Grid::Node*>(); // SOMETHING WENT WRONG!


		//Set the current node to the first node in the open set
		mNode* currentNode = openSet.at(0);

		//go through the open set array
		for(int i = 1; i < openSet.size(); ++i)
		{
			//and check if we have a node with a lower cost
			if ( openSet.at(i)->fCost() < currentNode->fCost() || (openSet.at(i)->fCost() == currentNode->fCost() && openSet.at(i)->hCost < currentNode->hCost) )
			{
				currentNode = openSet.at(i);
			}
		}

		//add the current node to the closed set
		closedSet.emplace_back(currentNode);
		//remove the current node from the open set
		removeNodeFromVector(currentNode, openSet);

#ifdef DebugLog
		Debug::Logger("\n#############################################\n");
		Debug::Logger("Current = " + currentNode->gridNode->GetPosition().AsString() + ".\n");
		Debug::Logger("Closed set size = " + std::to_string(closedSet.size()) + ".\n");
		for(auto node : closedSet)
		{
			//engine->DrawCircle((int32_t)(node->gridNode->GetPosition().X), (int32_t)(node->gridNode->GetPosition().Y), 10, olc::YELLOW);
			std::string msg = node->gridNode->GetPosition().AsString();
			Debug::Logger("node " + msg + "\n");
		}
#endif

		//if the current node is == target node, then we found the path
		if(*currentNode == *targetNode)
		{
			auto path = RetracePathGrid(startNode, currentNode);
			ClearSets(openSet, closedSet);
			bStillSearching = false;
			return path;
		}

		//if not we continue by checking current node's neighbors
		auto neighbours = CreateNeigbourNodes(currentNode, openSet, closedSet);
		for(int i = 0; i < neighbours.size(); ++i)
		{
			auto neighbour = neighbours.at(i);

			//(neighbour->gridNode->isOccupied() && neighbour != targetNode)||
			if (CheckIfNodeIsOccupied(neighbour, targetNode))
			{
				delete neighbours.at(i); // delete the node as it's unaccessible
				continue;
			}

			int newMovementCostToNeighbour = currentNode->gCost + GetDistance(currentNode->gridNode, neighbour->gridNode);

			//if(newMovementCostToNeighbour < neighbour->gCost)
			{
				//mNode* newNode = new mNode(neighbour->gridNode);
				neighbour->gCost = newMovementCostToNeighbour;
				neighbour->hCost = GetDistance(neighbour->gridNode, targetNode->gridNode);
				neighbour->parent = currentNode;
				openSet.emplace_back(neighbour);
			}
		}

		neighbours.clear();

		// we hit the limit of iterations so jump out of the function until the next frame.
		if (currentIteration > MaxIterations)
			return std::vector<Grid::Node*>();

		currentIteration++;
	}
}

std::vector<Grid::Node*> PathFinding::FindPath2(PixelMath::Vec2D startPos, PixelMath::Vec2D targetPos)
{
	if (startPos.X > GameWorld->GetGrid()->GridSize.X || startPos.Y > GameWorld->GetGrid()->GridSize.Y ||
		targetPos.X > GameWorld->GetGrid()->GridSize.X || targetPos.Y > GameWorld->GetGrid()->GridSize.Y)
		return std::vector<Grid::Node*>();

	CheckIfValuesAreInBounds(&startPos, &targetPos);

	mNode* startNode = new mNode(GameWorld->GetGrid()->GetNodeAt(startPos));
	mNode* targetNode = new mNode(GameWorld->GetGrid()->GetNodeAt(targetPos));
	GetClosestFreeNode(targetNode);

	Debug::Logger("\n\n\n\n\nNEW SEARCH\n\n");
	Debug::Logger("startNode = " + startNode->gridNode->GetPosition().AsString() + ".\n");
	Debug::Logger("targetNode = " + targetNode->gridNode->GetPosition().AsString() + ".\n");

	std::vector<mNode*> openSet;
	std::vector<mNode*> closedSet;

	openSet.push_back(startNode);

	while (openSet.size() > 0)
	{
		if (openSet.size() > GameWorld->GetGrid()->NodeCount() || closedSet.size() > GameWorld->GetGrid()->NodeCount())
			return std::vector<Grid::Node*>(); // SOMETHING WENT WRONG!

		mNode* currentNode = openSet.at(0);

		for (int i = 1; i < openSet.size(); ++i)
		{
			if (openSet.at(i)->fCost() < currentNode->fCost() || (openSet.at(i)->fCost() == currentNode->fCost() && openSet.at(i)->hCost < currentNode->hCost))
			{
				currentNode = openSet.at(i);
			}
		}

		closedSet.emplace_back(currentNode);
		removeNodeFromVector(currentNode, openSet);

		Debug::Logger("\n#############################################\n");
		Debug::Logger("Current = " + currentNode->gridNode->GetPosition().AsString() + ".\n");
		Debug::Logger("Closed set size = " + std::to_string(closedSet.size()) + ".\n");
		for (auto node : closedSet)
		{
			//engine->DrawCircle((int32_t)(node->gridNode->GetPosition().X), (int32_t)(node->gridNode->GetPosition().Y), 10, olc::YELLOW);
			std::string msg = node->gridNode->GetPosition().AsString();
			Debug::Logger("node " + msg + "\n");
		}

		//found the path
		if (*currentNode == *targetNode)
		{
			auto path = RetracePathGrid(startNode, currentNode);
			ClearSets(openSet, closedSet);
			return path;
		}

		//continue
		auto neighbours = CreateNeigbourNodes(currentNode, openSet, closedSet);
		for (auto neighbour : neighbours)
		{
			if (neighbour->gridNode->isOccupied() || vectorContainsNodeAtThatPos(neighbour->gridNode->GetPosition(), closedSet))
				continue;

			int newMovementCostToNeighbour = currentNode->gCost + GetDistance(currentNode->gridNode, neighbour->gridNode);

			if (newMovementCostToNeighbour < neighbour->gCost || !(vectorContainsNodeAtThatPos(neighbour->gridNode->GetPosition(), openSet)))
			{
				//mNode* newNode = new mNode(neighbour->gridNode);
				neighbour->gCost = newMovementCostToNeighbour;
				neighbour->hCost = GetDistance(neighbour->gridNode, targetNode->gridNode);
				neighbour->parent = currentNode;
				openSet.emplace_back(neighbour);
			}
		}

	}


}

std::vector<PixelMath::Vec2D> PathFinding::FindPath_Vec(PixelMath::Vec2D startPos, PixelMath::Vec2D targetPos)
{
	if (!bStillSearching)
	{
		CheckIfValuesAreInBounds(&startPos, &targetPos);

		startNode = new mNode(GameWorld->GetGrid()->GetNodeAt(startPos));
		targetNode = new mNode(GameWorld->GetGrid()->GetNodeAt(targetPos));

#ifdef DebugLog
		Debug::Logger("\n\n\n\n\nNEW SEARCH\n\n");
		Debug::Logger("startNode = " + startNode->gridNode->GetPosition().AsString() + ".\n");
		Debug::Logger("targetNode = " + targetNode->gridNode->GetPosition().AsString() + ".\n");
#endif // DebugLog

		//std::vector<mNode*> openSet;
		//std::vector<mNode*> closedSet;

		openSet.push_back(startNode);

	}//!bStillSearching

	int currentIteration = 0;

	while (openSet.size() > 0)
	{

		if (openSet.size() > GameWorld->GetGrid()->NodeCount() || closedSet.size() > GameWorld->GetGrid()->NodeCount())
			return std::vector<PixelMath::Vec2D>(); // SOMETHING WENT WRONG!


		//Set the current node to the first node in the open set
		mNode* currentNode = openSet.at(0);

		//go through the open set array
		for (int i = 1; i < openSet.size(); ++i)
		{
			//and check if we have a node with a lower cost
			if (openSet.at(i)->fCost() < currentNode->fCost() || (openSet.at(i)->fCost() == currentNode->fCost() && openSet.at(i)->hCost < currentNode->hCost))
			{
				currentNode = openSet.at(i);
			}
		}

		//add the current node to the closed set
		closedSet.emplace_back(currentNode);
		//remove the current node from the open set
		removeNodeFromVector(currentNode, openSet);

#ifdef DebugLog
		Debug::Logger("\n#############################################\n");
		Debug::Logger("Current = " + currentNode->gridNode->GetPosition().AsString() + ".\n");
		Debug::Logger("Closed set size = " + std::to_string(closedSet.size()) + ".\n");
		for (auto node : closedSet)
		{
			//engine->DrawCircle((int32_t)(node->gridNode->GetPosition().X), (int32_t)(node->gridNode->GetPosition().Y), 10, olc::YELLOW);
			std::string msg = node->gridNode->GetPosition().AsString();
			Debug::Logger("node " + msg + "\n");
		}
#endif

		//if the current node is == target node, then we found the path
		if (*currentNode == *targetNode)
		{
			auto path = RetracePathVec(startNode, currentNode);
			ClearSets(openSet, closedSet);
			bStillSearching = false;
			return path;
		}

		//if not we continue by checking current node's neighbors
		auto neighbours = CreateNeigbourNodes(currentNode, openSet, closedSet);
		for (int i = 0; i < neighbours.size(); ++i)
		{
			auto neighbour = neighbours.at(i);

			//(neighbour->gridNode->isOccupied() && neighbour != targetNode)||
			if (CheckIfNodeIsOccupied(neighbour, targetNode))
			{
				delete neighbours.at(i); // delete the node as it's unaccessible
				continue;
			}

			int newMovementCostToNeighbour = currentNode->gCost + GetDistance(currentNode->gridNode, neighbour->gridNode);

			//if(newMovementCostToNeighbour < neighbour->gCost)
			{
				//mNode* newNode = new mNode(neighbour->gridNode);
				neighbour->gCost = newMovementCostToNeighbour;
				neighbour->hCost = GetDistance(neighbour->gridNode, targetNode->gridNode);
				neighbour->parent = currentNode;
				openSet.emplace_back(neighbour);
			}
		}

		neighbours.clear();

		// we hit the limit of iterations so jump out of the function until the next frame.
		if (currentIteration > MaxIterations)
			return std::vector<PixelMath::Vec2D>();

		currentIteration++;
	}
}


void PathFinding::removeNodeFromVector(mNode* remove, std::vector<mNode*>& from)
{
	for(int i = from.size() - 1; i > -1; --i)
	{
		if(*(from.at(i)) == *remove)
		{
			from.erase(from.begin() + i, from.begin() + i + 1);
			return;
		}
	}
}

bool PathFinding::vectorContainsNodeAtThatPos(PixelMath::Vec2D pos, std::vector<mNode*>& list)
{
	for(auto node : list)
	{
		if (node->gridNode->GetPosition() == pos)
			return true;
	}

	return false;
}

int PathFinding::GetDistance(Grid::Node* nodeA, Grid::Node* nodeB)
{
	int dstX = abs(nodeA->GetPosition().X - nodeB->GetPosition().X);
	int dstY = abs(nodeA->GetPosition().Y - nodeB->GetPosition().Y);

	if (dstX > dstY)
		return 14 * dstY + 10 * (dstX - dstY);

	else
		return 14 * dstX + 10 * (dstY - dstX);
}

std::vector<Grid::Node*> PathFinding::RetracePathGrid(mNode* StartNode, mNode* EndNode)
{
	std::vector<Grid::Node*> path;

	mNode* currentNode = EndNode;

	while(*currentNode != *StartNode)
	{
		path.emplace_back(currentNode->gridNode);
		currentNode = currentNode->parent;
	}

	std::reverse(path.begin(), path.end());
	return path;
}

std::vector<PixelMath::Vec2D> PathFinding::RetracePathVec(mNode* StartNode, mNode* EndNode)
{
	std::vector<PixelMath::Vec2D> path;

	mNode* currentNode = EndNode;
	PixelMath::Vec2D nodeSize = { GameWorld->GetGrid()->GetNodeSize(), GameWorld->GetGrid()->GetNodeSize()};

	while (*currentNode != *StartNode)
	{
		path.emplace_back(currentNode->gridNode->GetPosition() + nodeSize);
		currentNode = currentNode->parent;
	}

	std::reverse(path.begin(), path.end());
	return path;
}

std::vector<PathFinding::mNode*> PathFinding::CreateNeigbourNodes(mNode* node, std::vector<mNode*> &OpenSet, std::vector<mNode*> & ClosedSet)
{
	std::vector<mNode*> vNeighbours;
	for(Grid::Node* neighbour : node->gridNode->GetNeighboursInVec())
	{
		if (neighbour)
			if (!(vectorContainsNodeAtThatPos(neighbour->GetPosition(), OpenSet)))
				if (!(vectorContainsNodeAtThatPos(neighbour->GetPosition(), ClosedSet)))
					vNeighbours.push_back(new mNode(neighbour));
	}

	return vNeighbours;
}

void PathFinding::ClearSets(std::vector<PathFinding::mNode*>& OpenSet, std::vector<PathFinding::mNode*>& ClosedSet)
{
	for (auto node : OpenSet)
		delete node;

	for (auto node : ClosedSet)
		delete node;

	OpenSet.clear();
	ClosedSet.clear();
}

inline bool PathFinding::CheckIfNodeIsOccupied(mNode* node, mNode* targetNode)
{
	//Check if both nodes are occupied as in any other case there is no possibly for the target & tested/current node to be occupied by the same entity
	if(targetNode->gridNode->isOccupied() && node->gridNode->isOccupied())
	{
		// check if target node and currently tested node are both occupied by the same entity 
		for(auto obj : node->gridNode->GetOccupiedBy())
		{
			for(auto obj2 : targetNode->gridNode->GetOccupiedBy())
			{
				if(obj == obj2)
				{
					//if both objects are the same we can ignore the fact that the node is occupied as
					//it's most likely occupied by our target
					return false;
				}
			}
		}
		return true;
	}

	return false;
}

inline void PathFinding::GetClosestFreeNode(mNode* node)
{
	if(node->gridNode->isOccupied())
	{
		mNode* originalNode = node;
		int searches = 0;
search:
		for(auto neighbour : node->gridNode->GetNeighboursInVec())
		{
			if(!(neighbour->isOccupied()))
			{
				node->gridNode = neighbour;
				return;
			}
		}

		//all neighbors are occupied too. Check neighbor's neighbors
		node->gridNode = originalNode->gridNode->GetNeighboursInVec().at(searches);
		searches++;
		goto search;
	}
}

inline void PathFinding::CheckIfValuesAreInBounds(PixelMath::Vec2D* Start, PixelMath::Vec2D* End)
{
	auto GridMax = GameWorld->GetGrid()->GridSize;
	GridMax.X -= GameWorld->GetGrid()->GetNodeSize();
	GridMax.Y -= GameWorld->GetGrid()->GetNodeSize();

	//START
	PixelMath::Functions::Clamp(Start->X, GridMax.X, 0.0F);
	PixelMath::Functions::Clamp(Start->Y, GridMax.Y, 0.0F);
	//END
	PixelMath::Functions::Clamp(End->X, GridMax.X, 0.0F);
	PixelMath::Functions::Clamp(End->Y, GridMax.Y, 0.0F);
}
