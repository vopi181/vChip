#include <iostream>  //c++ style io
#include <stdio.h>   //good ol' c style io
#include <vector> //gotta exclude c scum
#include <fstream>
#include <iterator> //ifstream_iter
#include <algorithm>
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



typedef unsigned char BYTE;

typedef struct {
	unsigned short opcode;
	std::vector<BYTE> memory;
	unsigned char V[16];
	unsigned short I;
	unsigned short pc;
	unsigned char gfx[64 * 32];
	unsigned char delay_timer = 0;
	unsigned char sound_timer = 0;
	unsigned short stack[16];
	unsigned short sp;
	unsigned char key[16];
	std::vector<BYTE> rom;
}chip8;
chip8 myChip;


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
	vec.reserve((unsigned long long int) fileSize);

	// read the data:
	vec.insert(vec.begin(),
		std::istream_iterator<BYTE>(file),
		std::istream_iterator<BYTE>());

	return vec;
}

void initialize(chip8* chip) {
	// init registers and memory once
    chip->memory = std::vector<BYTE>(4096);
	chip->pc = 0x200;
	chip->opcode = 0;
	chip->I = 0;
	chip->sp = 0;
    /*for (int size; size > 512; size++) {
        chip->memory.push_back(0);
    }*/

//    std::fill(chip->memory.begin(), chip->memory.begin() + 4096, 0);
//	for (int i = 0; i < (chip->rom.size()); i++) {
//		//int i = 0;
//		chip->memory[i + 512] = chip->rom[i];
//		//i++;
//
//
//	}
    std::copy(chip->rom.begin(), chip->rom.end(), chip->memory.begin() + 512);
    std::cout << "finish copying rom to memory\n";
	system("pause");
}



