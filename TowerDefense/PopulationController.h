#pragma once
#include <iostream>
#include <fstream>
#include <cassert>
#include <vector>
#include <utility>
#include <deque>

#include "Tower.h"

//Replay mode finds the most recent pop and showcases the result without running the genetic algorithm
#define REPLAY_MODE true

enum SelectionMethod
{
	Tournament,
	Roulette
};
enum CrossoverMethod
{
	One_Point,
	Uniform
};

#define POPULATION_COUNT 15

//Defines the chosen selection method
#define SELECTION Tournament
//Turns elitist selection on and off
#define ELITIST false

//Defines the crossover method
#define CROSSOVER Uniform

//Turns mutation on and off
#define MUTATION false

//Percentage of offspring that undergoes mutation
#define MUTATION_RATE 0.05f

typedef std::pair<TowerType, sf::Vector2i> TowerInPosition;
struct PopulationMember
{
	PopulationMember(std::deque<TowerInPosition> p_towerQueue, int p_score) : towerQueue(p_towerQueue), score(p_score) {}
	PopulationMember() { towerQueue = std::deque<TowerInPosition>(); }
	std::deque<TowerInPosition> towerQueue;
	int score = 0;
};

class PopulationController
{
	//Singleton pattern
public:
	
	static PopulationController* getInstance()
	{
		//If instance doesnt exist, create one
		if (!instance)
			instance = new PopulationController();
		return instance;
	}
private:
	//Private constructor for protection
	PopulationController() { bool init = Initialize(); assert(init == true); };
	static PopulationController* instance;

//Get sets
public:
	//Returns a copy of the population list
	std::vector<PopulationMember> GetPopulation() { return std::vector<PopulationMember>(currentPopulation); }

	void SetScore(int index, int score) { currentPopulation.at(index).score = score; }

//Member variables
private:
	//The number of generations that have come before this one
	int generationNumber = 0;
	//The population used for the genetic algorithm
	std::vector<PopulationMember> inputPopulation;
	//The population that is currently being worked on
	std::vector<PopulationMember> currentPopulation;

//Functions
private:
	//Finds the current generation number and loads the population
	bool Initialize();

	std::vector<PopulationMember> GenerateIntialPopulation();

	//Loads a population from a file
	std::vector<PopulationMember> LoadPopulation(std::ifstream& populationFile);

	//Saves the current population to a file
	void ExportCurrentPopulation();
};

