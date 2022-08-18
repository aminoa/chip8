#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <random>

uint8_t CHIP8_FONTSET[80] =
{
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

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

	Chip8()
	{
		SP = 0;
		PC = 0x200;
		I = 0;
		delay = 0;
		sound = 0;
		draw_flag = false;

		for (int i = 0; i < 80; ++i)
		{
			memory[i] = CHIP8_FONTSET[i];
		}

		for (int i = 80; i < 1024 * 4; ++i)
		{
			memory[i] = 0;
		}

		for (int i = 0; i < 64 * 32; ++i)
		{
			screen[i] = 0;
		}
		
		for (int i = 0; i < 16; ++i)
		{
			V[i] = 0;
			key[i] = 0;
		}

	}

	void EmulateChip8Op()
	{
		uint8_t* op = &memory[PC];

		uint8_t firstnib = (op[0] >> 4);
		uint8_t secondnib = (op[0] & 0xF);
		uint8_t thirdnib = (op[1] >> 4);
		uint8_t fourthnib = (op[1] & 0xF);

		switch (firstnib)
		{
		case 0x00:
			switch (op[1])
			{
			case 0xE0: 
			{
				memset(&screen, 0, 64 * 32);
				PC += 2;
			}
			break;
			case 0xEE:
			{
				PC = stack.top();
				stack.pop();
				SP -= 1;
			}
			break;
			default: std::cout << "Unknown Opcode" << std::endl;
			}
        break;
		case 0x01:
		{
			uint16_t target = ((op[0] & 0xF) << 8) | op[1];
			PC = target;
		}
		break;
		case 0x02:
		{
			SP += 1;
			stack.push(PC);
			uint16_t target = ((op[0] & 0xF) << 8) | op[1];
			PC = target;
		}
		case 0x03:
		{
			if (V[secondnib] == op[1])
			{
				PC += 2;
			}
			PC += 2;
		} 
		break;
		case 0x04: 
		{
			if (V[secondnib] != op[1])
			{
				PC += 2;
			}
			PC += 2;
		}
		case 0x05: 
		{
			if (V[secondnib] == V[thirdnib])
			{
				PC += 2;
			}
			PC += 2;
		}
		case 0x06:
		{
			V[secondnib] = op[1];
			PC += 2;
		}
		break;
		case 0x07: 
		{
			V[secondnib] += op[1];
			PC += 2;
		}
		break;
		case 0x08: 
		{
			switch (fourthnib)
			{
			case 0x00: 
			{
				V[secondnib] = V[thirdnib];
				PC += 2;
			}
			break;
			case 0x01:
			{
				V[secondnib] = V[secondnib] | V[thirdnib];
				PC += 2; 
			}
			break;
			case 0x02:
			{
				V[secondnib] = V[secondnib] & V[thirdnib];
				PC += 2;
			}
			break;
			case 0x03:
			{
				V[secondnib] = (!V[secondnib] != !V[thirdnib]);
				PC += 2;
			}
			break;
			case 0x04:
			{
				if (V[secondnib] + V[thirdnib] > 0xFF)
				{
					V[0xF] = 1;
				}
				else
				{
					V[0xF] = 0;
				}
				V[secondnib] = (V[secondnib] + V[thirdnib] & 0xFF);
				PC += 2;
			}
			break;
			case 0x05:
			{
				if (V[secondnib] > V[thirdnib])
				{
					V[0xF] = 1;
				}
				else
				{
					V[0xF] = 0;
				}
				V[secondnib] = V[secondnib] - V[thirdnib];
				PC += 2;
			}
			break;
			case 0x06:
			{
				if ((V[secondnib] & 0x1) == 0x1)
				{
					V[0xF] = 1;
				}
				else
				{
					V[0xF] = 0;
				}
				V[secondnib] = V[secondnib] >> 1;
				PC += 2;
			}
			break;
			case 0x07:
			{
				if (V[thirdnib] > V[secondnib])
				{
					V[0xF] = 1;
				}
				else
				{
					V[0xF] = 0;
				}
				V[secondnib] = V[secondnib] - V[thirdnib];
				PC += 2;
			}
			break;
			case 0x0E:
			{
				if ((V[secondnib] & 0x80) == 0x1)
				{
					V[0xF] = 1;
				}
				else
				{
					V[0xF] = 0;
				}
				V[secondnib] = V[secondnib] << 1;
				PC += 2;
			}
			break;
			}
		}
		case 0x09: 
		{
			if (V[secondnib] == V[thirdnib])
			{
				PC += 2;
			}
			PC += 2;
		}
		break;
		case 0x0A:
		{
			I = ((op[0] & 0xF) << 8) | op[1];
			PC += 2;
		}
		break;
		case 0x0B: 
		{
			PC = (((op[0] & 0xF) << 8) | op[1]) + V[0];
		}
		break;
		case 0x0C:
		{
			uint8_t random = rand() % 256;
			V[secondnib] = random & op[1];
			PC += 2;
		}
		break;
		case 0x0D: 
		{
			uint16_t x = V[secondnib] % 64;
			uint16_t y = V[thirdnib] % 32;
			V[0xF] = 0;
			uint16_t height = V[fourthnib];
			//uint16_t pixel;

			////Not sure what's going on here	
			//for (int i = 0; i < height; ++i) //number of rows
			//{
			//	pixel = memory[I + i];
			//	for (int j = 0; j < 8; ++j) //j has a fixed width
			//	{
			//		if ((pixel & (0x80 >> j)) != 0)
			//		{
			//			if (screen[(x + j + ((y + i) * 64))] == 1)
			//			{
			//				V[0xF] = 1;
			//			}
			//			screen[x + j + ((y + i) * 64)] ^= 1;
			//		}
			//	}
			//}

			//draw_flag = true;
			//PC += 2;
		}
		break;
		case 0x0E: 
		{
			switch (op[1])
			{
			case 0x9E: 
			{
				if (key[secondnib] != 0)
				{
					PC += 2;
				}
			}
			break;
			case 0xA1:
			{
				if (key[secondnib] == 0)
				{
					PC += 2;
				}
			}
			break;
			}
		}
		break;
		case 0x0F: 
		{
			switch (op[1])
			{
			case 0x07:
			{
				V[secondnib] = delay;
				PC += 2;
			}
			break;
			case 0x0A:
			{
				bool key_press = false;
				if (!key_press)
				{
					for (int i = 0; i < 16; ++i)
					{
						if (key[i] != 0)
						{
							key_press = true;
							V[secondnib] = i;
						}
					}
				}

				if (key_press)
				{
					PC += 2;
				}
			}
			break;
			case 0x15:
			{
				delay = V[secondnib];
				PC += 2;
			}
			break;
			case 0x18:
			{
				sound = V[secondnib];
				PC += 2;
			}
			break;
			case 0x1E:
			{
				I += V[secondnib];
				PC += 2;
			}
			break;
			case 0x29:
			{
				I = V[secondnib];
				PC += 2;
			}
			break;
			case 0x33:
			{
				uint8_t ones, tens, hundreds;
				uint8_t value = V[secondnib];

				ones = value % 10;
				value = value / 10;
				tens = value % 10;
				hundreds = value / 10;

				memory[I] = hundreds;
				memory[I + 1] = tens;
				memory[I + 2] = ones;
				PC += 2;
			}
			case 0x55:
			{
				for (uint16_t i = I; i < 16; ++i)
				{
					memory[i] = V[i];
				}
				PC += 2;
			}
			break;
			case 0x65:
			{
				for (uint16_t i = I; i < 16; ++i)
				{
					V[i] = memory[i];
				}
				PC += 2;
			}
			break;
			}
		}
		}
	}
};