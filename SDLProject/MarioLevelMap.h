#pragma once
#ifndef _LEVELMAP_H
#define _LEVELMAP_H
#include "Constants.h"
class MarioLevelMap
{
private:
	int** m_map;
public:
	MarioLevelMap(int map[MARIO_MAP_HEIGHT][MARIO_MAP_WIDTH]);
	~MarioLevelMap();

	int GetTileAt(unsigned int h, unsigned int w);
	void ChangeTileAt(unsigned int h, unsigned int w, int tile);
};
#endif
