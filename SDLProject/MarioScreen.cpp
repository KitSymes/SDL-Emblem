#include "MarioScreen.h"
#include <iostream>
#include "Texture2D.h"
#include "MarioPlayer.h"
#include "MarioMarioPlayer.h"
#include "LuigiMarioPlayer.h"
#include "Collisions.h"
#include "PowBlock.h"

MarioScreen::MarioScreen(SDL_Renderer* renderer) : GameScreen(renderer)
{
	m_level_map = nullptr;
	SetUpLevel();
}

MarioScreen::~MarioScreen()
{
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

	m_mario = new MarioMarioPlayer(m_renderer, "Images/Mario/Mario.png", Vector2D(9 * 32, 10 * 32), this);
	m_luigi = new LuigiMarioPlayer(m_renderer, "Images/Mario/Luigi.png", Vector2D(8 * 32, 10 * 32), this);

	m_pow_block = new PowBlock(m_renderer, m_level_map);
	m_screenshake = false;
	m_background_yPos = 0.0f;

	return true;
}

void MarioScreen::Render()
{
	m_background_texture->Render(Vector2D(0, m_background_yPos), SDL_FLIP_NONE);
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

// Collisions checked in MarioPlayer::Update rather than using the boxes
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
}
