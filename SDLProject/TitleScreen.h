#pragma once
#ifndef _TITLESCREEN_H
#define _TITLESCREEN_H
#include "Commons.h"
#include "GameScreen.h"
#include "Text.h"
#include <SDL_ttf.h>
#include <SDL_mixer.h>
class Texture2D;
class TitleScreen : GameScreen
{
private:
	Texture2D* m_background_texture;
	Texture2D* m_title_texture;

	Mix_Chunk* m_click_sound;

	TTF_Font* m_font;
	Text* m_empty;

	int m_buttonWidth = 180;
	int m_buttonHeight = 60;
	MENU m_menu;
	bool s1, s2, s3, sa; // Save Slots exist indicators
public:
	TitleScreen(SDL_Renderer* renderer, GameScreenManager* gsm);
	~TitleScreen();

	void Render() override;
	void Update(float deltaTime, SDL_Event e) override;
};
#endif
