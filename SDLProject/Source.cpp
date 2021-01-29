#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include "constants.h"
#include <iostream>
#include "Texture2D.h"
#include "Commons.h"
using namespace std;

// Function Prototypes
bool InitSDL();
void CloseSDL();
bool Update();
void Render();

// Global Pointers
SDL_Window* g_window = nullptr;
SDL_Renderer* g_renderer = nullptr;
Texture2D* g_texture = nullptr;

// Global Variables
bool quit = false;

int main(int argc, char* args[])
{
	// Try to create Window
	if (InitSDL())
	{
		// Window Created


		while (!quit)
		{
			Render();
			quit = Update();
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

		g_texture = new Texture2D(g_renderer);
		if (!g_texture->LoadFromFile("Images/test.bmp"))
			return false;
	}
}

void CloseSDL()
{
	// Free the Texture's Memory
	delete g_texture;
	g_texture = nullptr;
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
		return true;
		break;
	}
	return false;
}

void Render()
{
	// Clear the screen
	SDL_SetRenderDrawColor(g_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(g_renderer);

	g_texture->Render(Vector2D(), SDL_FLIP_NONE);

	SDL_RenderPresent(g_renderer);
}