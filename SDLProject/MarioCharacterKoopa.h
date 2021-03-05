#pragma once
#ifndef _MARIOKOOPA_H
#define _MARIOKOOPA_H
#include "MarioCharacter.h"

class MarioCharacterKoopa : public MarioCharacter
{
private:
	float m_single_sprite_w;
	float m_single_sprite_h;
	bool m_injured;
	float m_injured_time;

	void FlipRightwayUp();
public:
	MarioCharacterKoopa(SDL_Renderer* renderer, std::string imagePath, Vector2D start_position, float speed, MarioScreen* screen);
	~MarioCharacterKoopa();

	void TakeDamage();

	bool IsInjured() { return m_injured; }

	void Render();
	void Update(float deltaTime, SDL_Event e);
};
#endif
