#include "BattleScreen.h"
#include <iostream>
#include <fstream>
#include "GameScreenManager.h"
#include "Texture2D.h"
#include "SaveData.h"
#include "Collisions.h"

BattleScreen::BattleScreen(SDL_Renderer* renderer, GameScreenManager* gsm, char* path) : GameScreen(renderer, gsm)
{
	std::ifstream inFile;
	inFile.open(path);

	std::string image;

	inFile >> image;

	m_background_texture = new Texture2D(m_renderer, 512, 416);
	if (!m_background_texture->LoadFromFile(image))
	{
		std::cout << "Failed to load Battle Screen " << image << " texture!" << std::endl;
	}

	m_overlays_texture = new Texture2D(m_renderer, 46, 46);
	if (!m_overlays_texture->LoadFromFile("Images/BattleMaps/TileOverlays.png"))
	{
		std::cout << "Failed to load Images/BattleMaps/TileOverlays.png texture!" << std::endl;
	}

	m_tile_cursor_texture = new Texture2D(m_renderer, 46, 46);
	if (!m_tile_cursor_texture->LoadFromFile("Images/BattleMaps/Cursor.png"))
	{
		std::cout << "Failed to load Images/BattleMaps/Cursor.png texture!" << std::endl;
	}

	m_players_turn = true;
	m_tile_cursor_animate = 0;
	m_hovered = nullptr;
	m_hovered_selected = false;

	for (int y = 0; y < EMBLEM_MAP_DIMENSION; y++)
		for (int x = 0; x < EMBLEM_MAP_DIMENSION; x++)
		{
			int tileType;
			inFile >> tileType;
			switch (tileType)
			{
			case 2:
				m_map[y][x].moveCost = 2;
				break;
			case 1:
				break;
			default:
				std::cout << "Unrecognised Tile Type " << tileType << " in " << path << std::endl;
				break;
			}
			m_map[y][x].x = x;
			m_map[y][x].y = y;
		}
	int ally_unit_count;
	inFile >> ally_unit_count;
	for (int i = 0; i < ally_unit_count; i++)
	{
		int x, y;
		inFile >> x;
		inFile >> y;

		SaveData::Instance()->m_allies.at(i)->SetAlive(true);
		SaveData::Instance()->m_allies.at(i)->SetMapPosition(x, y);
		m_map[y][x].occupied = true;

		m_last_cursor_tile = Vector2D(x, y);
	}
	while (!inFile.eof())
	{
		Character* unit = nullptr;

		Vector2D pos = Vector2D();
		inFile >> pos.x;
		inFile >> pos.y;

		MOVE_TYPE moveType;
		std::string moveStr;
		inFile >> moveStr;
		if (moveStr == "Infantry")
		{
			moveType = INFANTRY;
		}
		else {
			std::cout << "Invalid Move Type " << moveStr << " in " << path << std::endl;
			return;
		}

		WEAPON_TYPE weaponType;
		std::string weaponStr;
		inFile >> weaponStr;
		if (weaponStr == "Lancer")
		{
			weaponType = LANCE;
		}
		else {
			std::cout << "Invalid weapon Type " << weaponStr << " in " << path << std::endl;
			return;
		}

		unit = new Character(m_renderer, moveType, weaponType, false, pos);
		unit->SetMapPosition(pos.x, pos.y);
		m_map[(int)pos.y][(int)pos.x].occupied = true;

		int levelMin, levelMax;
		inFile >> levelMin;
		inFile >> levelMax;
		// TODO Randomise Stats for Level
		m_enemy_units.push_back(unit);
		if (levelMax - levelMin != 0)
			unit->RandomStats(levelMin + (rand() % (levelMax - levelMin)));
		else
			unit->RandomStats(levelMin);

		unit->SetAlive(true);
	}
}

BattleScreen::~BattleScreen()
{
	delete m_background_texture;
	m_background_texture = nullptr;

	delete m_overlays_texture;
	m_overlays_texture = nullptr;

	m_enemy_units.clear();
}

