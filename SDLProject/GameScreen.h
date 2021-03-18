#pragma once
#ifndef _GAMESCREEN_H
#define _GAMESCREEN_H
#include <SDL.h>
#include <iostream>
#include <SDL_mixer.h>
class GameScreenManager;
class GameScreen
{
protected:
	SDL_Renderer* m_renderer;
	GameScreenManager* m_gsm;
	Mix_Music* g_music = nullptr;
public:
	GameScreen(SDL_Renderer* renderer, GameScreenManager* gsm);
	virtual ~GameScreen();

	void LoadMusic(std::string path);
	void ClearMusic();

	virtual void Render();
	virtual void Update(float deltaTime, SDL_Event e);
};
#endif
