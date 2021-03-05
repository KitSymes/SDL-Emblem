#pragma once
#ifndef _MARIO_SCREEN_H
#define _MARIO_SCREEN_H
#include "Commons.h"
#include "GameScreen.h"
#include "MarioLevelMap.h"
class Texture2D;
class MarioPlayer;
class PowBlock;
class MarioScreen : GameScreen
{
private:
	Texture2D* m_background_texture;
	MarioPlayer* m_mario;
	MarioPlayer* m_luigi;
	MarioLevelMap* m_level_map;
	PowBlock* m_pow_block;

	bool SetUpLevel();
	void SetLevelMap();
	void ShakeScreen();

	bool m_screenshake;
	float m_shake_time;
	float m_wobble;
	float m_background_yPos;
public:
	MarioScreen(SDL_Renderer* renderer);
	~MarioScreen();

	void Render() override;
	void Update(float deltaTime, SDL_Event e) override;

	MarioLevelMap* GetLevelMap() { return m_level_map; }

	void UpdatePOWBlock();
};
#endif
