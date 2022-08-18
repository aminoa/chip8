#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>

void Disassemble8080Opcode(uint8_t* buffer, int pc)
{
    uint8_t* code = &buffer[pc];
    uint8_t firstnib = (code[0] >> 4);
    uint8_t secondnib = (code[0] & 0xF);
    uint8_t thirdnib = (code[1] >> 4);
    uint8_t fourthnib = (code[1] & 0xF);

    printf("%04X %02X %02X ", pc, code[0], code[1]);

    switch (firstnib)
    {
    case 0x00:
        switch (code[1])
        {
        case 0xE0: printf("CLS"); break; 
        case 0xEE: printf("RET"); break;
        default: printf("SYS %01X%02X", secondnib, code[1]);
        }
        break;
    case 0x01: printf("JP %01X%02X", secondnib, code[1]); break;
    case 0x02: printf("CALL %01X%02X", secondnib, code[1]); break;
    case 0x03: printf("SE V%01X, %02X", secondnib, code[1]); break;
    case 0x04: printf("SNE V%01X, %02X", secondnib, code[1]); break;
    case 0x05: printf("SE V%01X, V%01X", secondnib, thirdnib); break;
    case 0x06: printf("LD V%01X, %02X", secondnib, code[1]); break;
    case 0x07: printf("ADD V%01X, %02X", secondnib, code[1]); break;
    case 0x08: 
        switch (fourthnib)
        {
        case 0x00: printf("LD V%01X, V%01X", secondnib, thirdnib); break;
        case 0x01: printf("OR V%01X, V%01X", secondnib, thirdnib); break;
        case 0x02: printf("AND V%01X, V%01X", secondnib, thirdnib); break;
        case 0x03: printf("XOR V%01X, V%01X", secondnib, thirdnib); break;
        case 0x04: printf("ADD V%01X, V%01X", secondnib, thirdnib); break;
        case 0x05: printf("SUB V%01X, V%01X", secondnib, thirdnib); break;
        case 0x06: printf("SHR V%01X {, V%01X}", secondnib, thirdnib); break;
        case 0x07: printf("SUBN V%01X, V%01X", secondnib, thirdnib); break;
        case 0x0E: printf("SHL V%01X {, V%01X}", secondnib, thirdnib); break;
        }
        break;
    case 0x09: printf("SNE V%01X, V%01X", secondnib, thirdnib); break;
    case 0x0A: printf("LD I, %01X%02X", secondnib, code[1]); break;
    case 0x0B: printf("JP V0, %01X%02X", secondnib, code[1]); break;
    case 0x0C: printf("RND V%01X, %02X", secondnib, code[1]); break;
    case 0x0D: printf("DRW V%01X, V%01X, %01X", secondnib, thirdnib, fourthnib); break;
    case 0x0E: 
        switch (code[1])
        {
        case 0x9E: printf("SKP V%0X1", secondnib); break;
        case 0xA1: printf("SKNP V%0X1", secondnib); break;
        }
        break;
    case 0x0F: 
        switch (code[1])
        {
        case 0x07: printf("LD V%0X1, DT", secondnib); break;
        case 0x0A: printf("LD V%0X1, K", secondnib); break;
        case 0x15: printf("LD DT, V%0X1", secondnib); break;
        case 0x18: printf("LD ST, V%0X1", secondnib); break;
        case 0x1E: printf("ADD I, V%0X1", secondnib); break;
        case 0x29: printf("LD F, V%0X1", secondnib); break;
        case 0x33: printf("LD B, V%0X1", secondnib); break;
        case 0x55: printf("LD [I], V%0X1", secondnib); break;
        case 0x65: printf("LD V%0X1, [I]", secondnib); break;
        }
        break;
    }
    printf("\n");



}

int main(int argc, char** argv)
{

    FILE* file = fopen(argv[1], "rb");
    if (!file)
    {
        std::cout << "Unable to open file. Exiting...";
        exit(1);
    }

    fseek(file, 0L, SEEK_END);
    int file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    unsigned char* buffer = new unsigned char[file_size + 0x200];
    fread(buffer + 0x200, file_size, 1, file);
    fclose(file);

    int pc = 0x200;
    while (pc < (file_size + 0x200))
    {
        Disassemble8080Opcode(buffer, pc);
        pc += 2;
    }

    return 0;
    
}