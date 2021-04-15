#include "MapScreen.h"
#include "Texture2D.h"
#include "Character.h"
#include "BattleScreen.h"
#include "SaveData.h"
#include "Collisions.h"
#include "Utils.h"
#include "GameScreenManager.h"

MapScreen::MapScreen(SDL_Renderer* renderer, GameScreenManager* gsm) : GameScreen(renderer, gsm)
{
	SetUpLevel();
}

MapScreen::~MapScreen()
{
	if (currentPos == start)
		SaveData::Instance()->lastMap = 0;
	else if (currentPos == firstLevel)
		SaveData::Instance()->lastMap = 1;
	else if (currentPos == forestLevel)
		SaveData::Instance()->lastMap = 2;
	else if (currentPos == alternateLevel)
		SaveData::Instance()->lastMap = 3;
	else if (currentPos == beforeBridge)
		SaveData::Instance()->lastMap = 4;
	else if (currentPos == bridgeLevel)
		SaveData::Instance()->lastMap = 5;
	else if (currentPos == lastSplit)
		SaveData::Instance()->lastMap = 6;
	else if (currentPos == castleLevel)
		SaveData::Instance()->lastMap = 7;
	else if (currentPos == caveLevel)
		SaveData::Instance()->lastMap = 8;

	currentPos = nullptr;

	delete m_background_texture;
	m_background_texture = nullptr;
	delete myChar;
	myChar = nullptr;
	delete m_levelLocked_texture;
	m_levelLocked_texture = nullptr;
	delete m_levelUnlocked_texture;
	m_levelUnlocked_texture = nullptr;
	delete m_levelComplete_texture;
	m_levelComplete_texture = nullptr;
	delete m_ui_textures;
	m_ui_textures = nullptr;

	delete start;
	delete firstLevel;
	delete forestLevel;
	delete alternateLevel;
	delete beforeBridge;
	delete bridgeLevel;
	delete lastSplit;
	delete castleLevel;
	delete caveLevel;

	delete m_empty;
	m_empty = nullptr;

	TTF_CloseFont(m_font);

	Mix_FreeChunk(m_click_sound);
	m_click_sound = nullptr;

	SaveData::Instance()->AutoSave();
}

