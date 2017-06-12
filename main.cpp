#include <iostream>  //c++ style io
#include <stdio.h>   //good ol' c style io
#include <vector> //gotta exclude c scum
#include <fstream>
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


class chip8 {
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
  std::vector<char> rom;
public: 
void loadrom(std::string file_name) {
  std::ifstream romfile("file_name", std::ios::binary | std::ios::in | std::ios::ate);
  char ch;
  std::streampos size;
  char * memblock;
  if (romfile.is_open())
  {
    size = romfile.tellg();
    memblock = new char [size];
    romfile.seekg (0, std::ios::beg);
    romfile.read (memblock, size);
    romfile.close();

    std::cout << "the entire file content is in memory";
    for (int i; i < size; i++) {
      rom.push_back(memblock[i + 512]);
    }


    delete[] memblock;
  }
  else std::cout << "Unable to open file";
  std::cout << "finished loading rom\n";
}
void initialize() {
  // init registers and memory once

  pc = 0x200;
  opcode = 0;
  I = 0;
  sp = 0;
  for (int size; size > rom.size(); size++) {
    //int i = 0;
    memory[size + 512] = rom[size];
    //i++;

  
  }
  std::cout << "finish copying rom to memory";
  system("pause");
}

void emulate_cycle() {
  // Fetch opcode
  std::cout << "DEBUG: memory[pc] = " << memory[pc] << "\n";
  std::cout << "DEBUG: memory[pc + 1] = " << memory[pc + 1] << "\n";
  opcode = memory[pc] << 8 | memory[pc + 1];

  // Decode Opcode
  switch (opcode & 0x0FFF) {
    case 0xA000:
      I = opcode & 0x0FFF;
      pc += 2;
      break;

    default: std::cout << "opcode err:" << opcode << "\n"; system("pause"); break;


  }
  // Execute Opcode
  // update timers
  if(delay_timer > 0)
    --delay_timer;
 
  if(sound_timer > 0)
  {
    if(sound_timer == 1)
      printf("BEEP!\n");
    --sound_timer;
  }  
}
};






chip8 myChip;
int main(int argc, char** argv) {
myChip.loadrom(argv[1]);
std::cout << argv[1];
myChip.initialize();

for (;;) {
  myChip.emulate_cycle();
}




}