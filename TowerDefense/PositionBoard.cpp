#include "PositionBoard.h"



using sf::Vector2f;

const std::vector<Vector2f> path = { Vector2f(15, 0), Vector2f(15, 4), Vector2f(
		20, 4), Vector2f(22, 4), Vector2f(22, 8), Vector2f(10, 8), Vector2f(10,
		6), Vector2f(4, 6), Vector2f(4, 13), Vector2f(12, 13), Vector2f(12, 15),
		Vector2f(17, 15), Vector2f(17, 12), Vector2f(21, 12), Vector2f(21, 18) };

PositionBoard::PositionBoard()
{
	//Load path into grid status
	for (unsigned int i = 0; i < path.size() - 1; i++) {
		sf::Vector2i curr = sf::Vector2i(path[i]);		// Current Vector2f
		sf::Vector2i next = sf::Vector2i(path[i + 1]);		// Next Vector2f
		if (curr.x == next.x) { // If two Vector2fs vertical			
			while (curr.y != next.y) {
				if (curr.y < next.y)
					curr.y++;		// Draw up or down until next Vector2f
				else
					curr.y--;
				gridStatus[curr.x][curr.y] = 1;
			}

		} else if (curr.y == next.y) { // If two Vector2fs horizontal
			while (curr.x != next.x) {
				if (curr.x < next.x)
					curr.x++;		// Draw left or right until next Vector2f
				else
					curr.x--;
				gridStatus[curr.x][curr.y] = 1;
			}
		}
	}
}

bool PositionBoard::gridSpaceAvailable(sf::Vector2i gridPos)
{
	if (gridStatus[gridPos.x][gridPos.y] == 0 && gridStatus[gridPos.x + 1][gridPos.y] == 0
		&& gridStatus[gridPos.x][gridPos.y + 1] == 0
		&& gridStatus[gridPos.x + 1][gridPos.y + 1] == 0) {
		return true;
	}
	else
		return false;
}

bool PositionBoard::AddTower(sf::Vector2i gridPos)
{
	if (gridSpaceAvailable(gridPos))
	{
		gridStatus[gridPos.x][gridPos.y] = 2;
		gridStatus[gridPos.x + 1][gridPos.y] = 2;
		gridStatus[gridPos.x][gridPos.y + 1] = 2;
		gridStatus[gridPos.x + 1][gridPos.y + 1] = 2;
		return true;
	}
	return false;
}

void PositionBoard::RemoveTower(sf::Vector2i gridPos)
{
	gridStatus[gridPos.x][gridPos.y] = 0;
	gridStatus[gridPos.x + 1][gridPos.y] = 0;
	gridStatus[gridPos.x][gridPos.y + 1] = 0;
	gridStatus[gridPos.x + 1][gridPos.y + 1] = 0;
}

sf::Vector2i PositionBoard::FindClosestAvailableTile(sf::Vector2i gridPos)
{
	BreadthSearchStartTile randomStartTile = (BreadthSearchStartTile)(rand() % 4);
	BreadthSearchDirection randomDirection = (BreadthSearchDirection)(rand() % 2);
	return FindClosestAvailableTile(gridPos, randomStartTile, randomDirection);
}

sf::Vector2i PositionBoard::FindClosestAvailableTile(sf::Vector2i gridPos, BreadthSearchStartTile searchStart, BreadthSearchDirection searchDirection)
{
	//Stores the visited positions
	std::unordered_map<int, std::unordered_map<int, bool>> visitMap;

	//Sorts the search into an order
	std::deque<sf::Vector2i> searchQueue;

	//Push start nodes into the data structures
	visitMap[gridPos.x][gridPos.y] = true;
	searchQueue.push_back(gridPos);

	//Search. Break when found a suitable node
	while (!searchQueue.empty())
	{
		//Get search position and remove from queue
		int x = searchQueue.front().x;
		int y = searchQueue.front().y;
		sf::Vector2i searchPos = sf::Vector2i(x, y);
		searchQueue.pop_front();
		//Check if the tower fits in this position, and return it if it does
		if (gridSpaceAvailable(searchPos))
			return searchPos;

		//Get neighbours
		std::vector<sf::Vector2i> neighbours = FindNeighbourPositions(searchPos, searchStart, searchDirection);
		for (sf::Vector2i neighbourPosition : neighbours)
		{
			//Check that the neighbour wasn't already visited
			if (!(visitMap.find(neighbourPosition.x) != visitMap.end() && visitMap[neighbourPosition.x].find(neighbourPosition.y) != visitMap[neighbourPosition.x].end()))
			{
				//Visit the unvisited neighbour, and add it onto the search queue
				visitMap[neighbourPosition.x][neighbourPosition.y] = true;
				searchQueue.push_back(neighbourPosition);
			}
		}
	}
}

