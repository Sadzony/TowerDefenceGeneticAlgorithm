#include <SFML/Graphics.hpp>

#include <chrono>
#include <ctime>
#include <cmath>
#include <string>		// String object
#include <vector>		// Vector object
#include <iostream>
#include <random>

#include "PopulationController.h"

#include "GameController.h"
#include "GameMenuController.h"
#include "MonsterController.h"
#include "TowerController.h"
#include "TowerAndMonsterController.h"
#include "Tower.h"
#include "GameState.h"
#include "Timer.h"
#include "AIController.h"



using sf::Vector2f;
using std::cout;
using std::endl;





const std::vector<Vector2f> path = { Vector2f(15, 0), Vector2f(15, 4), Vector2f(
		20, 4), Vector2f(22, 4), Vector2f(22, 8), Vector2f(10, 8), Vector2f(10,
		6), Vector2f(4, 6), Vector2f(4, 13), Vector2f(12, 13), Vector2f(12, 15),
		Vector2f(17, 15), Vector2f(17, 12), Vector2f(21, 12), Vector2f(21, 18) };

	/*const std::vector<Vector2f> path = { Vector2f(15, 0), Vector2f(15, 2), Vector2f(
				20, 2), Vector2f(22, 2), Vector2f(22, 8), Vector2f(10, 8), Vector2f(10,
				6), Vector2f(4, 6), Vector2f(4, 13), Vector2f(12, 13), Vector2f(12, 15),
				Vector2f(17, 15), Vector2f(17, 12), Vector2f(21, 12), Vector2f(21, 18) };*/



bool debug;

GameBoard::GameBoard(GameState* _gameState, TowerController* _towerController,
		int _width) :
		gameState(_gameState), towerController(_towerController), width(_width) {

	
	
	if (!font.loadFromFile("assets/georgia.ttf")) {
	}
	else {
		tamText.setFont(font);
		waveText.setFont(font);
		scoreText.setFont(font);
		healthText.setFont(font);
		waveWord.setFont(font);
		generationText.setFont(font);
		popIndexText.setFont(font);
	}

	_menuTexture = new sf::Texture;
	if (!_menuTexture->loadFromFile("assets/menuButtons.png")) {
		std::cerr << "The texture does not exist (1)" << std::endl;
	}
	_tamsCounter = new sf::RectangleShape(sf::Vector2f(416, 160));
	_tamsCounter->setPosition(36, 0);
	_tamsCounter->setTexture(_menuTexture);
	_tamsCounter->setTextureRect(sf::IntRect(0, 0, 416, 160));

	_livesCounter = new sf::RectangleShape(sf::Vector2f(416, 160));
	_livesCounter->setPosition(1104, 0);
	_livesCounter->setTexture(_menuTexture);
	_livesCounter->setTextureRect(sf::IntRect(416, 0, 416, 160));

	_wavesCounter = new sf::RectangleShape(sf::Vector2f(508, 153));
	_wavesCounter->setPosition(514, 10);
	_wavesCounter->setTexture(_menuTexture);
	_wavesCounter->setTextureRect(sf::IntRect(0, 192, 768, 224));

	/*sf::Texture* _helpTexture = new sf::Texture;
	if (!_helpTexture->loadFromFile("assets/help_screen.png")) {
		std::cerr << "Error loading the help screen" << std::endl;
	}

	helpScreen = sf::RectangleShape(sf::Vector2f(800, 600));
	helpScreen.setPosition(0, 0);
	helpScreen.setTexture(_helpTexture);*/

	tamText.setString(std::to_string(100));
	waveText.setString(std::to_string(0));
	scoreText.setString(std::to_string(gameState->getScore()));
	healthText.setString(std::to_string(100));
	waveWord.setString("Wave");

	generationText.setString("Generation: " + std::to_string(0));
	popIndexText.setString("Population Member: " + std::to_string(1));

	tamText.setPosition(220, 60);
	tamText.setCharacterSize(58);

	waveText.setPosition(820, 70);
	waveText.setCharacterSize(58);

	scoreText.setPosition(1750, 70);
	scoreText.setCharacterSize(58);

	healthText.setPosition(1250, 60);
	healthText.setCharacterSize(58);

	waveWord.setPosition(625, 85);
	waveWord.setCharacterSize(40);

	generationText.setPosition(10, 970);
	popIndexText.setPosition(10, 1005);
	generationText.setCharacterSize(35);
	popIndexText.setCharacterSize(35);

	// Grass Tile Objects
	grassTile = sf::RectangleShape(sf::Vector2f(60, 60));
	grassTile.setFillColor(sf::Color(100, 250, 50)); // green
	grassTile.setOutlineThickness(-1);
	grassTile.setOutlineColor(sf::Color(0, 0, 0, 50));
	// Ground Tile Objects
	groundTile = sf::RectangleShape(sf::Vector2f(60, 60));
	groundTile.setFillColor(sf::Color(153, 140, 85)); // brown
	groundTile.setOutlineThickness(-1);
	groundTile.setOutlineColor(sf::Color(0, 0, 0, 100));
	// Hover Object
	hoverOutline = sf::CircleShape(60);
	hoverOutline.setFillColor(sf::Color::Transparent);
	hoverOutline.setOutlineColor(sf::Color::Red);
	hoverOutline.setOutlineThickness(-3);
	// Shadow Object
	shadowTile = sf::RectangleShape(sf::Vector2f(60, 60));
	shadowTile.setFillColor(sf::Color(255, 0, 0, 150));
}

