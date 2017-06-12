#include <iostream>  //c++ style io
#include <stdio.h>   //good ol' c style io
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
} Chip8;

void initialize(Chip8* chip) {
  // init registers and memory once

  chip->pc = 0x200;
  chip->opcode = 0;
  chip->I = 0;
  chip->sp = 0;
}

void emulate_cycle(Chip8* chip) {
  // Fetch opcode
  chip->opcode = chip->memory[chip->pc] << 8 | chip->memory[chip->pc + 1];

  // Decode Opcode
  switch (chip->memory[chip->pc] << 1) {}
  // Execute Opcode
  // update timers
}

int main(int argc, char** argv) {
  Chip8* myChip8;
}