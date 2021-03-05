#pragma once
#include <SDL.h>
#include "MarioLevelMap.h"
#include "Commons.h"
#include <iostream>
class Texture2D;
class PowBlock
{
private:
	SDL_Renderer* m_renderer;
	Vector2D m_position;
	Texture2D* m_texture;
	MarioLevelMap* m_level_map;

	float m_single_sprite_w;
	float m_single_sprite_h;
	int m_num_hits_left;
public:
	PowBlock(SDL_Renderer* renderer, MarioLevelMap* map);
	~PowBlock();

	void Render();
	Rect2D GetCollisionBox() { return Rect2D(m_position.x, m_position.y, m_single_sprite_w, m_single_sprite_h); };
	void TakeHit();
	bool IsAvailable() { return m_num_hits_left > 0; };
};

