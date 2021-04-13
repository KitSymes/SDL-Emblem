#pragma once
#ifndef _CHARACTER_H
#define _CHARACTER_H
#include <SDL.h>
#include <iostream>
#include <string>
#include <SDL_ttf.h>
#include "Commons.h"
#include "Constants.h"
#include "Text.h"
class Texture2D;
class Character
{
protected:
	SDL_Renderer* m_renderer;

	int frame = 0;

	int m_sourceX, m_sourceY;
	Texture2D* m_texture;

	Vector2D m_position;
	Vector2D m_map_pos;

	bool m_alive;
	bool m_moved;
	bool m_animate;
	bool m_friendly;

	int m_range;
	MOVE_TYPE m_mov_type;
	WEAPON_TYPE m_weapon_type;

	// Text
	Text* m_t_name = nullptr;
	Text* m_t_level = nullptr;
	Text* m_t_atk = nullptr;
	Text* m_t_def = nullptr;
	Text* m_t_hp = nullptr;

	// Functions
	void DeleteText();
public:
	int m_attack, m_defence, m_speed;
	int m_health, m_max_health;
	int m_level;
	int m_exp;

	Character(SDL_Renderer* renderer, std::string imagePath, Vector2D start_position = Vector2D());
	Character(SDL_Renderer* renderer, MOVE_TYPE mov, WEAPON_TYPE weap, bool friendly, Vector2D start_position = Vector2D());
	~Character();

	virtual void Render();
	virtual void Update(float deltaTime, SDL_Event e);

	void UpdateText(TTF_Font* font);
	void RenderText(Vector2D pos);

	void RandomStats(int level);
	void LevelUp();

	Vector2D GetRawPosition() { return m_position; };
	void SetRawPosition(Vector2D new_position);

	Vector2D GetMapPosition() { return m_map_pos; };
	void SetMapPosition(int x, int y);

	bool IsAlive() { return m_alive; };
	void SetAlive(bool alive) { m_alive = alive; };

	bool HasMoved() { return m_moved; }
	void SetMoved(bool moved);

	MOVE_TYPE GetMoveType() { return m_mov_type; }
	int GetMoves() 
	{
		switch (GetMoveType())
		{
		case INFANTRY:
			return 3;
		default:
			return 1;
		}
	}
	WEAPON_TYPE GetWeaponType() { return m_weapon_type; }
	int GetRange() { return m_range; }
	bool IsFriendly() { return m_friendly; }

	bool AttackedBy(Character* attacker, Tile* theirs, Tile* mine);
	int GetDamageResult(Character* defender, Tile* theirs, Tile* mine);
};
#endif
