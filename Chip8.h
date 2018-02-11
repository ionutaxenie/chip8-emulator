#include <string>

class Chip8
{
public:
	Chip8();
	~Chip8();

	static const unsigned short GFX_SIZE = 64 * 32;
	static const unsigned short MEMORY_SIZE = 4096;
	static const unsigned short REGISTERS_COUNT = 16;
	static const unsigned short STACK_SIZE = 16;
	static const unsigned short KEYS_COUNT = 16;

	static const unsigned char chip8_fontset[80];

	bool drawFlag = false;

	unsigned short opcode;
	unsigned char memory[MEMORY_SIZE];
	unsigned char V[REGISTERS_COUNT];
	unsigned short I;
	unsigned short pc;
	unsigned char gfx[GFX_SIZE];
	unsigned char delay_timer;
	unsigned char sound_timer;
	unsigned short stack[STACK_SIZE];
	unsigned short sp;
	unsigned char key[KEYS_COUNT];

	void initialize();
	void loadGame(std::string);
	void emulateCycle();
	void setKeys();
	void updateTimers();
};
