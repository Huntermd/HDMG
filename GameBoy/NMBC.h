#pragma once
#include "Mapper.h"
#include "Cartridge.h"
#include <iostream>
class NMBC {
public:
	NMBC(Cartridge* cart);
	NMBC();
	~NMBC();
	uint8_t read(uint16_t address) ;
	void write(uint16_t address, uint8_t data) ;
	void loadCartridge(Cartridge* cart);

private:
	Cartridge* cartridge;
	bool ifCartLoaded;


};
