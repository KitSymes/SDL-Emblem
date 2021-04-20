#include "AStar.h"
#include "Character.h"
#include <stack>
using namespace std;

static bool isValid(Tile map[EMBLEM_MAP_DIMENSION][EMBLEM_MAP_DIMENSION], Character* c, int x, int y) { //If our Node is an obstacle it is not valid
	if (x < 0 || y < 0 || x >= EMBLEM_MAP_DIMENSION || y >= EMBLEM_MAP_DIMENSION) {
		return false;
	}
	// If the tile has a negative movement cost (e.g. it's completely unoccupiable)
	if (map[y][x].moveCost < 0 || map[y][x].moveCost == 999)
		return false;
	if (map[y][x].occupied)
		return false;
	// If the tile has a whitelist
	if (map[y][x].whitelist != 0)
	{
		// If the whitelist doesn't have the movement type, the tile is invalid
		if (!(map[y][x].whitelist & c->GetMoveType()))
			return false;
	}
	// Else if the tile has a blacklist
	else if (map[y][x].blacklist != 0)
	{
		// If the blacklist has the movement type, the tile is invalid
		if (map[y][x].blacklist & c->GetMoveType())
			return false;
	}
	return true;
}

static bool isDestination(int x, int y, AStar::Node dest) {
	if (x == dest.x && y == dest.y) {
		return true;
	}
	return false;
}

static double calculateH(int x, int y, AStar::Node dest) {
	double H = (sqrt((x - dest.x) * (x - dest.x)
		+ (y - dest.y) * (y - dest.y)));
	return H;
}

static vector<AStar::Node> makePath(array<array<AStar::Node, EMBLEM_MAP_DIMENSION>, EMBLEM_MAP_DIMENSION> map, AStar::Node dest) {
	try {
		//cout << "Found a path" << endl;
		int x = dest.x;
		int y = dest.y;
		stack<AStar::Node> path;
		vector<AStar::Node> usablePath;

		while (!(map[x][y].parentX == x && map[x][y].parentY == y)
			&& map[x][y].x != -1 && map[x][y].y != -1)
		{
			path.push(map[x][y]);
			int tempX = map[x][y].parentX;
			int tempY = map[x][y].parentY;
			x = tempX;
			y = tempY;

		}
		path.push(map[x][y]);

		while (!path.empty()) {
			AStar::Node top = path.top();
			path.pop();
			usablePath.emplace_back(top);
		}
		return usablePath;
	}
	catch (const exception& e) {
		cout << e.what() << endl;
	}
}

vector<AStar::Node> AStar::aStar(Tile map[EMBLEM_MAP_DIMENSION][EMBLEM_MAP_DIMENSION], Character* c, Node player, Node dest) {
	vector<Node> empty;
	if (isValid(map, c, dest.x, dest.y) == false) {
		cout << "Destination is an obstacle" << endl;
		return empty;
		//Destination is invalid
	}
	if (isDestination(player.x, player.y, dest)) {
		cout << "You are the destination" << endl;
		return empty;
		//You clicked on yourself
	}
	bool closedList[EMBLEM_MAP_DIMENSION][EMBLEM_MAP_DIMENSION];

	//Initialize whole map
	//Node allMap[50][25];
	array<array < Node, EMBLEM_MAP_DIMENSION>, EMBLEM_MAP_DIMENSION> allMap;
	for (int x = 0; x < EMBLEM_MAP_DIMENSION; x++) {
		for (int y = 0; y < EMBLEM_MAP_DIMENSION; y++) {
			allMap[x][y].fCost = FLT_MAX;
			allMap[x][y].gCost = FLT_MAX;
			allMap[x][y].hCost = FLT_MAX;
			allMap[x][y].parentX = -1;
			allMap[x][y].parentY = -1;
			allMap[x][y].x = x;
			allMap[x][y].y = y;

			closedList[x][y] = false;
		}
	}

	//Initialize our starting list
	int x = player.x;
	int y = player.y;
	allMap[x][y].fCost = 0.0;
	allMap[x][y].gCost = 0.0;
	allMap[x][y].hCost = 0.0;
	allMap[x][y].parentX = x;
	allMap[x][y].parentY = y;

	vector<Node> openList;
	openList.emplace_back(allMap[x][y]);
	bool destinationFound = false;

	while (!openList.empty() && openList.size() < EMBLEM_MAP_DIMENSION * EMBLEM_MAP_DIMENSION) {
		Node node;
		do {
			float temp = FLT_MAX;
			vector<Node>::iterator itNode;
			for (vector<Node>::iterator it = openList.begin();
				it != openList.end(); it = next(it)) {
				Node n = *it;
				if (n.fCost < temp) {
					temp = n.fCost;
					itNode = it;
				}
			}
			node = *itNode;
			openList.erase(itNode);
		} while (isValid(map, c, node.x, node.y) == false);

		x = node.x;
		y = node.y;
		closedList[x][y] = true;

		for (int i = 0; i < 4; i++) {
			{
				int newX = 0;
				int newY = 0;
				switch (i) // N, E, S, W
				{
				case 0:
					newY = -1;
					break;
				case 1:
					newX = 1;
					break;
				case 2:
					newY = 1;
					break;
				case 3:
					newX = -1;
					break;
				}
				double gNew, hNew, fNew;
				if (isValid(map, c, x + newX, y + newY)) {
					if (isDestination(x + newX, y + newY, dest))
					{
						//Destination found - make path
						allMap[x + newX][y + newY].parentX = x;
						allMap[x + newX][y + newY].parentY = y;
						destinationFound = true;
						return makePath(allMap, dest);
					}
					else if (closedList[x + newX][y + newY] == false)
					{
						gNew = node.gCost + map[y][x].moveCost;//1.0;
						hNew = calculateH(x + newX, y + newY, dest);
						fNew = gNew + hNew;
						// Check if this path is better than the one already present
						if (allMap[x + newX][y + newY].fCost == FLT_MAX ||
							allMap[x + newX][y + newY].fCost > fNew)
						{
							// Update the details of this neighbour node
							allMap[x + newX][y + newY].fCost = fNew;
							allMap[x + newX][y + newY].gCost = gNew;
							allMap[x + newX][y + newY].hCost = hNew;
							allMap[x + newX][y + newY].parentX = x;
							allMap[x + newX][y + newY].parentY = y;
							openList.emplace_back(allMap[x + newX][y + newY]);
						}
					}
				}
			}
		}
	}
	if (destinationFound == false) {
		//cout << "Destination not found" << endl;
		return empty;
	}
}