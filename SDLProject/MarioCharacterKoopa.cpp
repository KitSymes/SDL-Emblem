#include "MarioCharacterKoopa.h"
#include "Constants.h"

MarioCharacterKoopa::MarioCharacterKoopa(SDL_Renderer* renderer, std::string imagePath, Vector2D start_position, float speed, MarioScreen* screen)
	: MarioCharacter(renderer, imagePath, start_position, screen)
{
	m_facing_direction = (start_position.x < SCREEN_WIDTH / 2 ? FACING_RIGHT : FACING_LEFT);
	m_injured = false;
	m_canMoveOfscreen = true;
	m_screen_bounce = true;

	m_single_sprite_w = m_texture->GetWidth() / 2;
	m_single_sprite_h = m_texture->GetHeight();

	m_speed = speed;

	if (m_facing_direction == FACING_LEFT)
	{
		m_moving_left = true;
		m_moving_right = false;
	}
	else
	{
		m_moving_left = false;
		m_moving_right = true;
	}
}

MarioCharacterKoopa::~MarioCharacterKoopa()
{
}

void MarioCharacterKoopa::TakeDamage()
{
	m_injured = true;
	m_injured_time = KOOPA_INJURED_TIME;
	Jump();
}

void MarioCharacterKoopa::FlipRightwayUp()
{
	m_facing_direction = (m_facing_direction == FACING_RIGHT ? FACING_LEFT : FACING_RIGHT);
	m_injured = false;
	Jump();
}

void MarioCharacterKoopa::Render()
{
	int spriteXMin = (m_injured ? m_single_sprite_w : 0.0f);
	SDL_Rect sprite = { spriteXMin, 0, m_single_sprite_w, m_single_sprite_h };

	if (m_facing_direction == FACING_RIGHT)
		m_texture->Render(m_position, &sprite, SDL_FLIP_NONE);
	else
		m_texture->Render(m_position, &sprite, SDL_FLIP_HORIZONTAL);
}

void MarioCharacterKoopa::Update(float deltaTime, SDL_Event e)
{
	MarioCharacter::Update(deltaTime, e);

	if (!m_injured)
	{
		if (m_facing_direction == FACING_LEFT)
		{
			m_moving_left = true;
			m_moving_right = false;
		}
		else
		{
			m_moving_left = false;
			m_moving_right = true;
		}
	}
	else
	{
		m_moving_left = false;
		m_moving_right = false;

		m_injured_time -= deltaTime;

		if (m_injured_time <= 0.0f)
			FlipRightwayUp();
	}
}
