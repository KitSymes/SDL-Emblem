#include "PowBlock.h"
#include "Texture2D.h"

PowBlock::PowBlock(SDL_Renderer* renderer, MarioLevelMap* map)
{
	m_renderer = renderer;
	m_level_map = map;

	std::string imagePath = "Images/Mario/PowBlock.png";
	m_texture = new Texture2D(renderer, 64, 32);
	if (!m_texture->LoadFromFile(imagePath.c_str()))
	{
		std::cout << "Failed to load texture " << imagePath << std::endl;
		return;
	}
	m_single_sprite_w = 32;
	m_single_sprite_h = 32;
	m_num_hits_left = 3;
	m_position = Vector2D(224.0f, 256.0f);
}

PowBlock::~PowBlock()
{
	m_renderer = nullptr;
	delete m_texture;
	m_texture = nullptr;
	m_level_map = nullptr;
}

void PowBlock::TakeHit()
{
	m_num_hits_left -= 1;
	if (m_num_hits_left <= 0)
	{
		m_num_hits_left = 0;
		m_level_map->ChangeTileAt(8, 7, 0);
		m_level_map->ChangeTileAt(8, 8, 0);
	}
}

void PowBlock::Render()
{
	if (m_num_hits_left <= 0)
		return;

	SDL_Rect renderSource = { (m_num_hits_left - 1) * m_single_sprite_w, 0, 32, 32 };

	m_texture->Render(m_position, &renderSource, SDL_FLIP_NONE);
}
