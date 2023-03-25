#include "AIController.h"
#include "Timer.h"
#include "GameState.h"
#include <iostream>
#include <Windows.h>

#include "GameController.h"

#define AI_CONTROL true

using namespace std;
double g_elapsedSeconds = 0;

AIController::AIController()
{
	m_gene = PopulationMember();
	m_gameController = nullptr;
	m_gameBoard = nullptr;
	m_Timer = nullptr;
	m_gameState = nullptr;
	g_elapsedSeconds = 0;
}

AIController::~AIController()
{

}

void AIController::gameOver()
{
	g_elapsedSeconds = 0;
	towerLog.clear();
}

PopulationMember AIController::outputGene()
{
	m_gene.score = m_gameState->getScore();
	m_gene.towerQueue = std::deque<TowerInPosition>(towerLog);
	return m_gene;
}

void AIController::update()
{
	if (m_Timer == nullptr)
		return;

	// HINT
	double seconds = floor(m_Timer->elapsedSeconds());
	if (seconds > g_elapsedSeconds)
	{
		// a second has elapsed - your GA manager (GA Code) may decide to do something at this time...
		g_elapsedSeconds = seconds;

	}


	//GAManager::Instance()->Update(m_Timer->elapsedSeconds());


	#if AI_CONTROL == true
	//set next tower to empty tower
	TowerInPosition nextTower = std::make_pair(TowerType::empty, sf::Vector2i());

	//check queue for next tower
	if (!m_gene.towerQueue.empty())
		nextTower = m_gene.towerQueue.front();
	//if queue is empty, make a random tower
	else
	{
		//Find closest available tile around a random position
		int xPos = rand() % 26;
		int yPos = rand() % 18;
		sf::Vector2i towerPos = m_gameBoard->FindClosestAvailableTile(sf::Vector2i(xPos, yPos));

		//Randomize tower type
		TowerType type = (TowerType)(rand() % 3 + 1);
		nextTower = std::make_pair(type, towerPos);
		m_gene.towerQueue.push_back(nextTower);
	}
	addTower(nextTower.first, nextTower.second.x, nextTower.second.y);
		//If the tower was successfully placed, add it to log and pop it out of the queue.
	#endif

	recordScore();
}

bool AIController::addTower(TowerType type, int gridx, int gridy)
{
	// grid position can be from 0,0 to 25,17
	/*
	enum class TowerType {
	empty, slammer, swinger, thrower };
	*/

	bool towerAdded = m_gameBoard->addTower(type, gridx, gridy);
	if (towerAdded)
	{
		towerLog.push_back(m_gene.towerQueue.front());
		if(!m_gene.towerQueue.empty())
			m_gene.towerQueue.pop_front();
	}
	return towerAdded;
	// NOTE towerAdded might be false if the tower can't be placed in that position, is there isn't enough funds
}

void AIController::setupBoard()
{
	m_Timer->start();
}

int AIController::recordScore()
{
	int currentWave = m_gameState->getCurrentWave();
	int killCount = m_gameState->getMonsterEliminated();
	currentWave *= 10; // living longer is good
	int score = currentWave + killCount;

	static int iteration = 0;

	if (iteration == 0)
		//cout << "iteration" << "," << "wave" << "," << "kills" << "," << "score" << endl;

	//cout << iteration << "," << m_gameState->getCurrentWave() << "," << m_gameState->getMonsterEliminated() << "," << score << endl;
	iteration++;

	m_gameState->setScore(score);

	return score;
}