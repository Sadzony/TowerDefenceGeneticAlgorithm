#pragma once
#include <deque>
#include <unordered_map>

#include "Tower.h"
#include "BreadthSearchDirections.h"

class PositionBoard
{
private:
	int gridStatus[32][18] = { 0 };
public:
	PositionBoard();
	bool gridSpaceAvailable(sf::Vector2i gridPos);
	bool AddTower(sf::Vector2i gridPos);

	void RemoveTower(sf::Vector2i gridPos);

	//Breadth search around a tile in a random direction
	sf::Vector2i FindClosestAvailableTile(sf::Vector2i gridPos);

	//Controlled Breadth search around a tile
	sf::Vector2i FindClosestAvailableTile(sf::Vector2i gridPos, BreadthSearchStartTile searchStart, BreadthSearchDirection searchDirection);

	//Outputs the positions of neighbouring tiles
	std::vector<sf::Vector2i> FindNeighbourPositions(sf::Vector2i gridPos);

	//Outputs the positions of neighbouring tiles in a controlled direction
	std::vector<sf::Vector2i> FindNeighbourPositions(sf::Vector2i gridPos, BreadthSearchStartTile searchStart, BreadthSearchDirection searchDirection);
};

