#pragma once
#ifndef _UTILS_H
#define _UTILS_H
#include <iostream>
#include "Commons.h"
class Utils
{
public:
	static std::string MoveTypeToString(MOVE_TYPE move_type);
	static std::string WeaponTypeToString(WEAPON_TYPE weapon_type);
	static bool exists(const std::string& name);
	static bool IsEffective(WEAPON_TYPE attacker, WEAPON_TYPE defender);
	static bool IsIneffective(WEAPON_TYPE attacker, WEAPON_TYPE defender);
};
#endif
