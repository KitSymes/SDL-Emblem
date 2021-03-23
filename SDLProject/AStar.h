#pragma once
#ifndef ASTAR_H
#define ASTAR_H
#include <iostream>
#include <vector>
#include "Constants.h"
#include "Commons.h"
#include <array>
class Character;
class AStar
{
public:
    struct Node
    {
        int y;
        int x;
        int parentX;
        int parentY;
        float gCost;
        float hCost;
        float fCost;

        bool operator<(const Node& comp)
        {
            return fCost < comp.fCost;
        }
    };

	static std::vector<Node> aStar(Tile map[EMBLEM_MAP_DIMENSION][EMBLEM_MAP_DIMENSION], Character* c, Node player, Node dest);
};
#endif