void BattleScreen::RenderMoveOverlay(int x, int y)
{
	SDL_Rect source = { 0, 0, 23, 23 };
	m_overlays_texture->Render(Vector2D(x * 46, y * 46), &source, SDL_FLIP_NONE);
	if (m_hovered_selected)
		m_overlays_texture->Render(Vector2D(x * 46, y * 46), &source, SDL_FLIP_NONE);
}

void BattleScreen::RenderAttackOverlay(int x, int y)
{
	SDL_Rect source = { 23, 0, 23, 23 };
	m_overlays_texture->Render(Vector2D(x * 46, y * 46), &source, SDL_FLIP_NONE);
	if (m_hovered_selected)
		m_overlays_texture->Render(Vector2D(x * 46, y * 46), &source, SDL_FLIP_NONE);
}

void BattleScreen::RenderDangerOverlay(int x, int y)
{
	SDL_Rect source = { 0, 23, 23, 23 };
	m_overlays_texture->Render(Vector2D(x * 46, y * 46), &source, SDL_FLIP_NONE);
}

void BattleScreen::Render()
{
	m_background_texture->Render(Vector2D(), SDL_FLIP_NONE);

	if (m_players_turn)
	{
		//if (m_hovered != nullptr)
		{
			for (int y = 0; y < EMBLEM_MAP_DIMENSION; y++)
				for (int x = 0; x < EMBLEM_MAP_DIMENSION; x++)
				{
					if (m_current_movement_matrix[y][x] <= 0)
						continue;
					switch (m_current_movement_matrix[y][x])
					{
					case 1:
						if (!m_hovered->IsFriendly())
							RenderDangerOverlay(x, y);
						else
							RenderAttackOverlay(x, y);
						break;
					case 2:
						if (!m_hovered->IsFriendly())
							RenderDangerOverlay(x, y);
						else
							RenderMoveOverlay(x, y);
						break;
					}
				}
		}
	}

	for (Character* c : m_enemy_units)
		if (c->IsAlive())
			c->Render();
	for (Character* c : SaveData::Instance()->m_allies)
		if (c->IsAlive())
			c->Render();

	if (m_players_turn)
	{
		m_tile_cursor_animate += 2;
		if (m_tile_cursor_animate > 40)
			m_tile_cursor_animate = -59;
		int spriteXMin = ((m_tile_cursor_animate > 0 ? m_tile_cursor_animate : -m_tile_cursor_animate) / 20) * 23;
		SDL_Rect source = { spriteXMin, 0, 23, 23 };
		m_tile_cursor_texture->Render(Vector2D(m_last_cursor_tile.x * 46, m_last_cursor_tile.y * 46), &source, SDL_FLIP_NONE);
	}
}

void BattleScreen::Update(float deltaTime, SDL_Event e)
{
	switch (e.type)
	{
	case SDL_MOUSEMOTION:
		if (e.button.x / 46 < EMBLEM_MAP_DIMENSION && e.button.y / 46 < EMBLEM_MAP_DIMENSION)
		{
			Vector2D newV = Vector2D(e.button.x / 46, e.button.y / 46);
			if (newV != m_last_cursor_tile)
			{
				m_last_cursor_tile = newV;
				if (m_map[(int)newV.y][(int)newV.x].occupied)
				{
					bool found = false;
					for (Character* c : SaveData::Instance()->m_allies)
					{
						found = CheckHovered(c, e.button.x, e.button.y);
						if (found)
							break;
					}
					if (!found)
					{
						for (Character* c : m_enemy_units)
						{
							found = CheckHovered(c, e.button.x, e.button.y);
							if (found)
								break;
						}
					}
					if (!found)
					{
						std::cout << "Error: SDL_MOUSEMOTION Tile x" << newV.x << " y" << newV.y << " marked as occupied but no unit says they are on it" << std::endl;
					}
				}
				else if (m_hovered != nullptr)
				{
					m_hovered = nullptr;
					UpdateMovementMatrix();
				}
			}
		}
	}
}

