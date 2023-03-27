#include "PopulationController.h"
#include <list>
#include "PositionBoard.h"

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
		generationNumber++;
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
	population = NextEpoch();
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
			if (matingPool.size() >= MATING_POOL_COUNT)
			{
				matingPoolFound = true;
				break;
			}
		}
	}
	//Roulette selection
#elif SELECTION == 2
//find total score
	int totalScore = 0;
	for (PopulationMember gene : inputPopulation)
		totalScore += gene.score;
	//spin the wheel n times to fill the mating pool
	while (matingPool.size() < MATING_POOL_COUNT)
	{
		//value between 0 and sum of scores
		int roulette = rand() % (totalScore + 1);
		int range_min = 0;
		//iterate the population and find where the roulette landed
		for (PopulationMember gene : inputPopulation)
		{
			if (roulette <= range_min + gene.score)
			{
				matingPool.push_back(gene);
				break;
			}
			else
				range_min += gene.score;
		}
	}
#endif

	//The mating pool has been created. Perform crossover
	
	//Create offspring until output population is full
	while (outputPopulation.size() < POPULATION_COUNT)
	{
		//Pick 2 parents from the input population
		PopulationMember parent1, parent2;
		int parent1Index = rand() % POPULATION_COUNT;
		parent1 = inputPopulation.at(parent1Index);

		//Ensuring that the second parent is not the same as first parent
		int parent2Index = parent1Index;
		while (parent1Index == parent2Index)
			parent2Index = rand() % POPULATION_COUNT;
		parent2 = inputPopulation.at(parent2Index);
		outputPopulation.push_back(Crossover(parent1, parent2));
	}

	//Perform mutation
	#if MUTATION == true
	
	//iterate output population
	for (int i = 0; i < outputPopulation.size(); i++)
	{
		//Don't mutate the first member if elitist selection is on
		#if ELITIST == true
		if (i == 0)
			continue;
		#endif
		//number between 1 and 100
		int mutation = 1 + (rand() % 100);
		if (mutation < MUTATION_RATE)
		{
			PositionBoard geneBoard = PositionBoard();
			std::deque<TowerInPosition> towerQueue = outputPopulation.at(i).towerQueue;
			for (const TowerInPosition& tower : towerQueue)
				geneBoard.AddTower(tower.second);

			for (int i = 0; i < MUTATION_COUNT; i++) {
				//pick a mutation method at random
				int method = rand() % 2;
					
				//Swapper
				if(method == 0 && towerQueue.size() > 1)
				{
					//pick 2 DNA positions
					int dnaIndex1 = rand() % towerQueue.size();
					int dnaIndex2 = dnaIndex1;
					while (dnaIndex2 == dnaIndex1)
						dnaIndex2 = rand() % towerQueue.size();

					//swap their elements
					std::swap(towerQueue[dnaIndex1], towerQueue[dnaIndex2]);
				}
				//Value randomizer
				else
				{
					//Tower type or tower position
					int chromosome = rand() % 2;
					//Random DNA position
					int dnaIndex = rand() % towerQueue.size();
					if (chromosome == 0)
					{
						TowerType type = (TowerType)(1 + (rand() % 3));
						towerQueue.at(dnaIndex).first = type;
					}
					else
					{
						geneBoard.RemoveTower(towerQueue.at(dnaIndex).second);
						int xPos = rand() % 26;
						int yPos = rand() % 18;
						sf::Vector2i newPos = geneBoard.FindClosestAvailableTile(sf::Vector2i(xPos, yPos));
						geneBoard.AddTower(newPos);
						towerQueue.at(dnaIndex).second = newPos;
					}
				}

			}
			outputPopulation.at(i).towerQueue = towerQueue;
		}
	}

	#endif
	population = outputPopulation;
	return std::vector<PopulationMember>(population);
}
PopulationMember PopulationController::Crossover(const PopulationMember parent1, const PopulationMember parent2)
{
	PopulationMember offspring = PopulationMember();

	//Identify the member that has the longer chromosome. If they're the same it doesn't matter.
	const PopulationMember* longerChromosomeMember;
	const PopulationMember* shorterChromosomeMember;
	int longerChromosomeSize, shorterChromosomeSize;
	if (parent1.towerQueue.size() > parent2.towerQueue.size())
	{
		longerChromosomeMember = &parent1;
		longerChromosomeSize = parent1.towerQueue.size();
		shorterChromosomeMember = &parent2;
		shorterChromosomeSize = parent2.towerQueue.size();
	}
	else
	{
		longerChromosomeMember = &parent2;
		longerChromosomeSize = parent2.towerQueue.size();
		shorterChromosomeMember = &parent1;
		shorterChromosomeSize = parent1.towerQueue.size();
	}

	PositionBoard board = PositionBoard();

	//4 combinations are available. This determines which of them is chosen
	int offspringScenario = rand() % 4;

	//Point crossover
	#if CROSSOVER == 0

	//Find crossover points. They are placed at a random position within the shorter chromosome. Range 1 to n-1
	int typeCrossoverPoint = 1 + (rand() % shorterChromosomeSize);
	int positionCrossoverPoint = 1 + (rand() % shorterChromosomeSize);;

	//The offspring will be built up to the length of the longer chromosome
	//Iterate up to length
	for (int i = 0; i < longerChromosomeSize; i++)
	{
		TowerType nextType;
		sf::Vector2i nextPosition;
		TowerInPosition nextTower;
		//check if shorter chromosme is still accessible by i
		if (i < shorterChromosomeSize)
		{
			switch (offspringScenario)
			{
			//pos: 0 | 1, type: 0 | 1
			case 0:
				//Find next tower type
				if (i < typeCrossoverPoint)
				{
					nextType = longerChromosomeMember->towerQueue.at(i).first;
				}
				else
				{
					nextType = shorterChromosomeMember->towerQueue.at(i).first;
				}
				//find next tower position
				if (i < positionCrossoverPoint)
				{
					nextPosition = longerChromosomeMember->towerQueue.at(i).second;
				}
				else
				{
					nextPosition = shorterChromosomeMember->towerQueue.at(i).second;
				}
				nextTower = std::make_pair(nextType, nextPosition);
				//Check if the tower will fit in the position. If not, correct it
				if (board.gridSpaceAvailable(nextTower.second))
				{
					board.AddTower(nextTower.second);
					offspring.towerQueue.push_back(nextTower);
				}
				else
				{
					nextTower.second = board.FindClosestAvailableTile(nextTower.second);
					board.AddTower(nextTower.second);
					offspring.towerQueue.push_back(nextTower);
				}
				break;
			//pos: 1 | 0, type: 1 | 0
			case 1:
				//Find next tower type
				if (i < typeCrossoverPoint)
				{
					nextType = shorterChromosomeMember->towerQueue.at(i).first;
				}
				else
				{
					nextType = longerChromosomeMember->towerQueue.at(i).first;
				}
				//find next tower position
				if (i < positionCrossoverPoint)
				{
					nextPosition = shorterChromosomeMember->towerQueue.at(i).second;
				}
				else
				{
					nextPosition = longerChromosomeMember->towerQueue.at(i).second;
				}
				nextTower = std::make_pair(nextType, nextPosition);
				//Check if the tower will fit in the position. If not, correct it
				if (board.gridSpaceAvailable(nextTower.second))
				{
					board.AddTower(nextTower.second);
					offspring.towerQueue.push_back(nextTower);
				}
				else
				{
					nextTower.second = board.FindClosestAvailableTile(nextTower.second);
					board.AddTower(nextTower.second);
					offspring.towerQueue.push_back(nextTower);
				}
				break;
			//pos: 1 | 0, type: 0 | 1
			case 2:
				//Find next tower type
				if (i < typeCrossoverPoint)
				{
					nextType = shorterChromosomeMember->towerQueue.at(i).first;
				}
				else
				{
					nextType = longerChromosomeMember->towerQueue.at(i).first;
				}
				//find next tower position
				if (i < positionCrossoverPoint)
				{
					nextPosition = longerChromosomeMember->towerQueue.at(i).second;
				}
				else
				{
					nextPosition = shorterChromosomeMember->towerQueue.at(i).second;
				}
				nextTower = std::make_pair(nextType, nextPosition);
				//Check if the tower will fit in the position. If not, correct it
				if (board.gridSpaceAvailable(nextTower.second))
				{
					board.AddTower(nextTower.second);
					offspring.towerQueue.push_back(nextTower);
				}
				else
				{
					nextTower.second = board.FindClosestAvailableTile(nextTower.second);
					board.AddTower(nextTower.second);
					offspring.towerQueue.push_back(nextTower);
				}
				break;
			//pos: 0 | 1, type: 1 | 0
			case 3:
				//Find next tower type
				if (i < typeCrossoverPoint)
				{
					nextType = longerChromosomeMember->towerQueue.at(i).first;
				}
				else
				{
					nextType = shorterChromosomeMember->towerQueue.at(i).first;
				}
				//find next tower position
				if (i < positionCrossoverPoint)
				{
					nextPosition = shorterChromosomeMember->towerQueue.at(i).second;
				}
				else
				{
					nextPosition = longerChromosomeMember->towerQueue.at(i).second;
				}
				nextTower = std::make_pair(nextType, nextPosition);
				//Check if the tower will fit in the position. If not, correct it
				if (board.gridSpaceAvailable(nextTower.second))
				{
					board.AddTower(nextTower.second);
					offspring.towerQueue.push_back(nextTower);
				}
				else
				{
					nextTower.second = board.FindClosestAvailableTile(nextTower.second);
					board.AddTower(nextTower.second);
					offspring.towerQueue.push_back(nextTower);
				}
				break;
			}
		}
		//Otherwise, fill with longer DNA
		else
		{
			TowerInPosition nextTower = longerChromosomeMember->towerQueue.at(i);
			//Check if the tower will fit in the position
			if (board.gridSpaceAvailable(nextTower.second))
			{
				board.AddTower(nextTower.second);
				offspring.towerQueue.push_back(nextTower);
			}
			else
			{
				nextTower.second = board.FindClosestAvailableTile(nextTower.second);
				board.AddTower(nextTower.second);
				offspring.towerQueue.push_back(nextTower);
			}
		}
	}

	//Uniform crossover
	#elif CROSSOVER == 1


	//The offspring will be built up to the length of the longer chromosome
	//Iterate up to length
	for (int i = 0; i < longerChromosomeSize; i++)
	{
		TowerType nextType;
		sf::Vector2i nextPosition;
		TowerInPosition nextTower;
		//check if shorter chromosme is still accessible by i
		if (i < shorterChromosomeSize)
		{
			switch (offspringScenario)
			{
				//pos: 0101, type: 0101
			case 0:
				//Find next tower from parents

				if (i % 2 == 0)
				{
					nextTower = longerChromosomeMember->towerQueue.at(i);
				}
				else
				{
					nextTower = shorterChromosomeMember->towerQueue.at(i);
				}
				//Check if the tower will fit in the position. If not, correct it
				if (board.gridSpaceAvailable(nextTower.second))
				{
					board.AddTower(nextTower.second);
					offspring.towerQueue.push_back(nextTower);
				}
				else
				{
					nextTower.second = board.FindClosestAvailableTile(nextTower.second);
					board.AddTower(nextTower.second);
					offspring.towerQueue.push_back(nextTower);
				}
				break;
				//pos: 1010, type: 1010
			case 1:
				//Find next tower from parents
				if (i % 2 == 0)
				{
					nextTower = shorterChromosomeMember->towerQueue.at(i);
				}
				else
				{
					nextTower = longerChromosomeMember->towerQueue.at(i);
				}
				//Check if the tower will fit in the position. If not, correct it
				if (board.gridSpaceAvailable(nextTower.second))
				{
					board.AddTower(nextTower.second);
					offspring.towerQueue.push_back(nextTower);
				}
				else
				{
					nextTower.second = board.FindClosestAvailableTile(nextTower.second);
					board.AddTower(nextTower.second);
					offspring.towerQueue.push_back(nextTower);
				}
				break;
				//pos: 0101, type: 1010
			case 2:
				if (i % 2 == 0)
				{
					nextType = longerChromosomeMember->towerQueue.at(i).first;
					nextPosition = shorterChromosomeMember->towerQueue.at(i).second;
				}
				else
				{
					nextType = shorterChromosomeMember->towerQueue.at(i).first;
					nextPosition = longerChromosomeMember->towerQueue.at(i).second;
				}
				nextTower = std::make_pair(nextType, nextPosition);

				//Check if the tower will fit in the position. If not, correct it
				if (board.gridSpaceAvailable(nextTower.second))
				{
					board.AddTower(nextTower.second);
					offspring.towerQueue.push_back(nextTower);
				}
				else
				{
					nextTower.second = board.FindClosestAvailableTile(nextTower.second);
					board.AddTower(nextTower.second);
					offspring.towerQueue.push_back(nextTower);
				}
				break;
				//pos: 1010, type: 0101
			case 3:
				if (i % 2 == 0)
				{
					nextType = shorterChromosomeMember->towerQueue.at(i).first;
					nextPosition = longerChromosomeMember->towerQueue.at(i).second;
				}
				else
				{
					nextType = longerChromosomeMember->towerQueue.at(i).first;
					nextPosition = shorterChromosomeMember->towerQueue.at(i).second;
				}
				nextTower = std::make_pair(nextType, nextPosition);

				//Check if the tower will fit in the position. If not, correct it
				if (board.gridSpaceAvailable(nextTower.second))
				{
					board.AddTower(nextTower.second);
					offspring.towerQueue.push_back(nextTower);
				}
				else
				{
					nextTower.second = board.FindClosestAvailableTile(nextTower.second);
					board.AddTower(nextTower.second);
					offspring.towerQueue.push_back(nextTower);
				}
				break;
			}
		}
		//Otherwise, fill with longer DNA
		else
		{
			TowerInPosition nextTower = longerChromosomeMember->towerQueue.at(i);
			//Check if the tower will fit in the position
			if (board.gridSpaceAvailable(nextTower.second))
			{
				board.AddTower(nextTower.second);
				offspring.towerQueue.push_back(nextTower);
			}
			else
			{
				nextTower.second = board.FindClosestAvailableTile(nextTower.second);
				board.AddTower(nextTower.second);
				offspring.towerQueue.push_back(nextTower);
			}
		}
	}
	#endif



	return offspring;
}