void emulate_cycle(chip8* chip) {
	// Fetch opcode
	printf("DEBUG: memory[pc] = %x\n", chip->memory[chip->pc]);
    printf("DEBUG: memory[pc + 1] = %x\n", chip->memory[chip->pc + 1]);
	chip->opcode = chip->memory[chip->pc] << 8 | chip->memory[chip->pc + 1];

	// Decode Opcode
	switch (chip->opcode & 0xF000) {
        case 0x0000:
            switch (chip->opcode & 0x000F) {
                case 0x0000: //0x00E0: Clear Screen
                    //exec op
                    printf("clear screen\n");
                    chip->pc += 2;
                    break;
                case 0x000e: //0x00ee: return from subrtn
                    //exec op
                    chip->pc += 2;
                    break;
                default:
                    printf("Unknown opcode [0x0000]: 0x%x\n", chip->opcode);
            } break;
        case 0x1000:
            chip->pc = chip->opcode & 0x0FFF;
            break;
        case 0x2000:
            chip->stack[chip->sp] = chip->pc; //need to store previous address before jump
            ++chip->sp;
            chip->pc = chip->opcode & 0x0FFF;
            break; //no need to inc PC by 2
		case 0x3000: {
            auto NN = (chip->opcode & 0x00FF);
            if (chip->V[(chip->opcode & 0x0F00) >> 8] == NN) {
                chip->pc += 4;
            }
        }

            break;
        case 0x4000: {
            auto NN = (chip->opcode & 0x00FF);
            if (chip->V[(chip->opcode & 0x0F00) >> 8] != NN) {
                chip->pc += 4;
            }
            break;
        }
        case 0x5000: {
            auto Vx = (chip->opcode & 0x0F00);
            auto Vy = (chip->opcode & 0x00F0);
            if((chip->V[Vx] >> 8) == (chip->V[Vy] >> 8)) {
                chip->pc += 4;

            }
        }

        case 0x6000:{
            auto NN = (chip->opcode & 0x00FF);
            chip->V[(chip->opcode & 0x0F00) >> 8] = NN;
            chip->pc += 2;
            break;
        }
        case 0x7000: {
            auto NN = (chip->opcode & 0x00FF);
            chip->V[(chip->opcode & 0x0F00) >> 8] += NN;
            chip->pc += 2;
            break;
        }
        case 0x8000: {
            switch (chip->opcode & 0x000F) {
                case 0x0000: {
                    auto Vx = chip->opcode & 0x0F00;
                    auto Vy = chip->opcode & 0x00F0;
                    chip->V[Vx] = chip->V[Vy];
                    chip->pc += 2;
                    break;
                }
                case 0x0001: {
                    auto Vx = chip->opcode & 0x0F00;
                    auto Vy = chip->opcode & 0x00F0;
                    chip->V[Vx] = chip->V[Vx] | chip->V[Vy];
                    chip->pc += 2;
                    break;
                }
                case 0x0002: {
                    auto Vx = chip->opcode & 0x0F00;
                    auto Vy = chip->opcode & 0x00F0;
                    chip->V[Vx] = chip->V[Vx] & chip->V[Vy];
                    chip->pc += 2;
                    break;

                }
                case 0x0003: {
                    auto Vx = chip->opcode & 0x0F00;
                    auto Vy = chip->opcode & 0x00F0;
                    chip->V[Vx] = chip->V[Vx] ^ chip->V[Vy];
                    chip->pc += 2;
                    break;
                }
                case 0x0004: {
                    // this opcode is annoying
                    // We need to check for a carry
                    auto Vx = chip->opcode & 0x0F00;
                    auto Vy = chip->opcode & 0x00F0;
                    if (chip->V[Vy >> 4] > (0xFF - chip->V[Vx >> 8])) {
                        chip->V[0xF] = 1; // carry flag
                    } else {
                        chip->V[0xF] = 0; //no carry

                    }
                    (chip->V[Vx >> 8]) += chip->V[Vy >> 4];
                    chip->pc += 2;
                    break;
                }
                case 0x0005: {
                    // this opcode is annoying
                    // We need to check for a borrow
                    auto Vx = chip->opcode & 0x0F00;
                    auto Vy = chip->opcode & 0x00F0;
                    if (chip->V[Vy >> 4] > (0xFF - chip->V[Vx >> 8])) {
                        chip->V[0xF] = 1; // borrow flag
                    } else {
                        chip->V[0xF] = 0; //no borrow

                    }
                    (chip->V[Vx >> 8]) -= chip->V[Vy >> 4];
                    chip->pc += 2;
                    break;
                }
                case 0x0006: {
                    auto Vx = chip->opcode & 0x0F00;
                    auto Vy = chip->opcode & 0x00F0;
                    chip->V[0xF] = (chip->V[Vx] >> 2);
                    chip->V[Vx] >> 1;
                }
                case 0x0007: {
                    auto Vx = chip->opcode & 0x0F00;
                    auto Vy = chip->opcode & 0x00F0;
                    // this opcode is annoying
                    // We need to check for a borrow
                    if (chip->V[Vy >> 4] > (0xFF - chip->V[Vx >> 8])) {
                        chip->V[0xF] = 0; // borrow flag
                    } else {
                        chip->V[0xF] = 1; //no borrow

                    }
                    (chip->V[Vx]) = chip->V[Vy] - chip->V[Vx];
                    chip->pc += 2;
                    break;
                }
                default: {
                    printf("opcode err [0x8000]: 0x%x\n", chip->opcode);
                    break;
                }
            }
        }

        case 0xA000:
            chip->I = (unsigned short) (chip->opcode & 0x0FFF);
            chip->pc += 2;
            break;

	    default: {
            printf("opcode err: 0x%x\n", chip->opcode);
            break;
        }


	}
	// Execute Opcode
	// update timers
	if (chip->delay_timer > 0)
		--chip->delay_timer;

	if (chip->sound_timer > 0)
	{
		if (chip->sound_timer == 1)
			printf("BEEP!\n");
		--(chip->sound_timer);
	}
}


int main(int argc, char** argv) {
	

	std::cout << argv[1];

	auto file = readFile(argv[1]);
    std::copy(file.begin(), file.end(), std::back_inserter(myChip.rom));

	initialize(&myChip);
	printf("myChip.rom[1] = %x\n", myChip.rom[1]);
	printf("myChip.memory[514] = %x\n", myChip.memory[514]);

for (;;) {
  emulate_cycle(&myChip);

}




}