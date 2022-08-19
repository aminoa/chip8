#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <random>
#include <string>
#include "Chip8.h"

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

Chip8::Chip8()
{
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

	delay = 0;
	sound = 0;

	I = 0;
	SP = 0;
	PC = 0x200;
	draw_flag = false; 

}

bool Chip8::LoadROM(const char* path)
{
	FILE* file = fopen(path, "rb");
	if (!file)
	{
		std::cout << "Unable to open file. Exiting...";
		exit(1);
	}

	fseek(file, 0L, SEEK_END);
	int file_size = ftell(file);
	fseek(file, 0, SEEK_SET);

	unsigned char* buffer = new unsigned char[file_size];
	fread(buffer, file_size, 1, file);
	fclose(file);

	for (int i = 0; i < file_size; ++i)
	{
		memory[i + 0x200] = buffer[i];
	}

	delete[] buffer;

	return true;

}

void Chip8::EmulateChip8Op()
{
	uint8_t* op = &memory[PC];

	uint8_t firstnib = (op[0] >> 4);
	uint8_t secondnib = (op[0] & 0xF);
	uint8_t thirdnib = (op[1] >> 4);
	uint8_t fourthnib = (op[1] & 0xF);

	std::cout << std::hex << (int)PC << " " << std::hex << (int)op[0] << " " << std::hex << (int)op[1] << " ";
	switch (firstnib)
	{
	case 0x00:
		switch (op[1])
		{
		case 0xE0: 
		{
			memset(&screen, 0, 64 * 32);
			PC += 2;
			std::cout << "CLS" << std::endl;
		}
		break;
		case 0xEE:
		{
			PC = stack.top();
			stack.pop();
			SP -= 1;
			std::cout << "RET" << std::endl;
		}
		break;
		default: std::cout << "Unknown Opcode" << std::endl;
		}
	break;
	case 0x01:
	{
		uint16_t target = ((op[0] & 0xF) << 8) | op[1];
		PC = target;
		std::cout << "JP " << std::hex << (int)target << std::endl;
	}
	break;
	case 0x02:
	{
		SP += 1;
		stack.push(PC);
		uint16_t target = ((op[0] & 0xF) << 8) | op[1];
		PC = target;
		std::cout << "CALL " << std::hex << (int)target << std::endl;

	}
	case 0x03:
	{
		if (V[secondnib] == op[1])
		{
			PC += 4;
		}
		else
		{
			PC += 2;
		}
		std::cout << "SE V" << std::hex << (int)secondnib << ", " << std::hex << (int)op[1] << std::endl;
	} 
	break;
	case 0x04: 
	{
		if (V[secondnib] != op[1])
		{
			PC += 4;
		}
		else
		{
			PC += 2;
		}
		std::cout << "SNE V" << std::hex << (int)secondnib << ", " << std::hex << (int)op[1] << std::endl;
	}
	case 0x05: 
	{
		if (V[secondnib] == V[thirdnib])
		{
			PC += 4;
		}
		else
		{
			PC += 2;
		}
		std::cout << "SE V" << std::hex << (int)secondnib << ", V" << std::hex << (int)thirdnib << std::endl;
	}
	case 0x06:
	{
		V[secondnib] = op[1];
		PC += 2;
		std::cout << "LD V" << std::hex << (int)secondnib << ", " << std::hex << (int)op[1] << std::endl;
	}
	break;
	case 0x07: 
	{
		V[secondnib] += op[1];
		PC += 2;
		std::cout << "ADD V" << std::hex << (int)secondnib << ", " << std::hex << (int)op[1] << std::endl;
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
			std::cout << "LD V" << secondnib << ", V" << thirdnib << std::endl;
		}
		break;
		case 0x01:
		{
			V[secondnib] = V[secondnib] | V[thirdnib];
			PC += 2; 
			std::cout << "OR V" << secondnib << ", V" << thirdnib << std::endl;

		}
		break;
		case 0x02:
		{
			V[secondnib] = V[secondnib] & V[thirdnib];
			PC += 2;
			std::cout << "AND V" << secondnib << ", V" << thirdnib << std::endl;

		}
		break;
		case 0x03:
		{
			V[secondnib] = (!V[secondnib] != !V[thirdnib]);
			PC += 2;
			std::cout << "XOR V" << secondnib << ", V" << thirdnib << std::endl;

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
			std::cout << "ADD V" << secondnib << ", V" << thirdnib << std::endl;

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
			std::cout << "SUB V" << secondnib << ", V" << thirdnib << std::endl;

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
			std::cout << "SHR V" << secondnib << " {, V" << thirdnib << "}" << std::endl;

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
			std::cout << "SUBN V" << secondnib << ", V" << thirdnib << std::endl;
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
			std::cout << "SHL V" << secondnib << " {, V" << thirdnib << "}" << std::endl;

		}
		break;
		}
	}
	case 0x09: 
	{
		if (V[secondnib] == V[thirdnib])
		{
			PC += 4;
		}
		else
		{
			PC += 2;
		}
		std::cout << "SNE V" << secondnib << ", V" << thirdnib << std::endl;

	}
	break;
	case 0x0A:
	{
		I = ((op[0] & 0xF) << 8) | op[1];
		PC += 2;
		std::cout << "LD I, " << std::hex << (int)I << std::endl;
	}
	break;
	case 0x0B: 
	{
		PC = (((op[0] & 0xF) << 8) | op[1]) + V[0];
		std::cout << "JP V0, " << std::hex << (int)PC << std::endl;
	}
	break;
	case 0x0C:
	{
		uint8_t random = rand() % 256;
		V[secondnib] = random & op[1];
		PC += 2;
		std::cout << "RND V" << std::hex << (int) secondnib << ", " << std::hex << (int) op[1] << std::endl;

	}
	break;
	case 0x0D: 
	{
		uint16_t x = V[secondnib] % 64;
		uint16_t y = V[thirdnib] % 32;
		V[0xF] = 0;
		uint16_t height = op[0] & 0x000F;
		uint16_t pixel;

		////used graphics rendering from James Griffen	
		for (int i = 0; i < height; ++i) //number of rows
		{
			pixel = memory[I + i];
			for (int j = 0; j < 8; ++j) //j has a fixed width
			{
				if ((pixel & (0x80 >> j)) != 0)
				{
					if (screen[(x + j + ((y + i) * 64))] == 1)
					{
						V[0xF] = 1;
					}
					screen[x + j + ((y + i) * 64)] ^= 1;
				}
			}
		}

		draw_flag = true;
		PC += 2;
		std::cout << "DRW V" << std::hex << (int)secondnib << ", V" << std::hex << (int)thirdnib << ", " << std::hex << (int)fourthnib << std::endl;

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
				PC += 4;
			}
			else
			{
				PC += 2;
			}
			std::cout << "SKP V" << std::hex << (int)secondnib << std::endl;

		}
		break;
		case 0xA1:
		{
			if (key[secondnib] == 0)
			{
				PC += 4;
			}
			else
			{
				PC += 2;
			}
			std::cout << "SKNP V" << std::hex << (int)secondnib << std::endl;
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
			std::cout << "LD V" << std::hex << (int)secondnib << ", DT" << std::endl;

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
			std::cout << "LD V" << std::hex << (int)secondnib << ", K" << std::endl;

		}
		break;
		case 0x15:
		{
			delay = V[secondnib];
			PC += 2;
			std::cout << "LD DT, V" << std::hex << (int)secondnib << std::endl;

		}
		break;
		case 0x18:
		{
			sound = V[secondnib];
			PC += 2;
			std::cout << "LD ST, V" << std::hex << (int)secondnib << std::endl;

		}
		break;
		case 0x1E:
		{
			I += V[secondnib];
			PC += 2;
			std::cout << "ADD I, V" << std::hex << (int)secondnib << std::endl;
		}
		break;
		case 0x29:
		{
			I = V[secondnib];
			PC += 2;
			std::cout << "LD F, V" << std::hex << (int)secondnib << std::endl;
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
			std::cout << "LD B, V" << std::hex << (int)secondnib << std::endl;
		}
		case 0x55:
		{
			for (uint16_t i = I; i < 16; ++i)
			{
				memory[i] = V[i];
			}
			PC += 2;
			std::cout << "LD [I], V" << std::hex << (int)secondnib << std::endl;

		}
		break;
		case 0x65:
		{
			for (uint16_t i = I; i < 16; ++i)
			{
				V[i] = memory[i];
			}
			PC += 2;
			std::cout << "LD V" << std::hex << (int)secondnib << ", [I]" << std::endl;

		}
		break;
		}
	}
	}
}
