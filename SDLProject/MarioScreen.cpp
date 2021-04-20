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
	delete m_tile_map;
	m_tile_map = nullptr;

	delete m_mario;
	m_mario = nullptr;
	delete m_luigi;
	m_luigi = nullptr;
	delete m_pow_block;
	m_pow_block = nullptr;

	Mix_FreeChunk(m_coin_sound);
	m_coin_sound = nullptr;
}

bool MarioScreen::SetUpLevel()
{
	m_background_texture = new Texture2D(m_renderer);
	if (!m_background_texture->LoadFromFile("Images/Mario/BackgroundMB.png"))
	{
		std::cout << "Failed to load background texture!" << std::endl;
	}

	m_tile_map = new Texture2D(m_renderer, 32, 32);
	if (!m_tile_map->LoadFromFile("Images/Mario/TileMap.png"))
	{
		std::cout << "Failed to load the TileMap texture!" << std::endl;
	}

	m_coin_sound = Mix_LoadWAV("Sounds/Pickup_Coin.wav");
	if (!m_coin_sound)
	{
		std::cout << "Failed to load Sounds/Pickup_Coin.wav sound!" << std::endl;
		std::cout << Mix_GetError() << std::endl;
	}

	SetLevelMap();

	m_mario = new MarioPlayerMario(m_renderer, "Images/Mario/Mario.png", Vector2D(9 * 32, 10 * 32), this);
	m_luigi = new MarioPlayerLuigi(m_renderer, "Images/Mario/Luigi.png", Vector2D(8 * 32, 10 * 32), this);

	m_pow_block = new PowBlock(m_renderer, m_level_map);
	m_screenshake = false;
	m_background_yPos = 0.0f;
	m_koopa_klock = 0;

	CreateCoin(Vector2D(229, 100));
	CreateCoin(Vector2D(30, 350));
	CreateCoin(Vector2D(434, 350));
	CreateCoin(Vector2D(10, 250));
	CreateCoin(Vector2D(454, 250));
	CreateCoin(Vector2D(10, 150));
	CreateCoin(Vector2D(454, 150));

	LoadMusic("Sounds/Music_Mario.mp3");
	Mix_VolumeMusic(15);

	return true;
}

void MarioScreen::Render()
{
	m_background_texture->Render(Vector2D(0, m_background_yPos), SDL_FLIP_NONE);

	for (int i = 0; i < m_koopas.size(); i++)
		m_koopas[i]->Render();

	for (int y = 0; y < MARIO_MAP_HEIGHT; y++)
		for (int x = 0; x < MARIO_MAP_WIDTH; x++)
		{
			SDL_Rect source = { 0, 0, 32, 32 };
			switch (m_level_map->GetTileAt(y, x))
			{
			case 1: // Blue Blocks
				source.x = 64;
				m_tile_map->Render(Vector2D(x * 32, y * 32), &source, SDL_FLIP_NONE);
				break;
			case 2: // Bricks
				source.x = 64;
				source.y = 32;
				m_tile_map->Render(Vector2D(x * 32, y * 32), &source, SDL_FLIP_NONE);
				break;
			case 3: // Top Pipe
				m_tile_map->Render(Vector2D(x * 32, y * 32), &source, SDL_FLIP_NONE);
				break;
			case 4: // Bottom Pipe
				source.y = 32;
				m_tile_map->Render(Vector2D(x * 32, y * 32), &source, SDL_FLIP_NONE);
				break;
			case 5: // Top Pipe Exit
				source.x = 32;
				m_tile_map->Render(Vector2D(x * 32, y * 32), &source, x > 8 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
				break;
			case 6: // Bottom Pipe Exit
				source.x = 32;
				source.y = 32;
				m_tile_map->Render(Vector2D(x * 32, y * 32), &source, x > 8 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
				break;
			default:
				break;
			}
		}

	for (int i = 0; i < m_coins.size(); i++)
		m_coins[i]->Render();

	m_pow_block->Render();
	m_mario->Render();
	m_luigi->Render();
}

void MarioScreen::Update(float deltaTime, SDL_Event e)
{
	int delIndex = -1;
	for (int i = 0; i < m_coins.size(); i++)
	{
		MarioCharacterCoin* coin = m_coins[i];
		if (Collisions::Instance()->Circle(m_mario->GetPosition(), m_mario->GetCollisionRadius(), coin->GetPosition(), coin->GetCollisionRadius()) ||
			Collisions::Instance()->Circle(m_luigi->GetPosition(), m_luigi->GetCollisionRadius(), coin->GetPosition(), coin->GetCollisionRadius()))
		{
			Mix_PlayChannel(-1, m_coin_sound, 0);
			delIndex = i;
			break;
		}
	}
	if (delIndex >= 0)
	{
		m_coins.erase(m_coins.begin() + delIndex);
	}

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
		{ 3, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 3 },
		{ 4, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 4 },
		{ 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0 },
		{ 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 7, 7, 0, 0, 0, 0, 0, 0, 0 },
		{ 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 } };

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
	if (m_koopa_klock <= 0 && m_koopas.size() < 6)
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
