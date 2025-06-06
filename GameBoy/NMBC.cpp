#include "NMBC.h"

NMBC::NMBC(Cartridge* cart){
	ifCartLoaded = true;
	cartridge = cart;
}

NMBC::NMBC(){
	ifCartLoaded = false;
}

NMBC::~NMBC()
{
}

uint8_t NMBC::read(uint16_t address) {
	if (!ifCartLoaded && !cartridge->ifLoaded) {
		std::cout << "The cartridge isn't loaded" << "\n";
		return 0xFF;
	}
	if (address >= 0x0000 && address <= 0x3FFF) {
		return cartridge->read(address,0);
	}
	if (address >= 0x4000 && address <= 0x7FFF) {
		uint16_t offset = address - 0x4000;
		return cartridge->read(offset, 1);
	}
	
	return 0xFF;
}

void NMBC::write(uint16_t address, uint8_t data){

}

void NMBC::loadCartridge(Cartridge* cart){
	ifCartLoaded = true;
	cartridge = cart;
}
