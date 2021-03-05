#pragma once
#ifndef _LUIGIMARIOPLAYER_H
#define _LUIGIMARIOPLAYER_H
#include "MarioCharacter.h"
class MarioPlayerLuigi :
    public MarioCharacter
{
public:
	MarioPlayerLuigi(SDL_Renderer* renderer, std::string imagePath, Vector2D start_position, MarioScreen* screen);
	~MarioPlayerLuigi();

	void Update(float deltaTime, SDL_Event e);
};
#endif
