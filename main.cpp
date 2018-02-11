#include "SDL.h"
#include "Timer.h"
#include "Chip8.h"
#include <iostream>

const int SCREEN_FPS = 60;
const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;

Chip8 chip8;
SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *texture;
Uint32 pixels[64 * 32];

void setupGraphics()
{
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, 0);
	window = SDL_CreateWindow(
		"Chip 8",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		1024,
		512,
		0
	);
	renderer = SDL_CreateRenderer(window, -1, 0);
	texture = SDL_CreateTexture(renderer,
		SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, 64, 32);
	memset(pixels, 0, 64 * 32 * sizeof(Uint32));
}

void drawGraphics()
{
	for (unsigned short i = 0; i < chip8.GFX_SIZE; i++)
	{
		if (chip8.gfx[i] == 1)
		{
			pixels[i] = 0xFFFFFFFF;
		}
		else
		{
			pixels[i] = 0x0;
		}
	}

	SDL_Rect dest;
	dest.x = 0;
	dest.y = 0;
	dest.w = 1024;
	dest.h = 512;

	SDL_UpdateTexture(texture, NULL, pixels, 64 * sizeof(Uint32));
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, &dest);
	SDL_RenderPresent(renderer);
}

int main(int argc, char **argv)
{
	setupGraphics();

	chip8.initialize();

	std::string game;
	std::cin >> game;

	chip8.loadGame(game);

	Timer timer500;
	Timer timer60;
	timer500.start();
	timer60.start();

	for (;;)
	{
		if (timer500.getTicks() >= 2)
		{
			chip8.setKeys();
			chip8.emulateCycle();
			if (chip8.drawFlag)
			{
				drawGraphics();
				chip8.drawFlag = false;
			}
			timer500.start();
		}

		if (timer60.getTicks() >= 16)
		{
			chip8.updateTimers();
			timer60.start();
		}
		SDL_Delay(1);
	}

	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
