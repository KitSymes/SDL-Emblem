#pragma once
#ifndef _BATTLESCREEN_H
#define _BATTLESCREEN_H
#include "Commons.h"
#include "GameScreen.h"
#include "Constants.h"
#include "Character.h"
#include <vector>
#include <SDL.h>
class Texture2D;
class BattleScreen : public GameScreen
{
private:
	// Textures / Fonts
	Texture2D* m_background_texture;
	Texture2D* m_overlays_texture;
	Texture2D* m_tile_cursor_texture;
	Texture2D* m_action_background_texture;
	Texture2D* m_action_cursor_texture;

	// Enemy Stuff
	std::vector<Character*> m_enemy_units;

	// Cursor
	Vector2D m_last_cursor_tile;
	int m_tile_cursor_animate;
	int m_action_cursor_animate;

	// Hovered Unit
	Character* m_hovered;
	SelectedState m_hovered_selected_state;

	// Action UI
	int m_action_cursor_option;
	Vector2D m_action_ui;

	Tile m_map[EMBLEM_MAP_DIMENSION][EMBLEM_MAP_DIMENSION];
	// -1 = unchecked, 0 = can't reach, 1 = can attack, 2 = can move, 3 = occupied friendly
	int m_current_movement_matrix[EMBLEM_MAP_DIMENSION][EMBLEM_MAP_DIMENSION];

	// Misc Variables
	bool m_players_turn;

	// Functions
	void MoveMatrixRecurse(Vector2D previousPos, Vector2D newPos, int movesLeft, Character* c);
public:
	BattleScreen(SDL_Renderer* renderer, GameScreenManager* gsm, char* path);
	~BattleScreen();

	void Render() override;
	void Update(float deltaTime, SDL_Event e) override;

	// UI Checking
	bool CheckHovered(Character* c, int x, int y);
	void UpdateMovementMatrix();

	// Rending Movement Matrix
	void RenderMoveOverlay(int x, int y);
	void RenderAttackOverlay(int x, int y);
	void RenderDangerOverlay(int x, int y);

	// Gameplay Loops
	void PlayerTurn();
	void EnemyTurn();
};
#endif
