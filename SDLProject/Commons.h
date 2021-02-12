#pragma once
#ifndef _COMMONS_H
#define _COMMONS_H

struct Vector2D
{
	float x, y;

	Vector2D()
	{
		x = 0.0f;
		y = 0.0f;
	}

	Vector2D(float xPos, float yPos)
	{
		x = xPos;
		y = yPos;
	}
};

enum SCREENS
{
	SCREEN_TITLE,
	SCREEN_MAP,
	SCREEN_GAMEOVER
};
#endif
