#pragma once
#ifndef _BATTLESCREEN_H
#define _BATTLESCREEN_H
#include "Commons.h"
#include "GameScreen.h"
#include "Constants.h"
class Texture2D;
class BattleScreen : public GameScreen
{
private:
	Texture2D* m_background_texture;
	int map[EMBLEM_MAP_DIMENSION][EMBLEM_MAP_DIMENSION];
public:
	BattleScreen(SDL_Renderer* renderer, GameScreenManager* gsm, char* path);
	~BattleScreen();

	bool CheckMouse(int x, int y, Rect2D object);

	void Render() override;
	void Update(float deltaTime, SDL_Event e) override;
};
#endif
