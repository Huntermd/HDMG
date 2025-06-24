#pragma once
#include <iostream>
#include <fstream>
#include "Mapper.h"
#include "PPU.h"

class GameBoy {
	Mapper* map;
	PPU* ppu;
public:
	
	GameBoy();
	~GameBoy();
	
	void testRom();
	void loadComponets(Mapper* m, PPU* p);
	
	
	
	bool isRunning = true;
	
	
	
	
	
private:
	bool ifHalt = false;
	
	inline void getOpcode();	
	uint8_t cpuRead(uint16_t address);
	void cpuWrite(uint16_t address, uint8_t data);
	void tCycle(int cycleCount);
	void init();
	void cpuInit();
	uint8_t wram[8192];
	uint8_t hram[127];
	uint16_t AF;
	uint8_t a;
	//Lower 8 bits of AF
	uint8_t zeroFlag7;
	uint8_t subtractionFlag6;
	uint8_t halfCarryFlag5;
	uint8_t carryFlag4;
	//
	uint16_t BC;
	uint8_t b;
	uint8_t c;
	uint16_t DE;
	uint8_t d;
	uint8_t e;
	uint16_t HL;
	uint8_t h;
	uint8_t l;
	uint16_t stackPointer;
	uint16_t programCounter;
	uint8_t Opcode;
	uint8_t seperateByteCB;
	void InstructionSet();
	void cbInstructionSet();
	inline void LoadN8(uint8_t& registerVal, int count);
	inline void Add16(uint16_t& registerVal);
	inline void INC16(uint16_t& registerVal, uint8_t& upperBit, uint8_t& lowerBit);
	inline void DEC16(uint16_t& registerVal, uint8_t& upperBit, uint8_t& lowerBit);
	inline void INC8(uint8_t& registerVal);
	inline void DEC8(uint8_t& registerVal);
	inline void LoadR8R8(uint8_t& registerValLeft, uint8_t& registerValRight);
	inline void LoadAddressR8(uint8_t& registerVal);
	inline void LoadR8Address(uint8_t& registerVal);
	inline void AddR8R8(uint8_t& registerVal);
	inline void abcR8R8(uint8_t& registerVal);
	inline void subR8(uint8_t& registerVal);
	inline void sbcR8R8(uint8_t& registerVal);
	inline void andR8(uint8_t& registerVal);
	inline void xorR8(uint8_t& registerVal);
	inline void orR8(uint8_t& registerVal);
	inline void cpR8(uint8_t& registerVal);
	inline void retCC(bool cc);
	inline void popR16(uint8_t& upperBit, uint8_t& lowerBit, uint16_t& registerVal);
	inline void jumpCC(bool cc);
	inline void callCC(bool cc);
	inline void rstVec(uint8_t vec);
	inline void reti();
	inline void pushR16(uint8_t& upperBit, uint8_t& lowerBit, uint16_t& registerVal);
	inline void rlc(uint8_t& r8);
	inline void rrc(uint8_t& r8);
	inline void rl(uint8_t& r8);
	inline void rr(uint8_t& r8);
	inline void sla(uint8_t& r8);
	inline void sra(uint8_t& r8);
	inline void swap(uint8_t& r8);
	inline void srl(uint8_t& r8);
	inline void bit(uint8_t u3, uint8_t& r8);
	inline void res(uint8_t u3, uint8_t& r8);
	inline void set(uint8_t u3, uint8_t& r8);
	//Interruptions
	bool ifEi = false;
	
	uint8_t ime = 0;
	uint8_t ie = 0;
	uint8_t iF = 0;
	inline void jumpInterruptions(uint8_t address);
	void handleInterruptions();

	//I/O
	uint8_t ioRegisters[0x80];
	//Timer
	int timeOverall= 4194304;
	int clockSelection = 0;
	uint16_t bitSelected = 0;
	void updateTimer(int num);
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
	inline void incState();
	void sysClkChange(uint16_t new_value);
	inline void detectEdge(uint8_t before, uint8_t after);
	inline void mCycle();
	//DMA
	bool dmaActive = false;
	uint16_t sourceBase = 0;
	uint8_t dmaIndex = 0;
	uint8_t dmaDelay = 2;
	uint8_t readByte(uint16_t address);
	inline void dma();
	uint8_t byte = 0x00;

};