GameBoard::~GameBoard()
{
	delete _menuTexture;
	delete _tamsCounter;
	delete _livesCounter;
	delete _wavesCounter;
	delete _helpTexture;
}

bool GameBoard::gridSpaceAvailable(int gridX, int gridY) {
	if (gridStatus[gridX][gridY] == 0 && gridStatus[gridX + 1][gridY] == 0
			&& gridStatus[gridX][gridY + 1] == 0
			&& gridStatus[gridX + 1][gridY + 1] == 0) {
		return true;
	} else
		return false;
}

bool GameBoard::towerIsPurchasable(TowerType type) {
	if (type != TowerType::empty) {
		if (gameState->getTams() >= gameState->getTowerProps(type)["tam"]) {
			return true;
		}
	}
	return false;
}

bool GameBoard::addTower(TowerType type, int gridX, int gridY)
{
	if (towerIsPurchasable(type) && gridSpaceAvailable(gridX, gridY)) {
		gridStatus[gridX][gridY] = 2;
		gridStatus[gridX + 1][gridY] = 2;
		gridStatus[gridX][gridY + 1] = 2;
		gridStatus[gridX + 1][gridY + 1] = 2;
		sf::Vector2f spawnPos = sf::Vector2f((float)gridX * gameState->cubit,
			(float)gridY * gameState->cubit);
		towerController->spawnTower(spawnPos, type);
		gameState->updateTamBy(-(gameState->getTowerProps(type)["tam"]));
		return true;
	}

	return false;
}

sf::Vector2i GameBoard::FindClosestAvailableTile(sf::Vector2i gridPos)
{
	BreadthSearchStartTile randomStartTile = (BreadthSearchStartTile)(rand() % 4);
	BreadthSearchDirection randomDirection = (BreadthSearchDirection)(rand() % 2);
	return FindClosestAvailableTile(gridPos, randomStartTile, randomDirection);
}

sf::Vector2i GameBoard::FindClosestAvailableTile(sf::Vector2i gridPos, BreadthSearchStartTile searchStart, BreadthSearchDirection searchDirection)
{
	//Stores the visited positions
	std::unordered_map<int, std::unordered_map<int, bool>> visitMap;
	
	//Sorts the search into an order
	std::deque<sf::Vector2i> searchQueue;

	//Push start nodes into the data structures
	visitMap[gridPos.x][gridPos.y] = true;
	searchQueue.push_back(gridPos);

	//Search. Break when found a suitable node
	while (!searchQueue.empty())
	{
		//Get search position and remove from queue
		int x = searchQueue.front().x;
		int y = searchQueue.front().y;
		sf::Vector2i searchPos = sf::Vector2i(x, y);
		searchQueue.pop_front();
		//Check if the tower fits in this position, and return it if it does
		if(gridSpaceAvailable(searchPos.x, searchPos.y))
			return searchPos;

		//Get neighbours
		std::vector<sf::Vector2i> neighbours = FindNeighbourPositions(searchPos, searchStart, searchDirection);
		for (sf::Vector2i neighbourPosition : neighbours)
		{
			//Check that the neighbour wasn't already visited
			if (!(visitMap.find(neighbourPosition.x) != visitMap.end() && visitMap[neighbourPosition.x].find(neighbourPosition.y) != visitMap[neighbourPosition.x].end()))
			{
				//Visit the unvisited neighbour, and add it onto the search queue
				visitMap[neighbourPosition.x][neighbourPosition.y] = true;
				searchQueue.push_back(neighbourPosition);
			}
		}
	}
}

