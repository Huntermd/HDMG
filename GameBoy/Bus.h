#pragma once
#include <stdint.h>
#include "Mapper.h"
#include "PPU.h"
class Bus{
	
	Mapper* map;
	PPU* ppu;
public:
	Bus();
	~Bus();
	uint8_t read(uint16_t address);
	void write(uint16_t address, uint8_t data);
	void loadComponets(Mapper* m, PPU* p);
	uint8_t iF;
	uint8_t ie;
	bool busLoaded = true;
	bool isRunning = true;
	void busInit();
	void mCycle();
private:
	uint8_t internalRead(uint16_t address);
	void internalWrite(uint16_t address, uint8_t data);
	uint8_t wram[8192];
	uint8_t hram[127];
	uint8_t ioRegisters[0x80];
	//Timer
	int timeOverall = 4194304;
	int clockSelection = 0;
	uint16_t bitSelected = 0;
	
	int cycleCount = 0;
	uint16_t divCounter = 0xAC00;
	uint8_t tima = 0;
	uint8_t tma = 0;
	uint8_t tac = 0xF8;
	uint8_t sc = 0x7E;
	uint8_t lastBit = 0;
	bool timaReloadCycle = false;
	uint16_t sysClk = 0;
	uint8_t cyclesTilTimaIRQ = 0;
	bool raiseIRQ = false;
	uint16_t prevDCounter = 0;
	void incState();
	void sysClkChange(uint16_t new_value);
	void detectEdge(uint8_t before, uint8_t after);
	
	//DMA
	bool dmaActive = false;
	uint16_t sourceBase = 0;
	uint8_t dmaIndex = 0;
	uint8_t dmaDelay = 2;
	uint8_t readByte(uint16_t address);
	void dma();
	uint8_t byte = 0x00;

};
