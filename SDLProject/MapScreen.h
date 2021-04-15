#pragma once
#ifndef _GAMESCREENLEVEL1_H
#define _GAMESCREENLEVEL1_H
#include "GameScreen.h"
#include "Commons.h"
#include "Text.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
class Texture2D;
class Character;
class MapScreen : public GameScreen
{
private:
	Texture2D* m_background_texture;
	Texture2D* m_levelLocked_texture;
	Texture2D* m_levelUnlocked_texture;
	Texture2D* m_levelComplete_texture;
	Texture2D* m_ui_textures;

	Mix_Chunk* m_click_sound;

	TTF_Font* m_font;
	Text* m_empty;

	Character* myChar;

	bool SetUpLevel();

	// North = 0, East = 1, South = 2, West = 3
	struct PoI
	{
		int x = 0, y = 0;
		bool locked = true, complete = false;
		//int returnDir = 0;
		std::vector<int> north;
		std::vector<int> south;
		std::vector<int> east;
		std::vector<int> west;
		PoI* northPoI = nullptr;
		PoI* southPoI = nullptr;
		PoI* eastPoI = nullptr;
		PoI* westPoI = nullptr;

		std::string mapFile;

		PoI(int xPos, int yPos)
		{
			x = xPos;
			y = yPos;
		}

		~PoI()
		{
			if (!north.empty())
				north.clear();
			if (!south.empty())
				south.clear();
			if (!east.empty())
				east.clear();
			if (!west.empty())
				west.clear();

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
	PoI* caveLevel;

	bool moving = false;
	bool s1, s2, s3, sa; // Save Slots exist indicators
	MENU m_menu;
	int m_buttonWidth = 180;
	int m_buttonHeight = 60;

	std::vector<int>* m_current_path;
	int m_index;
	int m_timer;
public:
	MapScreen(SDL_Renderer* renderer, GameScreenManager* gsm);
	~MapScreen();

	void Render() override;
	void Update(float deltaTime, SDL_Event e) override;

	int InverseDir(int dir);
};
#endif
