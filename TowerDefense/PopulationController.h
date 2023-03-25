#pragma once
#include <iostream>
#include <fstream>
#include <cassert>
#include <vector>
#include <utility>

//library for json files, namepspace definition
#include "nlohmann/json.hpp"
using json = nlohmann::json;

#include "PopulationMember.h"

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

#define INITIAL_POPULATION_COUNT 2

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
	std::vector<PopulationMember> GetPopulation() { return std::vector<PopulationMember>(population); }

	void UpdateMember(int index, PopulationMember update) { population.at(index) = update; }

	int GetGenerationNumber() { return generationNumber; }

//Member variables
private:
	//The number of generations that have come before this one
	int generationNumber = 0;
	//The population that is currently being worked on
	std::vector<PopulationMember> population;

//Functions
private:
	//Finds the current generation number and loads the population
	bool Initialize();

	std::vector<PopulationMember> GenerateIntialPopulation();

	//Loads a population from a json object
	std::vector<PopulationMember> LoadPopulation(json populationObject);

public:
	//Takes the current population, and outputs the genetically modified offspring
	std::vector<PopulationMember> NextEpoch();

	//Saves the current population to a file
	void ExportCurrentPopulation();
};

