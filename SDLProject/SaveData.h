#pragma once
#ifndef _SAVEDATA_H
#define _SAVEDATA_H
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <SDL.h>
#include "Character.h"
class SaveData
{
private:
	SaveData();
	static SaveData* m_instance;
public:
	~SaveData();
	static SaveData* Instance();

	std::vector<Character*> m_allies;

	bool firstLevelClear, forestLevelClear, alternateLevelClear, bridgeLevelClear, castleLevelClear, caveLevelClear;
	int lastMap, returnDir;

	void DefaultValues();
	void CompleteMap(std::string name, SDL_Renderer* renderer);

	void SetBool(std::string name, bool value);
	void SetInt(std::string name, int value);

	void Save(std::string name);
	void AutoSave();
	void Load(std::string name, SDL_Renderer* renderer);
};
#endif
