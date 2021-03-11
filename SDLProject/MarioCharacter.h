#pragma once
#ifndef _MARIOPLAYER_H
#define _MARIOPLAYER_H
#include <SDL.h>
#include <iostream>
#include "Commons.h"
#include <string>
#include "Texture2D.h"
#include "MarioScreen.h"
#include "Collisions.h"

class MarioCharacter
{
protected:
	SDL_Renderer* m_renderer;
	Vector2D m_position;
	Texture2D* m_texture;
	FACING m_facing_direction;

	bool m_moving_left;
	bool m_moving_right;
	float m_velocity_right = 0;
	float m_velocity_left = 0;
	float m_velocity_vertical;
	bool m_jumping;
	bool m_can_jump;
	float m_jump_force;
	float m_max_momentum;
	double m_collision_radius;
	float m_speed = 1.0f;

	MarioScreen* m_screen;
protected:
	bool m_alive;
	bool m_canMoveOfscreen = false;
	bool m_screen_bounce = false;
public:
	MarioCharacter(SDL_Renderer* renderer, std::string imagePath, Vector2D start_position, MarioScreen* screen);
	~MarioCharacter();

	virtual void Render();
	virtual void Update(float deltaTime, SDL_Event e);

	void setPosition(Vector2D new_position);
	Vector2D GetPosition();
	double GetCollisionRadius();
	void MoveLeft(float deltaTime);
	void MoveRight(float deltaTime);
	void AddGravity(float deltaTime);
	void Jump();

	Rect2D GetCollisionBox() { return Rect2D(m_position.x, m_position.y, m_texture->GetWidth(), m_texture->GetHeight()); }
	void SetAlive(bool alive) { m_alive = alive; }
	bool IsAlive() { return m_alive; }

};
#endif
