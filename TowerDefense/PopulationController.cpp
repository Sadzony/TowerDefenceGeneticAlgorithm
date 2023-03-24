#include "PopulationController.h"

//instance definition
PopulationController* PopulationController::instance = nullptr;
bool PopulationController::Initialize()
{
//Find the most recent pop
	std::string epochDirectory = "epochs_current/";
	json popObject;
	while (true)
	{
		std::ifstream f(epochDirectory + "epoch" + to_string(generationNumber) + ".json");
		if (!f.good())
			break; //File doesn't exist, end here
		popObject = json::parse(f);
		generationNumber++;
	}
//If this is the first generation, create pop0
	if (generationNumber == 0)
	{
		population = GenerateIntialPopulation();
		return true;
	}

//Replay mode initialization
#if REPLAY_MODE == true
	population = LoadPopulation(popObject);
	
	return true;
//Genetic initialization
#elif REPLAY_MODE == false
	population = LoadPopulation(readFile);
	//Perform genetic methods on input population
	NextEpochNoExport();

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

std::vector<PopulationMember> PopulationController::LoadPopulation(nlohmann::json populationObject)
{
	return std::vector<PopulationMember>();
}


std::vector<PopulationMember> PopulationController::NextEpoch()
{
	generationNumber++;
	return population;
}

void PopulationController::ExportCurrentPopulation()
{
}
