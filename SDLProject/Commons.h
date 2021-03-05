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

struct Rect2D
{
	float x, y, width, height;
	Rect2D(float xPos, float yPos, float width, float height)
	{
		x = xPos;
		y = yPos;
		this->width = width;
		this->height = height;
	}
};

enum SCREENS
{
	SCREEN_TITLE,
	SCREEN_MARIO,
	SCREEN_MAP,
	SCREEN_GAMEOVER
};

enum FACING
{
	FACING_LEFT,
	FACING_RIGHT
};
#endif
