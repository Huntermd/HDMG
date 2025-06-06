#include "MBC1.h"

MBC1::MBC1(Cartridge* cart, bool ram, bool battery){
	ifCartLoaded = true;
	cartridge = cart;
	
	ifRam = ram;
	ifBattery = battery;
	
}

MBC1::MBC1(){
	ifCartLoaded = false;
	
}

MBC1::~MBC1(){
	
}

uint8_t MBC1::read(uint16_t address){
	if (!ifCartLoaded ) {
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
	if (address >= 0xA000 && address <= 0xBFFF && ifRam) {
		uint16_t offset = address - 0xA000;
		return cartridge->readRam(offset, ramBankNum);
	}
	return 0xFF;
}

void MBC1::write(uint16_t address, uint8_t data){
	if (!ifCartLoaded) {
		std::cout << "The cartridge isn't loaded" << "\n";
		return;
	}
	if (address >= 0x0000 && address <= 0x1FFF) {
		ifRam = (data == 0xA);
	}

	if (address >= 0x2000 && address <= 0x3FFF) {
		romBankNum = data & 0x1F;
		if (romBankNum == 0x00) {
			romBankNum = 0x1;
		}
		
		if (romBankNum == 0x00 && !mode1) {
			romBankNum = 0x10;
		}
		
		
	}

	if (address >= 0x4000 && address <= 0x5FFF) {
		if (mode1) {
			ramBankNum = (data & 0x03);
		}
		else {
			ramBankNum = 0x00;
		}
		

	}
	if (address >= 0xA000 && address <= 0xBFFF && ifRam) {
		uint16_t offset = address - 0xA000; // Calculate offset for RAM bank
		cartridge->write(offset,data,ramBankNum); // Write data to selected RAM bank
	}
}

void MBC1::loadCartridge(Cartridge* cart){
	ifCartLoaded = true;
	cartridge = cart;
	
}
