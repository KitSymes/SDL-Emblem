#include "MarioCharacter.h"
#include "Constants.h"

MarioCharacter::MarioCharacter(SDL_Renderer* renderer, std::string imagePath, Vector2D start_position, MarioScreen* screen)
{
	m_renderer = renderer;
	m_position = start_position;
	m_texture = new Texture2D(renderer);
	m_facing_direction = FACING_RIGHT;
	m_moving_left = false;
	m_moving_right = false;
	m_can_jump = true;
	m_jumping = false;
	m_jump_force = 6.0f;
	m_max_momentum = 5.0f;
	m_collision_radius = 15.0;
	m_screen = screen;
	m_alive = true;

	if (!m_texture->LoadFromFile(imagePath))
		std::cout << "Image " << imagePath << " failed to load" << std::endl;
}

MarioCharacter::~MarioCharacter()
{
	m_renderer = nullptr;
	delete m_texture;
	m_texture = nullptr;
}

void MarioCharacter::Render()
{
	if (m_facing_direction == FACING_RIGHT)
		m_texture->Render(m_position, SDL_FLIP_NONE);
	else
		m_texture->Render(m_position, SDL_FLIP_HORIZONTAL);
}

void MarioCharacter::Update(float deltaTime, SDL_Event e)
{
	/*if (m_moving_left)
	{
			MoveLeft(deltaTime);
	}*/

	if (m_moving_left)
		MoveLeft(deltaTime);

	if (m_moving_right)
		MoveRight(deltaTime);

	if (m_position.y >= SCREEN_HEIGHT - m_texture->GetHeight())
		m_can_jump = true;
	if (m_position.y < 0) {
		m_position.y = 0;
		if (m_velocity_vertical < 0)
			m_velocity_vertical = 0;
	}

	int centralX_position = (int)(m_position.x + (m_texture->GetWidth() * 0.5)) / MARIO_TILE_WIDTH;
	int xMaxPosition = (int)(m_position.x + (m_texture->GetWidth() * 0.9)) / MARIO_TILE_WIDTH;
	int xMinPosition = (int)(m_position.x + (m_texture->GetWidth() * 0.1)) / MARIO_TILE_WIDTH;
	int foot_position = (int)(m_position.y + m_texture->GetHeight()) / MARIO_TILE_HEIGHT;
	int yMaxPositionAfterMove = (int)(m_position.y + m_texture->GetHeight() + m_velocity_vertical) / MARIO_TILE_HEIGHT;
	int yMinPositionAfterMove = (m_position.y + m_velocity_vertical) / MARIO_TILE_HEIGHT;

	bool addVelo = true;
	bool hitPow = false;
	bool bonked = false;
	// Checking for Head Collisions
	{
		switch (m_screen->GetLevelMap()->GetTileAt(yMinPositionAfterMove, xMinPosition))
		{
		case 2:
			hitPow = true;
		case 1:
			bonked = true;
			break;
		default:
			break;
		}

		switch (m_screen->GetLevelMap()->GetTileAt(yMinPositionAfterMove, xMaxPosition))
		{
		case 2:
			hitPow = true;
		case 1:
			bonked = true;
			break;
		default:
			break;
		}
		if (bonked)
		{
			m_velocity_vertical = 1; // Head Bonk
			addVelo = false;
		}
		if (hitPow)
			m_screen->UpdatePOWBlock();
	}

	// Checking for Feet Collisions
	{
		bool stopped = false;
		switch (m_screen->GetLevelMap()->GetTileAt(yMaxPositionAfterMove, xMinPosition))
		{
		case 2:
		case 1:
			stopped = true;
			break;
		default:
			break;
		}

		switch (m_screen->GetLevelMap()->GetTileAt(yMaxPositionAfterMove, xMaxPosition))
		{
		case 2:
		case 1:
			stopped = true;
			break;
		default:
			break;
		}
		if (stopped)
		{
			m_velocity_vertical = 0;
			m_position.y = MARIO_TILE_HEIGHT * yMaxPositionAfterMove - m_texture->GetHeight();
			m_can_jump = true;
		}
		else m_can_jump = false;
		int yPlus = (int)(m_position.y + m_texture->GetHeight() + 0.01) / MARIO_TILE_HEIGHT;
		if ((m_screen->GetLevelMap()->GetTileAt(yPlus, xMinPosition) == 0
			&& m_screen->GetLevelMap()->GetTileAt(yPlus, xMinPosition) == 0)
			|| m_velocity_vertical < 0)
			AddGravity(deltaTime);
		else {
			if (m_velocity_vertical > 0)
				m_velocity_vertical = 0;
		}
	}

	if (addVelo)
		m_position.y += m_velocity_vertical;
	else
		m_position.y = (m_position.y + m_velocity_vertical) - ((int)(m_position.y + m_velocity_vertical) % ((int)MARIO_TILE_HEIGHT));

	if (m_position.y > SCREEN_HEIGHT - m_texture->GetHeight())
	{
		m_position.y = SCREEN_HEIGHT - m_texture->GetHeight();
		if (m_velocity_vertical > 0)
			m_velocity_vertical = 0.0f;
	}
}

