#pragma once
#ifndef _MARIO_SCREEN_H
#define _MARIO_SCREEN_H
#include "Commons.h"
#include "GameScreen.h"
#include "MarioLevelMap.h"
#include <vector>
#include <SDL_mixer.h>
class Texture2D;
class MarioCharacter;
class PowBlock;
class MarioCharacterKoopa;
class MarioCharacterCoin;

class MarioScreen : GameScreen
{
private:
	Texture2D* m_background_texture;
	Texture2D* m_tile_map;

	Mix_Chunk* m_coin_sound;

	MarioCharacter* m_mario;
	MarioCharacter* m_luigi;
	MarioLevelMap* m_level_map;
	PowBlock* m_pow_block;
	std::vector<MarioCharacterKoopa*> m_koopas;
	std::vector<MarioCharacterCoin*> m_coins;

	bool SetUpLevel();
	void SetLevelMap();
	void ShakeScreen();

	void UpdateEnemies(float deltaTime, SDL_Event e);
	void CreateKoopa(Vector2D position, float speed);
	void CreateCoin(Vector2D position);

	bool m_screenshake;
	float m_shake_time;
	float m_wobble;
	float m_background_yPos;
	int m_koopa_klock;
public:
	MarioScreen(SDL_Renderer* renderer, GameScreenManager* gsm);
	~MarioScreen();

	void Render() override;
	void Update(float deltaTime, SDL_Event e) override;

	MarioLevelMap* GetLevelMap() { return m_level_map; }

	void UpdatePOWBlock();
};
#endif
