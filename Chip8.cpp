#include "Chip8.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL.h>

const unsigned char Chip8::chip8_fontset[80] =
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
}


Chip8::~Chip8()
{
}

void Chip8::initialize()
{
	pc = 0x200;
	opcode = 0;
	I = 0;
	sp = 0;

	// Clear graphics, stack, registers and memory
	memset(gfx, 0, GFX_SIZE);
	memset(stack, 0, STACK_SIZE);
	memset(V, 0, REGISTERS_COUNT);
	memset(memory, 0, MEMORY_SIZE);
	memset(key, 0, KEYS_COUNT);
	
	memcpy(memory, chip8_fontset, 80);

	// Reset timers
	delay_timer = 0;
	sound_timer = 0;

	srand(time(NULL));
}

void Chip8::loadGame(std::string game)
{
	FILE * pFile;
	unsigned int size;
	char path_buffer[MAX_PATH];
	std::string path;
	std::string file;
	
	GetModuleFileName(NULL, path_buffer, MAX_PATH);
	std::string::size_type pos = std::string(path_buffer).find_last_of("\\/");
	path = std::string(path_buffer).substr(0, pos + 1);
	file = path + game;

	pFile = fopen(file.c_str(), "rb");
	if (pFile != NULL)
	{
		fseek(pFile, 0, SEEK_END);
		size = ftell(pFile);
		fseek(pFile, 0, SEEK_SET);
		fread(memory + 512, 1, size, pFile);
		fclose(pFile);
	}
}


