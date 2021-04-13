#include "Character.h"
#include "Texture2D.h"
#include "Utils.h"

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
	imagePath += Utils::MoveTypeToString(move_type);
	imagePath += "/";
	switch (weapon_type)
	{
	case LANCE:
		imagePath += "Lancer";
		break;
	case SWORD:
		imagePath += "Swordsman";
		break;
	case AXE:
		imagePath += "Huntsman";
		break;
	case BOW:
		imagePath += "Bowman";
		m_range = 2;
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
	DeleteText();

	delete m_texture;
	m_texture = nullptr;

	m_renderer = nullptr;
}

void Character::DeleteText()
{
	if (m_t_name != nullptr)
	{
		delete m_t_name;
		m_t_name = nullptr;
	}

	if (m_t_atk != nullptr)
	{
		delete m_t_atk;
		m_t_atk = nullptr;
	}

	if (m_t_def != nullptr)
	{
		delete m_t_def;
		m_t_def = nullptr;
	}

	if (m_t_hp != nullptr)
	{
		delete m_t_hp;
		m_t_hp = nullptr;
	}
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
}

void Character::UpdateText(TTF_Font* font)
{
	DeleteText();
	m_t_name = new Text(m_renderer, font, Utils::MoveTypeToString(m_mov_type).append(" " + Utils::WeaponTypeToString(m_weapon_type)), 88);
	m_t_level = new Text(m_renderer, font, "Lvl: " + std::to_string(m_level));
	m_t_atk = new Text(m_renderer, font, "Atk: " + std::to_string(m_attack));
	m_t_def = new Text(m_renderer, font, "Def: " + std::to_string(m_defence));
	std::string str = "HP: ";
	str.append(std::to_string(m_health)).append("/").append(std::to_string(m_max_health) + "");
	m_t_hp = new Text(m_renderer, font, str);
}

void Character::UpdateText(TTF_Font* font, Tile* tile)
{
	DeleteText();
	m_t_name = new Text(m_renderer, font, Utils::MoveTypeToString(m_mov_type).append(" " + Utils::WeaponTypeToString(m_weapon_type)), 88);
	m_t_level = new Text(m_renderer, font, "Lvl: " + std::to_string(m_level));
	m_t_atk = new Text(m_renderer, font, "Atk: " + std::to_string(m_attack + tile->attackBonus));
	m_t_def = new Text(m_renderer, font, "Def: " + std::to_string(m_defence + tile->defenceBonus));
	std::string str = "HP: ";
	str.append(std::to_string(m_health)).append("/").append(std::to_string(m_max_health) + "");
	m_t_hp = new Text(m_renderer, font, str);
}

void Character::RenderText(Vector2D pos)
{
	int offset = 20;

	if (m_t_name != nullptr)
		m_t_name->Render(Vector2D(pos.x, pos.y));

	if (m_t_level != nullptr)
		m_t_level->Render(Vector2D(pos.x, pos.y + offset * 2));

	if (m_t_atk != nullptr)
		m_t_atk->Render(Vector2D(pos.x, pos.y + offset * 3));

	if (m_t_def != nullptr)
		m_t_def->Render(Vector2D(pos.x, pos.y + offset * 4));

	if (m_t_hp != nullptr)
		m_t_hp->Render(Vector2D(pos.x, pos.y + offset * 5));
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
	m_attack = 1;
	//m_defence = level;
	m_level = 0;
	m_max_health = 5;
	m_speed = 1;

	for (int i = 0; i < level; i++)
		LevelUp();

	m_health = m_max_health;
}

void Character::LevelUp()
{
	m_level++;
	m_attack += (rand() % 2) + 1;
	if (m_friendly)
		m_attack++;
	if (m_weapon_type == BOW)
		m_attack += (rand() % 1);
	m_defence += (rand() % 1) + 1;
	if (m_weapon_type != BOW)
		m_defence++;
	m_speed += (rand() % 3) + 1;
	int extraHealth = (rand() % 3) + 1;
	m_max_health += extraHealth;
	m_health += extraHealth;
}

bool Character::AttackedBy(Character* attacker, Tile* theirs, Tile* mine)
{
	m_health -= attacker->GetDamageResult(this, mine, theirs);
	if (m_health <= 0)
	{
		m_health = 0;
		SetAlive(false);
		return true;
	}
	return false;
}

int Character::GetDamageResult(Character* defender, Tile* theirs, Tile* mine)
{
	float multi = 1;

	if (Utils::IsEffective(m_weapon_type, defender->GetWeaponType()))
		multi = 1.5;
	else if (Utils::IsIneffective(m_weapon_type, defender->GetWeaponType()))
		multi = 0.75;

	switch (m_weapon_type)
	{
	case LANCE:
	case AXE:
	case SWORD:
	case BOW:
		int damage = (m_attack + mine->attackBonus + (multi > 1 ? (m_attack < 5 ? 2 : 0) : 0)) - (defender->m_defence + theirs->defenceBonus);
		if (damage < 0)
			damage = 0;
		damage *= multi;
		return damage;
	}
}