std::vector<sf::Vector2i> PositionBoard::FindNeighbourPositions(sf::Vector2i gridPos)
{
	BreadthSearchStartTile randomStartTile = (BreadthSearchStartTile)(rand() % 4);
	BreadthSearchDirection randomDirection = (BreadthSearchDirection)(rand() % 2);
	return FindNeighbourPositions(gridPos, randomStartTile, randomDirection);
}

std::vector<sf::Vector2i> PositionBoard::FindNeighbourPositions(sf::Vector2i gridPos, BreadthSearchStartTile searchStart, BreadthSearchDirection searchDirection)
{
	std::vector<sf::Vector2i> neighbourPositions = std::vector<sf::Vector2i>();
	if (searchStart == BreadthSearchStartTile::LEFT)
	{
		//Add left Neighbour
		if (gridPos.x - 1 >= 0)
			neighbourPositions.push_back(gridPos + sf::Vector2i(-1, 0));
		if (searchDirection == BreadthSearchDirection::CLOCKWISE)
		{
			//Add top left
			if (gridPos.x - 1 >= 0 && gridPos.y + 1 < 18)
				neighbourPositions.push_back(gridPos + sf::Vector2i(-1, 1));
			//Add top
			if (gridPos.y + 1 < 18)
				neighbourPositions.push_back(gridPos + sf::Vector2i(0, 1));
			//Add top right
			if (gridPos.x + 1 < 26 && gridPos.y + 1 < 18)
				neighbourPositions.push_back(gridPos + sf::Vector2i(1, 1));
			//Add right
			if (gridPos.x + 1 < 26)
				neighbourPositions.push_back(gridPos + sf::Vector2i(1, 0));
			//Add bottom right
			if (gridPos.x + 1 < 26 && gridPos.y - 1 >= 0)
				neighbourPositions.push_back(gridPos + sf::Vector2i(1, -1));
			//Add bottom
			if (gridPos.y - 1 >= 0)
				neighbourPositions.push_back(gridPos + sf::Vector2i(0, -1));
			//Add bottom left
			if (gridPos.x - 1 >= 0 && gridPos.y - 1 >= 0)
				neighbourPositions.push_back(gridPos + sf::Vector2i(-1, -1));
		}
		else if (searchDirection == BreadthSearchDirection::ANTICLOCKWISE)
		{
			//Add bottom left
			if (gridPos.x - 1 >= 0 && gridPos.y - 1 >= 0)
				neighbourPositions.push_back(gridPos + sf::Vector2i(-1, -1));
			//Add bottom
			if (gridPos.y - 1 >= 0)
				neighbourPositions.push_back(gridPos + sf::Vector2i(0, -1));
			//Add bottom right
			if (gridPos.x + 1 < 26 && gridPos.y - 1 >= 0)
				neighbourPositions.push_back(gridPos + sf::Vector2i(1, -1));
			//Add right
			if (gridPos.x + 1 < 26)
				neighbourPositions.push_back(gridPos + sf::Vector2i(1, 0));
			//Add top right
			if (gridPos.x + 1 < 26 && gridPos.y + 1 < 18)
				neighbourPositions.push_back(gridPos + sf::Vector2i(1, 1));
			//Add top
			if (gridPos.y + 1 < 18)
				neighbourPositions.push_back(gridPos + sf::Vector2i(0, 1));
			//Add top left
			if (gridPos.x - 1 >= 0 && gridPos.y + 1 < 18)
				neighbourPositions.push_back(gridPos + sf::Vector2i(-1, 1));
		}
	}
	else if (searchStart == BreadthSearchStartTile::UP)
	{
		//Add top Neighbour
		if (gridPos.y + 1 < 18)
			neighbourPositions.push_back(gridPos + sf::Vector2i(0, 1));
		if (searchDirection == BreadthSearchDirection::CLOCKWISE)
		{
			//Add top right
			if (gridPos.x + 1 < 26 && gridPos.y + 1 < 18)
				neighbourPositions.push_back(gridPos + sf::Vector2i(1, 1));
			//Add right
			if (gridPos.x + 1 < 26)
				neighbourPositions.push_back(gridPos + sf::Vector2i(1, 0));
			//Add bottom right
			if (gridPos.x + 1 < 26 && gridPos.y - 1 >= 0)
				neighbourPositions.push_back(gridPos + sf::Vector2i(1, -1));
			//Add bottom
			if (gridPos.y - 1 >= 0)
				neighbourPositions.push_back(gridPos + sf::Vector2i(0, -1));
			//Add bottom left
			if (gridPos.x - 1 >= 0 && gridPos.y - 1 >= 0)
				neighbourPositions.push_back(gridPos + sf::Vector2i(-1, -1));
			//Add left
			if (gridPos.x - 1 >= 0)
				neighbourPositions.push_back(gridPos + sf::Vector2i(-1, 0));
			//Add top left
			if (gridPos.x - 1 >= 0 && gridPos.y + 1 < 18)
				neighbourPositions.push_back(gridPos + sf::Vector2i(-1, 1));
		}
		else if (searchDirection == BreadthSearchDirection::ANTICLOCKWISE)
		{
			//Add top left
			if (gridPos.x - 1 >= 0 && gridPos.y + 1 < 18)
				neighbourPositions.push_back(gridPos + sf::Vector2i(-1, 1));
			//Add left
			if (gridPos.x - 1 >= 0)
				neighbourPositions.push_back(gridPos + sf::Vector2i(-1, 0));
			//Add bottom left
			if (gridPos.x - 1 >= 0 && gridPos.y - 1 >= 0)
				neighbourPositions.push_back(gridPos + sf::Vector2i(-1, -1));
			//Add bottom
			if (gridPos.y - 1 >= 0)
				neighbourPositions.push_back(gridPos + sf::Vector2i(0, -1));
			//Add bottom right
			if (gridPos.x + 1 < 26 && gridPos.y - 1 >= 0)
				neighbourPositions.push_back(gridPos + sf::Vector2i(1, -1));
			//Add right
			if (gridPos.x + 1 < 26)
				neighbourPositions.push_back(gridPos + sf::Vector2i(1, 0));
			//Add top right
			if (gridPos.x + 1 < 26 && gridPos.y + 1 < 18)
				neighbourPositions.push_back(gridPos + sf::Vector2i(1, 1));
		}
	}
	else if (searchStart == BreadthSearchStartTile::RIGHT)
	{
		//Add right Neighbour
		if (gridPos.x + 1 < 26)
			neighbourPositions.push_back(gridPos + sf::Vector2i(1, 0));
		if (searchDirection == BreadthSearchDirection::CLOCKWISE)
		{
			//Add bottom right
			if (gridPos.x + 1 < 26 && gridPos.y - 1 >= 0)
				neighbourPositions.push_back(gridPos + sf::Vector2i(1, -1));
			//Add bottom
			if (gridPos.y - 1 >= 0)
				neighbourPositions.push_back(gridPos + sf::Vector2i(0, -1));
			//Add bottom left
			if (gridPos.x - 1 >= 0 && gridPos.y - 1 >= 0)
				neighbourPositions.push_back(gridPos + sf::Vector2i(-1, -1));
			//Add left
			if (gridPos.x - 1 >= 0)
				neighbourPositions.push_back(gridPos + sf::Vector2i(-1, 0));
			//Add top left
			if (gridPos.x - 1 >= 0 && gridPos.y + 1 < 18)
				neighbourPositions.push_back(gridPos + sf::Vector2i(-1, 1));
			//Add top
			if (gridPos.y + 1 < 18)
				neighbourPositions.push_back(gridPos + sf::Vector2i(0, 1));
			//Add top right
			if (gridPos.x + 1 < 26 && gridPos.y + 1 < 18)
				neighbourPositions.push_back(gridPos + sf::Vector2i(1, 1));
		}
		else if (searchDirection == BreadthSearchDirection::ANTICLOCKWISE)
		{
			//Add top right
			if (gridPos.x + 1 < 26 && gridPos.y + 1 < 18)
				neighbourPositions.push_back(gridPos + sf::Vector2i(1, 1));
			//Add top
			if (gridPos.y + 1 < 18)
				neighbourPositions.push_back(gridPos + sf::Vector2i(0, 1));
			//Add top left
			if (gridPos.x - 1 >= 0 && gridPos.y + 1 < 18)
				neighbourPositions.push_back(gridPos + sf::Vector2i(-1, 1));
			//Add left
			if (gridPos.x - 1 >= 0)
				neighbourPositions.push_back(gridPos + sf::Vector2i(-1, 0));
			//Add bottom left
			if (gridPos.x - 1 >= 0 && gridPos.y - 1 >= 0)
				neighbourPositions.push_back(gridPos + sf::Vector2i(-1, -1));
			//Add bottom
			if (gridPos.y - 1 >= 0)
				neighbourPositions.push_back(gridPos + sf::Vector2i(0, -1));
			//Add bottom right
			if (gridPos.x + 1 < 26 && gridPos.y - 1 >= 0)
				neighbourPositions.push_back(gridPos + sf::Vector2i(1, -1));
		}
	}
	else if (searchStart == BreadthSearchStartTile::DOWN)
	{
		//add bottom neighbour
		if (gridPos.y - 1 >= 0)
			neighbourPositions.push_back(gridPos + sf::Vector2i(0, -1));
		if (searchDirection == BreadthSearchDirection::CLOCKWISE)
		{
			//Add bottom left
			if (gridPos.x - 1 >= 0 && gridPos.y - 1 >= 0)
				neighbourPositions.push_back(gridPos + sf::Vector2i(-1, -1));
			//Add left
			if (gridPos.x - 1 >= 0)
				neighbourPositions.push_back(gridPos + sf::Vector2i(-1, 0));
			//Add top left
			if (gridPos.x - 1 >= 0 && gridPos.y + 1 < 18)
				neighbourPositions.push_back(gridPos + sf::Vector2i(-1, 1));
			//Add top
			if (gridPos.y + 1 < 18)
				neighbourPositions.push_back(gridPos + sf::Vector2i(0, 1));
			//Add top right
			if (gridPos.x + 1 < 26 && gridPos.y + 1 < 18)
				neighbourPositions.push_back(gridPos + sf::Vector2i(1, 1));
			//Add right
			if (gridPos.x + 1 < 26)
				neighbourPositions.push_back(gridPos + sf::Vector2i(1, 0));
			//Add bottom right
			if (gridPos.x + 1 < 26 && gridPos.y - 1 >= 0)
				neighbourPositions.push_back(gridPos + sf::Vector2i(1, -1));
		}
		else if (searchDirection == BreadthSearchDirection::ANTICLOCKWISE)
		{
			//Add bottom right
			if (gridPos.x + 1 < 26 && gridPos.y - 1 >= 0)
				neighbourPositions.push_back(gridPos + sf::Vector2i(1, -1));
			//Add right
			if (gridPos.x + 1 < 26)
				neighbourPositions.push_back(gridPos + sf::Vector2i(1, 0));
			//Add top right
			if (gridPos.x + 1 < 26 && gridPos.y + 1 < 18)
				neighbourPositions.push_back(gridPos + sf::Vector2i(1, 1));
			//Add top
			if (gridPos.y + 1 < 18)
				neighbourPositions.push_back(gridPos + sf::Vector2i(0, 1));
			//Add top left
			if (gridPos.x - 1 >= 0 && gridPos.y + 1 < 18)
				neighbourPositions.push_back(gridPos + sf::Vector2i(-1, 1));
			//Add left
			if (gridPos.x - 1 >= 0)
				neighbourPositions.push_back(gridPos + sf::Vector2i(-1, 0));
			//Add bottom left
			if (gridPos.x - 1 >= 0 && gridPos.y - 1 >= 0)
				neighbourPositions.push_back(gridPos + sf::Vector2i(-1, -1));
		}
	}
	return neighbourPositions;
}
