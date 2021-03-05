#pragma once
#ifndef _MARIOMARIOPLAYER_H
#define _MARIOMARIOPLAYER_H
#include "MarioCharacter.h"
class MarioPlayerMario :
    public MarioCharacter
{
public:
	MarioPlayerMario(SDL_Renderer* renderer, std::string imagePath, Vector2D start_position, MarioScreen* screen);
	~MarioPlayerMario();

	void Update(float deltaTime, SDL_Event e);
};

#endif _MARIOMARIOPLAYER_H
