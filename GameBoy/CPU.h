#pragma once
#include <stdint.h>

class CPU {
public:
	CPU();
	~CPU();
	uint8_t cpuRead(uint16_t address);
	void cpuWrite(uint16_t address, uint8_t data);
	void init();
	void testInstuctions();
private:
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
	void InstructionSet();
	int cycleCount = 0;
	void LoadN8(uint8_t &registerVal,int count);
	void Add16(uint16_t &registerVal);
	void INC16(uint16_t& registerVal);
	void DEC16(uint16_t& registerVal);
	void INC8(uint8_t& registerVal);
	void DEC8(uint8_t& registerVal);
	void LoadR8R8(uint8_t& registerValLeft, uint8_t& registerValRight);
	void LoadAddressR8(uint8_t& registerVal);
	void LoadR8Address(uint8_t& registerVal);
	void AddR8R8(uint8_t& registerVal);
	void abcR8R8(uint8_t& registerVal);
	void subR8(uint8_t& registerVal);
	void sbcR8R8(uint8_t& registerVal);
	void andR8(uint8_t& registerVal);
	void xorR8(uint8_t& registerVal);
	void orR8(uint8_t& registerVal);
	void cpR8(uint8_t& registerVal);
	void retCC(bool cc);
	void popR16(uint8_t& upperBit, uint8_t& lowerBit, uint16_t& registerVal);
	void jumpCC(bool cc);
	void callCC(bool cc);
	void rstVec(uint8_t vec);
	void reti();
	void pushR16(uint8_t& upperBit, uint8_t& lowerBit, uint16_t& registerVal);
};