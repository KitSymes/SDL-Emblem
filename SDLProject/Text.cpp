#include "Text.h"

Text::Text(SDL_Renderer* renderer, TTF_Font* font, std::string text, SDL_Color color)
{
	m_renderer = renderer;
	SDL_Surface* p_surface = TTF_RenderText_Solid(font, text.c_str(), color);
	m_texture = SDL_CreateTextureFromSurface(renderer, p_surface);
	m_width = p_surface->w;
	m_height = p_surface->h;
	SDL_FreeSurface(p_surface);
}

Text::Text(SDL_Renderer* renderer, TTF_Font* font, std::string text, int width, SDL_Color color)
{
	m_renderer = renderer;
	SDL_Surface* p_surface = TTF_RenderText_Blended_Wrapped(font, text.c_str(), color, width);
	m_texture = SDL_CreateTextureFromSurface(renderer, p_surface);
	m_width = p_surface->w;
	m_height = p_surface->h;
	SDL_FreeSurface(p_surface);
}

Text::~Text()
{
	SDL_DestroyTexture(m_texture);
}

void Text::Render(Vector2D position)
{
	SDL_Rect renderLocation = { position.x, position.y, m_width, m_height };
	SDL_Rect renderSource = { 0, 0, m_width, m_height };
	SDL_RenderCopyEx(m_renderer, m_texture, &renderSource, &renderLocation, 0, NULL, SDL_FLIP_NONE);
}