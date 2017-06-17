#include <iostream>  //c++ style io
#include <stdio.h>   //good ol' c style io
#include <vector> //gotta exclude c scum
#include <fstream>
#include <iterator> //ifstream_iter
// ref
// 0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
// 0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
// 0x200-0xFFF - Program ROM and work RAM

// ref cont
// The graphics system: The chip 8 has one instruction that
// draws sprite to the screen. Drawing is done in XOR mode and
// if a pixel is turned off as a result of drawing, the VF register is set.
// This is used for collision detection.
// The graphics of the Chip 8 are black and white and
// the screen has a total of 2048 pixels (64 x 32).
// This can easily be implemented using an array that hold the pixel state (1 or
// 0):
// decs


//class chip8 {
//  unsigned short opcode;
//  unsigned char memory[4096];
//  unsigned char V[16];
//  unsigned short I;
//  unsigned short pc;
//  unsigned char gfx[64 * 32];
//  unsigned char delay_timer;
//  unsigned char sound_timer;
//  unsigned short stack[16];
//  unsigned short sp;
//  unsigned char key[16];
//  std::vector<char> rom;
//public: 
//void loadrom(std::string file_name) {
//  std::ifstream romfile(file_name, std::ios::binary | std::ios::in | std::ios::ate);
//  std::vector<char> buffer((
//            std::istreambuf_iterator<char>(romfile)), 
//            (std::istreambuf_iterator<char>()));
//  rom = buffer;
//  std::cout << rom[4];
//  std::cout << "finished loading rom\n";
//}
//void initialize() {
//  // init registers and memory once
//
//  pc = 0x200;
//  opcode = 0;
//  I = 0;
//  sp = 0;
//  for (int size; size > rom.size(); size++) {
//    //int i = 0;
//    memory[size + 0x200] = rom[size];
//    //i++;
//
//  
//  }
//  std::cout << "finish copying rom to memory\n";
//  system("pause");
//}
//
//void emulate_cycle() {
//  // Fetch opcode
//  std::cout << "DEBUG: memory[pc] = " << memory[pc] << "\n";
//  std::cout << "DEBUG: memory[pc + 1] = " << memory[pc + 1] << "\n";
//  opcode = memory[pc] << 8 | memory[pc + 1];
//
//  // Decode Opcode
//  switch (opcode & 0x0FFF) {
//    case 0xA000:
//      I = opcode & 0x0FFF;
//      pc += 2;
//      break;
//
//    default: std::cout << "opcode err:" << opcode << "\n"; system("pause"); break;
//
//
//  }
//  // Execute Opcode
//  // update timers
//  if(delay_timer > 0)
//    --delay_timer;
// 
//  if(sound_timer > 0)
//  {
//    if(sound_timer == 1)
//      printf("BEEP!\n");
//    --sound_timer;
//  }  
//}
//};

typedef unsigned char BYTE;

typedef struct {
	unsigned short opcode;
	unsigned char memory[4096];
	unsigned char V[16];
	unsigned short I;
	unsigned short pc;
	unsigned char gfx[64 * 32];
	unsigned char delay_timer;
	unsigned char sound_timer;
	unsigned short stack[16];
	unsigned short sp;
	unsigned char key[16];
	std::vector<BYTE> rom;
}chip8;

std::vector<BYTE> readFile(const char* filename)
{
	// open the file:
	std::ifstream file(filename, std::ios::binary);

	// Stop eating new lines in binary mode!!!
	file.unsetf(std::ios::skipws);

	// get its size:
	std::streampos fileSize;

	file.seekg(0, std::ios::end);
	fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	// reserve capacity
	std::vector<BYTE> vec;
	vec.reserve(fileSize);

	// read the data:
	vec.insert(vec.begin(),
		std::istream_iterator<BYTE>(file),
		std::istream_iterator<BYTE>());

	return vec;
}

void initialize(chip8* chip) {
	// init registers and memory once

	chip->pc = 0x200;
	chip->opcode = 0;
	chip->I = 0;
	chip->sp = 0;
	for (int size; size > chip->rom.size(); size++) {
		//int i = 0;
		chip->memory[size + 0x200] = chip->rom[size];
		//i++;


	}
	std::cout << "finish copying rom to memory\n";
	system("pause");
}



void emulate_cycle(chip8* chip) {
	// Fetch opcode
	std::cout << "DEBUG: memory[pc] = " << chip->memory[chip->pc] << "\n";
	std::cout << "DEBUG: memory[pc + 1] = " << chip->memory[chip->pc + 1] << "\n";
	chip->opcode = chip->memory[chip->pc] << 8 | chip->memory[chip->pc + 1];

	// Decode Opcode
	switch (chip->opcode & 0x0FFF) {
	case 0xA000:
		chip->I = chip->opcode & 0x0FFF;
		chip->pc += 2;
		break;

	default: std::cout << "opcode err:" << chip->opcode << "\n"; system("pause"); break;


	}
	// Execute Opcode
	// update timers
	if (chip->delay_timer > 0)
		--chip->delay_timer;

	if (chip->sound_timer > 0)
	{
		if (chip->sound_timer == 1)
			printf("BEEP!\n");
		--chip->sound_timer;
	}
}

chip8 myChip;
int main(int argc, char** argv) {
	

	std::cout << argv[1];

	myChip.rom = readFile(argv[1]);
	initialize(&myChip);
	printf("%x", myChip.rom[1]);
	printf("%x", myChip.memory[514]);
//for (;;) {
//  emulate_cycle(myChip)
//}
//



}