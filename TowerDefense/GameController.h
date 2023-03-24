/*
 * GameController.h
 *
 *  Created on: Nov 6, 2018
 *      Author: Connor
 */
#pragma once
#include "GameState.h"
#include "Tower.h"
#include "TowerController.h"
#include "BreadthSearchDirections.h"

// Game Controller keeps all Game State Variables Running and Updated
class GameController {
private:

public:
	GameController();
	
	void process();
	void update();
	void render();
};



// Game Board generates the map, and handles interactions
// for any entities sitting on the map
class GameBoard {
private:
	GameState* gameState;
	TowerController* towerController;
	int width;



public:
	GameBoard(GameState* gameState, TowerController* towerController,
			int widthInPixels);
	~GameBoard();

	// Helper functions
	bool gridSpaceAvailable(int gridX, int gridY);
	bool towerIsPurchasable(TowerType type);

	bool validatePos(int mouseX, int mouseY, int range);
	void process(sf::Event event, sf::Vector2i mousePos);
	void update();
	void render(sf::RenderWindow* window);		// Render board
	void renderLabels(sf::RenderWindow* window);
	void renderRange(int mouseX, int mouseY, int range, sf::RenderWindow* window);
	void renderShadow(int mouseX, int mouseY, int range, sf::RenderWindow* window);

	bool addTower(TowerType type, int gridX, int gridY);

	//Breadth search around a tile in a random direction
	sf::Vector2i FindClosestAvailableTile(sf::Vector2i gridPos);

	//Controlled Breadth search around a tile
	sf::Vector2i FindClosestAvailableTile(sf::Vector2i gridPos, BreadthSearchStartTile searchStart, BreadthSearchDirection searchDirection);

	//Outputs the positions of neighbouring tiles
	std::vector<sf::Vector2i> FindNeighbourPositions(sf::Vector2i gridPos);

	//Outputs the positions of neighbouring tiles in a controlled direction
	std::vector<sf::Vector2i> FindNeighbourPositions(sf::Vector2i gridPos, BreadthSearchStartTile searchStart, BreadthSearchDirection searchDirection);
		

	int gridStatus[32][18] = { 0 };

	sf::Texture* _helpTexture;
	sf::Texture* _menuTexture;
	sf::RectangleShape grassTile;
	sf::RectangleShape groundTile;
	sf::CircleShape hoverOutline;
	sf::RectangleShape shadowTile;
	sf::RectangleShape* _tamsCounter;
	sf::RectangleShape* _livesCounter;
	sf::RectangleShape* _wavesCounter;
	sf::RectangleShape helpScreen;
	sf::Text tamText, waveText, waveWord, healthText, text, scoreText;
	sf::Font font;
	sf::Event event;
};
