#pragma once
#ifndef _MARIOCOIN_H
#define _MARIOCOIN_H
#include "MarioCharacter.h"

class MarioCharacterCoin : public MarioCharacter
{
private:
	float m_single_sprite_w;
	float m_single_sprite_h;

	int m_spin_timer;
public:
	MarioCharacterCoin(SDL_Renderer* renderer, std::string imagePath, Vector2D start_position, MarioScreen* screen);
	~MarioCharacterCoin();

	void Render();
	void Update(float deltaTime, SDL_Event e);
};
#endif
