#pragma once
#ifndef _BATTLESCREEN_H
#define _BATTLESCREEN_H
#include "Commons.h"
#include "GameScreen.h"
#include "Constants.h"
#include "Character.h"
#include "Text.h"
#include "AStar.h"
#include <vector>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
class Texture2D;
class BattleScreen : public GameScreen
{
private:
	// Textures / Fonts
	Texture2D* m_background_texture;
	Texture2D* m_overlays_texture;
	Texture2D* m_action_background_texture;
	Texture2D* m_phase_texture;
	Texture2D* m_tile_cursor_texture;
	Texture2D* m_action_cursor_texture;
	Texture2D* m_sparks_texture;
	Texture2D* m_bars_texture;
	Texture2D* m_icons_texture;

	Mix_Chunk* m_hurt_sound;
	Mix_Chunk* m_exp_sound;
	Mix_Chunk* m_lvl_up_sound;

	TTF_Font* m_font;
	Text* m_damage; // HP -> New HP text

	// Enemy Stuff
	std::vector<Character*> m_enemy_units;

	// Cursor
	Vector2D m_last_cursor_tile;
	int m_tile_cursor_animate;
	int m_action_cursor_animate;

	// Hovered Unit
	Character* m_hovered;
	SelectedState m_hovered_selected_state;
	Character* m_attack_target;

	// Action UI
	int m_move_proposed_x;
	int m_move_proposed_y;
	int m_action_cursor_option;
	Vector2D m_action_ui;

	Tile m_map[EMBLEM_MAP_DIMENSION][EMBLEM_MAP_DIMENSION];
	// -1 = unchecked, 0 = can't reach, 1 = can attack, 2 = can move, 3 = occupied friendly
	int m_current_movement_matrix[EMBLEM_MAP_DIMENSION][EMBLEM_MAP_DIMENSION];

	// Misc Variables
	bool m_players_turn;
	PHASE m_phase;
	int m_phase_timer, m_phase_int; // Resuable stuff
	bool m_smart_end_turn; // Indicates that a unit has decided on an action, so the next time the phase is IDLE, it needs to check if all units are exhausted
	bool m_smart_check_routed;
	std::string m_level_clear_bool;
	Vector2D m_health_location;
	int m_exp_old, m_exp_new;
	int m_health_left;

	// Enemy AI
	Character* m_current_enemy_unit;
	Character* m_finalTarget;
	std::vector<AStar::Node> m_finalPath;
	int m_enemy_mov_points;

	// Functions
	void MoveMatrixRecurse(Vector2D previousPos, Vector2D newPos, int movesLeft, Character* c);
public:
	BattleScreen(SDL_Renderer* renderer, GameScreenManager* gsm, char* path);
	~BattleScreen();

	void Render() override;
	void Update(float deltaTime, SDL_Event e) override;

	Character* GetUnitOnTile(Vector2D tile);
	Tile* GetTile(Character* unit);

	// UI Checking
	bool CheckHovered(Character* c, int x, int y);
	void UpdateMovementMatrix();

	// Rending Movement Matrix
	void RenderMoveOverlay(int x, int y);
	void RenderAttackOverlay(int x, int y);
	void RenderDangerOverlay(int x, int y);

	// Gameplay Loops
	void PlayerTurn();
	void ExhaustUnit(Character* c);
	void EnemyTurn();
};
#endif
