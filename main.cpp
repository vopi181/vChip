#include <iostream>  //c++ style io
#include <cstdio>  //good ol' c style io
#include <vector> //gotta exclude c scum
#include <fstream>
#include <iterator> //ifstream_iter
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <string>
#include <cstring>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <thread>








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


#define WIN_HEIGHT 640
#define WIN_WIDTH 320
//sprite width is static @ 8 bytes / 1 byte
#define SPRITE_WIDTH = 8

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
	bool draw_flag = false;
	std::vector<BYTE> rom;
}chip8;
chip8 myChip;

void updateKeyState(chip8* chip);

std::vector<BYTE> readFile(std::string filename)
{
	// open the file:
	std::ifstream file(filename, std::ios::in | std::ios::binary);

	// Stop eating new lines in binary mode!!!
	file.unsetf(std::ios::skipws);

	// get its size:
	std::streampos fileSize;
	//    file.clear();
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

std::string get_BCD(int num) {
	if (num <= 1)
	{
		return std::string() + (char)('0' + num);
	}

	int remain = num % 2;
	return get_BCD(num / 2) + (char)('0' + remain);

}
void initialize(chip8* chip) {
	// init registers and memory once
	chip->memory = std::vector<BYTE>(4096);
	chip->pc = 0x200;
	chip->opcode = 0;
	chip->I = 0;
	chip->sp = 0;
	std::vector<BYTE> fontset = {
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

	//copy fontset to 0x050-0x0A0
	for (int i = 0; i < fontset.size(); i++) { chip->memory[i + 0x050] = fontset[i]; }

	std::copy(chip->rom.begin(), chip->rom.end(), chip->memory.begin() + 512);
	std::cout << "finish copying rom to memory\n";
	system("pause");
}

void setKeyState(chip8* chip, unsigned int key, bool state) {
	chip->key[key] = state;
}

void emulate_cycle(chip8* chip, sf::RenderWindow* window) {
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
			window->clear(sf::Color::Black);
			chip->draw_flag = true;
			chip->pc += 2;
			break;
		case 0x000e: //0x00ee: return from subrtn
			//exec op
			--chip->sp;
			chip->pc = chip->stack[chip->sp];
			chip->pc += 2;
			break;
		default:
			if ((chip->opcode & 0x00FF) == 0x0000) {
				chip->pc = (chip->pc + 2) & 0x0FFF;
			}
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
		else {
			chip->pc += 2;
		}
		break;
	}

				 
	case 0x4000: {
		auto Vx = ((chip->opcode & 0x0F00) >> 8);
		auto NN = (chip->opcode & 0x00FF);

		if (chip->V[Vx] != NN) {
			chip->pc += 4;
		}
		else {
			chip->pc += 2;
		}
		break;
	}
	case 0x5000: {
		auto Vx = ((chip->opcode & 0x0F00) >> 8);
		auto Vy = ((chip->opcode & 0x00F0) >> 4);
		if ((chip->V[Vx]) == (chip->V[Vy])) {
			chip->pc += 4;

		}
		else {
			chip->pc += 2;
		}
		break;
	}

	case 0x6000: {
		auto NN = (chip->opcode & 0x00FF);
		chip->V[(chip->opcode & 0x0F00) >> 8] = NN;
		chip->pc += 2;
		break;
	}
	case 0x7000: {
		auto Vx = (chip->opcode & 0x0F00) >> 8;
		//auto total = (chip->V[Vx] + (chip->opcode & 0x00FF));

		//// carry? then V[F] = 1 :else: V[F] = 0
		//(total > 255) ? chip->V[0xF] = 1 : chip->V[0xF] = 0;

		//chip->V[Vx] = total % 256;

		chip->V[Vx] += chip->opcode & 0x00FF;
		chip->pc += 2; // Increment the program counter.
		break;
	}
	case 0x8000: {
		switch (chip->opcode & 0x000F) {
		case 0x0000: {
			auto Vx = (chip->opcode & 0x0F00) >> 8;
			auto Vy = (chip->opcode & 0x00F0) >> 4;
			chip->V[Vx] = chip->V[Vy];
			chip->pc += 2;
			break;
		}
		case 0x0001: {
			auto Vx = (chip->opcode & 0x0F00) >> 8;
			auto Vy = (chip->opcode & 0x00F0) >> 4;
			chip->V[Vx] |= chip->V[Vy];
			chip->pc += 2;
			break;
		}
		case 0x0002: {
			auto Vx = (chip->opcode & 0x0F00) >> 8;
			auto Vy = (chip->opcode & 0x00F0) >> 4;
			chip->V[Vx] &= chip->V[Vy];
			chip->pc += 2;
			break;

		}
		case 0x0003: {
			auto Vx = (chip->opcode & 0x0F00) >> 8;
			auto Vy = (chip->opcode & 0x00F0) >> 4;
			chip->V[Vx] ^= chip->V[Vy];
			chip->pc += 2;
			break;
		}
		case 0x0004: {
			// this opcode is annoying
			// We need to check for a carry
			auto Vx = (chip->opcode & 0x0F00) >> 8;
			auto Vy = (chip->opcode & 0x00F0) >> 4;
			if (chip->V[Vy] > (0xFF - chip->V[Vx])) {
				chip->V[0xF] = 1; // carry flag
			}
			else {
				chip->V[0xF] = 0; //no carry

			}
			(chip->V[Vx]) += chip->V[Vy];
			chip->pc += 2;
			break;
		}
		case 0x0005: {
			// this opcode is annoying
			// We need to check for a borrow
			auto Vx = (chip->opcode & 0x0F00) >> 8;
			auto Vy = (chip->opcode & 0x00F0) >> 4;
			if (chip->V[Vy] > (chip->V[Vx])) {
				chip->V[0xF] = 0; // borrow flag
			}
			else {
				chip->V[0xF] = 1; //no borrow

			}
			(chip->V[Vx]) -= chip->V[Vy];
			chip->pc += 2;
			break;
		}
		case 0x0006: {
			auto Vx = (chip->opcode & 0x0F00) >> 8;
			auto Vy = (chip->opcode & 0x00F0) >> 4;
			chip->V[0xF] = (chip->V[Vx] & 0x1);
			chip->V[Vx] >>= 1;
			chip->pc += 2;
			break;
		}
		case 0x0007: {
			auto Vx = (chip->opcode & 0x0F00) >> 8;
			auto Vy = (chip->opcode & 0x00F0) >> 4;
			// this opcode is annoying
			// We need to check for a borrow
			if (chip->V[Vx] > (chip->V[Vy])) {
				chip->V[0xF] = 0; // borrow flag
			}
			else {
				chip->V[0xF] = 1; //no borrow

			}
			(chip->V[Vx]) = chip->V[Vy] - chip->V[Vx];
			chip->pc += 2;
			break;
		}
		case 0x000E: {
			auto Vx = (chip->opcode & 0x0F00) >> 8;
			auto Vy = (chip->opcode & 0x00F0) >> 4;
			chip->V[0xF] = (chip->V[Vx] >> 7);
			chip->V[Vx] <<= 1;
			chip->pc += 2;
			break;
		}
		default: {
			printf("opcode err [0x8000]: 0x%x\n", chip->opcode);
			break;
		}

		}
	}
	case 0x9000: {
		auto Vx = (chip->opcode & 0x0F00) >> 8;
		auto Vy = (chip->opcode & 0x00F0) >> 4;
		if (chip->V[Vx] != chip->V[Vy]) {
			chip->pc += 4;
		}
		else {
			chip->pc += 2;
		}
		break;

	}
	case 0xA000:
		chip->I = (chip->opcode & 0x0FFF);
		chip->pc += 2;
		break;
	case 0xB000: {
		auto NNN = chip->opcode & 0x0FFF;
		chip->pc = NNN + chip->V[0];
		break;

	}
	case 0xC000: {
		auto Vx = (chip->opcode & 0x0F00) >> 8;
		auto NN = chip->opcode & 0x00FF;
		srand(time(NULL));
		int randnum = rand() % 256;
		chip->V[Vx] = randnum & NN;
		chip->pc += 2;
		break;
	}
	case 0xD000: {
		unsigned short x = chip->V[(chip->opcode & 0x0F00) >> 8];
		unsigned short y = chip->V[(chip->opcode & 0x00F0) >> 4];
		unsigned short height = chip->opcode & 0x000F;
		long pixel;

		chip->V[0xF] = 0;
		for (int yline = 0; yline < height; yline++)
		{
			pixel = chip->memory[chip->I + yline];
			for (int xline = 0; xline < 8; xline++)
			{
				if ((pixel & (0x80 >> xline)) != 0)
				{
					if (chip->gfx[(x + xline + ((y + yline) * 64))] == 1)
						chip->V[0xF] = 1;
					chip->gfx[x + xline + ((y + yline) * 64)] ^= 1;
				}
			}
		}

		
		printf("draw at x = %d y = %d h = %d\n", x, y, height);
		chip->draw_flag = true;
		chip->pc += 2;
		break;
	}
	case 0xE000: {
		switch (chip->opcode & 0x00FF) {
		case 0x009E: {
			
			auto Vx = (chip->opcode & 0x0F00) >> 8;
			
			if (chip->key[chip->V[Vx]]) {
				chip->pc += 4;
			} else {
				chip->pc += 2;
			
			break;
		}
		case 0x00A1: {
			
			auto Vx = (chip->opcode & 0x0F00) >> 8;
			if (!chip->key[chip->V[Vx]]) {
				chip->pc += 4;
			}
			else {
				chip->pc += 2;

				break;
			break;
		}
		default: {
			printf("opcode err [0xE000]: %x\n", chip->opcode);
			break;
		}
		}
	}
	case 0xF000: {
		switch (chip->opcode & 0x00FF) {
		case 0x0007: {
			auto Vx = (chip->opcode & 0x0F00) >> 8;
			chip->V[Vx] = chip->delay_timer;
			chip->pc += 2;
			break;

		}
		case 0x000A: {
			//blocking wait for keypress
			auto Vx = (chip->opcode & 0x0F00) >> 8;


			bool KeyPress = false;

			for (int i = 0; i < 16; i++) {
				if (chip->key[i]) {
					KeyPress = true;
					chip->V[Vx] = i;
					break;
				}

			}
			if (!KeyPress) {
				return; //exit now which wont increment program counter
			}
			
			chip->pc += 2;
			break;
		}
		case 0x0015: {
			auto Vx = (chip->opcode & 0x0F00) >> 8;
			chip->delay_timer = chip->V[Vx];
			chip->pc += 2;
			break;
		}
		case 0x0018: {
			auto Vx = (chip->opcode & 0x0F00) >> 8;
			chip->sound_timer = chip->V[Vx];
			chip->pc += 2;
			break;
		}
		case 0x001E: {
			auto Vx = (chip->opcode & 0x0F00) >> 8;
			if (chip->I + chip->V[Vx] > 0xFFF) {
				chip->V[0xF] = 1;
			}
			else {
				chip->V[0xF] = 0;
			}
				
			chip->I += chip->V[Vx];
			chip->pc += 2;
			break;
		}
		case 0x0029: {
			auto Vx = (chip->opcode & 0x0F00) >> 8;
			auto chr = chip->V[Vx];
			//@TODO
			// chip->I = sprite_address[chip->V[Vx]];
			chip->I = chip->V[Vx] * 0x5;
			chip->pc += 2;
			break;
		}
		case 0x0033: {
			//binary coded decimal yeah!
			auto Vx = (chip->opcode & 0x0F00) >> 8;
			chip->I = std::stoi(get_BCD(chip->V[Vx]));
			chip->pc += 2;
			break;
		}
		case 0x0055: {
			auto Vx = (chip->opcode & 0x0F00) >> 8;
			
			for (int i = 0; i <= Vx; ++i) {
				chip->memory[chip->I + i] = chip->V[i];
			}

			chip->I += Vx + 1;
			chip->pc += 2;
			break;
		}
		case 0x0065: {
			//similar to above but fills instead of storing
			auto Vx = (chip->opcode & 0x0F00) >> 8;
			for (int i = 0; i <= Vx; ++i) {
				chip->V[i] = chip->memory[chip->I + i];
			}
			chip->I += Vx + 1;
			chip->pc += 2;
			break;
		}
		default: {
			printf("opcode err [0xF000]: %x\n", chip->opcode);
			break;
		}

		}
		break;
	}


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

void sfml_update(sf::RenderWindow* window) {
	//called in infinite for loop

}
int main(int argc, char** argv) {


	std::cout << argv[1];

	auto file = readFile(argv[1]);
	std::copy(file.begin(), file.end(), std::back_inserter(myChip.rom));

	initialize(&myChip);
	printf("myChip.rom[1] = %x\n", myChip.rom[1]);
	printf("myChip.memory[514] = %x\n", myChip.memory[514]);
	//SFML STUFF


	sf::RenderWindow window(sf::VideoMode(WIN_HEIGHT, WIN_WIDTH), "vCHIP - CHIP-8 Emulator by vopi181");
	

	while (window.isOpen()) {
		updateKeyState(&myChip);
		emulate_cycle(&myChip, &window);
		sf::Event event;
		while (window.pollEvent(event))
		{
			// "close requested" event: we close the window
			if (event.type == sf::Event::Closed)
				window.close();
		}
		

		if (myChip.draw_flag == true) {
			auto vertPoint = 0;
			auto vertCounter = 0;
			for (int i = 0; i < sizeof(myChip.gfx); ++i) {
				auto horizPoint = (i % 64);
				
				if (myChip.gfx[i] == 1) {
					
					
					
					



					/*auto vertPoint = std::floor(64 / i) * 10;*/

					
					sf::RectangleShape pixel(sf::Vector2f(10, 10));
					pixel.setPosition(horizPoint * 10, vertPoint * 10);
					pixel.setFillColor(sf::Color::White);
					window.draw(pixel);

					
					
				}
				
				
				vertCounter++;
				if (vertCounter == 64) {
					vertPoint += 1;
					vertCounter = 0;
				}
				
			}
			myChip.draw_flag = false;
		}
		
		window.display();
		std::this_thread::sleep_for(std::chrono::microseconds(1200));
	}
}


void updateKeyState(chip8* chip) {
	setKeyState(chip, 0x1, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num1));
	setKeyState(chip, 0x2, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num2));
	setKeyState(chip, 0x3, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num3));
	setKeyState(chip, 0xC, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num4));
				
	setKeyState(chip, 0x4, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q));
	setKeyState(chip, 0x5, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W));
	setKeyState(chip, 0x6, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E));
	setKeyState(chip, 0xD, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R));
			
	setKeyState(chip, 0x7, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A));
	setKeyState(chip, 0x8, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S));
	setKeyState(chip, 0x9, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D));
	setKeyState(chip, 0xE, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::F));
			 
	setKeyState(chip, 0xA, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z));
	setKeyState(chip, 0x0, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::X));
	setKeyState(chip, 0xB, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::C));
	setKeyState(chip, 0xF, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::V));
}






