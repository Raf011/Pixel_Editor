#include "Grid.h"
#include "World.h"

Grid::Grid()
{
}


Grid::~Grid()
{
}

void Grid::SetNeighbours()
{

	for (int column = 0; column < GridTotalCellCount.Y; column++)
		for (int row = 0; row < GridTotalCellCount.X; row++)
		{
			Node* node = &Grid2D[column][row];

			//  Left
			if (node->Position.X > 0)
				node->neighbours.Left = GetNodeAt((int)node->Position.X - NodeSize, (int)node->Position.Y);

			//  Right
			if (node->Position.X < GridSize.X - NodeSize)
				node->neighbours.Right = GetNodeAt((int)node->Position.X + NodeSize, (int)node->Position.Y);

			// Top
			if (node->Position.Y > 0)
				node->neighbours.Top = GetNodeAt((int)node->Position.X, (int)node->Position.Y - NodeSize);

			//  Bottom
			if (node->Position.Y < GridSize.Y)
				node->neighbours.Bottom = GetNodeAt((int)node->Position.X, (int)node->Position.Y + NodeSize);

			//  Top Left
			if (node->Position.X > 0 && node->Position.Y > 0)
				node->neighbours.TopLeft = GetNodeAt((int)node->Position.X - NodeSize, (int)node->Position.Y - NodeSize);

			//  Top Right
			if (node->Position.X < GridSize.X - NodeSize && node->Position.Y > 0)
				node->neighbours.TopRight = GetNodeAt((int)node->Position.X + NodeSize, (int)node->Position.Y - NodeSize);

			//  Bottom Left
			if (node->Position.X > 0 && node->Position.Y < GridSize.Y - NodeSize)
				node->neighbours.BottomLeft = GetNodeAt((int)node->Position.X - NodeSize, (int)node->Position.Y + NodeSize);

			//  Bottom Right
			if (node->Position.X < GridSize.X - NodeSize && node->Position.Y < GridSize.Y - NodeSize)
				node->neighbours.BottomRight = GetNodeAt((int)node->Position.X + NodeSize, (int)node->Position.Y + NodeSize);
		}
}

bool Grid::GridInit()
{
	for (int column = 0; column < GridTotalCellCount.Y; column++)
		for (int row = 0; row < GridTotalCellCount.X; row++)
		{
			Grid2D[column][row].Position = { (float)row * NodeSize, (float)column * NodeSize };
			Grid2D[column][row]._decal = new olc::Decal(new olc::Sprite("..//PixelEditor//Sprites//Tile.png"));
			//Grid2D[column][row]._decal->sprite->width = 45;
			//Grid2D[column][row]._decal->sprite->height = 45;
		}

	// Calculate the size of the grid
	GridSize.X = GridTotalCellCount.X * NodeSize;
	GridSize.Y = GridTotalCellCount.Y * NodeSize;

	SetNeighbours();

	return true;
}

Grid::Node* Grid::GetNodeAt(PixelMath::Vec2D Location)
{
	// Lower than 0 check
	if (Location.X < 0) Location.X = 0;
	if (Location.Y < 0) Location.Y = 0;

	// Convert to int to use MOD
	int x = (int)Location.X;
	int y = (int)Location.Y;
	x = x - (x % (int)NodeSize); // Get closest grid cell
	y = y - (y % (int)NodeSize); // Get closest grid cell

	// if x or y is outside the grid, set its value to the last column/row
	if (x > GridSize.X) x = GridSize.X - NodeSize; // TODO - print warning
	if (y > GridSize.Y) y = GridSize.Y - NodeSize; // TODO - print warning

	int predictedCollumn = (x / NodeSize);
	int predictedRow = (y / NodeSize);

	return &Grid2D[predictedRow][predictedCollumn];
}