void MarioCharacter::setPosition(Vector2D new_position)
{
	m_position = new_position;
}

Vector2D MarioCharacter::GetPosition()
{
	return m_position;
}

void MarioCharacter::MoveLeft(float deltaTime)
{
	m_facing_direction = FACING_LEFT;

	Rect2D coll = GetCollisionBox();
	coll.height -= 2; // Make it slightly shorter than sprite itself
	coll.y += 1;
	Rect2D tileBox = Rect2D(0, 0, 32, 32);
	bool canMove = true;
	//std::cout << "Mario At " << m_position.x / MARIO_TILE_WIDTH << " " << m_position.y / MARIO_TILE_HEIGHT << std::endl;
	for (int i = 0; i < 2; i++)
	{
		int h = (int)((m_position.y + (m_texture->GetHeight() * 0.9f)) / MARIO_TILE_HEIGHT) + (i - 1);
		int w = (int)((m_position.x) / MARIO_TILE_WIDTH);
		if (h < 0 || h > MARIO_MAP_HEIGHT)
			continue;
		if (m_position.x < 0)
		{
			canMove = m_canMoveOfscreen;
			break;
		}
		else if (w > MARIO_MAP_WIDTH)
			continue;
		int tileType = m_screen->GetLevelMap()->GetTileAt(h, w);
		if (tileType == 0) continue;
		//std::cout << "Tile At " << w << " " << h << std::endl;
		tileBox.y = (h)*MARIO_TILE_HEIGHT;
		tileBox.x = (w)*MARIO_TILE_WIDTH;
		canMove = !Collisions::Instance()->Box(coll, tileBox);
		if (!canMove)
			break;
	}

	if (canMove)
	{
		m_position.x -= deltaTime * MOVEMENTSPEED * m_speed * m_velocity_left;
		if (m_velocity_left < m_max_momentum)
			m_velocity_left += 0.1;
	}
}

void MarioCharacter::MoveRight(float deltaTime)
{
	m_facing_direction = FACING_RIGHT;

	Rect2D coll = GetCollisionBox();
	coll.height -= 2; // Make it slightly shorter than sprite itself
	coll.y += 1;
	Rect2D tileBox = Rect2D(0, 0, 32, 32);
	bool canMove = true;
	//std::cout << "Mario At " << (m_position.x + (m_texture->GetWidth())) / MARIO_TILE_WIDTH << " " << (m_position.y + (m_texture->GetHeight() * 0.9)) / MARIO_TILE_HEIGHT << std::endl;
	for (int i = 0; i < 2; i++)
	{
		int h = (int)((m_position.y + (m_texture->GetHeight() * 0.9f)) / MARIO_TILE_HEIGHT) + (i - 1);
		int w = (int)((m_position.x + (m_texture->GetWidth() * 0.5f)) / MARIO_TILE_WIDTH) + 1;
		if (h < 0 || h > MARIO_MAP_HEIGHT)
			continue;
		if (m_position.x + m_texture->GetWidth() > SCREEN_WIDTH)
		{
			canMove = m_canMoveOfscreen;
			break;
		}
		else if (m_position.x < 0)
			continue;
		int tileType = m_screen->GetLevelMap()->GetTileAt(h, w);
		if (tileType == 0) continue;
		//std::cout << "Tile At " << w << " " << h << std::endl;
		tileBox.y = (h)*MARIO_TILE_HEIGHT;
		tileBox.x = (w)*MARIO_TILE_WIDTH;
		canMove = !Collisions::Instance()->Box(tileBox, coll);
		if (!canMove)
			break;
	}

	if (canMove)
	{
		m_position.x += deltaTime * MOVEMENTSPEED * m_speed * m_velocity_right;
		if (m_velocity_right < m_max_momentum)
			m_velocity_right += 0.1;
	}
}

void MarioCharacter::AddGravity(float deltaTime)
{
	if (m_position.y < SCREEN_HEIGHT - m_texture->GetHeight())
	{
		if (m_velocity_vertical < 0)
			m_velocity_vertical += 9.81 * 2 * deltaTime;
		else
			m_velocity_vertical += 9.81 * deltaTime;
	}
}

void MarioCharacter::Jump()
{
	if (m_can_jump)
	{
		float calc = m_velocity_right - m_velocity_left;

		if (calc < 0)
			calc = -calc;
		calc /= m_max_momentum;
		if (calc > 1)
			calc = 1;
		if (calc < 0.85)
			calc = 0.85;

		m_velocity_vertical = -m_jump_force * calc;
		m_can_jump = false;
	}
}

double MarioCharacter::GetCollisionRadius()
{
	return m_collision_radius;
}
