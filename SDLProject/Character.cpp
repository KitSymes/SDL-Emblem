#include "Character.h"
#include "Texture2D.h"

Character::Character(SDL_Renderer* renderer, std::string imagePath, Vector2D start_position)
{
	m_renderer = renderer;
	m_position = start_position;
	m_texture = new Texture2D(renderer, 32, 32);
	m_sourceX = 0;
	m_sourceY = 0;
	m_sourceWidth = 16;
	m_sourceHeight = 16;

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
	SDL_Rect source = {m_sourceX, m_sourceY, m_sourceWidth, m_sourceHeight};
	m_texture->Render(m_position, &source, SDL_FLIP_NONE);

	frame++;
	if (frame >= 20)
		frame = 0;

	if (frame < 10)
		m_sourceY = 0;
	else
		m_sourceY = 16;
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

void Character::setPosition(Vector2D new_position)
{
	m_position = new_position;
}

Vector2D Character::GetPosition()
{
	return m_position;
}