Grid::Node* Grid::GetNodeAt(int x, int y)
{
	// Lower than 0 check
	if (x < 0) x = 0;
	if (y < 0) y = 0;

	x = x - (x % (int)NodeSize); // Get closest grid cell
	y = y - (y % (int)NodeSize); // Get closest grid cell

	// if x or y is outside the grid, set its value to the last collumn/row
	if (x > GridSize.X) x = GridSize.X - NodeSize; // TODO - print warning
	if (y > GridSize.Y) y = GridSize.Y - NodeSize; // TODO - print warning

	int predictedCollumn = (x / NodeSize);
	int predictedRow = (y / NodeSize);

	return &Grid2D[predictedRow][predictedCollumn];
}

std::vector<Grid::Node*> Grid::GetNodesInTheArea(PixelMath::Vec2D AreaLocation, PixelMath::Vec2D AreaSize)
{
	std::vector<Grid::Node*> nodesFound; //we will store nodes/grid cells found in this vector
	float nodeSize = GetNodeSize(); //by default it is 50.

	//Snap the position to the closest grid cell on the left. Then update the size by how much we "moved" it left. 
	int modX = (float)((int)AreaLocation.X % (int)nodeSize);
	int modY = (float)((int)AreaLocation.Y % (int)nodeSize);
	AreaLocation.X -= modX;
	AreaLocation.Y -= modY;
	//AreaSize.X += modX;
	//AreaSize.Y += modY;

	if (AreaLocation.X < 0.0F) AreaLocation.X = 0.0F;
	if (AreaLocation.Y < 0.0F) AreaLocation.Y = 0.0F;

	//don't go over the last node on X
	if (AreaLocation.X + AreaSize.X > GridSize.X - nodeSize)
		AreaSize.X = GridSize.X - nodeSize - AreaLocation.X;

	//don't go over the last node on Y
	if (AreaLocation.Y + AreaSize.Y > GridSize.Y - nodeSize)
		AreaSize.Y = GridSize.Y - nodeSize - AreaLocation.Y;

	//Find grid cells/nodes in the area
	for (float x = AreaLocation.X; x < AreaLocation.X + AreaSize.X; x += nodeSize)
	{
		for (float y = AreaLocation.Y; y < AreaLocation.Y + AreaSize.Y; y += nodeSize)
		{
			Node* nodeFound = GetNodeAt({ x, y });

			for (Node* i : nodesFound)
				if (i == nodeFound) continue; // don't re-add the same value

			nodesFound.emplace_back(nodeFound);
		}
	}

	return nodesFound;
}

void Grid::UpdateGrid()
{
	if (GameWorld && mainCam)
	{
		UpdateOffsetPosition();

		//clear previously occupied nodes as we will recalculate this again.
		if (!OccupiedNodes.empty())
		{
			for (Node* occupiedNode : OccupiedNodes)
				occupiedNode->Occupied = false;

			OccupiedNodes.clear();
		}

		for (GameObject* actor : GameWorld->GetGameObjectsSpawnedInTheWorld())
		{
			// test what nodes actor occupies only if its collision is enabled
			if (actor->CanCollide())
			{
				//Get nodes at the locations occupied by one entity
				std::vector<Node*> nodes = GetNodesInTheArea(actor->GetWorldTransform().PositionFromTopLeftCorner, actor->GetWorldTransform().Size);

				////////////////////////////////DEBUG////////////////////////////////
				//int x = actor->GetWorldTransform().PositionFromTopLeftCorner.X;
				//int y = actor->GetWorldTransform().PositionFromTopLeftCorner.Y;
				//int Width = actor->GetWorldTransform().Size.X;
				//int Height = actor->GetWorldTransform().Size.Y;
				//engine->DrawRect(x - 1, y - 1, Width + 2, Height + 2, olc::YELLOW);
				/////////////////////////////////////////////////////////////////////

				//add nodes to the list of occupied node list
				if (!nodes.empty())
				{
					for(Node* node : nodes)
						if(!node->vecOccupiedBy.empty())
							node->vecOccupiedBy.clear(); // clear entities that occupy this grid cell

					for (Node* node : nodes)
					{
						node->Occupied = true;
						node->vecOccupiedBy.push_back(actor);
						OccupiedNodes.push_back(node);

						////////////////////////////////DEBUG////////////////////////////////
						//int tx = node->GetPosition().X;
						//int ty = node->GetPosition().Y;
						//
						//engine->DrawRect(tx - 1, ty - 1, 52, 52, olc::MAGENTA);
						/////////////////////////////////////////////////////////////////////
					}
				}
			}
		}

		if(_bHighlightOccupiedCells)
			for (auto i : OccupiedNodes)
				engine->DrawRect(i->GetPositionOffset().X, i->GetPositionOffset().Y, NodeSizeOffset, NodeSizeOffset, olc::RED);
	}
}

