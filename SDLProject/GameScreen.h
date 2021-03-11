#pragma once
#ifndef _GAMESCREEN_H
#define _GAMESCREEN_H
#include <SDL.h>
class GameScreenManager;
class GameScreen
{
protected:
	SDL_Renderer* m_renderer;
	GameScreenManager* m_gsm;
public:
	GameScreen(SDL_Renderer* renderer, GameScreenManager* gsm);
	~GameScreen();

	virtual void Render();
	virtual void Update(float deltaTime, SDL_Event e);
};
#endif
