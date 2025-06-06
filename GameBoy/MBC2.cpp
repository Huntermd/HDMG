#include "MBC2.h"

MBC2::MBC2(Cartridge* cart, bool ram, bool battery){
	ifCartLoaded = true;
	cartridge = cart;
	ifRam = ram;
	ifBattery = battery;
	
}

MBC2::MBC2(){
	ifCartLoaded = false;
}

MBC2::~MBC2()
{
}

uint8_t MBC2::read(uint16_t address){
	if (!ifCartLoaded && !cartridge->ifLoaded) {
		std::cout << "The cartridge isn't loaded" << "\n";
		return 0xFF;
	}
	if (address >= 0x0000 && address <= 0x3FFF) {
		return cartridge->read(address, bootBankNum);
	}
	if (address >= 0x4000 && address <= 0x7FFF) {
		if (romBankNum == 0) {
			romBankNum = 1;
		}
		uint16_t offset = address - 0x4000;
		return cartridge->read(offset, romBankNum);
	}
	if (address >= 0xA000 && address <= 0xBFFF) {
		uint16_t offset = address & 0x1FF;
		uint8_t data = cartridge->readRam(offset, ramBankNum);
		return data & 0x0F;
	}
	

	return 0xFF;
}

void MBC2::write(uint16_t address, uint8_t data){
	if (!ifCartLoaded && !cartridge->ifLoaded) {
		std::cout << "The cartridge isn't loaded" << "\n";
		return;
	}
	if (address >= 0x0000 && address <= 0x3FFF) {
		if (address & 0x0100) {
			ifRam = (data == 0xA);
		}
		else {
			romBankNum = data & 0x1F;
		}
	}
	if (address >= 0xA000 && address <= 0xBFFF) {
		if (ifRam) {
			uint16_t offset = address & 0x1FF;  
			cartridge->write(offset, data & 0x0F,ramBankNum);  
		}
	}
}

void MBC2::loadCartridge(Cartridge* cart){
	ifCartLoaded = true;
	cartridge = cart;
	
}
