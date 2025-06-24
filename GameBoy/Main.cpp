#define SDL_MAIN_HANDLED
#include <iostream>
#include "GameBoy.h"
#include <chrono>
#include <thread>
#include "SDL2/SDL.h"
#include "Mapper.h"
#include "PPU.h"


int main(int argc, char* argv[]) {
	std::string fileName;
	fileName = argv[1];
		
		if (fileName.substr(fileName.size() - 3) != ".gb") {
			std::cout << "Error Second Argument is not a .gb file";
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