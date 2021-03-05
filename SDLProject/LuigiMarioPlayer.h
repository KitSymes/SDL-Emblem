#pragma once
#ifndef _LUIGIMARIOPLAYER_H
#define _LUIGIMARIOPLAYER_H
#include "MarioPlayer.h"
class LuigiMarioPlayer :
    public MarioPlayer
{
public:
	LuigiMarioPlayer(SDL_Renderer* renderer, std::string imagePath, Vector2D start_position, MarioScreen* screen);
	~LuigiMarioPlayer();

	void Update(float deltaTime, SDL_Event e);
};
#endif