bool BattleScreen::CheckHovered(Character* c, int x, int y)
{
	if (!c->IsAlive()) return false;

	Vector2D pos = c->GetMapPosition();
	if (Collisions::Instance()->Inside(x, y, Rect2D(c->GetRawPosition().x - 7, c->GetRawPosition().y - 7, 46, 46)))
	{
		if (m_hovered != c)
		{
			m_hovered = c;
			UpdateMovementMatrix();
		}
		return true;
	}
}

void BattleScreen::UpdateMovementMatrix()
{
	for (int y = 0; y < EMBLEM_MAP_DIMENSION; y++)
		for (int x = 0; x < EMBLEM_MAP_DIMENSION; x++)
		{
			if (m_hovered != nullptr && m_map[y][x].occupied)
			{
				Vector2D newPos = Vector2D(x, y);
				Character* occupied = nullptr;
				for (Character* character : m_enemy_units)
					if (character->IsAlive() && newPos == character->GetMapPosition())
					{
						occupied = character;
						break;
					}
				if (occupied == nullptr)
					for (Character* character : SaveData::Instance()->m_allies)
						if (character->IsAlive() && newPos == character->GetMapPosition())
						{
							occupied = character;
							break;
						}

				if (occupied != nullptr)
				{
					if (occupied->IsFriendly() == m_hovered->IsFriendly())
						m_current_movement_matrix[(int)newPos.y][(int)newPos.x] = 3;
					else
						m_current_movement_matrix[(int)newPos.y][(int)newPos.x] = -1;
				}
				else
					std::cout << "Error: UpdateMovementMatrix x" << x << " y" << y << " marked as occupied but no unit says they are on it" << std::endl;
			}
			else
				m_current_movement_matrix[y][x] = -1;
		}

	if (m_hovered == nullptr)
		return;
	int movesLeft;
	switch (m_hovered->GetMoveType())
	{
	case INFANTRY:
		movesLeft = 3;
		break;
	default:
		movesLeft = 1;
		break;
	}
	MoveMatrixRecurse(m_hovered->GetMapPosition(), m_hovered->GetMapPosition(), movesLeft, m_hovered);
}

