#pragma once
#ifndef _GAMESCREENLEVEL1_H
#define _GAMESCREENLEVEL1_H
#include "Commons.h"
#include "GameScreen.h"
class Texture2D;
class Character;
class MapScreen : GameScreen
{
private:
	Texture2D* m_background_texture;
	Texture2D* m_levelLocked_texture;
	Texture2D* m_levelUnlocked_texture;
	Texture2D* m_levelComplete_texture;
	Character* myChar;

	bool SetUpLevel();

	// North = 0, East = 1, South = 2, West = 3
	struct PoI
	{
		int x = 0, y = 0;
		bool locked = true, complete = false;
		int returnDir = 0;
		int* north = nullptr;
		int* south = nullptr;
		int* east = nullptr;
		int* west = nullptr;
		PoI* northPoI = nullptr;
		PoI* southPoI = nullptr;
		PoI* eastPoI = nullptr;
		PoI* westPoI = nullptr;

		SCREENS screen = SCREEN_MAP;

		PoI(int xPos, int yPos)
		{
			x = xPos;
			y = yPos;
		}

		~PoI()
		{
			if (north != nullptr)
				delete north;
			if (south != nullptr)
				delete south;
			if (east != nullptr)
				delete east;
			if (west != nullptr)
				delete west;

			northPoI = nullptr;
			southPoI = nullptr;
			eastPoI = nullptr;
			westPoI = nullptr;
		}
	};

	PoI* currentPos;
	PoI* start;
	PoI* firstLevel;
	PoI* forestLevel;
	PoI* alternateLevel;
	PoI* beforeBridge;
	PoI* bridgeLevel;
	PoI* lastSplit;
	PoI* castleLevel;

	bool moving = false;
public:
	MapScreen(SDL_Renderer* renderer);
	~MapScreen();

	void Render() override;
	void Update(float deltaTime, SDL_Event e) override;
};
#endif
