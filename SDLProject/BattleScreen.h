#pragma once
#ifndef _BATTLESCREEN_H
#define _BATTLESCREEN_H
#include "Commons.h"
#include "GameScreen.h"
#include "Constants.h"
#include "Character.h"
#include <vector>
class Texture2D;
class BattleScreen : public GameScreen
{
private:
	Texture2D* m_background_texture;
	Texture2D* m_overlays_texture;
	Texture2D* m_tile_cursor_texture;
	Tile m_map[EMBLEM_MAP_DIMENSION][EMBLEM_MAP_DIMENSION];
	std::vector<Character*> m_enemy_units;
	bool m_players_turn;
	Character* m_hovered;
	// -1 = unchecked, 0 = can't reach, 1 = can attack, 2 = can move, 3 = occupied friendly
	int m_current_movement_matrix[EMBLEM_MAP_DIMENSION][EMBLEM_MAP_DIMENSION];
	Vector2D m_last_cursor_tile;
	int m_tile_cursor_animate;
	bool m_hovered_selected;

	void MoveMatrixRecurse(Vector2D previousPos, Vector2D newPos, int movesLeft, Character* c);
public:
	BattleScreen(SDL_Renderer* renderer, GameScreenManager* gsm, char* path);
	~BattleScreen();

	bool CheckHovered(Character* c, int x, int y);
	void UpdateMovementMatrix();

	void Render() override;
	void Update(float deltaTime, SDL_Event e) override;

	void RenderMoveOverlay(int x, int y);
	void RenderAttackOverlay(int x, int y);
	void RenderDangerOverlay(int x, int y);

	void PlayerTurn();

	void EnemyTurn();
};
#endif