void BattleScreen::MoveMatrixRecurse(Vector2D previousPos, Vector2D newPos, int movesLeft, Character* c)
{
	if (previousPos == newPos)
	{
		m_current_movement_matrix[(int)previousPos.y][(int)previousPos.x] = 3;

		for (int x = -(c->GetRange)(); x <= c->GetRange(); x++) // Method 2: ~1000 ns to do
		{
			if (0 <= newPos.x + x && newPos.x + x < EMBLEM_MAP_DIMENSION)
			{
				int posX = (x >= 0 ? x : -x);

				int theY = newPos.y + (c->GetRange() - posX);

				if (0 <= theY && theY < EMBLEM_MAP_DIMENSION)
					if (m_current_movement_matrix[theY][(int)newPos.x + x] < 1)
						m_current_movement_matrix[theY][(int)newPos.x + x] = 1;

				theY = newPos.y - (c->GetRange() - posX);

				if (0 <= theY && theY < EMBLEM_MAP_DIMENSION)
					if (m_current_movement_matrix[theY][(int)newPos.x + x] < 1)
						m_current_movement_matrix[theY][(int)newPos.x + x] = 1;

			}
		}

		// Check north, east, south, west
		if (newPos.y - 1 >= 0)
			MoveMatrixRecurse(newPos, Vector2D(newPos.x, newPos.y - 1), movesLeft, c);
		if (newPos.x + 1 < EMBLEM_MAP_DIMENSION)
			MoveMatrixRecurse(newPos, Vector2D(newPos.x + 1, newPos.y), movesLeft, c);
		if (newPos.y + 1 < EMBLEM_MAP_DIMENSION)
			MoveMatrixRecurse(newPos, Vector2D(newPos.x, newPos.y + 1), movesLeft, c);
		if (newPos.x - 1 >= 0)
			MoveMatrixRecurse(newPos, Vector2D(newPos.x - 1, newPos.y), movesLeft, c);
		return;
	}

	movesLeft -= m_map[(int)newPos.y][(int)newPos.x].moveCost;

	if (movesLeft >= 0)
	{
		/*Character* occupied = nullptr;
		for (Character* character : m_enemy_units)
		{
			if (character->IsAlive() && newPos == character->GetMapPosition())
			{
				occupied = character;
				break;
			}
		}
		if (occupied == nullptr)
			for (Character* character : SaveData::Instance()->m_allies)
			{
				if (character->IsAlive() && newPos == character->GetMapPosition())
				{
					occupied = character;
					break;
				}
			}

		if (occupied != nullptr)
		{
			if (occupied->IsFriendly() == c->IsFriendly())
				m_current_movement_matrix[(int)newPos.y][(int)newPos.x] = 3;
			else
			{
				m_current_movement_matrix[(int)newPos.y][(int)newPos.x] = 0;
				return;
			}
		}
		else
			m_current_movement_matrix[(int)newPos.y][(int)newPos.x] = 2;*/
		if (m_current_movement_matrix[(int)newPos.y][(int)newPos.x] != 3) // If not friendly occupied
		{
			if (m_map[(int)newPos.y][(int)newPos.x].occupied) // If Enemy Occupied
				return;
			else m_current_movement_matrix[(int)newPos.y][(int)newPos.x] = 2;
		}

		// Create attack range around this reachable tile
		for (int x = -(c->GetRange)(); x <= c->GetRange(); x++) // Method 2: ~1000 ns to do
		{
			if (0 <= newPos.x + x && newPos.x + x < EMBLEM_MAP_DIMENSION)
			{
				int posX = (x >= 0 ? x : -x);

				int theY = newPos.y + (c->GetRange() - posX);

				if (0 <= theY && theY < EMBLEM_MAP_DIMENSION)
					if (m_current_movement_matrix[theY][(int)newPos.x + x] < 1)
						m_current_movement_matrix[theY][(int)newPos.x + x] = 1;

				theY = newPos.y - (c->GetRange() - posX);

				if (0 <= theY && theY < EMBLEM_MAP_DIMENSION)
					if (m_current_movement_matrix[theY][(int)newPos.x + x] < 1)
						m_current_movement_matrix[theY][(int)newPos.x + x] = 1;

			}
		}
		/*for (int y = 0; y < EMBLEM_MAP_DIMENSION; y++) // Method 1: ~ 3000 ns to do
			for (int x = 0; x < EMBLEM_MAP_DIMENSION; x++)
			{
				int distX = newPos.x - x;
				int distY = newPos.y - y;
				int total = (distX < 0 ? -distX : distX) + (distY < 0 ? -distY : distY);
				if (total != 0 && total <= c->GetRange())
				{
					if (m_current_movement_matrix[y][x] < 1)
						m_current_movement_matrix[y][x] = 1;
				}
			}*/

		// Check north, east, south, west
		if (newPos.y - 1 >= 0 && Vector2D(newPos.x, newPos.y - 1) != previousPos)
			MoveMatrixRecurse(newPos, Vector2D(newPos.x, newPos.y - 1), movesLeft, c);

		if (newPos.x + 1 < EMBLEM_MAP_DIMENSION && Vector2D(newPos.x + 1, newPos.y) != previousPos)
			MoveMatrixRecurse(newPos, Vector2D(newPos.x + 1, newPos.y), movesLeft, c);

		if (newPos.y + 1 < EMBLEM_MAP_DIMENSION && Vector2D(newPos.x, newPos.y + 1) != previousPos)
			MoveMatrixRecurse(newPos, Vector2D(newPos.x, newPos.y + 1), movesLeft, c);

		if (newPos.x - 1 >= 0 && Vector2D(newPos.x - 1, newPos.y) != previousPos)
			MoveMatrixRecurse(newPos, Vector2D(newPos.x - 1, newPos.y), movesLeft, c);
	}
}