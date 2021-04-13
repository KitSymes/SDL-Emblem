#include "TitleScreen.h"
#include <iostream>
#include "GameScreenManager.h"
#include "Texture2D.h"
#include "SaveData.h"
#include "Collisions.h"
#include "Utils.h"

TitleScreen::TitleScreen(SDL_Renderer* renderer, GameScreenManager* gsm) : GameScreen(renderer, gsm)
{
	m_menu = MENU_TITLE;

	m_background_texture = new Texture2D(m_renderer, 512, 416);
	if (!m_background_texture->LoadFromFile("Images/TitleScreen/Background.png"))
	{
		std::cout << "Failed to load TitleScreen/Background!" << std::endl;
	}

	m_title_texture = new Texture2D(m_renderer);
	if (!m_title_texture->LoadFromFile("Images/TitleScreen/Title.png"))
	{
		std::cout << "Failed to load TitleScreen/Title!" << std::endl;
	}

	m_font = TTF_OpenFont("Fonts/calibri.ttf", 15);
	SDL_Color colour = { 0, 0, 0 };
	m_empty = new Text(renderer, m_font, "Empty", colour);
}

TitleScreen::~TitleScreen()
{
	delete m_background_texture;
	m_background_texture = nullptr;
	delete m_title_texture;
	m_title_texture = nullptr;

	delete m_empty;
	m_empty = nullptr;

	TTF_CloseFont(m_font);
}

void TitleScreen::Render()
{
	m_background_texture->Render(Vector2D(), SDL_FLIP_NONE);
	if (m_menu == MENU_TITLE)
	{
		SDL_Rect source = { 0, 0, 90, 94 };
		SDL_Rect dest = { 166, 114, 90 * 2, 94 * 2 };
		m_title_texture->Render(&source, &dest, SDL_FLIP_NONE);
	}
	else if (m_menu == MENU_FILES)
	{
		SDL_Rect source = { 90, 0, 90, 182 };
		SDL_Rect dest = { 166, 0, 90 * 2, 182 * 2 };
		m_title_texture->Render(&source, &dest, SDL_FLIP_NONE);

		if (!s1)
			m_empty->Render(Vector2D(277, 106));
		if (!s2)
			m_empty->Render(Vector2D(277, 170));
		if (!s3)
			m_empty->Render(Vector2D(277, 234));
		if (!sa)
			m_empty->Render(Vector2D(277, 298));
	}
}

void TitleScreen::Update(float deltaTime, SDL_Event e)
{
	switch (e.type)
	{
	case SDL_MOUSEBUTTONDOWN:
		if (e.button.button == SDL_BUTTON_LEFT)
		{
			if (m_menu == MENU_TITLE) {
				if (Collisions::Instance()->Inside(e.button.x, e.button.y, Rect2D(166, 114, m_buttonWidth, m_buttonHeight)))
				{
					s1 = Utils::exists("Saves/Save1.txt");
					s2 = Utils::exists("Saves/Save2.txt");
					s3 = Utils::exists("Saves/Save3.txt");
					sa = Utils::exists("Saves/AutoSave.txt");
					m_menu = MENU_FILES;
				}
				else if (Collisions::Instance()->Inside(e.button.x, e.button.y, Rect2D(166, 178, m_buttonWidth, m_buttonHeight)))
				{
					Character* first = new Character(m_renderer, INFANTRY, LANCE, true, Vector2D());
					first->RandomStats(2);
					//first->m_attack = 99;
					//first->m_defence = 99;
					SaveData::Instance()->m_allies.push_back(first);
					m_gsm->ChangeScreen(SCREEN_MAP);
				}
				else if (Collisions::Instance()->Inside(e.button.x, e.button.y, Rect2D(166, 242, m_buttonWidth, m_buttonHeight)))
					m_gsm->ChangeScreen(SCREEN_MARIO);
			}
			else if (m_menu == MENU_FILES) {
				if (Collisions::Instance()->Inside(e.button.x, e.button.y, Rect2D(166, 82, m_buttonWidth, m_buttonHeight))) // Slot 1
				{
					if (s1)
					{
						SaveData::Instance()->Load("Save1", m_renderer);
						m_gsm->ChangeScreen(SCREEN_MAP);
					}
				}
				else if (Collisions::Instance()->Inside(e.button.x, e.button.y, Rect2D(166, 146, m_buttonWidth, m_buttonHeight))) // Slot 2
				{
					if (s2)
					{
						SaveData::Instance()->Load("Save2", m_renderer);
						m_gsm->ChangeScreen(SCREEN_MAP);
					}
				}
				else if (Collisions::Instance()->Inside(e.button.x, e.button.y, Rect2D(166, 210, m_buttonWidth, m_buttonHeight))) // Slot 3
				{
					if (s3)
					{
						SaveData::Instance()->Load("Save3", m_renderer);
						m_gsm->ChangeScreen(SCREEN_MAP);
					}
				}
				else if (Collisions::Instance()->Inside(e.button.x, e.button.y, Rect2D(166, 274, m_buttonWidth, m_buttonHeight))) // Auto Save
				{
					if (sa)
					{
						SaveData::Instance()->Load("AutoSave", m_renderer);
						m_gsm->ChangeScreen(SCREEN_MAP);
					}
				}
				else if (Collisions::Instance()->Inside(e.button.x, e.button.y, Rect2D(176, 338, 30, 26))) // Back
				{
					m_menu = MENU_TITLE;
				}
			}
		}
	}
}