void Chip8::emulateCycle()
{
	opcode = memory[pc] << 8 | memory[pc + 1];

	switch (opcode & 0xF000)
	{

	case 0x0000:
	{
		switch (opcode & 0xFF)
		{
			// 0x00E0 CLS
			// Clear display
		case 0x00E0:
			memset(gfx, 0, GFX_SIZE);
			pc += 2;
			break;

			// 0x00EE RET
			// Return from subroutine
		case 0x00EE:
			pc = stack[sp];
			sp--;
			pc += 2;
			break;

			// 0x0000 SYS addr
		default:
			pc += 2;
			break;
		}
		break;
	}

	// 0x1nnn JP addr
	// Jump to location nnn
	case 0x1000:
	{
		unsigned short addr = opcode & 0x0FFF;
		pc = addr;
		break;
	}

	// 0x2nnn CALL addr
	// Calls the subroutine at address NNN
	// TODO: Make sure this is right
	case 0x2000:
	{
		++sp;
		stack[sp] = pc;
		pc = opcode & 0xFFF;
		break;
	}

	// 0x3xkk SE Vx, byte
	// Skip next instruction if Vx == kk
	case 0x3000:
	{
		unsigned char reg_index = (opcode & 0x0F00) >> 8;
		unsigned char vx = V[reg_index];
		unsigned char value = opcode & 0x00FF;

		if (vx == value)
		{
			pc += 2;
		}

		pc += 2;
		break;
	}

	// 0x4xkk SNE Vx, byte
	// Skip next instruction if Vx != kk
	case 0x4000:
	{
		unsigned char reg_index = (opcode & 0x0F00) >> 8;
		unsigned char vx = V[reg_index];
		unsigned char value = opcode & 0x00FF;

		if (vx != value)
		{
			pc += 2;
		}

		pc += 2;
		break;
	}

	// 0x5xy0 SE Vx, Vy
	// Skip next instruction if Vx == Vy
	case 0x5000:
	{
		unsigned char reg1_index = (opcode & 0x0F00) >> 8;
		unsigned char reg2_index = (opcode & 0x00F0) >> 4;

		if (V[reg1_index] == V[reg2_index])
		{
			pc += 2;
		}

		pc += 2;
		break;
	}

	// 0x6xkk LD Vx, byte
	// Set Vx = kk
	case 0x6000:
	{
		unsigned char reg_index = (opcode & 0x0F00) >> 8;
		unsigned char value = opcode & 0x00FF;
		V[reg_index] = value;
		pc += 2;
		break;
	}

	// 0x7xkk ADD Vx, byte
	// Set Vx = Vx + kk
	case 0x7000:
	{
		unsigned char reg_index = (opcode & 0x0F00) >> 8;
		unsigned char value = opcode & 0x00FF;
		V[reg_index] += value;
		pc += 2;
		break;
	}

	case 0x8000:
	{
		unsigned char regx_index = (opcode & 0x0F00) >> 8;
		unsigned char regy_index = (opcode & 0x00F0) >> 4;

		switch (opcode & 0x000F)
		{
			// 0x8xy0 LD Vx, Vy
			// Set Vx = Vy
		case 0x0000:
		{
			V[regx_index] = V[regy_index];
			break;
		}

		// 0x8xy1 OR Vx, Vy
		// Set Vx = Vx OR Vy
		case 0x0001:
		{
			V[regx_index] = V[regx_index] | V[regy_index];
			break;
		}

		// 0x8xy2 AND Vx, Vy
		// Set Vx = Vx AND Vy
		case 0x0002:
		{
			V[regx_index] = V[regx_index] & V[regy_index];
			break;
		}

		// 0x8xy3 XOR Vx, Vy
		// Set Vx = Vx XOR Vy
		case 0x0003:
		{
			V[regx_index] = V[regx_index] ^ V[regy_index];
			break;
		}

		// 0x8xy4 ADD Vx, Vy
		// Set Vx = Vx + Vy, set VF = carry
		case 0x0004:
		{
			unsigned short result = V[regx_index] + V[regy_index];
			V[0xF] = ((result & 0xFF00) > 0) ? 1 : 0;
			V[regx_index] = result & 0x00FF;
			break;
		}

		// 0x8xy5 SUB Vx, Vy
		// Set Vx = Vx - Vy, set VF = NOT borrow
		case 0x0005:
		{
			V[0xF] = (V[regx_index] > V[regy_index]) ? 1 : 0;
			V[regx_index] -= V[regy_index];
			break;
		}

		// 0x8xy6 SHR Vx {, Vy}
		// Set Vx = Vx SHR 1
		case 0x0006:
		{
			V[0xF] = ((V[regx_index] & 0x01) == 1) ? 1 : 0;
			V[regx_index] /= 2;
			break;
		}

		// 0x8xy7 SUBN Vx, Vy
		// Set Vx = Vy - Vx, set VF = NOT borrow
		case 0x0007:
		{
			V[0xF] = (V[regy_index] > V[regx_index]) ? 1 : 0;
			V[regx_index] = V[regy_index] - V[regx_index];
			break;
		}

		// 0x8xyE SHL Vx {, Vy}
		// Set Vx = Vx SHL 1
		case 0x000E:
		{
			V[0xF] = ((V[regx_index] & 0x80) == 1) ? 1 : 0;
			V[regx_index] *= 2;
			break;
		}

		default:
		{
			printf("Unknown opcode: 0x%X\n", opcode);
			break;
		}

		}

		pc += 2;
		break;
	}

	// 0x9xy0 SNE Vx, Vy
	// Skip next instruction if Vx != Vy
	case 0x9000:
	{
		unsigned char regx_index = (opcode & 0x0F00) >> 8;
		unsigned char regy_index = (opcode & 0x00F0) >> 4;
		if (V[regx_index] != V[regy_index])
		{
			pc += 2;
		}
		pc += 2;
		break;
	}

	// 0xAnnn LD I, addr
	// Set I = nnn
	case 0xA000:
	{
		I = opcode & 0x0FFF;
		pc += 2;
		break;
	}

	// 0xBnnn JP V0, addr
	// Jump to location nnn + V0
	case 0xB000:
	{
		unsigned short value = opcode & 0x0FFF;
		pc = value + V[0];
	}

	// 0xCxkk RND Vx, byte
	// Set Vx = random byte AND kk
	case 0xC000:
	{
		unsigned char regx_index = (opcode & 0x0F00) >> 8;
		unsigned char random_byte = rand() % 256;
		unsigned char and_byte = opcode & 0x00FF;
		V[regx_index] = random_byte & and_byte;
		pc += 2;
		break;
	}

	// Dxyn - DRW Vx, Vy, nibble
	// Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision
	case 0xD000:
	{
		unsigned short x = V[(opcode & 0x0F00) >> 8];
		unsigned short y = V[(opcode & 0x00F0) >> 4];
		unsigned short height = opcode & 0x000F;
		unsigned short pixel;

		V[0xF] = 0;
		for (int yline = 0; yline < height; yline++)
		{
			pixel = memory[I + yline];
			for (int xline = 0; xline < 8; xline++)
			{
				if ((pixel & (0x80 >> xline)) != 0)
				{
					if (gfx[(x + xline + ((y + yline) * 64))] == 1)
						V[0xF] = 1;
					gfx[x + xline + ((y + yline) * 64)] ^= 1;
				}
			}
		}

		drawFlag = true;
		pc += 2;

		break;
	}

	case 0xE000:
	{
		switch (opcode & 0x00FF)
		{
		// 0xEx9E SKP Vx
		// Skip the next instruction if key with the value of Vx is pressed
		case 0x009E:
		{
			unsigned char regx_index = (opcode & 0x0F00) >> 8;
			if (key[V[regx_index]] == 1)
			{
				pc += 2;
			}
			break;
		}

		// 0xExA1 SKNP Vx
		// Skip next instruction if key with the value of Vx is not pressed
		case 0x00A1:
		{
			unsigned char regx_index =( opcode & 0x0F00) >> 8;
			if (key[V[regx_index]] == 0)
			{
				pc += 2;
			}
			break;
		}

		default:
		{
			printf("Unknown opcode: 0x%X\n", opcode);
			break;
		}

		}
		
		pc += 2;
		break;
	}

	case 0xF000:
	{
		unsigned char regx_index = (opcode & 0x0F00) >> 8;

		switch (opcode & 0x00FF)
		{
		// 0xFx07 LD Vx, DT
		// Set Vx = delay timer value
		case 0x0007:
		{
			V[regx_index] = delay_timer;
			break;
		}

		// 0xFx0A LD Vx, K
		// Wait for a key press, store the value of the key in Vx
		// TODO Implement this
		// This is a hack at this point, most games don't use it
		case 0x000A:
		{
			V[regx_index] = 0;
			break;
		}

		// 0xFx15 LD DT, Vx
		// Set delay timer = Vx
		case 0x0015:
		{
			delay_timer = V[regx_index];
			break;
		}

		// 0xFx18 LD ST, Vx
		// Set sound timer = Vx
		case 0x0018:
		{
			sound_timer = V[regx_index];
			break;
		}

		// 0xFx1E ADD I, Vx
		// Set I = I + Vx
		case 0x001E:
		{
			I += V[regx_index];
			break;
		}

		// 0xFx29 LD F, Vx
		// Set I = location of sprite for digit Vx
		case 0x0029:
		{
			I = V[regx_index] * 5;
			break;
		}

		// 0xFx33 LD B, Vx
		// Store BCD representation of Vx in memory locations I, I+1 and I+2
		case 0x0033:
		{
			memory[I] = V[regx_index] / 100;
			memory[I + 1] = (V[regx_index] / 10) % 10;
			memory[I + 2] = (V[regx_index] % 100) % 10;
			break;
		}

		// 0xFx55 LD [I], Vx
		// Store registers V0 through Vx in memory starting at location I
		case 0x0055:
		{
			memcpy(&memory[I], V, regx_index + 1);
			break;
		}

		// 0xFx65 LD Vx, [I]
		// Read registers V0 through Vx from memory starting at location I
		case 0x0065:
		{
			memcpy(V, &memory[I], regx_index + 1);
			break;
		}

		default:
		{
			printf("Unknown opcode: 0x%X\n", opcode);
			break;
		}

		}

		pc += 2;
		break;
	}

	default:
		printf("Unknown opcode: 0x%X\n", opcode);
		break;
	}
}