bool MapScreen::SetUpLevel()
{
	// PoIs with no battles

	start = new PoI(11, 9);
	start->locked = false;
	start->complete = true;
	start->north.push_back(0);
	start->north.push_back(0);

	beforeBridge = new PoI(7, 4);
	beforeBridge->locked = false;
	beforeBridge->complete = true;
	beforeBridge->north.push_back(0);
	beforeBridge->north.push_back(1);
	beforeBridge->north.push_back(1);
	beforeBridge->north.push_back(1);
	beforeBridge->north.push_back(1);
	beforeBridge->north.push_back(1);
	beforeBridge->west.push_back(3);
	beforeBridge->south.push_back(2);
	beforeBridge->south.push_back(2);
	beforeBridge->south.push_back(2);

	lastSplit = new PoI(2, 4);
	lastSplit->locked = false;
	lastSplit->complete = true;
	lastSplit->north.push_back(0);
	lastSplit->north.push_back(0);
	lastSplit->south.push_back(2);
	lastSplit->south.push_back(2);
	lastSplit->east.push_back(1);
	lastSplit->east.push_back(1);
	lastSplit->east.push_back(1);
	lastSplit->east.push_back(1);

	// PoIs with battles

	firstLevel = new PoI(11, 7);
	firstLevel->mapFile = "map1.txt";
	firstLevel->locked = false;
	//firstLevel->returnDir = 2;
	firstLevel->south.push_back(2);
	firstLevel->south.push_back(2);
	firstLevel->east.push_back(1);
	firstLevel->east.push_back(0);
	firstLevel->east.push_back(0);
	firstLevel->east.push_back(0);
	firstLevel->east.push_back(0);
	firstLevel->west.push_back(3);
	firstLevel->west.push_back(3);
	firstLevel->west.push_back(3);
	firstLevel->west.push_back(3);

	forestLevel = new PoI(7, 7);
	//forestLevel->returnDir = 1;
	forestLevel->mapFile = "map2-A.txt";
	forestLevel->east.push_back(1);
	forestLevel->east.push_back(1);
	forestLevel->east.push_back(1);
	forestLevel->east.push_back(1);
	forestLevel->north.push_back(0);
	forestLevel->north.push_back(0);
	forestLevel->north.push_back(0);

	alternateLevel = new PoI(12, 3);
	//alternateLevel->returnDir = 2;
	alternateLevel->mapFile = "map2-B.txt";
	alternateLevel->west.push_back(3);
	alternateLevel->west.push_back(3);
	alternateLevel->west.push_back(3);
	alternateLevel->west.push_back(3);
	alternateLevel->west.push_back(3);
	alternateLevel->west.push_back(2);
	alternateLevel->south.push_back(2);
	alternateLevel->south.push_back(2);
	alternateLevel->south.push_back(2);
	alternateLevel->south.push_back(2);
	alternateLevel->south.push_back(3);

	bridgeLevel = new PoI(6, 4);
	//bridgeLevel->returnDir = 1;
	bridgeLevel->mapFile = "map3.txt";
	bridgeLevel->west.push_back(3);
	bridgeLevel->west.push_back(3);
	bridgeLevel->west.push_back(3);
	bridgeLevel->west.push_back(3);
	bridgeLevel->east.push_back(1);

	castleLevel = new PoI(2, 2);
	//castleLevel->returnDir = 2;
	castleLevel->mapFile = "map4.txt";
	castleLevel->south.push_back(2);
	castleLevel->south.push_back(2);

	caveLevel = new PoI(2, 6);
	caveLevel->north.push_back(0);
	caveLevel->north.push_back(0);

	start->northPoI = firstLevel;
	firstLevel->southPoI = start;
	firstLevel->eastPoI = alternateLevel;
	firstLevel->westPoI = forestLevel;
	forestLevel->eastPoI = firstLevel;
	forestLevel->northPoI = beforeBridge;
	alternateLevel->southPoI = firstLevel;
	alternateLevel->westPoI = beforeBridge;
	beforeBridge->southPoI = forestLevel;
	beforeBridge->northPoI = alternateLevel;
	beforeBridge->westPoI = bridgeLevel;
	bridgeLevel->eastPoI = beforeBridge;
	bridgeLevel->westPoI = lastSplit;
	lastSplit->eastPoI = bridgeLevel;
	lastSplit->northPoI = castleLevel;
	lastSplit->southPoI = caveLevel;
	castleLevel->southPoI = lastSplit;
	caveLevel->northPoI = lastSplit;

	// Image Loading

	m_background_texture = new Texture2D(m_renderer, 512, 416);
	if (!m_background_texture->LoadFromFile("Images/WorldMap/WorldMap.png"))
	{
		std::cout << "Failed to load background texture!" << std::endl;
	}

	m_levelLocked_texture = new Texture2D(m_renderer, 32, 64);
	if (!m_levelLocked_texture->LoadFromFile("Images/WorldMap/Locked.png"))
	{
		std::cout << "Failed to load level locked texture!" << std::endl;
	}

	m_levelUnlocked_texture = new Texture2D(m_renderer, 32, 64);
	if (!m_levelUnlocked_texture->LoadFromFile("Images/WorldMap/Unlocked.png"))
	{
		std::cout << "Failed to load level unlocked texture!" << std::endl;
	}

	m_levelComplete_texture = new Texture2D(m_renderer, 32, 64);
	if (!m_levelComplete_texture->LoadFromFile("Images/WorldMap/Complete.png"))
	{
		std::cout << "Failed to load level complete texture!" << std::endl;
	}

	m_ui_textures = new Texture2D(m_renderer);
	if (!m_ui_textures->LoadFromFile("Images/WorldMap/UI.png"))
	{
		std::cout << "Failed to load Images/WorldMap/UI.png!" << std::endl;
	}

	m_click_sound = Mix_LoadWAV("Sounds/Click.wav");
	if (!m_click_sound)
	{
		std::cout << "Failed to load Sounds/Click.wav sound!" << std::endl;
		std::cout << Mix_GetError() << std::endl;
	}

	m_font = TTF_OpenFont("Fonts/calibri.ttf", 15);
	SDL_Color colour = { 0, 0, 0 };
	m_empty = new Text(m_renderer, m_font, "Empty", colour);

	// Save Data Loading

	switch (SaveData::Instance()->lastMap)
	{
	case 0:
		currentPos = start;
		break;
	case 1:
		currentPos = firstLevel;
		break;
	case 2:
		currentPos = forestLevel;
		break;
	case 3:
		currentPos = alternateLevel;
		break;
	case 4:
		currentPos = beforeBridge;
		break;
	case 5:
		currentPos = bridgeLevel;
		break;
	case 6:
		currentPos = lastSplit;
		break;
	case 7:
		currentPos = castleLevel;
		break;
	case 8:
		currentPos = caveLevel;
		break;
	}
	myChar = new Character(m_renderer, "Images/Allies/Infantry/Lancer.png", Vector2D(currentPos->x * 32 - 10, currentPos->y * 32 - 10));

	if (SaveData::Instance()->firstLevelClear)
	{
		firstLevel->complete = true;
		forestLevel->locked = false;
		alternateLevel->locked = false;
	}

	if (SaveData::Instance()->forestLevelClear)
	{
		forestLevel->complete = true;
		bridgeLevel->locked = false;
	}

	if (SaveData::Instance()->alternateLevelClear)
	{
		alternateLevel->complete = true;
		bridgeLevel->locked = false;
	}

	if (SaveData::Instance()->bridgeLevelClear)
	{
		bridgeLevel->complete = true;
		castleLevel->locked = false;
		caveLevel->locked = false;
	}

	if (SaveData::Instance()->castleLevelClear)
	{
		castleLevel->complete = true;
	}

	if (SaveData::Instance()->caveLevelClear)
	{
		caveLevel->complete = true;
	}

	// Set party to be dead so that unused members don't get checked in BattleScreen logic (it also sets participating members to alive in the constructor)
	for (Character* c : SaveData::Instance()->m_allies)
		c->SetAlive(false);


	return true;
}

