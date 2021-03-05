#pragma once
#ifndef _CHARACTER_H
#define _CHARACTER_H
#include <SDL.h>
#include <iostream>
#include "Commons.h"
#include <string>
class Texture2D;
class Character
{
protected:
	SDL_Renderer* m_renderer;
	Vector2D m_position;
	Texture2D* m_texture;
	int frame = 0;
	int m_sourceX, m_sourceY;
	int m_sourceWidth, m_sourceHeight;
public:
	Character(SDL_Renderer* renderer, std::string imagePath, Vector2D start_position);
	~Character();

	virtual void Render();
	virtual void Update(float deltaTime, SDL_Event e);
	void setPosition(Vector2D new_position);
	Vector2D GetPosition();
};
#endif