void Grid::UpdateOffsetPosition()
{
	if (bCanUpdateOffsetPosition)
	{
		//add this only once
		if (DebugAllNodes.empty())
			DebugAllNodes.clear();

		//update single node size based on the camera zoom.
		NodeSizeOffset = NodeSize * mainCam->GetZoom();

		//update offside position of the individual nodes
		Node* testedNode = nullptr;

		for (int column = 0; column < GridTotalCellCount.Y; column++)
			for (int row = 0; row < GridTotalCellCount.X; row++)
			{
				testedNode = &Grid2D[column][row];
				testedNode->PositionOffset = (testedNode->Position - mainCam->GetPosition()) * mainCam->GetZoom();

				//add this only once
				if (DebugAllNodes.empty())
					DebugAllNodes.emplace_back(testedNode);
			}
	}
}

void Grid::LoadATileMap(std::string imagePath, PixelMath::Vec2D gridNodeOrigin)
{
	olc::Sprite sceneTemp;
	sceneTemp.LoadFromFile(imagePath);
	auto nodesTemp = GetNodesInTheArea(gridNodeOrigin, { (float)sceneTemp.width, (float)sceneTemp.height });
	int nodeSizeXY = (int)GetNodeSize() + 1;

	for (auto gridCell : nodesTemp)
	{
		// Y axis
		for(int Y = 0; Y < nodeSizeXY; ++Y)
		{
			// X axis
			for( int X = 0; X < nodeSizeXY; ++X )
			{
				//Get X and Y position of the grid cell
				int PosX = (int)gridCell->Position.X - gridNodeOrigin.X;
				int PosY = (int)gridCell->Position.Y - gridNodeOrigin.Y;

				//load pixel from that position 
				olc::Pixel P = sceneTemp.GetPixel(PosX + X, PosY + Y);
			
				//get tiles/grid nodes in the area from the origin to the scene/sprite size and set there sprites;
				gridCell->GetDecal()->sprite->SetPixel(X, Y, P);
				gridCell->GetDecal()->Update();
			}
		}
	}
}

Grid::Node::Node()
{
}

Grid::Node::~Node()
{
	if (_decal)
	{
		if (_decal->sprite)
			delete _decal->sprite;

		delete _decal;
	}
}

//Crash on bottom corners?? 
PixelMath::Vec2D Grid::Node::GetPosition()
{
	return Position;
}

PixelMath::Vec2D Grid::Node::GetPositionOffset()
{
	return PositionOffset;
}

Grid::Node::Neighbours Grid::Node::GetNeighbours()
{
	return neighbours;
}

std::vector<Grid::Node*> Grid::Node::GetNeighboursInVec()
{
	std::vector<Grid::Node*> NeighboursVec;
	if (neighbours.Top) NeighboursVec.push_back(neighbours.Top);
	if (neighbours.TopLeft) NeighboursVec.push_back(neighbours.TopLeft);
	if (neighbours.TopRight) NeighboursVec.push_back(neighbours.TopRight);

	if (neighbours.Left) NeighboursVec.push_back(neighbours.Left);
	if (neighbours.Right) NeighboursVec.push_back(neighbours.Right);

	if (neighbours.Bottom) NeighboursVec.push_back(neighbours.Bottom);
	if (neighbours.BottomRight) NeighboursVec.push_back(neighbours.BottomRight);
	if (neighbours.BottomLeft) NeighboursVec.push_back(neighbours.BottomLeft);

	return NeighboursVec;
}
