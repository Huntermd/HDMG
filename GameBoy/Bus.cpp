#include "Bus.h"

Bus::Bus()
{
	busInit();
}

Bus::~Bus()
{
}

uint8_t Bus::read(uint16_t address){
	uint8_t data = internalRead(address);
	mCycle();
	return data;
}

void Bus::write(uint16_t address, uint8_t data){
	internalWrite(address, data);
	mCycle();
	
}

void Bus::loadComponets(Mapper* m, PPU* p){
	this->map = m;
	this->ppu = p;
}

void Bus::busInit(){
	for (int i = 0; i < 8192; i++) {
		wram[i] = 0;

	}
	for (int i = 0; i < 127; i++) {
		hram[i] = 0;

	}
	for (int i = 0; i < 0x80; i++) {
		ioRegisters[i] = 0;
	}
	iF = 0xE1;
	ie = 0;
}

uint8_t Bus::internalRead(uint16_t address){
	if (address >= 0x0000 && address <= 0x3FFF && !dmaActive) {
		uint8_t data = map->read(address);
		return data;
	}
	if (address >= 0x4000 && address <= 0x7FFF && !dmaActive) {
		uint8_t data = map->read(address);
		return data;
	}
	if (address >= 0x8000 && address <= 0x9FFF && !dmaActive) {
		return ppu->readVRAM(address);
	}
	if (address >= 0xA000 && address <= 0xBFFF && !dmaActive) {
		return map->read(address);
	}
	if (address >= 0xC000 && address <= 0xDFFF && !dmaActive) {
		return wram[address - 0xC000];
	}
	if (address >= 0xFE00 && address <= 0xFE9F && !dmaActive) {
		return ppu->readOAM(address);
	}
	if (address == 0xFF00 && !dmaActive) {
		return ppu->ppuRead(address);
	}
	if (address == 0xFF0F && !dmaActive) {
		return iF;

	}
	if (address == 0xFF04 && !dmaActive) {
		return (sysClk >> 6) & 0xFF;
	}
	if (address == 0xFF05 && !dmaActive) {
		return tima;
	}
	if (address == 0xFF06 && !dmaActive) {
		return tma;
	}
	if (address == 0xFF07 && !dmaActive) {
		return tac;
	}
	if (address >= 0xFF40 && address <= 0xFF4B && !dmaActive) {
		return ppu->ppuRead(address);
	}
	if (address >= 0xFF00 && address <= 0xFF7F && !dmaActive) {
		return ioRegisters[address - 0xFF00];
	}

	if (address >= 0xFF80 && address <= 0xFFFE) {
		uint8_t data = hram[address - 0xFF80];

		return data;
	}
	if (address == 0xFFFF && !dmaActive) {
		return ie;
	}
	return 0xFF;
}

void Bus::internalWrite(uint16_t address, uint8_t data)
{
	if (address >= 0x0000 && address <= 0x3FFF && !dmaActive) {
		map->write(address, data);
	}
	if (address >= 0x4000 && address <= 0x7FFF && !dmaActive) {
		map->write(address, data);
	}
	if (address >= 0x8000 && address <= 0x9FFF && !dmaActive) {
		ppu->writeVRAM(address, data);
	}
	if (address >= 0xA000 && address <= 0xBFFF && !dmaActive) {
		map->write(address, data);
	}
	if (address >= 0xC000 && address <= 0xDFFF && !dmaActive) {
		wram[address - 0xC000] = data;
	}
	if (address >= 0xFE00 && address <= 0xFE9F && !dmaActive) {

		ppu->writeOAM(address, data);
	}
	if (address == 0xFF00 && !dmaActive) {
		ppu->ppuWrite(address, data, iF);
		return;
	}
	if (address == 0xFF0F && !dmaActive) {
		iF = data;
		return;

	}
	if (address == 0xFF04 && !dmaActive) {
		sysClkChange(0);

		return;
	}
	if (address == 0xFF05 && !dmaActive) {
		if (!timaReloadCycle) tima = data;
		if (cyclesTilTimaIRQ == 1) cyclesTilTimaIRQ = 0;
		return;
	}
	if (address == 0xFF06 && !dmaActive) {
		if (timaReloadCycle) tima = data;
		tma = data;
		return;
	}
	if (address == 0xFF07 && !dmaActive) {
		uint8_t myLastBit = lastBit;
		lastBit &= ((data & 4) >> 2);
		detectEdge(myLastBit, lastBit);
		tac = data;
		return;
	}
	if (address >= 0xFF40 && address <= 0xFF4B && !dmaActive) {
		if (address == 0xFF46) {
			dmaActive = true;
			sourceBase = data << 8;
			dmaIndex = 0;
			dmaDelay = 2;
		}
		ppu->ppuWrite(address, data, iF);
		return;
	}
	if (address >= 0xFF00 && address <= 0xFF7F && !dmaActive) {


		ioRegisters[address - 0xFF00] = data;

		if (ioRegisters[2] == 0x81) {
			char c = ioRegisters[1];
			std::cout << c;
			ioRegisters[2] = 0x0;
		}

	}

	if (address >= 0xFF80 && address <= 0xFFFE) {

		hram[address - 0xFF80] = data;
	}
	if (address == 0xFFFF) {
		ie = data;
	}
}


void Bus::incState(){
	timaReloadCycle = false;
	if (cyclesTilTimaIRQ > 0) {
		cyclesTilTimaIRQ--;
		if (cyclesTilTimaIRQ == 0) {
			iF |= 0x04;
			tima = tma;
			timaReloadCycle = true;
		}
	}
	sysClkChange((sysClk + 1) & 0xFFFF);
}

void Bus::sysClkChange(uint16_t new_value){
	sysClk = new_value;
	uint8_t this_bit;
	switch (tac & 3) {
	case 0: {
		this_bit = (sysClk >> 7) & 1;
		break;
	}
	case 3: {
		this_bit = (sysClk >> 5) & 1;
		break;
	}
	case 2: {
		this_bit = (sysClk >> 3) & 1;
		break;
	}
	case 1: {
		this_bit = (sysClk >> 1) & 1;
		break;
	}
	}
	this_bit &= ((tac & 4) >> 2);
	detectEdge(lastBit, this_bit);
	lastBit = this_bit;
}

void Bus::detectEdge(uint8_t before, uint8_t after){
	if ((before == 1) && (after == 0)) {
		tima = (tima + 1) & 0xFF;
		if (tima == 0) {
			cyclesTilTimaIRQ = 1;
		}
	}
}

void Bus::mCycle(){
	ppu->checkEvents(isRunning, iF);
	dma();
	incState();
	ppu->updatePPU(iF);
}

uint8_t Bus::readByte(uint16_t address){
	if (address >= 0x0000 && address <= 0x3FFF) {
		uint8_t data = map->read(address);
		return data;
	}
	if (address >= 0x4000 && address <= 0x7FFF) {
		uint8_t data = map->read(address);
		return data;
	}
	if (address >= 0x8000 && address <= 0x9FFF) {
		return ppu->readVRAM(address);
	}
	if (address >= 0xA000 && address <= 0xBFFF) {
		return map->read(address);
	}
	if (address >= 0xC000 && address <= 0xDFFF) {
		return wram[address - 0xC000];
	}





	return 0xFF;
}

void Bus::dma(){
	if (dmaActive) {
		if (dmaDelay > 0) {
			dmaDelay--;
		}
		else {
			byte = readByte(sourceBase++);
			ppu->writeOAM(0xFE00 + dmaIndex, byte);
			dmaIndex++;
			if (dmaIndex == 160) {
				dmaActive = false;
			}
		}

	}
}
