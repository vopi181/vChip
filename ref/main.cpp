#include <stdlib.h>  //self explainatory
#include <stdio.h>   // io
#include <stdint.h>  //unintX_t etc

/*
    CHIP 8 tech ref
    4k of ram -> program ram starts at 0x200
    Display is  64x32 (1 bit per pixel)
    Display buffer located at ram offset 0xF00
    stack located at -> 0xEA0
    16 8 bit registers V0, V1, ... VF
    memory address register called i
    has stack instructions(so needs stack pointer)
    2 timers(1 delay, 1 sound)

*/

void DisassembleChip8Op(uint8_t* codebuffer, int pc) {
  uint8_t* code = &codebuffer[pc];
  uint8_t firstnib = (code[0] >> 4);

  printf("%04x %02x %02x ", pc, code[0], code[1]);
  switch (firstnib) {
    case 0x00: {
      switch (code[1]) {
        case 0xe0:
          printf("%-10s", "CLS");
          break;
        case 0xee:
          printf("%-10s", "RTS");
          break;
      }
    } break;
    case 0x01:
      printf("%-10s $%01x%02x", "JMP", code[0] & 0xf, code[1]);
      break;
    case 0x02:
      printf("%-10s $%01x%02x", "CALL", code[0] & 0xf, code[1]);
      break;
    case 0x03:
      printf("%-10s V%01x,#$02x", "SKPEQ", code[0] & 0xf, code[1]);
      break;
    case 0x04:
      printf("%-10s V%01x,#$02x", "SKPNQ", code[0] & 0xf, code[1]);
      break;
    case 0x05:
      printf("%-10s V%01x,V%01x", "SKPREGEQ", code[0] & 0xf, code[1] >> 4);
      // Most bitshifts are
      // for text alightment
      break;
    case 0x06: {
      uint8_t reg = code[0] & 0x0f;
      printf("%-10s V%01X,#$%02x", "MVI", reg, code[1]);
    } break;
    case 0x07: {
      uint8_t reg = code[0] & 0x0f;
      // unsigned n;
      // char* dec = &code[1];
      // sscanf(dec, "%x", &n);
      printf("%-10s V%01X,#$%02x", "ADD", reg, code[1]);
      // printf("; %d", n);
      // commented out trys to add decimal to disassmebly
      // not really useful so idk.
    } break;
    case 0x8: {
      uint8_t lastnib = code[1] >> 4;
      switch (lastnib) {
        case 0:
          printf("%-10s V%01X,V%01X", "MOV.", code[0] & 0xf, code[1] >> 4);
          break;
        case 1:
          printf("%-10s V%01X,V%01X", "OR.", code[0] & 0xf, code[1] >> 4);
          break;
        case 2:
          printf("%-10s V%01X,V%01X", "AND.", code[0] & 0xf, code[1] >> 4);
          break;
        case 3:
          printf("%-10s V%01X,V%01X", "XOR.", code[0] & 0xf, code[1] >> 4);
          break;
        case 4:
          printf("%-10s V%01X,V%01X", "ADD.", code[0] & 0xf, code[1] >> 4);
          break;
        case 5:
          printf("%-10s V%01X,V%01X,V%01X", "SUB.", code[0] & 0xf,
                 code[0] & 0xf, code[1] >> 4);
          break;
        case 6:
          printf("%-10s V%01X,V%01X", "SHR.", code[0] & 0xf, code[1] >> 4);
          break;
        case 7:
          printf("%-10s V%01X,V%01X,V%01X", "SUB.", code[0] & 0xf, code[1] >> 4,
                 code[1] >> 4);
          break;
        case 0xe:
          printf("%-10s V%01X,V%01X", "SHL.", code[0] & 0xf, code[1] >> 4);
          break;
        default:
          printf("UNKNOWN 8");
          break;
      }
    } break;
    case 0x9:
      printf("%-10s V%01X,V%01X", "SKIP.NE", code[0] & 0xf, code[1] >> 4);
      break;
    case 0xa:
      printf("%-10s I,#$%01x%02x", "MVI", code[0] & 0xf, code[1]);
      break;
    case 0xb:
      printf("%-10s $%01x%02x(V0)", "JUMP", code[0] & 0xf, code[1]);
      break;
    case 0xc:
      printf("%-10s V%01X,#$%02X", "RNDMSK", code[0] & 0xf, code[1]);
      break;
    case 0xd:
      printf("%-10s V%01X,V%01X,#$%01x", "SPRITE", code[0] & 0xf, code[1] >> 4,
             code[1] & 0xf);
      break;
    case 0xe:
      switch (code[1]) {
        case 0x9E:
          printf("%-10s V%01X", "SKIPKEY.Y", code[0] & 0xf);
          break;
        case 0xA1:
          printf("%-10s V%01X", "SKIPKEY.N", code[0] & 0xf);
          break;
        default:
          printf("UNKNOWN E");
          break;
      }
      break;
    case 0xf:
      switch (code[1]) {
        case 0x07:
          printf("%-10s V%01X,DELAY", "MOV", code[0] & 0xf);
          break;
        case 0x0a:
          printf("%-10s V%01X", "KEY", code[0] & 0xf);
          break;
        case 0x15:
          printf("%-10s DELAY,V%01X", "MOV", code[0] & 0xf);
          break;
        case 0x18:
          printf("%-10s SOUND,V%01X", "MOV", code[0] & 0xf);
          break;
        case 0x1e:
          printf("%-10s I,V%01X", "ADI", code[0] & 0xf);
          break;
        case 0x29:
          printf("%-10s I,V%01X", "SPRITECHAR", code[0] & 0xf);
          break;
        case 0x33:
          printf("%-10s (I),V%01X", "MOVBCD", code[0] & 0xf);
          break;
        case 0x55:
          printf("%-10s (I),V0-V%01X", "MOVM", code[0] & 0xf);
          break;
        case 0x65:
          printf("%-10s V0-V%01X,(I)", "MOVM", code[0] & 0xf);
          break;
        default:
          printf("UNKNOWN F");
          break;
      }
      break;
  }
}
typedef struct Chip8State {
  uint8_t v[16];  // registers
  uint16_t I;     // memory address reg
  uint16_t SP;    // stack pointer
  uint16_t PC;    // Program Counter
  uint8_t delay;  // delay timer
  uint8_t sound;  // sound delay
  uint8_t* memory;
  uint8_t* screen;  // memory[0xF00]

} Chip8State;

