#pragma once
#ifndef _TEXT_H
#define _TEXT_H
#include <iostream>
#include <SDL_ttf.h>
#include "Commons.h"

class Text
{
private:
	SDL_Renderer* m_renderer;
	SDL_Texture* m_texture;
	int m_width = 0, m_height = 0;
public:
	Text(SDL_Renderer* renderer, TTF_Font* font, std::string text, SDL_Color color = { 255,255,255 });
	Text(SDL_Renderer* renderer, TTF_Font* font, std::string text, int width, SDL_Color color = { 255,255,255 });
	~Text();

	void Render(Vector2D position);
};
#endif
