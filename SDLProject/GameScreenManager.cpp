#include "GameScreenManager.h"
#include "GameScreen.h"
#include "GameScreenLevel1.h"


GameScreenManager::GameScreenManager(SDL_Renderer* renderer, SCREENS startScreen)
{
	m_renderer = renderer;
	ChangeScreen(startScreen);
}

GameScreenManager::~GameScreenManager()
{
	m_renderer = nullptr;
	delete m_current_screen;
	m_current_screen = nullptr;
}

void GameScreenManager::Render()
{
	m_current_screen->Render();
}

void GameScreenManager::Update(float deltaTime, SDL_Event e)
{
	m_current_screen->Update(deltaTime, e);
}

void GameScreenManager::ChangeScreen(SCREENS screen)
{
	// If there is a screen clear it first
	if (m_current_screen != nullptr)
		delete m_current_screen;

	switch (screen)
	{
	case SCREEN_MAP:
		m_current_screen = (GameScreen*)new GameScreenLevel1(m_renderer);
		break;
	case SCREEN_TITLE:
	case SCREEN_GAMEOVER:
	default:
		break;
	}
}