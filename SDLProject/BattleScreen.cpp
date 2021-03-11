#include "BattleScreen.h"
#include <iostream>
#include <fstream>
#include "GameScreenManager.h"
#include "Texture2D.h"

BattleScreen::BattleScreen(SDL_Renderer* renderer, GameScreenManager* gsm, char* path) : GameScreen(renderer, gsm)
{

	std::ifstream inFile;
	inFile.open(path);

	std::string image;

	inFile >> image;

	m_background_texture = new Texture2D(m_renderer, 512, 416);
	if (!m_background_texture->LoadFromFile(image))
	{
		std::cout << "Failed to load Battle Screen " << image << " texture!" << std::endl;
	}

	for (int y = 0; y < EMBLEM_MAP_DIMENSION; y++)
		for (int x = 0; x < EMBLEM_MAP_DIMENSION; x++)
			inFile >> map[y][x];
}

BattleScreen::~BattleScreen()
{
	delete m_background_texture;
	m_background_texture = nullptr;
}

void BattleScreen::Render()
{
	m_background_texture->Render(Vector2D(), SDL_FLIP_NONE);
}

void BattleScreen::Update(float deltaTime, SDL_Event e)
{
}
