#ifndef CHIP8_H
#define CHIP8_H

#include <stack>
class Chip8
{
public:
	uint8_t V[16];
	uint16_t I;
	uint8_t SP;
	uint16_t PC;
		
	uint8_t delay;
	uint8_t sound;

	uint8_t key[16];
	uint8_t memory[1024 * 4];
	uint8_t screen[64 * 32];

	std::stack<uint16_t> stack;
	bool draw_flag;

	Chip8();

	bool LoadROM(const char* path);

	void EmulateChip8Op();
};

#endif CHIP8_H
