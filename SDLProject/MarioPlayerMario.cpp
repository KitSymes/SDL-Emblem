#include "MarioPlayerMario.h"
#include "Texture2D.h"
#include "Constants.h"

MarioPlayerMario::MarioPlayerMario(SDL_Renderer* renderer, std::string imagePath, Vector2D start_position, MarioScreen* screen) : MarioCharacter(renderer, imagePath, start_position, screen)
{
	m_jump_force = 10.0f;
	m_max_momentum = 6.0f;
}

MarioPlayerMario::~MarioPlayerMario()
{
}

void MarioPlayerMario::Update(float deltaTime, SDL_Event e)
{
	MarioCharacter::Update(deltaTime, e);

	switch (e.type)
	{
	case SDL_KEYDOWN:
		switch (e.key.keysym.sym)
		{
		case SDLK_a:
			m_moving_left = true;
			break;
		case SDLK_d:
			m_moving_right = true;
			break;
		case SDLK_w:
			Jump();
			break;
		}
		break;
	case SDL_KEYUP:
		switch (e.key.keysym.sym)
		{
		case SDLK_a:
			m_moving_left = false;
			m_velocity_left = 0;
			break;
		case SDLK_d:
			m_moving_right = false;
			m_velocity_right = 0;
			break;
		}
		break;
	}
}