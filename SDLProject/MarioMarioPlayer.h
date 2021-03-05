#pragma once
#ifndef _MARIOMARIOPLAYER_H
#define _MARIOMARIOPLAYER_H
#include "MarioPlayer.h"
class MarioMarioPlayer :
    public MarioPlayer
{
public:
	MarioMarioPlayer(SDL_Renderer* renderer, std::string imagePath, Vector2D start_position, MarioScreen* screen);
	~MarioMarioPlayer();

	void Update(float deltaTime, SDL_Event e);
};

#endif _MARIOMARIOPLAYER_H
