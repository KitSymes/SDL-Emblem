#include "BattleScreen.h"
#include <iostream>
#include <fstream>
#include "GameScreenManager.h"
#include "Texture2D.h"
#include "SaveData.h"
#include "Collisions.h"
#include "AStar.h"
#include "Utils.h"

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

	m_overlays_texture = new Texture2D(m_renderer, EMBLEM_TILE_DIMENSION, EMBLEM_TILE_DIMENSION);
	if (!m_overlays_texture->LoadFromFile("Images/BattleMaps/TileOverlays.png"))
	{
		std::cout << "Failed to load Images/BattleMaps/TileOverlays.png texture!" << std::endl;
	}

	m_tile_cursor_texture = new Texture2D(m_renderer, EMBLEM_TILE_DIMENSION, EMBLEM_TILE_DIMENSION);
	if (!m_tile_cursor_texture->LoadFromFile("Images/BattleMaps/Cursor.png"))
	{
		std::cout << "Failed to load Images/BattleMaps/Cursor.png texture!" << std::endl;
	}

	m_action_cursor_texture = new Texture2D(m_renderer, 80, 10);
	if (!m_action_cursor_texture->LoadFromFile("Images/BattleMaps/ActionSelectCursor.png"))
	{
		std::cout << "Failed to load Images/BattleMaps/ActionSelectCursor.png texture!" << std::endl;
	}

	m_action_background_texture = new Texture2D(m_renderer);
	if (!m_action_background_texture->LoadFromFile("Images/BattleMaps/ChoiceBackground.png"))
	{
		std::cout << "Failed to load Images/BattleMaps/ChoiceBackground.png texture!" << std::endl;
	}

	m_font = TTF_OpenFont("Fonts/calibri.ttf", 15);

	m_players_turn = true;
	m_tile_cursor_animate = 1;
	m_action_cursor_animate = 1;
	m_action_cursor_option = 0;
	m_action_ui = Vector2D();
	m_hovered = nullptr;
	m_hovered_selected_state = SELECTED_NONE;

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

	/*AStar::Node enemy;
	enemy.x = m_enemy_units[1]->GetMapPosition().x;
	enemy.y = m_enemy_units[1]->GetMapPosition().y;

	for (AStar::Node node : AStar::aStar(m_map, m_enemy_units[1], enemy, player)) {
		std::cout << "(x" << node.x << " y" << node.y << ")" << std::endl;
	}*/
}

BattleScreen::~BattleScreen()
{
	TTF_CloseFont(m_font);

	if (m_t_name != nullptr)
	{
		delete m_t_name;
		m_t_name = nullptr;
	}

	if (m_t_atk != nullptr)
	{
		delete m_t_atk;
		m_t_atk = nullptr;
	}

	if (m_t_def != nullptr)
	{
		delete m_t_def;
		m_t_def = nullptr;
	}

	if (m_t_hp != nullptr)
	{
		delete m_t_hp;
		m_t_hp = nullptr;
	}

	delete m_background_texture;
	m_background_texture = nullptr;

	delete m_overlays_texture;
	m_overlays_texture = nullptr;

	m_enemy_units.clear();
}

void BattleScreen::RenderMoveOverlay(int x, int y)
{
	SDL_Rect source = { 0, 0, EMBLEM_TILE_DIMENSION / 2, EMBLEM_TILE_DIMENSION / 2 };
	m_overlays_texture->Render(Vector2D(x * EMBLEM_TILE_DIMENSION, y * EMBLEM_TILE_DIMENSION), &source, SDL_FLIP_NONE);
	if (m_hovered_selected_state == SELECTED_MOVING)
		m_overlays_texture->Render(Vector2D(x * EMBLEM_TILE_DIMENSION, y * EMBLEM_TILE_DIMENSION), &source, SDL_FLIP_NONE);
}

void BattleScreen::RenderAttackOverlay(int x, int y)
{
	SDL_Rect source = { EMBLEM_TILE_DIMENSION / 2, 0, EMBLEM_TILE_DIMENSION / 2, EMBLEM_TILE_DIMENSION / 2 };
	m_overlays_texture->Render(Vector2D(x * EMBLEM_TILE_DIMENSION, y * EMBLEM_TILE_DIMENSION), &source, SDL_FLIP_NONE);
	if (m_hovered_selected_state == SELECTED_MOVING)
		m_overlays_texture->Render(Vector2D(x * EMBLEM_TILE_DIMENSION, y * EMBLEM_TILE_DIMENSION), &source, SDL_FLIP_NONE);
}

void BattleScreen::RenderDangerOverlay(int x, int y)
{
	SDL_Rect source = { 0, EMBLEM_TILE_DIMENSION / 2, EMBLEM_TILE_DIMENSION / 2, EMBLEM_TILE_DIMENSION / 2 };
	m_overlays_texture->Render(Vector2D(x * EMBLEM_TILE_DIMENSION, y * EMBLEM_TILE_DIMENSION), &source, SDL_FLIP_NONE);
}

