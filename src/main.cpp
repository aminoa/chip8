#include "SDL.h"
#include "chip8.h"
#undef main
#include <iostream>

////don't do this, this is just an example
//SDL_Renderer* renderer;
//SDL_Window* window;
//bool isRunning;
//bool fullscreen;
//void handleEvents();
//void update();
//void render();

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

	SDL_Window* window = SDL_CreateWindow("[PUT_ROM_TITLE_HERE]", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 64, 32, 0);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
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
		SDL_Event e;

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
		}
	}

	if (chip8.draw_flag)
	{
		chip8.draw_flag = false;
	}

	////frees memory associated with renderer and window
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);	//error here
	SDL_Quit();


	return 0;
}