static void UnimplementedInstruction(Chip8State* state) {
  // pc will have advanced one, so undo that
  printf("Error: Unimplemented instruction\n");
  DisassembleChip8Op(state->memory, state->PC);
  printf("\n");
  exit(1);
}

Chip8State* InitChip8() {
  Chip8State* state = calloc(sizeof(Chip8State), 1);
  state->memory = calloc(1024 * 4, 1);
  state->screen = &state->memory[0xf00];
  state->SP = 0xfa0;
  state->PC = 0x200;
  return state;
}
void EmulateChip8Op(Chip8State* state) {
  uint8_t* op = &state->memory[state->PC];

  int highnib = (*op & 0xf0) >> 4;
  switch (highnib) {
    case 0x00:
      UnimplementedInstruction(state);
      break;
    case 0x01:  // JUMP $NNN
    {
      uint16_t target = ((code[0] & 0xf) << 8) | code[1];
      state->PC = target;
    } break;
    case 0x02:
      UnimplementedInstruction(state);
      break;
    case 0x03:  // SKIP.EQ VX,#$NN
    {
      uint8_t reg = code[0] & 0xf;
      if (state->V[reg] == code[1])
        state->PC += 2;
      state->PC += 2;
    } break;
    case 0x04:
      UnimplementedInstruction(state);
      break;
    case 0x05:
      UnimplementedInstruction(state);
      break;
    case 0x06:  // MOV VX,#$NN
    {
      uint8_t reg = code[0] & 0xf;
      state->V[reg] = code[1];
      state->PC += 2;
    } break;
    case 0x07:
      UnimplementedInstruction(state);
      break;
    case 0x08:
      UnimplementedInstruction(state);
      break;
    case 0x09:
      UnimplementedInstruction(state);
      break;
    case 0x0a:  // MOV I, #$NNN
    {
      state->I = ((code[0] & 0xf) << 8) | code[1];
      state->PC += 2;
    } break;
    case 0x0b:
      UnimplementedInstruction(state);
      break;
    case 0x0c:
      UnimplementedInstruction(state);
      break;
    case 0x0d:
      UnimplementedInstruction(state);
      break;
    case 0x0e:
      UnimplementedInstruction(state);
      break;
    case 0x0f:
      UnimplementedInstruction(state);
      break;
  }
}
int main(int argc, char** argv) {}