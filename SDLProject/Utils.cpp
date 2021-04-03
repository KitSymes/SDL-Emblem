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
	default:
		std::cout << "Weapon Type Not Handled: " + weapon_type << std::endl;
		return "error";
	}
}