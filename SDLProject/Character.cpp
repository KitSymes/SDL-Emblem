#include "Character.h"
#include "Texture2D.h"

Character::Character(SDL_Renderer* renderer, std::string imagePath, Vector2D start_position)
{
	m_renderer = renderer;
	m_position = start_position;
	m_texture = new Texture2D(renderer, EMBLEM_TILE_DIMENSION, EMBLEM_TILE_DIMENSION);
	m_sourceX = 0;
	m_sourceY = 0;
	m_animate = true;

	if (!m_texture->LoadFromFile(imagePath))
		std::cout << "Image " << imagePath << " failed to load" << std::endl;
}

Character::Character(SDL_Renderer* renderer, MOVE_TYPE move_type, WEAPON_TYPE weapon_type, bool friendly, Vector2D start_position)
{
	m_renderer = renderer;
	m_position = start_position;
	m_texture = new Texture2D(renderer, EMBLEM_TILE_DIMENSION, EMBLEM_TILE_DIMENSION);
	m_sourceX = 0;
	m_sourceY = 0;
	m_animate = true;
	m_mov_type = move_type;
	m_weapon_type = weapon_type;
	m_range = 1;
	m_alive = false;
	m_level = 1;
	m_exp = 0;
	m_friendly = friendly;
	m_moved = false;


	std::string imagePath = "Images/";
	imagePath += (friendly ? "Allies" : "Enemies");
	imagePath += "/";
	switch (move_type)
	{
	case INFANTRY:
		imagePath += "Infantry";
		break;
	default:
		std::cout << "Move Type Not Handled in Character " + move_type << std::endl;
		break;
	}
	imagePath += "/";
	switch (weapon_type)
	{
	case LANCE:
		imagePath += "Lancer";
		break;
	default:
		std::cout << "Weapon Type Not Handled in Character " + weapon_type << std::endl;
		break;
	}
	imagePath += ".png";

	if (!m_texture->LoadFromFile(imagePath))
		std::cout << "Image " << imagePath << " failed to load" << std::endl;
}

Character::~Character()
{
	m_renderer = nullptr;
	delete m_texture;
	m_texture = nullptr;
}

void Character::Render()
{
	SDL_Rect source = { m_sourceX, (m_animate ? m_sourceY : 0), EMBLEM_TILE_DIMENSION, EMBLEM_TILE_DIMENSION / 2 };
	m_texture->Render(m_position, &source, SDL_FLIP_NONE);

	frame++;
	if (frame >= 20)
		frame = 0;

	if (frame < 10)
		m_sourceY = 0;
	else
		m_sourceY = EMBLEM_TILE_DIMENSION / 2;
}

void Character::Update(float deltaTime, SDL_Event e)
{
	/*switch (e.type)
	{
	case SDL_KEYDOWN:
		switch (e.key.keysym.sym)
		{
		case SDLK_a:
			m_position.x -= 32;
			break;
		case SDLK_d:
			m_position.x += 32;
			break;
		case SDLK_w:
			m_position.y -= 32;
			break;
		case SDLK_s:
			m_position.y += 32;
			break;
		}
		break;
	}*/
}

void Character::SetRawPosition(Vector2D new_position)
{
	m_position = new_position;
}

void Character::SetMapPosition(int x, int y)
{
	m_map_pos = Vector2D(x, y);
	SetRawPosition(Vector2D(x * EMBLEM_TILE_DIMENSION, y * EMBLEM_TILE_DIMENSION));
}

void Character::SetMoved(bool moved)
{
	m_moved = moved;
	m_animate = !moved;
}

void Character::RandomStats(int level)
{
	m_attack = level;
	m_defence = level / 2;
	m_max_health = 5;
	m_speed = 1;

	for (int i = 0; i < level; i++)
		LevelUp();

	m_health = m_max_health;
}

void Character::LevelUp()
{
	m_attack += (rand() % 3);
	m_defence += (rand() % 3);
	m_speed += (rand() % 4);
	m_max_health += (rand() % 4);
}
