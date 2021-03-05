#include "MarioLevelMap.h"

// 0 = air, 1 = blocked
MarioLevelMap::MarioLevelMap(int map[MARIO_MAP_HEIGHT][MARIO_MAP_WIDTH])
{
	// Allocate Memory
	m_map = new int* [MARIO_MAP_HEIGHT];
	for (unsigned int i = 0; i < MARIO_MAP_HEIGHT; i++)
		m_map[i] = new int[MARIO_MAP_WIDTH];

	// Populate the Array
	for (unsigned int i = 0; i < MARIO_MAP_HEIGHT; i++)
		for (unsigned int j = 0; j < MARIO_MAP_WIDTH; j++)
			m_map[i][j] = map[i][j];
}

MarioLevelMap::~MarioLevelMap()
{
	for (unsigned int i = 0; i < MARIO_MAP_HEIGHT; i++)
		delete[] m_map[i];
	delete[] m_map;
}

int MarioLevelMap::GetTileAt(unsigned int h, unsigned int w)
{
	return m_map[h][w];
}

void MarioLevelMap::ChangeTileAt(unsigned int h, unsigned int w, int tile)
{
	m_map[h][w] = tile;
}
