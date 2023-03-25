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
	for (int i = 0; i < INITIAL_POPULATION_COUNT; i++)
	{
		population.push_back(PopulationMember());
	}
	return population;
}

std::vector<PopulationMember> PopulationController::LoadPopulation(nlohmann::json populationObject)
{
	//Initialize list
	std::vector<PopulationMember> loadedPopulation = std::vector<PopulationMember>();

	//iterate genes
	for (const auto& gene : populationObject.items())
	{
		//Gene1, Gene2...

		//Initialize variables
		int score = 0;
		std::deque<TowerInPosition> towerQueue = std::deque<TowerInPosition>();

		//iterate data
		for (const auto& geneData : gene.value().items())
		{
			//Score, Towers

			//Get score
			if (geneData.key() == "Score")
				score = geneData.value();
			
			//Get chromosome
			else if (geneData.key() == "Towers")
			{
				//Iterate chromosome
				for (const auto& chromosome : geneData.value().items())
				{
					//Tower1, tower2....
					TowerType nextType = TowerType::empty;
					int nextX = -1;
					int nextY = -1;
					//iterate tower data
					for (const auto& towerData : chromosome.value().items())
					{
						if (towerData.key() == "Type")
							nextType = (TowerType)towerData.value();
						else if (towerData.key() == "X")
							nextX = towerData.value();
						else if (towerData.key() == "Y")
							nextY = towerData.value();
					}
					//Add tower to queue
					towerQueue.push_back(std::make_pair(nextType, sf::Vector2i(nextX, nextY)));
				}
			}
		}
		//Add gene to list
		loadedPopulation.push_back(PopulationMember(towerQueue, score));
	}
	return loadedPopulation;
}


std::vector<PopulationMember> PopulationController::NextEpoch()
{
	generationNumber++;
	//population = ??
	return std::vector<PopulationMember>(population);
}

void PopulationController::ExportCurrentPopulation()
{
	std::string epochDirectory = "epochs_current/";
	json populationData;
	
	//iterate each gene in population
	for (int i = 0; i < INITIAL_POPULATION_COUNT; i++)
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
