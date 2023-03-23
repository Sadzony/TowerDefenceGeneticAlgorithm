#include "PopulationController.h"
#include "nlohmann/json.hpp"

bool PopulationController::Initialize()
{
//Find the most recent pop
	std::string epochDirectory = "/epochs_current";
	std::ifstream readFile(epochDirectory + to_string(generationNumber) + ".json");
//If no pop found, create pop0


//Replay mode initialization
#if REPLAY_MODE == true
	currentPopulation = LoadPopulation(readFile);
	
	return true;
//Genetic initialization
#elif REPLAY_MODE == false
	inputPopulation = LoadPopulation(readFile);
	//Perform genetic methods on input population


	//Set current population
	generationNumber++;
	currentPopulation = inputPopulation;
	return true;
#endif
}

std::vector<PopulationMember> PopulationController::GenerateIntialPopulation()
{
	std::vector<PopulationMember> population;
	for (int i = 0; i < POPULATION_COUNT; i++)
	{
		population.push_back(PopulationMember());
	}
	return population;
}

std::vector<PopulationMember> PopulationController::LoadPopulation(std::ifstream& populationFile)
{
	return std::vector<PopulationMember>();
}

void PopulationController::ExportCurrentPopulation()
{
}
