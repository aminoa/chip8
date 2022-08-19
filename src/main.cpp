#include "SDL.h"
#include "chip8.h"
#undef main
#include <iostream>
#include <chrono>
#include <thread>

const uint8_t keymap[16] = {
	SDLK_x,
	SDLK_1,
	SDLK_2,
	SDLK_3,
	SDLK_q,
	SDLK_w,
	SDLK_e,
	SDLK_a,
	SDLK_s,
	SDLK_d,
	SDLK_z,
	SDLK_c,
	SDLK_4,
	SDLK_r,
	SDLK_f,
	SDLK_v,
};


int main(int argc, char** argv) {

	if (argc != 2)
	{
		std::cout << "Usage: Chip8 <ROM> [args] " << std::endl;
		return 1;
	}

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		std::cout << "Error initializing SDL: %s\n" << SDL_GetError();
		return 0;
	}

	SDL_Window* window = SDL_CreateWindow(argv[1], SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 512, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_RenderSetLogicalSize(renderer, 1024, 512);
	if (renderer)
	{
		SDL_SetRenderDrawColor(renderer, 121, 121, 121, 255);
		std::cout << "Renderer created!\n";
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	}
	Chip8 chip8 = Chip8();
	
	if (!chip8.LoadROM(argv[1]))
	{
		return 0;
	}

	SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);

	uint32_t pixels[2048];

	while (true)
	{
		chip8.EmulateChip8Op();
		/*SDL_Event e;

		while (SDL_PollEvent(&e))
		{

			if (e.type == SDL_KEYDOWN)
			{
				if (e.key.keysym.sym == SDLK_ESCAPE)
				{
					exit(0);
				}

				for (int i = 0; i < 16; ++i)
				{
					if (e.key.keysym.sym == keymap[i])
					{
						chip8.key[i] = 1;
					}
				}
			}

			if (e.type == SDL_KEYUP)
			{
				for (int i = 0; i < 16; ++i)
				{
					if (e.key.keysym.sym == keymap[i])
					{
						chip8.key[i] = 0;
					}
				}
			}
		}*/

		if (chip8.draw_flag)
		{
			chip8.draw_flag = false;
			
			for (int i = 0; i < 2048; ++i)
			{
				uint8_t pixel = chip8.screen[i];
				pixels[i] = (0x00FFFFFF * pixel) | 0xFF000000;
			}

			SDL_UpdateTexture(texture, NULL, pixels, 64 * sizeof(Uint32));
			SDL_RenderClear(renderer);
			SDL_RenderCopy(renderer, texture, NULL, NULL);
			SDL_RenderPresent(renderer);
		}

		std::this_thread::sleep_for(std::chrono::microseconds(1200));
	}

	return 0;
}