void Chip8::setKeys()
{
	for (unsigned char i = 0; i < 16; i++)
	{
		key[i] = 0;
	}

	SDL_PumpEvents();

	const Uint8 *state = SDL_GetKeyboardState(NULL);
	
	if (state[SDL_SCANCODE_1])
	{
		key[1] = 1;
	}
	if (state[SDL_SCANCODE_2])
	{
		key[2] = 1;
	}
	if (state[SDL_SCANCODE_3])
	{
		key[3] = 1;
	}
	if (state[SDL_SCANCODE_4])
	{
		key[0xC] = 1;
	}
	if (state[SDL_SCANCODE_Q])
	{
		key[4] = 1;
	}
	if (state[SDL_SCANCODE_W])
	{
		key[5] = 1;
	}
	if (state[SDL_SCANCODE_E])
	{
		key[6] = 1;
	}
	if (state[SDL_SCANCODE_R])
	{
		key[0xD] = 1;
	}
	if (state[SDL_SCANCODE_A])
	{
		key[7] = 1;
	}
	if (state[SDL_SCANCODE_S])
	{
		key[8] = 1;
	}
	if (state[SDL_SCANCODE_D])
	{
		key[9] = 1;
	}
	if (state[SDL_SCANCODE_F])
	{
		key[0xE] = 1;
	}
	if (state[SDL_SCANCODE_Z])
	{
		key[0xA] = 1;
	}
	if (state[SDL_SCANCODE_X])
	{
		key[0] = 1;
	}
	if (state[SDL_SCANCODE_C])
	{
		key[0xB] = 1;
	}
	if (state[SDL_SCANCODE_V])
	{
		key[0xF] = 1;
	}
}

void Chip8::updateTimers()
{
	if (delay_timer > 0)
	{
		--delay_timer;
	}

	if (sound_timer > 0)
	{
		if (sound_timer == 1)
		{
			// Do BEEP
			--sound_timer;
		}
	}
}
