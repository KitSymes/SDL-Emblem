#include "MarioScreen.h"
#include <iostream>
#include "Texture2D.h"
#include "MarioCharacter.h"
#include "MarioPlayerMario.h"
#include "MarioPlayerLuigi.h"
#include "Collisions.h"
#include "PowBlock.h"
#include "MarioCharacterKoopa.h"
#include "MarioCharacterCoin.h"

MarioScreen::MarioScreen(SDL_Renderer* renderer, GameScreenManager* gsm) : GameScreen(renderer, gsm)
{
	m_level_map = nullptr;
	SetUpLevel();
}

MarioScreen::~MarioScreen()
{
	m_koopas.clear();
	m_coins.clear();
	delete m_background_texture;
	m_background_texture = nullptr;
	delete m_mario;
	m_mario = nullptr;
	delete m_luigi;
	m_luigi = nullptr;
	delete m_pow_block;
	m_pow_block = nullptr;
}

bool MarioScreen::SetUpLevel()
{
	m_background_texture = new Texture2D(m_renderer);
	if (!m_background_texture->LoadFromFile("Images/Mario/BackgroundMB.png"))
	{
		std::cout << "Failed to load background texture!" << std::endl;
	}

	SetLevelMap();

	m_mario = new MarioPlayerMario(m_renderer, "Images/Mario/Mario.png", Vector2D(9 * 32, 10 * 32), this);
	m_luigi = new MarioPlayerLuigi(m_renderer, "Images/Mario/Luigi.png", Vector2D(8 * 32, 10 * 32), this);

	m_pow_block = new PowBlock(m_renderer, m_level_map);
	m_screenshake = false;
	m_background_yPos = 0.0f;
	m_koopa_klock = 0;

	CreateCoin(Vector2D(224, 100));

	return true;
}

void MarioScreen::Render()
{
	for (int i = 0; i < m_koopas.size(); i++)
		m_koopas[i]->Render();

	m_background_texture->Render(Vector2D(0, m_background_yPos), SDL_FLIP_NONE);

	for (int i = 0; i < m_coins.size(); i++)
		m_coins[i]->Render();

	m_pow_block->Render();
	m_mario->Render();
	m_luigi->Render();
}

void MarioScreen::Update(float deltaTime, SDL_Event e)
{
	/*if (Collisions::Instance()->Circle(m_mario->GetPosition(), m_mario->GetCollisionRadius(), m_luigi->GetPosition(), m_luigi->GetCollisionRadius()))
	{

	}*/

	if (m_screenshake)
	{
		m_shake_time -= deltaTime;
		m_wobble++;
		m_background_yPos = sin(m_wobble);
		m_background_yPos *= 3.0f;

		if (m_shake_time <= 0.0f)
		{
			m_shake_time = false;
			m_background_yPos = 0.0f;
		}
	}

	UpdateEnemies(deltaTime, e);

	m_mario->Update(deltaTime, e);
	m_luigi->Update(deltaTime, e);
}

void MarioScreen::SetLevelMap()
{
	int map[MARIO_MAP_HEIGHT][MARIO_MAP_WIDTH] = {
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0 },
		{ 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0 }, // Will be 1, 1 in middle for POW block
		{ 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } };

	if (m_level_map != nullptr)
		delete m_level_map;

	m_level_map = new MarioLevelMap(map);

	m_pow_block = new PowBlock(m_renderer, m_level_map);
}

// Collisions checked in MarioCharacter::Update rather than using the boxes here
void MarioScreen::UpdatePOWBlock()
{
	if (!m_pow_block->IsAvailable())
		return;
	ShakeScreen();
	m_pow_block->TakeHit();
}

void MarioScreen::ShakeScreen()
{
	m_screenshake = true;
	m_shake_time = SHAKE_DURATION;
	m_wobble = 0.0f;

	for (unsigned int i = 0; i < m_koopas.size(); i++)
		m_koopas[i]->TakeDamage();
}
void MarioScreen::UpdateEnemies(float deltaTime, SDL_Event e)
{
	if (m_koopa_klock == 0)
	{
		CreateKoopa(Vector2D(50, 32), 0.5f);
		CreateKoopa(Vector2D(425, 32), 0.5f);
		m_koopa_klock = 500;
	}
	else
		m_koopa_klock--;

	if (!m_koopas.empty())
	{
		int enemyIndexToDelete = -1;
		for (unsigned int i = 0; i < m_koopas.size(); i++)
		{
			m_koopas[i]->Update(deltaTime, e);

			if (m_koopas[i]->GetPosition().y > 300.0f) // If on bottom row of tiles
			{
				if (m_koopas[i]->GetPosition().x < (float)(-m_koopas[i]->GetCollisionBox().width * 0.5f)
					|| m_koopas[i]->GetPosition().x>SCREEN_WIDTH - (float)(m_koopas[i]->GetCollisionBox().width * 0.55f))
					m_koopas[i]->SetAlive(false);
			}

			if ((m_koopas[i]->GetPosition().y > 300.0f || m_koopas[i]->GetPosition().x > SCREEN_WIDTH - 96.0f))
			{
				// Ignore collisions if behind a pipe
			}
			else
			{
				if (Collisions::Instance()->Circle(m_koopas[i]->GetPosition(), m_koopas[i]->GetCollisionRadius(), m_mario->GetPosition(), m_mario->GetCollisionRadius()))
				{
					if (m_koopas[i]->IsInjured())
					{
						m_koopas[i]->SetAlive(false);
					}
					else
					{
						// Kill Mario
					}
				}
				if (Collisions::Instance()->Circle(m_koopas[i]->GetPosition(), m_koopas[i]->GetCollisionRadius(), m_luigi->GetPosition(), m_luigi->GetCollisionRadius()))
				{
					if (m_koopas[i]->IsInjured())
					{
						m_koopas[i]->SetAlive(false);
					}
					else
					{
						// Kill Luigi
					}
				}
			}

			if (!m_koopas[i]->IsAlive())
				enemyIndexToDelete = i;
		}
		if (enemyIndexToDelete != -1)
			m_koopas.erase(m_koopas.begin() + enemyIndexToDelete);
	}
}

void MarioScreen::CreateKoopa(Vector2D position, float speed)
{
	MarioCharacterKoopa* koopa = new MarioCharacterKoopa(m_renderer, "Images/Mario/Koopa.png", position, speed, this);
	m_koopas.push_back(koopa);
}

void MarioScreen::CreateCoin(Vector2D position)
{
	MarioCharacterCoin* coin = new MarioCharacterCoin(m_renderer, "Images/Mario/Coin.png", position, this);
	m_coins.push_back(coin);
}
