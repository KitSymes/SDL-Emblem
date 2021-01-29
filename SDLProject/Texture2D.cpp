#include "Texture2D.h"
#include <SDL_Image.h>
#include <iostream>

Texture2D::Texture2D(SDL_Renderer* renderer)
{
	m_renderer = renderer;
}

Texture2D::~Texture2D()
{
	Free();

	m_renderer = nullptr;
}

bool Texture2D::LoadFromFile(std::string path)
{
	// Clear memory used for last texture
	Free();

	SDL_Surface* p_surface = IMG_Load(path.c_str());
	if (p_surface != nullptr)
	{
		// Colour key the Image to be transparent
		SDL_SetColorKey(p_surface, SDL_TRUE, SDL_MapRGB(p_surface->format, 0, 0XFF, 0XFF));
		m_texture = SDL_CreateTextureFromSurface(m_renderer, p_surface);
		if (m_texture == nullptr)
		{
			std::cout << "Unable to create texture from surface. Error: " << SDL_GetError();
		}
		else
		{
			m_width = p_surface->w;
			m_height = p_surface->h;
		}
		SDL_FreeSurface(p_surface);
	}
	else {
		std::cout << "Unable to create texture from surface. Error: " << IMG_GetError();
	}

	return m_texture != nullptr;
}

void Texture2D::Free()
{
	if (m_texture != nullptr)
	{
		SDL_DestroyTexture(m_texture);
		m_texture = nullptr;
	}
}

void Texture2D::Render(Vector2D position, SDL_RendererFlip flip, double angle)
{
	SDL_Rect renderLocation = { position.x, position.y, m_width, m_height };
	SDL_RenderCopyEx(m_renderer, m_texture, NULL, &renderLocation, angle, NULL, flip);
}
