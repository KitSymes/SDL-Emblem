#define _CRT_SECURE_NO_DEPRECATE
#include "Utils.h"

std::string Utils::MoveTypeToString(MOVE_TYPE move_type)
{
	switch (move_type)
	{
	case INFANTRY:
		return "Infantry";
	case FLYING:
		return "Flying";
	case ARMOUR:
		return "Armoured";
	default:
		std::cout << "Move Type Not Handled: " + move_type << std::endl;
		return "error";
	}
}

std::string Utils::WeaponTypeToString(WEAPON_TYPE weapon_type)
{
	switch (weapon_type)
	{
	case LANCE:
		return "Lancer";
	case SWORD:
		return "Swordsman";
	case AXE:
		return "Huntsman";
	case BOW:
		return "Bowman";
	default:
		std::cout << "Weapon Type Not Handled: " + weapon_type << std::endl;
		return "error";
	}
}

bool Utils::exists(const std::string& name) {
	if (FILE* file = fopen(name.c_str(), "r")) {
		fclose(file);
		return true;
	}
	else {
		return false;
	}
}

bool Utils::IsEffective(WEAPON_TYPE attacker, WEAPON_TYPE defender)
{
	switch (attacker)
	{
	case LANCE:
		switch (defender)
		{
		case SWORD:
			return true;
			break;
		}
		break;
	case AXE:
		switch (defender)
		{
		case LANCE:
			return true;
			break;
		}
		break;
	case SWORD:
		switch (defender)
		{
		case AXE:
			return true;
			break;
		}
		break;
	}
	return false;
}

bool Utils::IsIneffective(WEAPON_TYPE attacker, WEAPON_TYPE defender)
{
	switch (attacker)
	{
	case LANCE:
		switch (defender)
		{
		case AXE:
			return true;
			break;
		}
		break;
	case AXE:
		switch (defender)
		{
		case SWORD:
			return true;
			break;
		}
		break;
	case SWORD:
		switch (defender)
		{
		case LANCE:
			return true;
			break;
		}
		break;
	}
	return false;
}