void BattleScreen::Render()
{
	m_background_texture->Render(Vector2D(), SDL_FLIP_NONE);

	int xpos = 420;
	int ypos = 50;
	int offset = 20;

	if (m_t_name != nullptr)
		m_t_name->Render(Vector2D(xpos, ypos));

	if (m_t_level != nullptr)
		m_t_level->Render(Vector2D(xpos, ypos + offset));

	if (m_t_atk != nullptr)
		m_t_atk->Render(Vector2D(xpos, ypos + offset * 2));

	if (m_t_def != nullptr)
		m_t_def->Render(Vector2D(xpos, ypos + offset * 3));

	if (m_t_hp != nullptr)
		m_t_hp->Render(Vector2D(xpos, ypos + offset * 4));

	if (m_players_turn && m_hovered != nullptr)
	{
		if (m_hovered_selected_state == SELECTED_NONE || m_hovered_selected_state == SELECTED_MOVING)
		{
			// Draw Movement Matrix tiles
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
		else if (m_hovered_selected_state == SELECTED_ATTACKING)
		{
			if (m_move_proposed_x - 1 >= 0)
				RenderAttackOverlay(m_move_proposed_x - 1, m_move_proposed_y);
			if (m_move_proposed_x + 1 < EMBLEM_MAP_DIMENSION)
				RenderAttackOverlay(m_move_proposed_x + 1, m_move_proposed_y);

			if (m_move_proposed_y - 1 >= 0)
				RenderAttackOverlay(m_move_proposed_x, m_move_proposed_y - 1);
			if (m_move_proposed_y + 1 < EMBLEM_MAP_DIMENSION)
				RenderAttackOverlay(m_move_proposed_x, m_move_proposed_y + 1);
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
		int spriteXMin = ((m_tile_cursor_animate > 0 ? m_tile_cursor_animate : -m_tile_cursor_animate) / 20) * EMBLEM_TILE_DIMENSION / 2;
		SDL_Rect source = { spriteXMin, 0, EMBLEM_TILE_DIMENSION / 2, EMBLEM_TILE_DIMENSION / 2 };
		m_tile_cursor_texture->Render(Vector2D(m_last_cursor_tile.x * EMBLEM_TILE_DIMENSION, m_last_cursor_tile.y * EMBLEM_TILE_DIMENSION), &source, SDL_FLIP_NONE);

		if (m_hovered_selected_state == SELECTED_CHOICES)
		{
			// Render Background
			m_action_background_texture->Render(m_action_ui, SDL_FLIP_NONE, 2);

			// Render Cursor
			m_action_cursor_animate += 2;
			if (m_action_cursor_animate > 100)
				m_action_cursor_animate = -79;
			int spriteYMin = ((m_action_cursor_animate > 0 ? m_action_cursor_animate : -m_action_cursor_animate) / 20) * 10;
			SDL_Rect source = { 0, spriteYMin, m_action_cursor_texture->GetWidth(), m_action_cursor_texture->GetHeight() };
			m_action_cursor_texture->Render(Vector2D(m_action_ui.x + 20, m_action_ui.y + 20 + 42 * m_action_cursor_option), &source, SDL_FLIP_NONE, 2);
		}
	}
}

void BattleScreen::Update(float deltaTime, SDL_Event e)
{
	switch (e.type)
	{
	case SDL_MOUSEBUTTONDOWN:
		if (e.button.button == SDL_BUTTON_LEFT)
		{
			if (m_hovered_selected_state == SELECTED_MOVING) // If player is moving their character
			{
				if (m_current_movement_matrix[(int)m_last_cursor_tile.y][(int)m_last_cursor_tile.x] == 2) // This square can be moved to
				{
					m_hovered_selected_state = SELECTED_CHOICES;
					m_move_proposed_x = m_last_cursor_tile.x;
					m_move_proposed_y = m_last_cursor_tile.y;

					m_action_cursor_option = 0;

					m_action_ui.x = m_hovered->GetRawPosition().x + EMBLEM_TILE_DIMENSION;
					if (m_action_ui.x + m_action_background_texture->GetWidth() * 2 >= SCREEN_WIDTH)
						m_action_ui.x = m_hovered->GetRawPosition().x - m_action_background_texture->GetWidth() * 2;

					m_action_ui.y = m_hovered->GetRawPosition().y - 24;
					if (m_action_ui.y + m_action_background_texture->GetHeight() * 2 >= SCREEN_HEIGHT)
						m_action_ui.y = m_hovered->GetRawPosition().y - 48;
					else if (m_action_ui.y < 0)
						m_action_ui.y = 0;

					/*m_map[(int)m_hovered->GetMapPosition().y][(int)m_hovered->GetMapPosition().x].occupied = false;
					m_hovered->SetMapPosition(m_last_cursor_tile.x, m_last_cursor_tile.y);
					//m_hovered->SetMoved(true);
					m_map[(int)m_hovered->GetMapPosition().y][(int)m_hovered->GetMapPosition().x].occupied = true;
					m_hovered = nullptr;
					UpdateMovementMatrix();*/
				}
			}
			else if (m_hovered_selected_state == SELECTED_CHOICES) // Player needs to choose what this unit should do
			{
				if (Collisions::Instance()->Inside(e.button.x, e.button.y, Rect2D(m_action_ui.x + 20, m_action_ui.y + 20, 152, 20))) // Attack
				{
					m_hovered_selected_state = SELECTED_ATTACKING;
				}
				else if (Collisions::Instance()->Inside(e.button.x, e.button.y, Rect2D(m_action_ui.x + 20, m_action_ui.y + 62, 152, 20))) // Wait
				{
					m_map[(int)m_hovered->GetMapPosition().y][(int)m_hovered->GetMapPosition().x].occupied = false;
					m_hovered->SetMoved(true);
					m_hovered->SetMapPosition(m_move_proposed_x, m_move_proposed_y);
					m_map[(int)m_hovered->GetMapPosition().y][(int)m_hovered->GetMapPosition().x].occupied = true;
					m_hovered_selected_state = SELECTED_NONE;
					m_hovered = nullptr;
				}
			}
			else if (m_hovered_selected_state == SELECTED_NONE) // Otherwise they want to move a character, so check if they can
			{
				if (m_hovered != nullptr && m_hovered->IsFriendly() && !m_hovered->HasMoved())
					m_hovered_selected_state = SELECTED_MOVING;
			}
		}
		else if (e.button.button == SDL_BUTTON_RIGHT) // Player wants to cancel
		{
			if (m_hovered_selected_state == SELECTED_MOVING)
			{
				m_hovered_selected_state = SELECTED_NONE;
				m_hovered->SetRawPosition(Vector2D(m_hovered->GetMapPosition().x * EMBLEM_TILE_DIMENSION, m_hovered->GetMapPosition().y * EMBLEM_TILE_DIMENSION));
				m_hovered = nullptr;
			}
			else if (m_hovered_selected_state == SELECTED_CHOICES)
			{
				m_hovered_selected_state = SELECTED_MOVING;
			}
			else if (m_hovered_selected_state == SELECTED_ATTACKING)
			{
				m_hovered_selected_state = SELECTED_CHOICES;
			}
		}
		break;
	case SDL_MOUSEMOTION:
		if (e.button.x / EMBLEM_TILE_DIMENSION < EMBLEM_MAP_DIMENSION && e.button.y / EMBLEM_TILE_DIMENSION < EMBLEM_MAP_DIMENSION)
		{
			Vector2D newV = Vector2D(e.button.x / EMBLEM_TILE_DIMENSION, e.button.y / EMBLEM_TILE_DIMENSION);
			if (newV != m_last_cursor_tile)
			{
				m_last_cursor_tile = newV;
				if (m_hovered_selected_state == SELECTED_NONE)
				{
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
				else if (m_hovered_selected_state == SELECTED_MOVING) // Player wants to move their character
				{
					if (m_current_movement_matrix[(int)newV.y][(int)newV.x] == 2)
						m_hovered->SetRawPosition(Vector2D(newV.x * EMBLEM_TILE_DIMENSION, newV.y * EMBLEM_TILE_DIMENSION));
				}
			}
		}
		if (m_hovered_selected_state == SELECTED_CHOICES)
		{
			if (m_action_cursor_option != 0 && Collisions::Instance()->Inside(e.button.x, e.button.y, Rect2D(m_action_ui.x + 20, m_action_ui.y + 20, 152, 20))) // Attack
				m_action_cursor_option = 0;
			else if (m_action_cursor_option != 1 && Collisions::Instance()->Inside(e.button.x, e.button.y, Rect2D(m_action_ui.x + 20, m_action_ui.y + 62, 152, 20))) // Wait
				m_action_cursor_option = 1;
		}
	}
}

bool BattleScreen::CheckHovered(Character* c, int x, int y)
{
	if (!c->IsAlive()) return false;

	Vector2D pos = c->GetMapPosition();
	if (Collisions::Instance()->Inside(x, y, Rect2D(c->GetMapPosition().x * EMBLEM_TILE_DIMENSION, c->GetMapPosition().y * EMBLEM_TILE_DIMENSION, EMBLEM_TILE_DIMENSION, EMBLEM_TILE_DIMENSION)))
	{
		if (m_hovered != c)
		{
			m_hovered = c;
			m_t_name = new Text(m_renderer, m_font, Utils::MoveTypeToString(c->GetMoveType()).append(" " + Utils::WeaponTypeToString(c->GetWeaponType())), 88);
			m_t_level = new Text(m_renderer, m_font, "Lvl: " + c->m_level);
			m_t_atk = new Text(m_renderer, m_font, "Atk: " + std::to_string(c->m_attack));
			m_t_def = new Text(m_renderer, m_font, "Def: " + std::to_string(c->m_defence));
			std::string str = "HP: ";
			str.append(std::to_string(c->m_health)).append("/").append(std::to_string(c->m_max_health) + "");
			m_t_hp = new Text(m_renderer, m_font, str);
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
		m_current_movement_matrix[(int)previousPos.y][(int)previousPos.x] = 2;

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
