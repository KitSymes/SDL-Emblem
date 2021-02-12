#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include "constants.h"
#include <iostream>
#include "Texture2D.h"
#include "Commons.h"
#include "GameScreenManager.h"
#include<stdio.h>
#include "LTimer.h"
using namespace std;

// Function Prototypes
bool InitSDL();
void CloseSDL();
bool Update();
void Render();

// Global Pointers
SDL_Window* g_window = nullptr;
SDL_Renderer* g_renderer = nullptr;
GameScreenManager* game_screen_manager;
Uint32 g_old_time;

// Global Variables
bool quit = false;

// Global Constants
const int SCREEN_FPS = 20;
const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;

int main(int argc, char* args[])
{

	// Try to create Window
	if (InitSDL())
	{
		// Window Created Successfully
		game_screen_manager = new GameScreenManager(g_renderer, SCREEN_MAP);
		g_old_time = SDL_GetTicks();

		LTimer fpsTimer;
		LTimer capTimer;
		int frames = 0;
		fpsTimer.start();

		while (!quit)
		{
			capTimer.start();

			Render();
			quit = Update();

			float averageFPS = frames / (fpsTimer.getTicks() / 100.0f);
			if (averageFPS > 2000000)
				averageFPS = 0;

			++frames;

			//If frame finished early
			int frameTicks = capTimer.getTicks();
			if (frameTicks < SCREEN_TICKS_PER_FRAME)
			{
				//Wait remaining time
				SDL_Delay(SCREEN_TICKS_PER_FRAME - frameTicks);
			}
		}
	}

	CloseSDL();

	return 0;
}

bool InitSDL()
{
	// Setup SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		cout << "SDL did not initialise. Error: " << SDL_GetError();
		return false;
	}
	else {
		// Create Window
		g_window = SDL_CreateWindow("SDL Emblem",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			SCREEN_WIDTH,
			SCREEN_HEIGHT,
			SDL_WINDOW_SHOWN);
		// If unsuccessful
		if (g_window == nullptr)
		{
			cout << "Window was not created. Error: " << SDL_GetError();
			return false;
		}
		g_renderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED);
		if (g_renderer != nullptr)
		{
			int imageFlags = IMG_INIT_PNG;
			if (!(IMG_Init(imageFlags) & imageFlags))
			{
				cout << "SDL_Image could not initialise. Error: " << IMG_GetError();
				return false;
			}
		}
		else {
			cout << "Renderer could not initialise. Error: " << SDL_GetError();
			return false;
		}
	}
}

void CloseSDL()
{
	// Free the Game Screen's Memory
	delete game_screen_manager;
	game_screen_manager = nullptr;
	// Release the Renderer
	SDL_DestroyRenderer(g_renderer);
	g_renderer = nullptr;
	// Destroy Window
	SDL_DestroyWindow(g_window);
	g_window = nullptr;
	// Quit SDL Subsystems
	IMG_Quit();
	SDL_Quit();
}

bool Update()
{
	Uint32 new_time = SDL_GetTicks();

	SDL_Event e;
	SDL_PollEvent(&e);

	switch (e.type)
	{
	case SDL_QUIT:
		return true;
		break;
	case SDL_KEYUP:
		switch (e.key.keysym.sym)
		{
		case SDLK_q:
			return true;
			break;
		}
		break;
	case SDL_MOUSEBUTTONDOWN:
		break;
	}

	game_screen_manager->Update((float)(new_time - g_old_time) / 1000.0f, e);
	g_old_time = new_time;
	return false;
}

void Render()
{
	// Clear the screen
	SDL_SetRenderDrawColor(g_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(g_renderer);

	game_screen_manager->Render();

	SDL_RenderPresent(g_renderer);
}