std::vector<sf::Vector2i> GameBoard::FindNeighbourPositions(sf::Vector2i gridPos)
{
	BreadthSearchStartTile randomStartTile = (BreadthSearchStartTile)(rand() % 4);
	BreadthSearchDirection randomDirection = (BreadthSearchDirection)(rand() % 2);
	return FindNeighbourPositions(gridPos, randomStartTile, randomDirection);
}

std::vector<sf::Vector2i> GameBoard::FindNeighbourPositions(sf::Vector2i gridPos, BreadthSearchStartTile searchStart, BreadthSearchDirection searchDirection)
{
	std::vector<sf::Vector2i> neighbourPositions = std::vector<sf::Vector2i>();
	if (searchStart == BreadthSearchStartTile::LEFT)
	{
		//Add left Neighbour
		if (gridPos.x - 1 >= 0)
			neighbourPositions.push_back(gridPos + sf::Vector2i(-1, 0));
		if (searchDirection == BreadthSearchDirection::CLOCKWISE)
		{
			//Add top left
			if(gridPos.x - 1 >= 0 && gridPos.y + 1 < 18)
				neighbourPositions.push_back(gridPos + sf::Vector2i(-1, 1));
			//Add top
			if (gridPos.y + 1 < 18)
				neighbourPositions.push_back(gridPos + sf::Vector2i(0, 1));
			//Add top right
			if(gridPos.x + 1 < 26 && gridPos.y + 1 < 18)
				neighbourPositions.push_back(gridPos + sf::Vector2i(1, 1));
			//Add right
			if (gridPos.x + 1 < 26)
				neighbourPositions.push_back(gridPos + sf::Vector2i(1, 0));
			//Add bottom right
			if (gridPos.x + 1 < 26 && gridPos.y -1 >= 0)
				neighbourPositions.push_back(gridPos + sf::Vector2i(1, -1));
			//Add bottom
			if (gridPos.y - 1 >= 0)
				neighbourPositions.push_back(gridPos + sf::Vector2i(0, -1));
			//Add bottom left
			if (gridPos.x - 1 >= 0 && gridPos.y - 1 >= 0)
				neighbourPositions.push_back(gridPos + sf::Vector2i(-1, -1));
		}
		else if (searchDirection == BreadthSearchDirection::ANTICLOCKWISE)
		{
			//Add bottom left
			if (gridPos.x - 1 >= 0 && gridPos.y - 1 >= 0)
				neighbourPositions.push_back(gridPos + sf::Vector2i(-1, -1));
			//Add bottom
			if (gridPos.y - 1 >= 0)
				neighbourPositions.push_back(gridPos + sf::Vector2i(0, -1));
			//Add bottom right
			if (gridPos.x + 1 < 26 && gridPos.y - 1 >= 0)
				neighbourPositions.push_back(gridPos + sf::Vector2i(1, -1));
			//Add right
			if (gridPos.x + 1 < 26)
				neighbourPositions.push_back(gridPos + sf::Vector2i(1, 0));
			//Add top right
			if (gridPos.x + 1 < 26 && gridPos.y + 1 < 18)
				neighbourPositions.push_back(gridPos + sf::Vector2i(1, 1));
			//Add top
			if (gridPos.y + 1 < 18)
				neighbourPositions.push_back(gridPos + sf::Vector2i(0, 1));
			//Add top left
			if (gridPos.x - 1 >= 0 && gridPos.y + 1 < 18)
				neighbourPositions.push_back(gridPos + sf::Vector2i(-1, 1));
		}
	}
	else if (searchStart == BreadthSearchStartTile::UP)
	{
		//Add top Neighbour
		if (gridPos.y + 1 < 18)
			neighbourPositions.push_back(gridPos + sf::Vector2i(0, 1));
		if (searchDirection == BreadthSearchDirection::CLOCKWISE)
		{
			//Add top right
			if (gridPos.x + 1 < 26 && gridPos.y + 1 < 18)
				neighbourPositions.push_back(gridPos + sf::Vector2i(1, 1));
			//Add right
			if (gridPos.x + 1 < 26)
				neighbourPositions.push_back(gridPos + sf::Vector2i(1, 0));
			//Add bottom right
			if (gridPos.x + 1 < 26 && gridPos.y - 1 >= 0)
				neighbourPositions.push_back(gridPos + sf::Vector2i(1, -1));
			//Add bottom
			if (gridPos.y - 1 >= 0)
				neighbourPositions.push_back(gridPos + sf::Vector2i(0, -1));
			//Add bottom left
			if (gridPos.x - 1 >= 0 && gridPos.y - 1 >= 0)
				neighbourPositions.push_back(gridPos + sf::Vector2i(-1, -1));
			//Add left
			if (gridPos.x - 1 >= 0)
				neighbourPositions.push_back(gridPos + sf::Vector2i(-1, 0));
			//Add top left
			if (gridPos.x - 1 >= 0 && gridPos.y + 1 < 18)
				neighbourPositions.push_back(gridPos + sf::Vector2i(-1, 1));
		}
		else if (searchDirection == BreadthSearchDirection::ANTICLOCKWISE)
		{
			//Add top left
			if (gridPos.x - 1 >= 0 && gridPos.y + 1 < 18)
				neighbourPositions.push_back(gridPos + sf::Vector2i(-1, 1));
			//Add left
			if (gridPos.x - 1 >= 0)
				neighbourPositions.push_back(gridPos + sf::Vector2i(-1, 0));
			//Add bottom left
			if (gridPos.x - 1 >= 0 && gridPos.y - 1 >= 0)
				neighbourPositions.push_back(gridPos + sf::Vector2i(-1, -1));
			//Add bottom
			if (gridPos.y - 1 >= 0)
				neighbourPositions.push_back(gridPos + sf::Vector2i(0, -1));
			//Add bottom right
			if (gridPos.x + 1 < 26 && gridPos.y - 1 >= 0)
				neighbourPositions.push_back(gridPos + sf::Vector2i(1, -1));
			//Add right
			if (gridPos.x + 1 < 26)
				neighbourPositions.push_back(gridPos + sf::Vector2i(1, 0));
			//Add top right
			if (gridPos.x + 1 < 26 && gridPos.y + 1 < 18)
				neighbourPositions.push_back(gridPos + sf::Vector2i(1, 1));
		}
	}
	else if (searchStart == BreadthSearchStartTile::RIGHT)
	{
		//Add right Neighbour
		if (gridPos.x + 1 < 26)
			neighbourPositions.push_back(gridPos + sf::Vector2i(1, 0));
		if (searchDirection == BreadthSearchDirection::CLOCKWISE)
		{
			//Add bottom right
			if (gridPos.x + 1 < 26 && gridPos.y - 1 >= 0)
				neighbourPositions.push_back(gridPos + sf::Vector2i(1, -1));
			//Add bottom
			if (gridPos.y - 1 >= 0)
				neighbourPositions.push_back(gridPos + sf::Vector2i(0, -1));
			//Add bottom left
			if (gridPos.x - 1 >= 0 && gridPos.y - 1 >= 0)
				neighbourPositions.push_back(gridPos + sf::Vector2i(-1, -1));
			//Add left
			if (gridPos.x - 1 >= 0)
				neighbourPositions.push_back(gridPos + sf::Vector2i(-1, 0));
			//Add top left
			if (gridPos.x - 1 >= 0 && gridPos.y + 1 < 18)
				neighbourPositions.push_back(gridPos + sf::Vector2i(-1, 1));
			//Add top
			if (gridPos.y + 1 < 18)
				neighbourPositions.push_back(gridPos + sf::Vector2i(0, 1));
			//Add top right
			if (gridPos.x + 1 < 26 && gridPos.y + 1 < 18)
				neighbourPositions.push_back(gridPos + sf::Vector2i(1, 1));
		}
		else if (searchDirection == BreadthSearchDirection::ANTICLOCKWISE)
		{
			//Add top right
			if (gridPos.x + 1 < 26 && gridPos.y + 1 < 18)
				neighbourPositions.push_back(gridPos + sf::Vector2i(1, 1));
			//Add top
			if (gridPos.y + 1 < 18)
				neighbourPositions.push_back(gridPos + sf::Vector2i(0, 1));
			//Add top left
			if (gridPos.x - 1 >= 0 && gridPos.y + 1 < 18)
				neighbourPositions.push_back(gridPos + sf::Vector2i(-1, 1));
			//Add left
			if (gridPos.x - 1 >= 0)
				neighbourPositions.push_back(gridPos + sf::Vector2i(-1, 0));
			//Add bottom left
			if (gridPos.x - 1 >= 0 && gridPos.y - 1 >= 0)
				neighbourPositions.push_back(gridPos + sf::Vector2i(-1, -1));
			//Add bottom
			if (gridPos.y - 1 >= 0)
				neighbourPositions.push_back(gridPos + sf::Vector2i(0, -1));
			//Add bottom right
			if (gridPos.x + 1 < 26 && gridPos.y - 1 >= 0)
				neighbourPositions.push_back(gridPos + sf::Vector2i(1, -1));
		}
	}
	else if (searchStart == BreadthSearchStartTile::DOWN)
	{
		//add bottom neighbour
		if (gridPos.y - 1 >= 0)
			neighbourPositions.push_back(gridPos + sf::Vector2i(0, -1));
		if (searchDirection == BreadthSearchDirection::CLOCKWISE)
		{
			//Add bottom left
			if (gridPos.x - 1 >= 0 && gridPos.y - 1 >= 0)
				neighbourPositions.push_back(gridPos + sf::Vector2i(-1, -1));
			//Add left
			if (gridPos.x - 1 >= 0)
				neighbourPositions.push_back(gridPos + sf::Vector2i(-1, 0));
			//Add top left
			if (gridPos.x - 1 >= 0 && gridPos.y + 1 < 18)
				neighbourPositions.push_back(gridPos + sf::Vector2i(-1, 1));
			//Add top
			if (gridPos.y + 1 < 18)
				neighbourPositions.push_back(gridPos + sf::Vector2i(0, 1));
			//Add top right
			if (gridPos.x + 1 < 26 && gridPos.y + 1 < 18)
				neighbourPositions.push_back(gridPos + sf::Vector2i(1, 1));
			//Add right
			if (gridPos.x + 1 < 26)
				neighbourPositions.push_back(gridPos + sf::Vector2i(1, 0));
			//Add bottom right
			if (gridPos.x + 1 < 26 && gridPos.y - 1 >= 0)
				neighbourPositions.push_back(gridPos + sf::Vector2i(1, -1));
		}
		else if (searchDirection == BreadthSearchDirection::ANTICLOCKWISE)
		{
			//Add bottom right
			if (gridPos.x + 1 < 26 && gridPos.y - 1 >= 0)
				neighbourPositions.push_back(gridPos + sf::Vector2i(1, -1));
			//Add right
			if (gridPos.x + 1 < 26)
				neighbourPositions.push_back(gridPos + sf::Vector2i(1, 0));
			//Add top right
			if (gridPos.x + 1 < 26 && gridPos.y + 1 < 18)
				neighbourPositions.push_back(gridPos + sf::Vector2i(1, 1));
			//Add top
			if (gridPos.y + 1 < 18)
				neighbourPositions.push_back(gridPos + sf::Vector2i(0, 1));
			//Add top left
			if (gridPos.x - 1 >= 0 && gridPos.y + 1 < 18)
				neighbourPositions.push_back(gridPos + sf::Vector2i(-1, 1));
			//Add left
			if (gridPos.x - 1 >= 0)
				neighbourPositions.push_back(gridPos + sf::Vector2i(-1, 0));
			//Add bottom left
			if (gridPos.x - 1 >= 0 && gridPos.y - 1 >= 0)
				neighbourPositions.push_back(gridPos + sf::Vector2i(-1, -1));
		}
	}
	return neighbourPositions;
}

