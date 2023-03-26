#pragma once
#include "Tower.h"
#include <deque>
typedef std::pair<TowerType, sf::Vector2i> TowerInPosition;
struct PopulationMember
{
	PopulationMember(std::deque<TowerInPosition> p_towerQueue, int p_score) : towerQueue(p_towerQueue), score(p_score) {}
	PopulationMember() { towerQueue = std::deque<TowerInPosition>(); }
	std::deque<TowerInPosition> towerQueue;
	int score = 0;

	//Function used to sort the members Descending
	static bool PopulationMemberComparison(const PopulationMember& first, const PopulationMember& second)
	{
		return (first.score > second.score);
	}
};