#pragma once
#ifndef _MARIO_SCREEN_H
#define _MARIO_SCREEN_H
#include "Commons.h"
#include "GameScreen.h"
#include "MarioLevelMap.h"
#include <vector>
class Texture2D;
class MarioCharacter;
class PowBlock;
class MarioCharacterKoopa;

class MarioScreen : GameScreen
{
private:
	Texture2D* m_background_texture;
	MarioCharacter* m_mario;
	MarioCharacter* m_luigi;
	MarioLevelMap* m_level_map;
	PowBlock* m_pow_block;
	std::vector<MarioCharacterKoopa*> m_koopas;

	bool SetUpLevel();
	void SetLevelMap();
	void ShakeScreen();

	void UpdateEnemies(float deltaTime, SDL_Event e);
	void CreateKoopa(Vector2D position, float speed);

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