// Determine if any action needs ton be taken by
// cliking on the game board
void GameBoard::process(sf::Event event, sf::Vector2i mousePos) {
	if (mousePos.x >= width) {
		return;
	}

	int gridX = (int)ceil(mousePos.x / 60);
	int gridY = (int)ceil(mousePos.y / 60);
	if ((event.type == sf::Event::MouseButtonPressed)
			&& (event.mouseButton.button == sf::Mouse::Left)) {
		//Remove selected game tower. If double clicking a tower,
		// it will simply regain the connection by the towerController
		gameState->setBoardTower(nullptr);
		TowerType type = gameState->getPurchaseTower();
		// If an open space exists, fill the board with twos.
		addTower(type, gridX, gridY);
		//PRINT BOARD
		//if (debug) {
		//	for (int i = 0; i < 18; i++) {
		//		for (int j = 0; j < 32; j++) {
		//			std::cout << gridStatus[j][i] << " ";
		//		}
		//		std::cout << std::endl;
		//	}
		//	std::cout << std::endl;
		//}
	}
}

bool GameBoard::validatePos(int mouseX, int mouseY, int range) {
	int gridX = (int)ceil(mouseX / 60);
	int gridY = (int)ceil(mouseY / 60);
	for (int x = 0; x < range; x++) {
		for (int y = 0; y < range; y++) {
			if (gridStatus[gridX + x][gridY + y] != 0)
				return false;
		}
	}
	return true;
}

