#pragma once
#ifndef _SAVEDATA_H
#define _SAVEDATA_H
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
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

	void Save(std::string name);
};
#endif
