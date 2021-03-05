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

class MarioPlayer
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

	MarioScreen* m_screen;
public:
	MarioPlayer(SDL_Renderer* renderer, std::string imagePath, Vector2D start_position, MarioScreen* screen);
	~MarioPlayer();

	virtual void Render();
	virtual void Update(float deltaTime, SDL_Event e);

	void setPosition(Vector2D new_position);
	Vector2D GetPosition();
	double GetCollisionRadius();
	void MoveLeft(float deltaTime);
	void MoveRight(float deltaTime);
	void AddGravity(float deltaTime);
	void Jump(float deltaTime);
	Rect2D GetCollisionBox() { return Rect2D(m_position.x, m_position.y, m_texture->GetWidth(), m_texture->GetHeight()); }
};
#endif
