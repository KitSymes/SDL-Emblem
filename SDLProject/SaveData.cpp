#include "SaveData.h"

SaveData* SaveData::m_instance = nullptr;

SaveData::SaveData()
{

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

void SaveData::Save(std::string name)
{
	std::ofstream saveFile;
	saveFile.open("Saves/" + name + ".txt", std::ios::trunc);
	bool firstline = true;
	for (Character* c : m_allies)
	{
		if (firstline)
			firstline = false;
		else
			saveFile << std::endl;
		saveFile << "Character ";
		saveFile << c->GetMoveType() << " ";
		saveFile << c->GetWeaponType() << " ";
		saveFile << c->m_level << " ";
		saveFile << c->m_exp << " ";
		saveFile << c->m_max_health << " ";
		saveFile << c->m_attack << " ";
		saveFile << c->m_defence;
	}
	saveFile.close();
}