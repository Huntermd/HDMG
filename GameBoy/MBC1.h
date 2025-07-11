#pragma once
#include "Mapper.h"
#include "Cartridge.h"

class MBC1 {
public:
	MBC1(Cartridge* cart,bool ram, bool battery);
	MBC1();
	~MBC1();
	uint8_t read(uint16_t address);
	void write(uint16_t address, uint8_t data);
	void loadCartridge(Cartridge* cart);
private:
	int bootBankNum = 0;
	int romBankNum = 1;
	int ramBankNum = 0;
	bool ifRam = false;
	bool ifBattery = false;
	Cartridge* cartridge;
	bool ifCartLoaded;
	bool mode1 = false;
	uint8_t upperRegister;
	uint8_t lowerRegister;
};
