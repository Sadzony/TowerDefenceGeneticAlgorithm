#include "PopulationController.h"
#include <list>

//instance definition
PopulationController* PopulationController::instance = nullptr;
bool PopulationController::Initialize()
{
//Find the most recent pop
	std::string epochDirectory = "epochs_current/";
	json populationData;
	generationNumber = -1;
	while (true)
	{
		std::ifstream epochFile(epochDirectory + "epoch" + to_string(generationNumber+1) + ".json");
		if (!epochFile.good())
			break; //File doesn't exist, end here
		populationData = json::parse(epochFile);
		generationNumber++;
	}
//If this is the first generation, create pop0
	if (generationNumber == -1)
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
	ExportCurrentPopulation();
	//Perform genetic methods on input population
	NextEpoch();

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
	//Initialize list
	std::vector<PopulationMember> loadedPopulation = std::vector<PopulationMember>();
	int iteration = 0;
	//iterate genes
	for (const auto& gene : populationObject.items())
	{
		//Break if loading more than population count
		if (iteration >= POPULATION_COUNT)
			break;
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
		iteration++;
	}
	//Fill missing population members up to Population Count
	for (int i = loadedPopulation.size(); i < POPULATION_COUNT; i++)
	{
		loadedPopulation.push_back(PopulationMember());
	}
	//Sort the members in descending score. Done by copying the data into a list object, so the sort function can be used
	std::list<PopulationMember> populationAsList(loadedPopulation.begin(), loadedPopulation.end());
	populationAsList.sort(PopulationMember::PopulationMemberComparison);
	loadedPopulation = std::vector<PopulationMember>(populationAsList.begin(), populationAsList.end());

	return loadedPopulation;
}


std::vector<PopulationMember> PopulationController::NextEpoch()
{
	generationNumber++;
	//Initialize mating pool
	std::vector<PopulationMember> matingPool = std::vector<PopulationMember>();
	//Initialize output population
	std::vector<PopulationMember> outputPopulation = std::vector<PopulationMember>();

	#if ELITIST == true
	//Output the best chromosome straight to the output population
	outputPopulation.push_back(population.at(0));
	#endif


	std::vector<PopulationMember> inputPopulation = std::vector<PopulationMember>(population);

	//Tournament selection
	#if SELECTION == 0
	while (matingPool.size() < MATING_POOL_COUNT)
	{
		std::list<PopulationMember> selectionGroup = std::list<PopulationMember>();
		//Get n genes from the pool
		for (int i = 0; i < SELECTION_GROUP_SIZE; i++)
		{
			//Get random gene, add to selection group
			int randomIndex = rand() % inputPopulation.size();
			selectionGroup.push_back(inputPopulation.at(randomIndex));
		}
		//pick the best one from selection group
		selectionGroup.sort(PopulationMember::PopulationMemberComparison);
		matingPool.push_back(*selectionGroup.begin());
	}
	//Rank/Trunctating selection
	#elif SELECTION == 1

	//Get top n genes
	std::list<PopulationMember> selectionGroup = std::list<PopulationMember>();
	for (int i = 0; i < SELECTION_GROUP_SIZE; i++)
	{
		selectionGroup.push_back(inputPopulation.at(i));
	}
	bool matingPoolFound = false;
	//Keep adding the genes from selection pool until mating pool filled
	while (!matingPoolFound)
	{
		for (PopulationMember gene : selectionGroup)
		{
			matingPool.push_back(gene);
			if (matingPool.size() > MATING_POOL_COUNT - 1)
			{
				matingPoolFound = true;
				break;
			}
		}
	}
	//Roulette selection
	#elif SELECTION == 2
	//Give each member a fitness ranking
	//give each member selection probability
	//Roll n times to fill mating pool
	#endif

	//population = ??
	return std::vector<PopulationMember>(population);
}

void PopulationController::ExportCurrentPopulation()
{
	//Sort the members in descending score. Done by copying the data into a list object, so the sort function can be used
	std::list<PopulationMember> populationAsList(population.begin(), population.end());
	populationAsList.sort(PopulationMember::PopulationMemberComparison);
	population = std::vector<PopulationMember>(populationAsList.begin(), populationAsList.end());

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
