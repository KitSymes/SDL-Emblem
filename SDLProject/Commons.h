#pragma once
#ifndef _COMMONS_H
#define _COMMONS_H
#include <iostream>

// Structs

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

	bool operator==(const Vector2D& comp)
	{
		return x == comp.x && y == comp.y;
	}

	bool operator!=(const Vector2D& comp)
	{
		return !(x == comp.x && y == comp.y);
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
	Rect2D(Vector2D pos, float width, float height)
	{
		x = pos.x;
		y = pos.y;
		this->width = width;
		this->height = height;
	}
};

struct Tile
{
	int x = -1, y = -1;
	int attackBonus = 0;
	int defenceBonus = 0;
	int whitelist = 0;
	int blacklist = 0;
	int moveCost = 1;
	bool occupied = false;
};

// Enums

enum SCREENS
{
	SCREEN_TITLE,
	SCREEN_MARIO,
	SCREEN_MAP
};

enum MENU
{
	MENU_NONE, MENU_TITLE, MENU_FILES, MENU_EXTRA
};

enum FACING
{
	FACING_LEFT,
	FACING_RIGHT
};

enum MOVE_TYPE
{
	INFANTRY = 1,
	ARMOUR = 2,
	FLYING = 4
};

enum WEAPON_TYPE
{
	LANCE,
	SWORD,
	AXE,
	BOW
};

enum SelectedState
{
	SELECTED_NONE,
	SELECTED_MOVING,
	SELECTED_CHOICES,
	SELECTED_ATTACKING
};

enum PHASE
{
	PHASE_START,
	PHASE_IDLE,
	PHASE_MOVING,
	PHASE_ATTACKING,
	PHASE_EXP,
	PHASE_END,
	PHASE_WON,
	PHASE_LOST
};
#endif
