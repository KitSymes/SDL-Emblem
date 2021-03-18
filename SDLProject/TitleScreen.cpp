#include "TitleScreen.h"
#include <iostream>
#include "GameScreenManager.h"
#include "Texture2D.h"
#include "SaveData.h"
#include "Collisions.h"

TitleScreen::TitleScreen(SDL_Renderer* renderer, GameScreenManager* gsm) : GameScreen(renderer, gsm)
{
	m_menu = TITLE;
	m_background_texture = new Texture2D(m_renderer, 512, 416);
	if (!m_background_texture->LoadFromFile("Images/TitleScreen/Background.png"))
	{
		std::cout << "Failed to load TitleScreen/Background!" << std::endl;
	}
	m_title_texture = new Texture2D(m_renderer, 512, 416);
	if (!m_title_texture->LoadFromFile("Images/TitleScreen/Title.png"))
	{
		std::cout << "Failed to load TitleScreen/Title!" << std::endl;
	}
}

TitleScreen::~TitleScreen()
{
	delete m_background_texture;
	m_background_texture = nullptr;
	delete m_title_texture;
	m_title_texture = nullptr;
}

void TitleScreen::Render()
{
	m_background_texture->Render(Vector2D(), SDL_FLIP_NONE);
	if (m_menu == TITLE)
		m_title_texture->Render(Vector2D(), SDL_FLIP_NONE);
}

void TitleScreen::Update(float deltaTime, SDL_Event e)
{
	switch (e.type)
	{
	case SDL_MOUSEBUTTONDOWN:
		if (e.button.button == SDL_BUTTON_LEFT)
		{
			if (m_menu == TITLE) {
				if (Collisions::Instance()->Inside(e.button.x, e.button.y, Rect2D(166, 114, m_buttonWidth, m_buttonHeight)))
					std::cout << "Load" << std::endl;
				if (Collisions::Instance()->Inside(e.button.x, e.button.y, Rect2D(166, 178, m_buttonWidth, m_buttonHeight)))
				{
					Character* first = new Character(m_renderer, INFANTRY, LANCE, true, Vector2D());
					first->RandomStats(1);
					SaveData::Instance()->m_allies.push_back(first);
					m_gsm->ChangeScreen(SCREEN_MAP);
				}
				if (Collisions::Instance()->Inside(e.button.x, e.button.y, Rect2D(166, 242, m_buttonWidth, m_buttonHeight)))
					m_gsm->ChangeScreen(SCREEN_MARIO);
			}
		}
	}
}
