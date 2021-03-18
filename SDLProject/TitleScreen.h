#pragma once
#ifndef _TITLESCREEN_H
#define _TITLESCREEN_H
#include "Commons.h"
#include "GameScreen.h"
class Texture2D;
class TitleScreen : GameScreen
{
private:
	Texture2D* m_background_texture;
	Texture2D* m_title_texture;
	int m_buttonWidth = 160;
	int m_buttonHeight = 60;
	enum MENU
	{
		TITLE, LOAD
	};
	MENU m_menu;
public:
	TitleScreen(SDL_Renderer* renderer, GameScreenManager* gsm);
	~TitleScreen();

	void Render() override;
	void Update(float deltaTime, SDL_Event e) override;
};
#endif
