#define SDL_MAIN_HANDLED
#include <iostream>
#include "GameBoy.h"
#include <chrono>
#include <thread>
#include "SDL2/SDL.h"
#include "Mapper.h"
#include "PPU.h"


int main(int argc, char* argv[]) {
	if (argc < 2) {
		std::cout << "Usage: " << argv[0] << " <rom.gb|rom.gbc>\n";
		exit(1);
	}

	std::string fileName = argv[1];
	auto dot = fileName.rfind('.');

	if (dot == std::string::npos ||
		(fileName.substr(dot) != ".gb" && fileName.substr(dot) != ".gbc")) {
		std::cout << "Error: not a .gb or .gbc file\n";
		exit(1);
	}
		PPU ppu = PPU();
		if (ppu.initSdl() == -1) {
			exit(1);
		}
		Mapper map = Mapper(fileName);
		GameBoy gb = GameBoy();
		gb.loadComponets(&map, &ppu);
		//gb.testInstuctions();

		
		while (gb.isRunning)
		{
			gb.testRom();
			//std::this_thread::sleep_for(std::chrono::nanoseconds(700));


		}
		
		

	
	
	
	//GameBoy gb = GameBoy();
	//gb.testInstuctions();
	

	//Cartridge cart("06-ld r,r.gb");
	
	/*GameBoy gb = GameBoy();
	//gb.testInstuctions();
	
	gb.initLogging();
	while (gb.isRunning)
	{
		gb.testRom();
		//std::this_thread::sleep_for(std::chrono::microseconds(1));
		
		
	}
	gb.closeTxt();*/

	//gb.testInstuctions();
	
	//Cartridge* cart = new Cartridge("06-ld r,r.gb");
	//Mapper* map ;
	//MBC1 MBC1;
	//MBC1.loadCartridge(cart);
	//map = &MBC1;
	
	
	
	//map->write(0x1000, 0x01);
	//uint8_t data = map->read(0x4611);
	//std::cout << std::hex<<std::uppercase<< static_cast<int>(data);

	
	return 0;
}