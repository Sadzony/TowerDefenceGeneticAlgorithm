#include "PopulationController.h"

//instance definition
PopulationController* PopulationController::instance = nullptr;
bool PopulationController::Initialize()
{
//Find the most recent pop
	std::string epochDirectory = "epochs_current/";
	json populationData;
	while (true)
	{
		std::ifstream epochFile(epochDirectory + "epoch" + to_string(generationNumber) + ".json");
		if (!epochFile.good())
			break; //File doesn't exist, end here
		populationData = json::parse(epochFile);
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
	population = LoadPopulation(populationData);
	
	return true;
//Genetic initialization
#elif REPLAY_MODE == false
	population = LoadPopulation(populationData);
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
	//population = ??
	return population;
}

void PopulationController::ExportCurrentPopulation()
{
	std::string epochDirectory = "epochs_current/";
	json populationData;
	
	//iterate each gene in population
	for (int i = 0; i < POPULATION_COUNT; i++)
	{
		json geneData;
		//Save score to a json data object
		geneData["Score"] = population.at(i).score;
		

		//Iterate the tower data to create a chromosome
		int towerCount = population.at(i).towerQueue.size();
		for (int j = 0; j < towerCount; j++)
		{
			//Save DNA to json data object
			json towerData;
			towerData["Type"] = (int)population.at(i).towerQueue.at(j).first;
			towerData["X"] = (int)population.at(i).towerQueue.at(j).second.x;
			towerData["Y"] = (int)population.at(i).towerQueue.at(j).second.y;

			//Add the DNA to the chromosome of the gene
			geneData["Towers"].push_back(towerData);
		}
		//Add the gene to the population data
		populationData["Gene" + to_string(i + 1)] = geneData;
	}

	//Write the json data to the file
	std::ofstream outputFile(epochDirectory + "epoch" + to_string(generationNumber) + ".json");
	if (!outputFile.good())
		return;
	outputFile << std::setw(4) << populationData;
}