// Draws Map with Ground and Grass Objects
void GameBoard::render(sf::RenderWindow* window) {
	sf::Vector2i mousePos = gameState->mousePos;

	// Each square is to be 60 pixels wide,
	// with an exact height of 18 tiles

	// Draw Grass Everywhere
	for (int x = 0; x < 32; x++) {
		for (int y = 0; y < 18; y++) {
			grassTile.setPosition((float)x * 60, (float)y * 60);
			window->draw(grassTile);
		}
	}

	// Draw Ground on Path Only
	for (unsigned int i = 0; i < path.size() - 1; i++) {
		sf::Vector2i curr = sf::Vector2i(path[i]);		// Current Vector2f
		sf::Vector2i next = sf::Vector2i(path[i + 1]);		// Next Vector2f
		if (curr.x == next.x) {			// If two Vector2fs vertical
			while (curr.y != next.y) {
				groundTile.setPosition((float)curr.x * 60, (float)curr.y * 60);
				window->draw(groundTile);
				if (curr.y < next.y)
					curr.y++;		// Draw up or down until next Vector2f
				else
					curr.y--;
				gridStatus[curr.x][curr.y] = 1;
			}
		} else if (curr.y == next.y) {	// If two Vector2fs horizontal
			while (curr.x != next.x) {
				groundTile.setPosition((float)curr.x * 60, (float)curr.y * 60);
				window->draw(groundTile);
				if (curr.x < next.x)
					curr.x++;		// Draw left or right until next Vector2f
				else
					curr.x--;
				gridStatus[curr.x][curr.y] = 1;
			}
		}
	}

	TowerType type = gameState->getPurchaseTower();
	if (type != TowerType::empty) {
		renderRange(mousePos.x, mousePos.y,
				gameState->getTowerProps(type)["range"], window);
		renderShadow(mousePos.x, mousePos.y, 2, window);
	}
}

