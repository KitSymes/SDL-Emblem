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
};
#endif
