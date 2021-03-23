#pragma once
#ifndef _CHARACTER_H
#define _CHARACTER_H
#include <SDL.h>
#include <iostream>
#include <string>
#include "Commons.h"
#include "Constants.h"
class Texture2D;
class Character
{
protected:
	SDL_Renderer* m_renderer;
	Vector2D m_position;
	Texture2D* m_texture;
	int frame = 0;
	int m_sourceX, m_sourceY;
	int m_range;
	MOVE_TYPE m_mov_type;
	WEAPON_TYPE m_weapon_type;
	bool m_animate;
	bool m_alive;
	bool m_friendly;
	Vector2D m_map_pos;
	bool m_moved;
public:
	int m_attack, m_defence, m_speed;
	int m_health, m_max_health;
	int m_level;
	int m_exp;

	Character(SDL_Renderer* renderer, std::string imagePath, Vector2D start_position);
	Character(SDL_Renderer* renderer, MOVE_TYPE mov, WEAPON_TYPE weap, bool friendly, Vector2D start_position);
	~Character();

	virtual void Render();
	virtual void Update(float deltaTime, SDL_Event e);

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
	WEAPON_TYPE GetWeaponType() { return m_weapon_type; }
	int GetRange() { return m_range; }
	bool IsFriendly() { return m_friendly; }
};
#endif