void GameBoard::renderLabels(sf::RenderWindow* window) {
	window->draw(*_tamsCounter);
	window->draw(*_livesCounter);
	window->draw(*_wavesCounter);
	window->draw(tamText);
	window->draw(waveText);
	window->draw(scoreText);
	window->draw(healthText);
	window->draw(waveWord);
	window->draw(generationText);
	window->draw(popIndexText);
}

// Draw Range
void GameBoard::renderRange(int mouseX, int mouseY, int range, sf::RenderWindow* window) {
	int gridX = (int)ceil(mouseX / 60) * 60 + 60;
	int gridY = (int)ceil(mouseY / 60) * 60 + 60;

	hoverOutline.setRadius((float)range);
	hoverOutline.setPosition((float)gridX - range, (float)gridY - range);
	window->draw(hoverOutline);
}

// Draw Placement Shadow
void GameBoard::renderShadow(int mouseX, int mouseY, int range, sf::RenderWindow* window) {
	int gridX = (int)ceil(mouseX / 60);
	int gridY = (int)ceil(mouseY / 60);
	shadowTile.setSize(sf::Vector2f((float)range * 60, (float)range * 60));
	shadowTile.setPosition((float)gridX * 60, (float)gridY * 60);
	window->draw(shadowTile);
}

GameController::GameController() {
	int x = 0;
	x++;
}

