#pragma once
#ifndef _TEXTURE2D_H
#define _TEXTURE2D_H
#include <SDL.h>
#include <string>
#include "Commons.h"
class Texture2D
{
public:
	Texture2D(SDL_Renderer* renderer);
	Texture2D(SDL_Renderer* renderer, int width, int height);
	//Texture2D(SDL_Renderer* renderer, int width, int height, int sx, int sy, int sWidth, int sHeight);
	~Texture2D();

	bool LoadFromFile(std::string path);
	void Free();
	void Render(Vector2D position, SDL_RendererFlip flip, int scale = 1.0, double angle = 0.0);
	void Render(Vector2D position, SDL_Rect* source, SDL_RendererFlip flip, int scale = 1.0, double angle = 0.0);
	void Render(SDL_Rect* source, SDL_Rect* dest, SDL_RendererFlip flip, int scale = 1.0, double angle = 0.0);

	int GetWidth() { return m_width; }
	int GetHeight() { return m_height; }

	//int m_sourceX, m_sourceY, m_sourceWidth, m_sourceHeight;
private:
	SDL_Renderer* m_renderer;
	SDL_Texture* m_texture;

	int m_width = -1, m_height = -1;

	void Init(SDL_Renderer* renderer, int width, int height/*, int sx, int sy, int sWidth, int sHeight*/);
};
#endif