void MapScreen::Render()
{
	m_background_texture->Render(Vector2D(), SDL_FLIP_NONE);

	PoI* poiPointer = firstLevel;
	while (true)
	{
		if (poiPointer->complete)
			m_levelComplete_texture->Render(Vector2D(poiPointer->x * 32.0f, poiPointer->y * 32.0f), SDL_FLIP_NONE);
		else if (poiPointer->locked)
			m_levelLocked_texture->Render(Vector2D(poiPointer->x * 32.0f, poiPointer->y * 32.0f), SDL_FLIP_NONE);
		else
			m_levelUnlocked_texture->Render(Vector2D(poiPointer->x * 32.0f, poiPointer->y * 32.0f), SDL_FLIP_NONE);

		if (poiPointer == firstLevel)
			poiPointer = forestLevel;
		else if (poiPointer == forestLevel)
			poiPointer = alternateLevel;
		else if (poiPointer == alternateLevel)
			poiPointer = bridgeLevel;
		else if (poiPointer == bridgeLevel)
			/*poiPointer = castleLevel;
		else if (poiPointer == castleLevel)*/
			break;
	}
	if (moving)
	{
		if (m_timer >= 20)
		{
			m_index++;
			m_timer = 0;
			if (m_index >= m_current_path->size())
				moving = false;
		}
		else
		{
			int step;
			switch (m_timer % 5)
			{
			case 0:
			case 3:
				step = 1;
				break;
			case 1:
			case 2:
			case 4:
				step = 2;
				break;
			}
			switch (m_current_path->at(m_index))
			{
			case 0:
				myChar->SetRawPosition(Vector2D(myChar->GetRawPosition().x, myChar->GetRawPosition().y - step));
				break;
			case 1:
				myChar->SetRawPosition(Vector2D(myChar->GetRawPosition().x + step, myChar->GetRawPosition().y));
				break;
			case 2:
				myChar->SetRawPosition(Vector2D(myChar->GetRawPosition().x, myChar->GetRawPosition().y + step));
				break;
			case 3:
				myChar->SetRawPosition(Vector2D(myChar->GetRawPosition().x - step, myChar->GetRawPosition().y));
				break;
			default:
				std::cout << "ERROR IN PATH, INVALID VALUE GIVEN: " << m_current_path->at(m_index) << std::endl;
				break;
			}
			m_timer++;
		}
	}
	myChar->Render();

	if (m_menu == MENU_TITLE)
	{
		SDL_Rect source = { 0, 0, 90, 150 };
		SDL_Rect dest = { 166, 0, 90 * 2, 150 * 2 };
		m_ui_textures->Render(&source, &dest, SDL_FLIP_NONE);
	}
	else if (m_menu == MENU_FILES)
	{
		SDL_Rect source = { 90, 0, 90, 182 };
		SDL_Rect dest = { 166, 0, 90 * 2, 182 * 2 };
		m_ui_textures->Render(&source, &dest, SDL_FLIP_NONE);

		if (!s1)
			m_empty->Render(Vector2D(277, 106));
		if (!s2)
			m_empty->Render(Vector2D(277, 170));
		if (!s3)
			m_empty->Render(Vector2D(277, 234));
		if (!sa)
			m_empty->Render(Vector2D(277, 298));
	}
	else if (m_menu == MENU_EXTRA)
	{
		int i = 0;
		for (Character* c : SaveData::Instance()->m_allies)
		{
			Vector2D vec = c->GetRawPosition();

			SDL_Rect source = { 180, 0, 60, 75 };
			SDL_Rect dest = { vec.x + 10, vec.y - 38, 60 * 2, 75 * 2 };
			m_ui_textures->Render(&source, &dest, SDL_FLIP_NONE);

			c->Render();
			vec.x += 45;
			vec.y -= 33;
			c->RenderText(vec);
			i++;
		}
	}
}

