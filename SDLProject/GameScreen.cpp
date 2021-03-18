#include "GameScreen.h"
#include "GameScreenManager.h"

GameScreen::GameScreen(SDL_Renderer* renderer, GameScreenManager* gsm)
{
	m_renderer = renderer;
	m_gsm = gsm;
}

GameScreen::~GameScreen()
{
	ClearMusic();
	m_renderer = nullptr;
}

void GameScreen::LoadMusic(std::string path)
{
	g_music = Mix_LoadMUS(path.c_str());
	if (g_music == nullptr)
	{
		std::cout << "Failed to load music. Error: " << Mix_GetError() << std::endl;
	}

	if (Mix_PlayingMusic() == 0)
	{
		Mix_PlayMusic(g_music, -1);
	}
}

void GameScreen::ClearMusic()
{
	if (g_music != nullptr)
	{
		Mix_FreeMusic(g_music);
		g_music = nullptr;
	}
}

void GameScreen::Render() {}

void GameScreen::Update(float deltaTime, SDL_Event e) {}
