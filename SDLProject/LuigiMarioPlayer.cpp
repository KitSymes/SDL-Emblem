#include "LuigiMarioPlayer.h"
#include "Texture2D.h"
#include "Constants.h"

LuigiMarioPlayer::LuigiMarioPlayer(SDL_Renderer* renderer, std::string imagePath, Vector2D start_position, MarioScreen* screen) : MarioPlayer(renderer, imagePath, start_position, screen)
{
	m_jump_force = 12.0f;
}

LuigiMarioPlayer::~LuigiMarioPlayer()
{

}

void LuigiMarioPlayer::Update(float deltaTime, SDL_Event e)
{
	MarioPlayer::Update(deltaTime, e);

	switch (e.type)
	{
	case SDL_KEYDOWN:
		switch (e.key.keysym.sym)
		{
		case SDLK_LEFT:
			m_moving_left = true;
			break;
		case SDLK_RIGHT:
			m_moving_right = true;
			break;
		case SDLK_UP:
			Jump(deltaTime);
			break;
		}
		break;
	case SDL_KEYUP:
		switch (e.key.keysym.sym)
		{
		case SDLK_LEFT:
			m_moving_left = false;
			m_velocity_left = 0;
			break;
		case SDLK_RIGHT:
			m_moving_right = false;
			m_velocity_right = 0;
			break;
		}
		break;
	}
}