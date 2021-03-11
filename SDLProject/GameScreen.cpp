#include "GameScreen.h"
#include "GameScreenManager.h"

GameScreen::GameScreen(SDL_Renderer* renderer, GameScreenManager* gsm)
{
	m_renderer = renderer;
	m_gsm = gsm;
}

GameScreen::~GameScreen()
{
	m_renderer = nullptr;
}

void GameScreen::Render() {}

void GameScreen::Update(float deltaTime, SDL_Event e) {}
