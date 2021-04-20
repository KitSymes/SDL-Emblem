#include "BattleScreen.h"
#include <iostream>
#include <fstream>
#include "GameScreenManager.h"
#include "Texture2D.h"
#include "SaveData.h"
#include "Collisions.h"
#include "Utils.h"

BattleScreen::BattleScreen(SDL_Renderer* renderer, GameScreenManager* gsm, char* path) : GameScreen(renderer, gsm)
{
	std::ifstream inFile;
	inFile.open(path);

	std::string image;

	inFile >> image;
	inFile >> m_level_clear_bool;

	// Textures

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

	m_phase_texture = new Texture2D(m_renderer, 178, 22);
	if (!m_phase_texture->LoadFromFile("Images/BattleMaps/PhaseDisplays.png"))
	{
		std::cout << "Failed to load Images/BattleMaps/PhaseDisplays.png texture!" << std::endl;
	}

	m_sparks_texture = new Texture2D(m_renderer, 26, 26);
	if (!m_sparks_texture->LoadFromFile("Images/BattleMaps/Sparks.png"))
	{
		std::cout << "Failed to load Images/BattleMaps/Sparks.png texture!" << std::endl;
	}

	m_bars_texture = new Texture2D(m_renderer, 5, 20);
	if (!m_bars_texture->LoadFromFile("Images/BattleMaps/Bars.png"))
	{
		std::cout << "Failed to load Images/BattleMaps/Bars.png texture!" << std::endl;
	}

	m_icons_texture = new Texture2D(m_renderer, 10, 10);
	if (!m_icons_texture->LoadFromFile("Images/BattleMaps/Icons.png"))
	{
		std::cout << "Failed to load Images/BattleMaps/Icons.png texture!" << std::endl;
	}

	// Sounds

	m_hurt_sound = Mix_LoadWAV("Sounds/Hit_Hurt.wav");
	if (!m_hurt_sound)
	{
		std::cout << "Failed to load Sounds/Hit_Hurt.wav sound!" << std::endl;
		std::cout << Mix_GetError() << std::endl;
	}

	m_exp_sound = Mix_LoadWAV("Sounds/Exp_Point.wav");
	if (!m_exp_sound)
	{
		std::cout << "Failed to load Sounds/Exp_Point.wav sound!" << std::endl;
		std::cout << Mix_GetError() << std::endl;
	}

	m_lvl_up_sound = Mix_LoadWAV("Sounds/Level_Up.wav");
	if (!m_lvl_up_sound)
	{
		std::cout << "Failed to load Sounds/Level_Up.wav sound!" << std::endl;
		std::cout << Mix_GetError() << std::endl;
	}

	m_click_sound = Mix_LoadWAV("Sounds/Click.wav");
	if (!m_click_sound)
	{
		std::cout << "Failed to load Sounds/Click.wav sound!" << std::endl;
		std::cout << Mix_GetError() << std::endl;
	}

	m_font = TTF_OpenFont("Fonts/calibri.ttf", 15);

	m_players_turn = true;
	m_tile_cursor_animate = 1;
	m_action_cursor_animate = 1;
	m_action_cursor_option = 0;
	m_action_ui = Vector2D();
	m_hovered = nullptr;
	m_hovered_selected_state = SELECTED_NONE;
	m_attack_target = nullptr;
	m_damage = nullptr;
	m_smart_end_turn = false;
	m_smart_check_routed = false;

	for (int y = 0; y < EMBLEM_MAP_DIMENSION; y++)
		for (int x = 0; x < EMBLEM_MAP_DIMENSION; x++)
		{
			int tileType;
			inFile >> tileType;
			switch (tileType)
			{
			case 1: // Regular Tile
				break;
			case 2: // Forest Tile
				m_map[y][x].moveCost = 2;
				m_map[y][x].defenceBonus = 1;
				break;
			case 3: // Inaccesable Tile
				m_map[y][x].moveCost = 999;
				break;
			case 4: // Throne Tile
				m_map[y][x].attackBonus = 3;
				m_map[y][x].defenceBonus = 3;
				break;
			default:
				std::cout << "Unrecognised Tile Type " << tileType << " in " << path << std::endl;
				break;
			}
			m_map[y][x].x = x;
			m_map[y][x].y = y;
		}
	int ally_unit_count, currentTotal = 0;
	inFile >> ally_unit_count;

	for (int i = 0; i < ally_unit_count; i++)
	{
		int x, y;
		inFile >> x;
		inFile >> y;
		currentTotal++;

		Character* ally = SaveData::Instance()->m_allies[i];

		ally->SetAlive(true);
		ally->SetMoved(false);
		ally->m_health = ally->m_max_health;
		ally->SetMapPosition(x, y);
		m_map[y][x].occupied = true;

		m_last_cursor_tile = Vector2D(x, y);
	}
	while (currentTotal < ally_unit_count)
	{
		int x, y;
		inFile >> x;
		inFile >> y;
		currentTotal++;
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
		else if (moveStr == "Armoured")
		{
			moveType = ARMOUR;
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
		else if (weaponStr == "Swordsman")
		{
			weaponType = SWORD;
		}
		else if (weaponStr == "Huntsman")
		{
			weaponType = AXE;
		}
		else if (weaponStr == "Bowman")
		{
			weaponType = BOW;
		}
		else {
			std::cout << "Invalid Weapon Type " << weaponStr << " in " << path << std::endl;
			return;
		}

		unit = new Character(m_renderer, moveType, weaponType, false, pos);
		unit->SetMapPosition(pos.x, pos.y);
		m_map[(int)pos.y][(int)pos.x].occupied = true;

		int levelMin, levelMax;
		inFile >> levelMin;
		inFile >> levelMax;
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
	LoadMusic("Sounds/Music_Battle.mp3");
	Mix_VolumeMusic(15);
	PlayerTurn();
}

BattleScreen::~BattleScreen()
{
	Mix_FreeChunk(m_hurt_sound);
	m_hurt_sound = nullptr;
	Mix_FreeChunk(m_exp_sound);
	m_exp_sound = nullptr;
	Mix_FreeChunk(m_lvl_up_sound);
	m_lvl_up_sound = nullptr;
	Mix_FreeChunk(m_click_sound);
	m_click_sound = nullptr;

	TTF_CloseFont(m_font);
	m_font = nullptr;

	m_enemy_units.clear();
	m_hovered = nullptr;
	m_attack_target = nullptr;

	m_current_enemy_unit = nullptr;
	m_finalTarget = nullptr;
	m_finalPath.clear();

	if (m_damage != nullptr)
		delete m_damage;

	delete m_background_texture;
	m_background_texture = nullptr;

	delete m_overlays_texture;
	m_overlays_texture = nullptr;

	delete m_action_background_texture;
	m_action_background_texture = nullptr;

	delete m_action_background_texture;
	m_action_background_texture = nullptr;

	delete m_phase_texture;
	m_phase_texture = nullptr;

	delete m_tile_cursor_texture;
	m_tile_cursor_texture = nullptr;

	delete m_action_cursor_texture;
	m_action_cursor_texture = nullptr;

	delete m_sparks_texture;
	m_sparks_texture = nullptr;

	delete m_bars_texture;
	m_bars_texture = nullptr;

	delete m_icons_texture;
	m_icons_texture = nullptr;
}

Character* BattleScreen::GetUnitOnTile(Vector2D tile)
{
	for (Character* c : SaveData::Instance()->m_allies)
		if (c->IsAlive() && c->GetMapPosition() == tile)
			return c;
	for (Character* c : m_enemy_units)
		if (c->IsAlive() && c->GetMapPosition() == tile)
			return c;
	return nullptr;
}

Tile* BattleScreen::GetTile(Character* unit)
{
	return &m_map[(int)unit->GetMapPosition().y][(int)unit->GetMapPosition().x];
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

	if (m_players_turn && m_hovered != nullptr && m_phase == PHASE_IDLE)
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
		else if (m_hovered_selected_state == SELECTED_ATTACKING && m_phase != PHASE_ATTACKING)
		{
			for (int x = -(m_hovered->GetRange()); x <= m_hovered->GetRange(); x++) // Modified from MoveMatrixRecurse
			{
				if (0 <= m_move_proposed_x + x && m_move_proposed_x + x < EMBLEM_MAP_DIMENSION)
				{
					int absX = abs(x);
					int theY = m_move_proposed_y + (m_hovered->GetRange() - absX);

					if (0 <= theY && theY < EMBLEM_MAP_DIMENSION) // Valid tile so render overlay
						RenderAttackOverlay(m_move_proposed_x + x, theY);

					if (theY == m_move_proposed_y - (m_hovered->GetRange() - absX))
						continue;

					theY = m_move_proposed_y - (m_hovered->GetRange() - absX);

					if (0 <= theY && theY < EMBLEM_MAP_DIMENSION) // Valid tile so render overlay
						RenderAttackOverlay(m_move_proposed_x + x, theY);

				}
			}
			/*if (m_move_proposed_x - 1 >= 0)
				RenderAttackOverlay(m_move_proposed_x - 1, m_move_proposed_y);
			if (m_move_proposed_x + 1 < EMBLEM_MAP_DIMENSION)
				RenderAttackOverlay(m_move_proposed_x + 1, m_move_proposed_y);

			if (m_move_proposed_y - 1 >= 0)
				RenderAttackOverlay(m_move_proposed_x, m_move_proposed_y - 1);
			if (m_move_proposed_y + 1 < EMBLEM_MAP_DIMENSION)
				RenderAttackOverlay(m_move_proposed_x, m_move_proposed_y + 1);*/

			if (m_attack_target != nullptr)
			{
				m_attack_target->RenderText(Vector2D(440, 250));

				if (Utils::IsEffective(m_hovered->GetWeaponType(), m_attack_target->GetWeaponType()))
				{
					SDL_Rect source = { 10,0,10,10 };
					m_icons_texture->Render(Vector2D(425, 271), &source, SDL_FLIP_NONE);
					SDL_Rect source2 = { 0,0,10,10 };
					m_icons_texture->Render(Vector2D(425, 71), &source2, SDL_FLIP_NONE);
				}
				else if (Utils::IsIneffective(m_hovered->GetWeaponType(), m_attack_target->GetWeaponType()))
				{
					SDL_Rect source = { 0,0,10,10 };
					m_icons_texture->Render(Vector2D(425, 271), &source, SDL_FLIP_NONE);
					SDL_Rect source2 = { 10,0,10,10 };
					m_icons_texture->Render(Vector2D(425, 71), &source2, SDL_FLIP_NONE);
				}

				if (GetTile(m_attack_target)->attackBonus > 0)
				{
					SDL_Rect source = { 0,0,10,10 };
					m_icons_texture->Render(Vector2D(425, 312), &source, SDL_FLIP_NONE);
				}
				else if (GetTile(m_attack_target)->attackBonus < 0)
				{
					SDL_Rect source = { 10,0,10,10 };
					m_icons_texture->Render(Vector2D(425, 312), &source, SDL_FLIP_NONE);
				}

				if (GetTile(m_attack_target)->defenceBonus > 0)
				{
					SDL_Rect source = { 0,0,10,10 };
					m_icons_texture->Render(Vector2D(425, 332), &source, SDL_FLIP_NONE);
				}
				else if (GetTile(m_attack_target)->defenceBonus < 0)
				{
					SDL_Rect source = { 10,0,10,10 };
					m_icons_texture->Render(Vector2D(425, 332), &source, SDL_FLIP_NONE);
				}

				if (m_damage != nullptr)
					m_damage->Render(Vector2D(445, 365));
			}
		}

		m_hovered->RenderText(Vector2D(440, 50));

		if (m_map[m_move_proposed_y][m_move_proposed_x].attackBonus > 0)
		{
			SDL_Rect source = { 0,0,10,10 };
			m_icons_texture->Render(Vector2D(425, 112), &source, SDL_FLIP_NONE);
		}
		else if (m_map[m_move_proposed_y][m_move_proposed_x].attackBonus < 0)
		{
			SDL_Rect source = { 10,0,10,10 };
			m_icons_texture->Render(Vector2D(425, 112), &source, SDL_FLIP_NONE);
		}

		if (m_map[m_move_proposed_y][m_move_proposed_x].defenceBonus > 0)
		{
			SDL_Rect source = { 0,0,10,10 };
			m_icons_texture->Render(Vector2D(425, 132), &source, SDL_FLIP_NONE);
		}
		else if (m_map[m_move_proposed_y][m_move_proposed_x].defenceBonus < 0)
		{
			SDL_Rect source = { 10,0,10,10 };
			m_icons_texture->Render(Vector2D(425, 132), &source, SDL_FLIP_NONE);
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

	if (m_phase == PHASE_START)
	{
		SDL_Rect source = { 0, !m_players_turn * 22, m_phase_texture->GetWidth(), m_phase_texture->GetHeight() };
		m_phase_texture->Render(Vector2D(m_phase_int - 35, 186), &source, SDL_FLIP_NONE, 2);
	}
	else if (m_phase == PHASE_IDLE)
	{

	}
	else if (m_phase == PHASE_MOVING)
	{

	}
	else if (m_phase == PHASE_ATTACKING)
	{
		Character* attacker = m_players_turn ? m_hovered : m_current_enemy_unit;
		Character* defender = m_players_turn ? m_attack_target : m_finalTarget;

		if (m_phase_timer > 20)
		{
		}
		else
		{
			if (m_phase_timer <= 11) // 0-10 // 0-11
			{

			}
			else // 11-20 // 12-20
			{
				SDL_Rect source = { 26 * ((m_phase_timer - 11) / 3), 0, 26, 26 };
				m_sparks_texture->Render(defender->GetRawPosition(), &source, SDL_FLIP_NONE, 2);
				m_sparks_texture->Render(defender->GetRawPosition(), &source, SDL_FLIP_NONE, 2);
				m_sparks_texture->Render(defender->GetRawPosition(), &source, SDL_FLIP_NONE, 2);
				for (int i = 1; i <= 10; i++)
				{
					SDL_Rect source2 = { 0, (m_health_left) >= (i * (double)defender->m_max_health / 10) ? 20 : 0, 5, 20 };
					m_bars_texture->Render(Vector2D(m_health_location.x + (i - 1) * 5, m_health_location.y), &source2, SDL_FLIP_NONE);
				}
			}
		}
	}
	else if (m_phase == PHASE_EXP)
	{
		for (int i = 1; i <= 10; i++)
		{
			SDL_Rect source2 = { 5, m_phase_int % 10 >= i ? 20 : 0, 5, 20 };
			m_bars_texture->Render(Vector2D(m_health_location.x + (i - 1) * 5, m_health_location.y), &source2, SDL_FLIP_NONE);
		}
	}
	else if (m_phase == PHASE_END)
	{

	}
	else if (m_phase == PHASE_WON)
	{
		SDL_Rect source = { 0, 2 * 22, m_phase_texture->GetWidth(), m_phase_texture->GetHeight() };
		m_phase_texture->Render(Vector2D(m_phase_int - 35, 186), &source, SDL_FLIP_NONE, 2);
	}
	else if (m_phase == PHASE_LOST)
	{
		SDL_Rect source = { 0, 3 * 22, m_phase_texture->GetWidth(), m_phase_texture->GetHeight() };
		m_phase_texture->Render(Vector2D(m_phase_int - 35, 186), &source, SDL_FLIP_NONE, 2);
	}


}

void BattleScreen::Update(float deltaTime, SDL_Event e)
{
	if (m_phase == PHASE_START)
	{
		int add = m_phase_timer - 20;
		m_phase_int += (add * add) / 40;
		m_phase_timer++;
		if (m_phase_timer > 60)
			m_phase = PHASE_IDLE;
	}
	else if (m_phase == PHASE_IDLE)
	{
		if (m_smart_end_turn)
		{
			bool endTurn = true;
			if (m_players_turn)
			{
				for (Character* c : SaveData::Instance()->m_allies)
					if (c->IsAlive() && !c->HasMoved())
					{
						endTurn = false;
						break;
					}
			}
			else
			{
				for (Character* c : m_enemy_units)
					if (c->IsAlive() && !c->HasMoved())
					{
						endTurn = false;
						break;
					}
			}

			if (endTurn)
				m_phase = PHASE_END;

			m_smart_end_turn = false;
		}
		else
		{
			if (!m_players_turn)
			{
				for (Character* enemy : m_enemy_units)
					if (enemy->IsAlive() && !enemy->HasMoved())
					{
						m_current_enemy_unit = enemy;
						m_finalTarget = nullptr;
						int finalTargetDistance = 999;
						m_finalPath.clear();
						for (Character* player : SaveData::Instance()->m_allies)
							if (player->IsAlive())
							{
								AStar::Node nodeEnemy;
								nodeEnemy.x = enemy->GetMapPosition().x;
								nodeEnemy.y = enemy->GetMapPosition().y;
								AStar::Node nodePlayer;
								nodePlayer.x = player->GetMapPosition().x;
								nodePlayer.y = player->GetMapPosition().y;

								Tile map[EMBLEM_MAP_DIMENSION][EMBLEM_MAP_DIMENSION];
								for (int y = 0; y < EMBLEM_MAP_DIMENSION; y++)
									for (int x = 0; x < EMBLEM_MAP_DIMENSION; x++)
										map[y][x] = m_map[y][x];

								map[(int)enemy->GetMapPosition().y][(int)enemy->GetMapPosition().x].occupied = false;
								map[(int)player->GetMapPosition().y][(int)player->GetMapPosition().x].occupied = false;

								std::vector<AStar::Node> path;

								if (enemy->GetRange() == 1)
								{
									path = AStar::aStar(map, enemy, nodeEnemy, nodePlayer);

									if (path.size() == 0) // Can't pathfind to target
									{
										path.clear();
										continue;
									}

									int tileDistanceFromTarget = path.at(path.size() - 2).gCost; // Because last node (size - 1) is the destination
									if (tileDistanceFromTarget < finalTargetDistance)
									{
										m_finalTarget = player;
										finalTargetDistance = tileDistanceFromTarget;
										m_finalPath.clear();
										for (int i = 1; i < path.size() - 1; i++) // 0 is their current tile, size() - 1 is target's square, so going to size() - 1 will be the closest spot to attack from
											m_finalPath.push_back(path.at(i));
									}
									else {
										path.clear();
									}
								}
								else if (enemy->GetRange() >= 2)
								{
									int distance = -1;
									AStar::Node temp;
									for (int x = -(enemy->GetRange()); x <= enemy->GetRange(); x++) // Modified from MoveMatrixRecurse
									{
										if (distance == 0)
											break;

										if (0 <= player->GetMapPosition().x + x && player->GetMapPosition().x + x < EMBLEM_MAP_DIMENSION)
										{
											int absX = abs(x);
											int theY = player->GetMapPosition().y + (enemy->GetRange() - absX);

											if (0 <= theY && theY < EMBLEM_MAP_DIMENSION) // Valid tile so calculate distance
											{
												if (enemy->GetMapPosition() == Vector2D(player->GetMapPosition().x + x, theY))
												{
													distance = 0;
													break;
												}

												temp.x = player->GetMapPosition().x + x;
												temp.y = theY;
												path = AStar::aStar(map, enemy, nodeEnemy, temp);
												if (path.size() != 0)
												{
													int newDist = path.at(path.size() - 2).gCost + 1;
													if (distance == -1 || newDist < distance)
													{
														distance = newDist;
														nodePlayer.x = player->GetMapPosition().x + x;
														nodePlayer.y = theY;
													}
												}
											}

											theY = player->GetMapPosition().y - (enemy->GetRange() - absX);

											if (0 <= theY && theY < EMBLEM_MAP_DIMENSION) // Valid tile so calculate distance
											{
												if (enemy->GetMapPosition() == Vector2D(player->GetMapPosition().x + x, theY))
												{
													distance = 0;
													break;
												}

												temp.x = player->GetMapPosition().x + x;
												temp.y = theY;
												path = AStar::aStar(map, enemy, nodeEnemy, temp);
												if (path.size() != 0)
												{
													int newDist = path.at(path.size() - 2).gCost + 1;
													if (distance == -1 || newDist < distance)
													{
														distance = newDist;
														nodePlayer.x = player->GetMapPosition().x + x;
														nodePlayer.y = theY;
													}
												}
											}

										}
									}
									path = AStar::aStar(map, enemy, nodeEnemy, nodePlayer);
									if (distance == 0)
									{
										m_finalTarget = player;
										finalTargetDistance = 0;
										m_finalPath.clear();
									}
									else if (path.size() != 0)
									{
										int tileDistanceFromTarget = path.at(path.size() - 2).gCost;
										if (tileDistanceFromTarget < finalTargetDistance)
										{
											m_finalTarget = player;
											finalTargetDistance = tileDistanceFromTarget;
											m_finalPath.clear();
											for (int i = 1; i < path.size(); i++)
												m_finalPath.push_back(path.at(i));
										}
										else {
											path.clear();
										}
									}
								}
							}
						/*std::cout << "Shortest path for unit is " << finalTargetDistance << " tiles long: " << std::endl;
						for (int i = 0; i < m_finalPath.size() - 1; i++)
							std::cout << "(x" << m_finalPath.at(i).x << " y" << m_finalPath.at(i).y << ")" << std::endl;*/

						if (finalTargetDistance == 999)
							ExhaustUnit(enemy);
						else
						{
							m_enemy_mov_points = enemy->GetMoves();
							m_phase = PHASE_MOVING;
						}
						break;
					}
			}
		}
	}
	else if (m_phase == PHASE_MOVING)
	{
		if (!m_players_turn) // Will not add move anim for players
		{
			if (m_finalPath.empty()) // Unit is at destination
			{
				ExhaustUnit(m_current_enemy_unit);
				m_phase_timer = 0;

				if (m_finalTarget == nullptr)
				{
					std::cout << "Enemy had no attack target!" << std::endl;
					ExhaustUnit(m_current_enemy_unit);
					m_phase = PHASE_IDLE;
				}
				else
				{
					if (m_finalTarget->GetMapPosition().y < 7)
						m_health_location = Vector2D(m_finalTarget->GetRawPosition().x,
							m_finalTarget->GetRawPosition().y + EMBLEM_TILE_DIMENSION);
					else
						m_health_location = Vector2D(m_finalTarget->GetRawPosition().x,
							m_finalTarget->GetRawPosition().y - 10);

					m_phase = PHASE_ATTACKING;
				}
			}
			else if (m_enemy_mov_points - m_map[m_finalPath.at(0).y][m_finalPath.at(0).x].moveCost < 0) // Ran out of moves
			{
				ExhaustUnit(m_current_enemy_unit);
				m_phase_timer = 0;
				m_phase = PHASE_IDLE;
			}
			else
			{
				if (m_current_enemy_unit->GetRawPosition() == Vector2D(m_finalPath.at(0).x * EMBLEM_TILE_DIMENSION, m_finalPath.at(0).y * EMBLEM_TILE_DIMENSION))
				{
					GetTile(m_current_enemy_unit)->occupied = false;
					m_current_enemy_unit->SetMapPosition(m_finalPath.at(0).x, m_finalPath.at(0).y);
					GetTile(m_current_enemy_unit)->occupied = true;
					m_enemy_mov_points -= GetTile(m_current_enemy_unit)->moveCost;
					m_finalPath.erase(m_finalPath.begin());
				}
				else
				{
					int step = 4;
					if (m_current_enemy_unit->GetRawPosition().x < m_finalPath.at(0).x * EMBLEM_TILE_DIMENSION)
						m_current_enemy_unit->SetRawPosition(Vector2D(m_current_enemy_unit->GetRawPosition().x + step, m_current_enemy_unit->GetRawPosition().y));
					if (m_current_enemy_unit->GetRawPosition().x > m_finalPath.at(0).x * EMBLEM_TILE_DIMENSION)
						m_current_enemy_unit->SetRawPosition(Vector2D(m_current_enemy_unit->GetRawPosition().x - step, m_current_enemy_unit->GetRawPosition().y));
					if (m_current_enemy_unit->GetRawPosition().y < m_finalPath.at(0).y * EMBLEM_TILE_DIMENSION)
						m_current_enemy_unit->SetRawPosition(Vector2D(m_current_enemy_unit->GetRawPosition().x, m_current_enemy_unit->GetRawPosition().y + step));
					if (m_current_enemy_unit->GetRawPosition().y > m_finalPath.at(0).y * EMBLEM_TILE_DIMENSION)
						m_current_enemy_unit->SetRawPosition(Vector2D(m_current_enemy_unit->GetRawPosition().x, m_current_enemy_unit->GetRawPosition().y - step));
				}
			}
		}
	}
	else if (m_phase == PHASE_ATTACKING)
	{
		Character* attacker = m_players_turn ? m_hovered : m_current_enemy_unit;
		Character* defender = m_players_turn ? m_attack_target : m_finalTarget;

		if (m_phase_timer > 20)
		{
			bool xp = false;
			if (defender->AttackedBy(attacker, GetTile(attacker), GetTile(defender))) // Returns True if attack target dies
			{
				GetTile(defender)->occupied = false;
				m_smart_check_routed = true;
				if (m_players_turn)
				{
					m_exp_old = attacker->m_exp;
					m_exp_new = m_exp_old + pow(2, defender->m_level + 3 - attacker->m_level);
					xp = true;
				}
			}

			attacker->SetMapPosition(attacker->GetMapPosition().x, attacker->GetMapPosition().y);

			if (m_players_turn)
			{
				m_hovered_selected_state = SELECTED_NONE;
				if (!xp)
					m_hovered = nullptr;
				m_attack_target = nullptr;
			}

			m_phase_timer = 0;
			m_phase_int = 0;
			if (xp)
			{
				m_phase_int = m_exp_old;

				if (attacker->GetMapPosition().y < 7)
					m_health_location = Vector2D(attacker->GetRawPosition().x,
						attacker->GetRawPosition().y + EMBLEM_TILE_DIMENSION);
				else
					m_health_location = Vector2D(attacker->GetRawPosition().x,
						attacker->GetRawPosition().y - 10);

				m_phase = PHASE_EXP;
			}
			else
				m_phase = PHASE_IDLE;
		}
		else
		{
			int step = 4;
			int xDir = 0;
			if (attacker->GetMapPosition().x < defender->GetMapPosition().x)
				xDir = 1;
			else if (attacker->GetMapPosition().x > defender->GetMapPosition().x)
				xDir = -1;
			int yDir = 0;
			if (attacker->GetMapPosition().y < defender->GetMapPosition().y)
				yDir = 1;
			else if (attacker->GetMapPosition().y > defender->GetMapPosition().y)
				yDir = -1;

			if (m_phase_timer <= 10) // 0-10
				attacker->SetRawPosition(Vector2D(attacker->GetRawPosition().x + step * xDir, attacker->GetRawPosition().y + step * yDir));
			else // 11-20
			{
				if (m_phase_timer == 11)
					Mix_PlayChannel(-1, m_hurt_sound, 0);
				attacker->SetRawPosition(Vector2D(attacker->GetRawPosition().x - step * xDir, attacker->GetRawPosition().y - step * yDir));
				int maxDamage = attacker->GetDamageResult(defender, GetTile(defender), GetTile(attacker));
				if (maxDamage > defender->m_health)
					maxDamage = defender->m_health;
				double healthLeftD = defender->m_health - ((m_phase_timer - 11) * maxDamage / 8.0);
				m_health_left = ceil(healthLeftD);
				m_health_left = std::max(0, m_health_left);
			}
			m_phase_timer++;
		}
	}
	else if (m_phase == PHASE_EXP)
	{
		if (m_phase_int < m_exp_new && m_phase_timer % 10 == 1)
		{
			m_phase_int += 1; // current EXP points
			Mix_PlayChannel(-1, m_exp_sound, 0);
			if (m_phase_int != 0 && m_phase_int % 10 == 0)
				Mix_PlayChannel(-1, m_lvl_up_sound, 0);
		}

		m_phase_timer++;
		if (m_phase_timer >= m_phase_int * 10 + 5)
		{
			m_hovered->m_exp = m_exp_new;
			while (m_hovered->m_exp >= 10)
			{
				m_hovered->m_exp -= 10;
				m_hovered->LevelUp();
			}
			m_hovered = nullptr;
			m_phase = PHASE_IDLE;
		}
	}
	else if (m_phase == PHASE_END)
	{
		for (Character* c : SaveData::Instance()->m_allies)
			if (c->IsAlive())
				c->SetMoved(false);
		for (Character* c : m_enemy_units)
			if (c->IsAlive())
				c->SetMoved(false);
		if (m_players_turn)
			EnemyTurn();
		else
			PlayerTurn();
	}
	else if (m_phase == PHASE_WON)
	{
		int add = m_phase_timer - 20;
		m_phase_int += (add * add) / 40;
		m_phase_timer++;
		if (m_phase_timer > 60)
		{
			SaveData::Instance()->CompleteMap(m_level_clear_bool, m_renderer);
			SaveData::Instance()->AutoSave();
			m_gsm->ChangeScreen(SCREEN_MAP);
		}
	}
	else if (m_phase == PHASE_LOST)
	{
		int add = m_phase_timer - 20;
		m_phase_int += (add * add) / 40;
		m_phase_timer++;
		if (m_phase_timer > 60)
		{
			SaveData::Instance()->Load("AutoSave", m_renderer);
			m_gsm->ChangeScreen(SCREEN_MAP);
		}
	}

	if (m_smart_check_routed && m_phase == PHASE_IDLE)
	{
		m_smart_check_routed = false;
		bool playerDead = true;
		bool enemyDead = true;

		for (Character* c : SaveData::Instance()->m_allies)
			if (c->IsAlive())
			{
				playerDead = false;
				break;
			}

		if (playerDead)
		{
			m_phase_timer = 0;
			m_phase_int = 0;
			m_phase = PHASE_LOST;
		}

		for (Character* c : m_enemy_units)
			if (c->IsAlive())
			{
				enemyDead = false;
				break;
			}

		if (enemyDead)
		{
			m_phase_timer = 0;
			m_phase_int = 0;
			m_phase = PHASE_WON;
		}
	}

	if (!m_players_turn)
		return;

	switch (e.type)
	{
	case SDL_MOUSEBUTTONDOWN:
		if (m_phase != PHASE_IDLE)
			break;
		if (e.button.button == SDL_BUTTON_LEFT)
		{
			if (m_hovered_selected_state == SELECTED_MOVING) // If player is moving their character
			{
				if (m_current_movement_matrix[(int)m_last_cursor_tile.y][(int)m_last_cursor_tile.x] == 2 && // This square can be moved to
					(m_last_cursor_tile == m_hovered->GetMapPosition() || m_map[(int)m_last_cursor_tile.y][(int)m_last_cursor_tile.x].occupied == false)) // And is unoccupied
				{
					m_hovered_selected_state = SELECTED_CHOICES;
					m_action_cursor_option = 0;

					m_move_proposed_x = m_last_cursor_tile.x;
					m_move_proposed_y = m_last_cursor_tile.y;

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
					m_hovered->UpdateText(m_font, &m_map[m_move_proposed_y][m_move_proposed_x]);
					m_hovered_selected_state = SELECTED_ATTACKING;
					Mix_PlayChannel(-1, m_click_sound, 0);
				}
				else if (Collisions::Instance()->Inside(e.button.x, e.button.y, Rect2D(m_action_ui.x + 20, m_action_ui.y + 62, 152, 20))) // Wait
				{
					ExhaustUnit(m_hovered);
					m_hovered_selected_state = SELECTED_NONE;
					m_hovered = nullptr;
					Mix_PlayChannel(-1, m_click_sound, 0);
				}
			}
			else if (m_hovered_selected_state == SELECTED_ATTACKING)
			{
				if (m_attack_target != nullptr && !m_attack_target->IsFriendly())
				{
					if (m_attack_target->GetMapPosition().y < 7)
						m_health_location = Vector2D(m_attack_target->GetRawPosition().x,
							m_attack_target->GetRawPosition().y + EMBLEM_TILE_DIMENSION);
					else
						m_health_location = Vector2D(m_attack_target->GetRawPosition().x,
							m_attack_target->GetRawPosition().y - 10);
					ExhaustUnit(m_hovered);
					m_phase_timer = 0;
					m_phase = PHASE_ATTACKING;
				}
			}
			else if (m_hovered_selected_state == SELECTED_NONE) // Otherwise they want to move a character, so check if they can
			{
				if (m_hovered != nullptr && m_hovered->IsFriendly() && !m_hovered->HasMoved())
					m_hovered_selected_state = SELECTED_MOVING;
			}
		}
		else if (e.button.button == SDL_BUTTON_RIGHT) // Player wants to cancel their current action
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
				m_move_proposed_x = m_hovered->GetMapPosition().x;
				m_move_proposed_y = m_hovered->GetMapPosition().y;
				m_hovered->UpdateText(m_font, GetTile(m_hovered));
				m_hovered_selected_state = SELECTED_CHOICES;
			}
		}
		break;
	case SDL_MOUSEMOTION:
		if (e.button.x / EMBLEM_TILE_DIMENSION < EMBLEM_MAP_DIMENSION && e.button.y / EMBLEM_TILE_DIMENSION < EMBLEM_MAP_DIMENSION)
		{
			Vector2D newV = Vector2D(e.button.x / EMBLEM_TILE_DIMENSION, e.button.y / EMBLEM_TILE_DIMENSION);
			if (newV != m_last_cursor_tile && m_phase == PHASE_IDLE)
			{
				m_last_cursor_tile = newV;
				if (m_hovered_selected_state == SELECTED_NONE)
				{
					if (m_map[(int)newV.y][(int)newV.x].occupied)
					{
						Character* unit = GetUnitOnTile(newV);
						if (unit == nullptr)
						{
							std::cout << "Error: SDL_MOUSEMOTION Tile x" << newV.x << " y" << newV.y << " marked as occupied but no unit says they are on it" << std::endl;
						}
						else if (m_hovered != unit)
						{
							m_hovered = unit;
							m_hovered->UpdateText(m_font, GetTile(m_hovered));
							m_move_proposed_x = m_last_cursor_tile.x;
							m_move_proposed_y = m_last_cursor_tile.y;
							UpdateMovementMatrix();
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
					{
						m_hovered->SetRawPosition(Vector2D(newV.x * EMBLEM_TILE_DIMENSION, newV.y * EMBLEM_TILE_DIMENSION));
						m_move_proposed_x = m_last_cursor_tile.x;
						m_move_proposed_y = m_last_cursor_tile.y;
						m_hovered->UpdateText(m_font, &m_map[m_move_proposed_y][m_move_proposed_x]);
					}
				}
				else if (m_hovered_selected_state == SELECTED_ATTACKING && m_phase == PHASE_IDLE) // Player wants to attack a character
				{
					bool lastCursorIsAttackable = false;


					for (int x = -(m_hovered->GetRange()); x <= m_hovered->GetRange(); x++) // Modified from MoveMatrixRecurse
					{
						if (0 <= m_move_proposed_x + x && m_move_proposed_x + x < EMBLEM_MAP_DIMENSION)
						{
							int absX = abs(x);
							int theY = m_move_proposed_y + (m_hovered->GetRange() - absX);

							if (0 <= theY && theY < EMBLEM_MAP_DIMENSION) // Valid Tile
								if (m_last_cursor_tile == Vector2D(m_move_proposed_x + x, theY)) // Tile is selected so attackable
									lastCursorIsAttackable = true;

							if (theY == m_move_proposed_y - (m_hovered->GetRange() - absX))
								continue;

							theY = m_move_proposed_y - (m_hovered->GetRange() - absX);

							if (0 <= theY && theY < EMBLEM_MAP_DIMENSION) // Valid Tile
								if (m_last_cursor_tile == Vector2D(m_move_proposed_x + x, theY)) // Tile is selected so attackable
									lastCursorIsAttackable = true;

						}
					}

					/*if (m_move_proposed_x - 1 >= 0)
						if (m_last_cursor_tile.x == m_move_proposed_x - 1 && m_last_cursor_tile.y == m_move_proposed_y)
							lastCursorIsAttackable = true;

					if (!lastCursorIsAttackable)
						if (m_move_proposed_x + 1 < EMBLEM_MAP_DIMENSION)
							if (m_last_cursor_tile.x == m_move_proposed_x + 1 && m_last_cursor_tile.y == m_move_proposed_y)
								lastCursorIsAttackable = true;

					if (!lastCursorIsAttackable)
						if (m_move_proposed_y - 1 >= 0)
							if (m_last_cursor_tile.x == m_move_proposed_x && m_last_cursor_tile.y == m_move_proposed_y - 1)
								lastCursorIsAttackable = true;

					if (!lastCursorIsAttackable)
						if (m_move_proposed_y + 1 < EMBLEM_MAP_DIMENSION)
							if (m_last_cursor_tile.x == m_move_proposed_x && m_last_cursor_tile.y == m_move_proposed_y + 1)
								lastCursorIsAttackable = true;*/

					if (lastCursorIsAttackable) // Player is hovering over a square they want to attack
					{
						if (m_map[(int)m_last_cursor_tile.y][(int)m_last_cursor_tile.x].occupied) // There is a unit on this tile, friendly check will be done "on attack confirm"
						{
							Character* unit = GetUnitOnTile(m_last_cursor_tile);
							if (unit != m_hovered)
							{
								if (unit != m_attack_target)
								{
									if (m_damage != nullptr)
										delete m_damage;
									m_attack_target = unit;
									m_attack_target->UpdateText(m_font, GetTile(m_attack_target));
									int hpAfterAttack = m_attack_target->m_health - m_hovered->GetDamageResult(m_attack_target, GetTile(m_attack_target), &m_map[m_move_proposed_y][m_move_proposed_x]);
									if (hpAfterAttack < 0)
										hpAfterAttack = 0;
									std::string text = std::to_string(m_attack_target->m_health).append(" -> ").append(std::to_string(hpAfterAttack));
									m_damage = new Text(m_renderer, m_font, text);
								}
							}
							else
								m_attack_target = nullptr;
						}
						else
							m_attack_target = nullptr;
					}
					else
						m_attack_target = nullptr;
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
		return true;
	}
	return false;
}

void BattleScreen::UpdateMovementMatrix()
{
	for (int y = 0; y < EMBLEM_MAP_DIMENSION; y++)
		for (int x = 0; x < EMBLEM_MAP_DIMENSION; x++)
		{
			if (m_hovered != nullptr && m_map[y][x].occupied)
			{
				Vector2D newPos = Vector2D(x, y);
				Character* occupied = GetUnitOnTile(newPos);

				if (occupied != nullptr)
				{
					if (occupied->IsFriendly() == m_hovered->IsFriendly())
						m_current_movement_matrix[(int)newPos.y][(int)newPos.x] = 3;
					else
						m_current_movement_matrix[(int)newPos.y][(int)newPos.x] = -1;
				}
				else
					std::cout << "Error: UpdateMovementMatrix x" << x << " y" << y << " marked as occupied but no alive unit says they are on it" << std::endl;
			}
			else
				m_current_movement_matrix[y][x] = -1;
		}

	if (m_hovered == nullptr)
		return;
	MoveMatrixRecurse(m_hovered->GetMapPosition(), m_hovered->GetMapPosition(), m_hovered->GetMoves(), m_hovered);
}

void BattleScreen::MoveMatrixRecurse(Vector2D previousPos, Vector2D newPos, int movesLeft, Character* c)
{
	if (previousPos == newPos)
	{
		m_current_movement_matrix[(int)previousPos.y][(int)previousPos.x] = 2;

		for (int x = -(c->GetRange()); x <= c->GetRange(); x++) // Method 2: ~1000 ns to do - SEE COMMENTS BELOW
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

		// Check north, east, south, west tiles
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
				int posX = abs(x);

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

			// Check north, east, south, west tiles
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

void BattleScreen::ExhaustUnit(Character* c)
{
	c->SetMoved(true);
	if (m_players_turn)
	{
		GetTile(c)->occupied = false;
		c->SetMapPosition(m_move_proposed_x, m_move_proposed_y);
		GetTile(c)->occupied = true;
	}
	m_smart_end_turn = true;
}

void BattleScreen::PlayerTurn()
{
	m_players_turn = true;
	m_phase = PHASE_START;
	m_phase_timer = 0;
	m_phase_int = 0;

	for (Character* c : SaveData::Instance()->m_allies)
		if (c->IsAlive())
		{
			m_last_cursor_tile = c->GetMapPosition();
			m_hovered = c;
			m_hovered->UpdateText(m_font, GetTile(m_hovered));
			UpdateMovementMatrix();
			break;
		}
}

void BattleScreen::EnemyTurn()
{
	m_players_turn = false;
	m_phase = PHASE_START;
	m_phase_timer = 0;
	m_phase_int = 0;
}
