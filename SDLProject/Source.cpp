#include<SDL.h>
#include<SDL_image.h>
#include<SDL_mixer.h>
#include"constants.h"
#include<iostream>
using namespace std;

bool InitSDL();
void CloseSDL();

SDL_Window* g_window = nullptr;

int main(int argc, char* args[])
{
	// Try to create Window
	if (InitSDL())
	{
		// Window Created

		SDL_Delay(5000);
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
	}
}
void CloseSDL()
{
	// Destroy Window
	SDL_DestroyWindow(g_window);
	g_window = nullptr;
	// Quit SDL Subsystems
	IMG_Quit();
	SDL_Quit();
}