void MapScreen::Update(float deltaTime, SDL_Event e)
{
	if (!moving && m_menu == MENU_NONE)
		switch (e.type)
		{
		case SDL_KEYDOWN:
			switch (e.key.keysym.sym)
			{
			case SDLK_w:
				if (!currentPos->north.empty() && (currentPos->complete || SaveData::Instance()->returnDir == 0))
				{
					SaveData::Instance()->returnDir = InverseDir(currentPos->north[currentPos->north.size() - 1]);
					m_current_path = &currentPos->north;
					currentPos = currentPos->northPoI;
					m_index = 0;
					m_timer = 0;
					moving = true;
				}
				break;
			case SDLK_s:
				if (!currentPos->south.empty() && (currentPos->complete || SaveData::Instance()->returnDir == 2))
				{
					SaveData::Instance()->returnDir = InverseDir(currentPos->south[currentPos->south.size() - 1]);
					m_current_path = &currentPos->south;
					currentPos = currentPos->southPoI;
					m_index = 0;
					m_timer = 0;
					moving = true;
				}
				break;
			case SDLK_d:
				if (!currentPos->east.empty() && (currentPos->complete || SaveData::Instance()->returnDir == 1))
				{
					SaveData::Instance()->returnDir = InverseDir(currentPos->east[currentPos->east.size() - 1]);
					m_current_path = &currentPos->east;
					currentPos = currentPos->eastPoI;
					m_index = 0;
					m_timer = 0;
					moving = true;
				}
				break;
			case SDLK_a:
				if (!currentPos->west.empty() && (currentPos->complete || SaveData::Instance()->returnDir == 3))
				{
					SaveData::Instance()->returnDir = InverseDir(currentPos->west[currentPos->west.size() - 1]);
					m_current_path = &currentPos->west;
					currentPos = currentPos->westPoI;
					m_index = 0;
					m_timer = 0;
					moving = true;
				}
				break;
			case SDLK_RETURN:
			case SDLK_KP_ENTER:
				if (!currentPos->mapFile.empty())
				{
					BattleScreen* bs = new BattleScreen(m_renderer, m_gsm, (char*)("BattleMaps/" + currentPos->mapFile).c_str());
					m_gsm->ChangeScreen(bs);
				}
				break;
			}
			break;
		}

	switch (e.type)
	{
	case SDL_KEYDOWN:
		switch (e.key.keysym.sym)
		{
		case SDLK_ESCAPE:
			if (m_menu == MENU_NONE)
				m_menu = MENU_TITLE;
			else
				m_menu = MENU_NONE;
			break;
		}
		break;
	case SDL_MOUSEBUTTONDOWN:
		if (e.button.button == SDL_BUTTON_LEFT)
		{
			if (m_menu == MENU_TITLE) {
				if (Collisions::Instance()->Inside(e.button.x, e.button.y, Rect2D(166, 82, m_buttonWidth, m_buttonHeight))) // Party Button
				{
					int i = 0;
					for (Character* c : SaveData::Instance()->m_allies)
					{
						c->SetMoved(false);
						c->m_health = c->m_max_health;
						c->SetRawPosition(Vector2D(-9 + i * 130, 100));
						c->UpdateText(m_font);
						i++;
					}
					Mix_PlayChannel(-1, m_click_sound, 0);
					m_menu = MENU_EXTRA;
				}
				else if (Collisions::Instance()->Inside(e.button.x, e.button.y, Rect2D(166, 146, m_buttonWidth, m_buttonHeight))) // Save Button
				{
					s1 = Utils::exists("Saves/Save1.txt");
					s2 = Utils::exists("Saves/Save2.txt");
					s3 = Utils::exists("Saves/Save3.txt");
					sa = Utils::exists("Saves/AutoSave.txt");
					Mix_PlayChannel(-1, m_click_sound, 0);
					m_menu = MENU_FILES;
				}
				else if (Collisions::Instance()->Inside(e.button.x, e.button.y, Rect2D(166, 210, m_buttonWidth, m_buttonHeight))) // Main Menu Button
				{
					SaveData::Instance()->AutoSave();
					Mix_PlayChannel(-1, m_click_sound, 0);
					m_gsm->ChangeScreen(SCREEN_TITLE);
				}
				else if (Collisions::Instance()->Inside(e.button.x, e.button.y, Rect2D(176, 274, 30, 26))) // Back
				{
					Mix_PlayChannel(-1, m_click_sound, 0);
					m_menu = MENU_NONE;
				}
			}
			else if (m_menu == MENU_FILES) {
				if (Collisions::Instance()->Inside(e.button.x, e.button.y, Rect2D(166, 82, m_buttonWidth, m_buttonHeight))) // Slot 1
				{
					SaveData::Instance()->Save("Save1");
					s1 = true;
					Mix_PlayChannel(-1, m_click_sound, 0);
				}
				else if (Collisions::Instance()->Inside(e.button.x, e.button.y, Rect2D(166, 146, m_buttonWidth, m_buttonHeight))) // Slot 2
				{
					SaveData::Instance()->Save("Save2");
					s2 = true;
					Mix_PlayChannel(-1, m_click_sound, 0);
				}
				else if (Collisions::Instance()->Inside(e.button.x, e.button.y, Rect2D(166, 210, m_buttonWidth, m_buttonHeight))) // Slot 3
				{
					SaveData::Instance()->Save("Save3");
					s3 = true;
					Mix_PlayChannel(-1, m_click_sound, 0);
				}
				else if (Collisions::Instance()->Inside(e.button.x, e.button.y, Rect2D(166, 274, m_buttonWidth, m_buttonHeight))) // Auto Save
				{
					SaveData::Instance()->AutoSave();
					sa = true;
					Mix_PlayChannel(-1, m_click_sound, 0);
				}
				else if (Collisions::Instance()->Inside(e.button.x, e.button.y, Rect2D(176, 338, 30, 26))) // Back
				{
					Mix_PlayChannel(-1, m_click_sound, 0);
					m_menu = MENU_TITLE;
				}
			}
			else if (m_menu == MENU_EXTRA)
			{
				for (int i = 0; i < SaveData::Instance()->m_allies.size(); i++)
				{
					if (i > 0 &&
						Collisions::Instance()->Inside(e.button.x, e.button.y, Rect2D(i * 130 + 31, 192, 24, 18))) // Move Party Member Left
					{
						std::vector<Character*> tmp(SaveData::Instance()->m_allies.begin() + i, SaveData::Instance()->m_allies.begin() + i + 1);
						SaveData::Instance()->m_allies.erase(SaveData::Instance()->m_allies.begin() + i, SaveData::Instance()->m_allies.begin() + i + 1);
						SaveData::Instance()->m_allies.insert(SaveData::Instance()->m_allies.begin() + i - 1, tmp.begin(), tmp.end());

						int i = 0;
						for (Character* c : SaveData::Instance()->m_allies)
						{
							c->SetMoved(false);
							c->SetRawPosition(Vector2D(-9 + i * 130, 100));
							c->UpdateText(m_font);
							i++;
						}
						Mix_PlayChannel(-1, m_click_sound, 0);
					}
					else if (i < SaveData::Instance()->m_allies.size() - 1 &&
						Collisions::Instance()->Inside(e.button.x, e.button.y, Rect2D(i * 130 + 67, 192, 24, 18))) // Move Party Member Right
					{
						std::vector<Character*> tmp(SaveData::Instance()->m_allies.begin() + i, SaveData::Instance()->m_allies.begin() + i + 1);
						SaveData::Instance()->m_allies.erase(SaveData::Instance()->m_allies.begin() + i, SaveData::Instance()->m_allies.begin() + i + 1);
						SaveData::Instance()->m_allies.insert(SaveData::Instance()->m_allies.begin() + i + 1, tmp.begin(), tmp.end());

						int i = 0;
						for (Character* c : SaveData::Instance()->m_allies)
						{
							c->SetMoved(false);
							c->SetRawPosition(Vector2D(-9 + i * 130, 100));
							c->UpdateText(m_font);
							i++;
						}
						Mix_PlayChannel(-1, m_click_sound, 0);
					}
				}
			}
		}
		break;
	}
}

int MapScreen::InverseDir(int dir)
{
	switch (dir)
	{
	case 0:
		return 2;
	case 2:
		return 0;
	case 1:
		return 3;
	case 3:
		return 1;
	default:
		return dir;
	}
}
