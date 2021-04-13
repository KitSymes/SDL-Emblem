#include "SaveData.h"

SaveData* SaveData::m_instance = nullptr;

SaveData::SaveData()
{
	// Load Defaults
	DefaultValues();
}

void SaveData::DefaultValues()
{
	firstLevelClear = false;
	forestLevelClear = false;
	alternateLevelClear = false;
	bridgeLevelClear = false;
	lastMap = 0;
	returnDir = 0;
}

SaveData::~SaveData()
{
	m_instance = nullptr;
}

SaveData* SaveData::Instance()
{
	if (!m_instance)
	{
		m_instance = new SaveData;
	}

	return m_instance;
}

void SaveData::CompleteMap(std::string name, SDL_Renderer* renderer)
{
	if (name == "firstLevelClear")
	{
		if (!firstLevelClear)
		{
			Character* first = new Character(renderer, INFANTRY, SWORD, true, Vector2D());
			first->RandomStats(3);
			SaveData::Instance()->m_allies.push_back(first);
		}
		firstLevelClear = true;
	}
	else if (name == "forestLevelClear")
	{
		if (!firstLevelClear)
		{
			Character* first = new Character(renderer, INFANTRY, AXE, true, Vector2D());
			first->RandomStats(3);
			SaveData::Instance()->m_allies.push_back(first);
		}
		forestLevelClear = true;
	}
	else if (name == "alternateLevelClear")
	{
		if (!firstLevelClear)
		{
			Character* first = new Character(renderer, INFANTRY, BOW, true, Vector2D());
			first->RandomStats(3);
			SaveData::Instance()->m_allies.push_back(first);
		}
		alternateLevelClear = true;
	}
	else if (name == "bridgeLevelClear")
	{
		bridgeLevelClear = true;
	}
	else if (name == "castleLevelClear")
	{
		castleLevelClear = true;
	}
	else if (name == "caveLevelClear")
	{
		caveLevelClear = true;
	}
}

void SaveData::SetBool(std::string name, bool value)
{
	if (name == "firstLevelClear") firstLevelClear = value;
	else if (name == "forestLevelClear") forestLevelClear = value;
	else if (name == "alternateLevelClear") alternateLevelClear = value;
	else if (name == "bridgeLevelClear") bridgeLevelClear = value;
	else if (name == "castleLevelClear") castleLevelClear = value;
	else if (name == "caveLevelClear") caveLevelClear = value;
}

void SaveData::SetInt(std::string name, int value)
{
	if (name == "lastMap") lastMap = value;
	else if (name == "returnDir") returnDir = value;
}

void SaveData::Save(std::string name)
{
	std::ofstream saveFile;
	saveFile.open("Saves/" + name + ".txt", std::ios::trunc);
	for (Character* c : m_allies)
	{
		saveFile << "Character ";
		saveFile << c->GetMoveType() << " ";
		saveFile << c->GetWeaponType() << " ";
		saveFile << c->m_level << " ";
		saveFile << c->m_exp << " ";
		saveFile << c->m_max_health << " ";
		saveFile << c->m_attack << " ";
		saveFile << c->m_defence << std::endl;
	}
	saveFile << "Bool firstLevelClear " << firstLevelClear << std::endl;
	saveFile << "Bool forestLevelClear " << forestLevelClear << std::endl;
	saveFile << "Bool alternateLevelClear " << alternateLevelClear << std::endl;
	saveFile << "Bool bridgeLevelClear " << bridgeLevelClear << std::endl;
	saveFile << "Bool castleLevelClear " << castleLevelClear << std::endl;
	saveFile << "Bool caveLevelClear " << caveLevelClear << std::endl;
	saveFile << "Int lastMap " << lastMap << std::endl;
	saveFile << "Int returnDir " << returnDir << std::endl;
	saveFile.close();
}

void SaveData::AutoSave()
{
	Save("AutoSave");
}

void SaveData::Load(std::string name, SDL_Renderer* renderer)
{
	DefaultValues();
	std::ifstream saveFile;
	saveFile.open("Saves/" + name + ".txt");

	while (!saveFile.eof())
	{
		std::string read;
		saveFile >> read;
		if (read == "Character")
		{
			int mov;
			int weap;

			saveFile >> mov;
			saveFile >> weap;
			Character* c = new Character(renderer, (MOVE_TYPE)mov, (WEAPON_TYPE)weap, true);
			saveFile >> c->m_level;
			saveFile >> c->m_exp;
			saveFile >> c->m_max_health;
			c->m_health = c->m_max_health;
			saveFile >> c->m_attack;
			saveFile >> c->m_defence;
			m_allies.push_back(c);
		}
		else if (read == "Bool")
		{
			std::string b;
			bool v;

			saveFile >> b;
			saveFile >> v;


			SetBool(b, v);
		}
		else if (read == "Int")
		{
			std::string i;
			int v;

			saveFile >> i;
			saveFile >> v;

			SetInt(i, v);
		}
	}


	saveFile.close();
}