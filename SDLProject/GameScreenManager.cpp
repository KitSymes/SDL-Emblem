#include "GameScreenManager.h"
#include "GameScreen.h"
#include "MapScreen.h"
#include "MarioScreen.h"
#include "TitleScreen.h"

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
	case SCREEN_MARIO:
		m_current_screen = (GameScreen*)new MarioScreen(m_renderer, this);
		break;
	case SCREEN_MAP:
		m_current_screen = (GameScreen*)new MapScreen(m_renderer, this);
		break;
	case SCREEN_TITLE:
		m_current_screen = (GameScreen*)new TitleScreen(m_renderer, this);
		break;
	case SCREEN_GAMEOVER:
	default:
		break;
	}
}

void GameScreenManager::ChangeScreen(GameScreen* screen)
{
	// If there is a screen clear it first
	if (m_current_screen != nullptr)
		delete m_current_screen;

	m_current_screen = screen;
}