void deathLoop(sf::RenderWindow* window, sf::Event event) {
	sf::Texture* texture = new sf::Texture;
	if (!texture->loadFromFile("assets/game_over.png")) {
		std::cerr << "Error loading the help screen" << std::endl;
	}

	sf::RectangleShape deathScreen = sf::RectangleShape(
			sf::Vector2f(1920, 1080));
	deathScreen.setPosition(0, 0);
	deathScreen.setTexture(texture);
	window->draw(deathScreen);
	window->display();
	while (true) {
		while (window->pollEvent(event)) {
			if (event.type == sf::Event::EventType::Closed
					|| (event.type == sf::Event::MouseButtonPressed)
					|| (event.type == sf::Event::KeyPressed)) {
				return;
			}
		}
	}
}

void cleanGame(Timer** clk, GameState** gameState, GameMenuController** gameMenuController, TowerController** towerController,
	MonsterController** monsterController, GameBoard** gameBoard, TowerAndMonsterController** attackController)
{
	delete* clk;  *clk = nullptr;
	delete *gameState; *gameState = nullptr;
	delete *gameMenuController; *gameMenuController = nullptr;
	delete *towerController; *towerController = nullptr;
	delete *monsterController; *monsterController = nullptr;
	delete *gameBoard; *gameBoard = nullptr;

	delete *attackController; *attackController = nullptr;
}

void resetGame(Timer** clk, GameState** gameState, GameMenuController** gameMenuController, TowerController** towerController, 
				MonsterController** monsterController, GameBoard** gameBoard, TowerAndMonsterController** attackController, sf::RenderWindow* window)
{
	*clk = new Timer();
	*gameState = new GameState(*clk);
	*gameMenuController = new GameMenuController(window,
		*gameState);
	*towerController = new TowerController(window, *gameState);
	*monsterController = new MonsterController(window, *gameState,
		path);
	*gameBoard = new GameBoard(*gameState, *towerController, (int)(*gameMenuController)->getMenuPos().x);

	*attackController = new TowerAndMonsterController(
		window, *gameState, *monsterController, (*towerController)->getTowerVec(),
		(*monsterController)->getMonsterVec());
}

