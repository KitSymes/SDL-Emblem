#include "TitleScreen.h"
#include <iostream>
#include "GameScreenManager.h"
#include "Texture2D.h"

TitleScreen::TitleScreen(SDL_Renderer* renderer, GameScreenManager* gsm) : GameScreen(renderer, gsm)
{
	m_background_texture = new Texture2D(m_renderer, 512, 416);
	if (!m_background_texture->LoadFromFile("Images/WorldMap/TitleScreen.png"))
	{
		std::cout << "Failed to load TitleScreen texture!" << std::endl;
	}
}

TitleScreen::~TitleScreen()
{
	delete m_background_texture;
	m_background_texture = nullptr;
}

void TitleScreen::Render()
{
	m_background_texture->Render(Vector2D(), SDL_FLIP_NONE);
}

void TitleScreen::Update(float deltaTime, SDL_Event e)
{

	switch (e.type)
	{
	case SDL_MOUSEBUTTONDOWN:
		if (e.button.button == SDL_BUTTON_LEFT)
		{
			if (CheckMouse(e.button.x, e.button.y, 166,114))
				std::cout << "Load" << std::endl;
			if (CheckMouse(e.button.x, e.button.y, 166, 178))
				m_gsm->ChangeScreen(SCREEN_MAP);
			if (CheckMouse(e.button.x, e.button.y, 166, 242))
				m_gsm->ChangeScreen(SCREEN_MARIO);
		}
	}
}

bool TitleScreen::CheckMouse(int x, int y, int buttonX, int buttonY)
{
	return (buttonX <= x && x <= buttonX + m_buttonWidth) && (buttonY <= y && y <= buttonY + m_buttonHeight);
}
