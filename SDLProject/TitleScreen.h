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
	int m_buttonWidth = 160;
	int m_buttonHeight = 60;
public:
	TitleScreen(SDL_Renderer* renderer, GameScreenManager* gsm);
	~TitleScreen();

	bool CheckMouse(int x, int y, int buttonX, int buttonY);

	void Render() override;
	void Update(float deltaTime, SDL_Event e) override;
};
#endif