// Main
int main() {
// Initialization
	//Get a seed
	std::srand(time(NULL));
	debug = false;
	sf::RenderWindow* window = new sf::RenderWindow(sf::VideoMode(1920, 1080), "Monster Defence", sf::Style::Close);
	window->setFramerateLimit(60);
	//window->setFramerateLimit(0);
	//window->setVerticalSyncEnabled(false);

	std::vector<PopulationMember> population = PopulationController::getInstance()->GetPopulation();

	int populationIndex = 0;

	AIController aIController;
	
	Timer* clk;
	GameState* gameState;
	GameMenuController* gameMenuController;
	TowerController* towerController;
	MonsterController* monsterController;
	GameBoard* gameBoard;
	TowerAndMonsterController* attackController;
	resetGame(&clk, &gameState, &gameMenuController, &towerController, &monsterController, &gameBoard, &attackController, window);
	//m_AIController.setGameController() 
	aIController.setGameBoard(gameBoard);
	aIController.setTimer(clk);
	gameState->setPopIndex(populationIndex);
	aIController.setGameState(gameState);
	aIController.setGene(population.at(populationIndex));
	aIController.setupBoard();

	

	gameMenuController->setDebug(debug);
// Main game loop
	while (window->isOpen()) {
		sf::Vector2i mousePos = sf::Mouse::getPosition(*window);
		gameState->mousePos = mousePos;

		// Display the help menu
		if (gameState->getHelperState()) {
			/*window->clear();
			window->draw(gameBoard->helpScreen);
			window->display();
			while (gameState->getHelperState()) {
				while (window->pollEvent(gameBoard->event)) {
					if (gameBoard->event.type == sf::Event::EventType::Closed
							|| (gameBoard->event.type == sf::Event::KeyPressed
									&& gameBoard->event.key.code == sf::Keyboard::Escape)) {
						delete gameBoard;
						gameBoard = nullptr;
						window->close();
						return 0;
					} else if ((gameBoard->event.type == sf::Event::MouseButtonReleased)
							&& (gameBoard->event.mouseButton.button == sf::Mouse::Left)) {
						gameState->toggleHelpScreen();
					}
				}
			}*/
		}

		// Process
		while (window->pollEvent(gameBoard->event)) {
			if (gameBoard->event.type == sf::Event::EventType::Closed
					|| (gameBoard->event.type == sf::Event::KeyPressed
							&& gameBoard->event.key.code == sf::Keyboard::Escape)) {
				window->setActive(false);

				cleanGame(&clk, &gameState, &gameMenuController, &towerController, &monsterController, &gameBoard, &attackController);
				window->close();
				exit(0);
				return 0;
			} else {
				gameMenuController->process(gameBoard->event, mousePos);
				gameBoard->process(gameBoard->event, mousePos);
				towerController->process(gameBoard->event, mousePos);
			}
		}

		
		gameMenuController->update();
		
		if (clk->newTick()) {
			//update
			for (int i = 0; i < GAME_SPEED; i++) {
				monsterController->update();
				attackController->update();
			}
		}

		if (gameState->dirtyBit) {
			gameBoard->scoreText.setString(std::to_string(gameState->getScore()));
			gameBoard->waveText.setString(std::to_string(gameState->getCurrentWave()));
			gameBoard->healthText.setString(std::to_string(gameState->getHealth()));
			gameBoard->tamText.setString(std::to_string(gameState->getTams()));
			gameBoard->generationText.setString("Generation: " + std::to_string(PopulationController::getInstance()->GetGenerationNumber()));
			gameBoard->popIndexText.setString("Population Member: " + std::to_string(gameState->getPopIndex() + 1));
			gameState->dirtyBit = false;
		}
		// Render
		window->clear();
		gameBoard->render(window);
		monsterController->render();
		towerController->render();
		gameMenuController->render();
		attackController->render();

		aIController.update();

		gameBoard->renderLabels(window);
		if (debug) {
			gameBoard->text.setString(std::to_string(clk->elapsedTicks()));
			gameBoard->text.setFont(gameBoard->font);
			gameBoard->text.setPosition(float(mousePos.x), float(mousePos.y));
			window->draw(gameBoard->text);
		}

		if (gameState->getHealth() <= 0) {
			clk->stop();
			
			//Output the modified gene into the population
			PopulationController::getInstance()->UpdateMember(populationIndex, aIController.outputGene());
			population = PopulationController::getInstance()->GetPopulation();
			aIController.gameOver();
			//deathLoop(window, gameBoard->event);

			//Get next population member
			if (populationIndex + 1 < population.size())
				populationIndex++;
			//If no more members, go to next epoch, or restart the replay
			else
			{
				PopulationController::getInstance()->ExportCurrentPopulation();
				#if REPLAY_MODE == false
				population = PopulationController::getInstance()->NextEpoch();
				#endif
				populationIndex = 0;
			}

			cleanGame(&clk, &gameState, &gameMenuController, &towerController, &monsterController, &gameBoard, &attackController);
			resetGame(&clk, &gameState, &gameMenuController, &towerController, &monsterController, &gameBoard, &attackController, window);
			aIController.setGameBoard(gameBoard);
			aIController.setTimer(clk);
			gameState->setPopIndex(populationIndex);
			aIController.setGameState(gameState);
			aIController.setGene(population.at(populationIndex));
			aIController.setupBoard();
			//return 0;
		}
		window->display();
	} // End of main game loop

	delete gameBoard;
	gameBoard = nullptr;
	window->close();
	return 0;
}
