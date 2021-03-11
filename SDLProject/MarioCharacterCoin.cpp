#include "MarioCharacterCoin.h"

MarioCharacterCoin::MarioCharacterCoin(SDL_Renderer* renderer, std::string imagePath, Vector2D start_position, MarioScreen* screen)
	: MarioCharacter(renderer, imagePath, start_position, screen)
{
	m_single_sprite_w = m_texture->GetWidth() / 3;
	m_single_sprite_h = m_texture->GetHeight();
	m_spin_timer = -60;
}

MarioCharacterCoin::~MarioCharacterCoin()
{
}

void MarioCharacterCoin::Render()
{
	m_spin_timer+=2;
	if (m_spin_timer == -20)
		m_spin_timer += 00;
	if (m_spin_timer > 40)
		m_spin_timer = -59;
	int spriteXMin = ((m_spin_timer > 0 ? m_spin_timer : -m_spin_timer) / 20) * 16;
	SDL_Rect sprite = { spriteXMin, 0, m_single_sprite_w, m_single_sprite_h };
	if (m_spin_timer > 0)
		m_texture->Render(m_position, &sprite, SDL_FLIP_HORIZONTAL);
	else
		m_texture->Render(m_position, &sprite, SDL_FLIP_NONE);
}

void MarioCharacterCoin::Update(float deltaTime, SDL_Event e)
{
}
