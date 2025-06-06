#include "CPU.h"

CPU::CPU(){
	
}

CPU::~CPU(){
}

uint8_t CPU::cpuRead(uint16_t address)
{
	return wram[address];
}

void CPU::cpuWrite(uint16_t address, uint8_t data)
{
	wram[address] = data;
}

void CPU::init(){
	for (int i = 0; i < 8192; i++){
		wram[i] = 0;

	}
	for (int i = 0; i < 127; i++) {
		hram[i] = 0;

	}
}

void CPU::testInstuctions(){

}

void CPU::InstructionSet(){
	/*
	n8 - means immediate 8-bit data
	n16- means immediate little-endian 16-bit data
	a8-means 8-bit unsigned data, which is added to $FF00 in certain instructions to create a 16-bit address in HRAM (High RAM)
	a16-means little-endian 16-bit address
	e8-means 8-bit signed data
	r8-Any of the 8-bit registers
	r16-Any of the 16-bit registers
	[] means memory address
	*/
	switch (Opcode) {
	case 0x01: {// LD BC, n16
		c = map->read(programCounter++);
		b = map->read(programCounter++);
		BC = (b << 8) | c;
		cycleCount += 12;
		break;
	}
	case 0x02: {// LD [BC], a
		map->write(BC, a);
		cycleCount += 8;
		break;
	}
	case 0x03: {// inc BC
		INC16(BC);
		break;
	}
	case 0x04: {//INC B
		uint8_t tempB = b;
		b += 1;
		zeroFlag7 = (b == 0);
		subtractionFlag6 = 0;
		halfCarryFlag5 = ((tempB & 0x0F) + 1) > 0x0F;
		cycleCount += 4;
		break;
	}
	case 0x05: {//DEC b
		uint8_t tempB = b;
		b -= 1;
		zeroFlag7 = (b == 0);
		subtractionFlag6 = 1;
		halfCarryFlag5 = (tempB & 0x0F) == 0x00;
		cycleCount += 4;
		break;
	}
	case 0x06: {//LD b, n8
		LoadN8(b, 8);
		break;
	}
	case 0x07: { //RLCA
		carryFlag4 = (a & 0x80) >> 7;
		a <<= 1;
		a |= carryFlag4;
		zeroFlag7 = 0;
		halfCarryFlag5 = 0;
		subtractionFlag6 = 0;
		cycleCount += 4;
		break;
	}
	case 0x08: {//LD[a16],SP
		uint8_t lowerBit = map->read(programCounter++);
		uint8_t upperBit = map->read(programCounter++);
		uint16_t a16 = (upperBit << 8) | lowerBit;
		uint8_t lowerSp = (stackPointer & 0x00FF);
		uint8_t upperSp = (stackPointer >> 8);
		map->write(a16, lowerSp);
		map->write(a16 + 1, upperSp);
		cycleCount += 20;
		break;
	}
	case 0x09: {
		Add16(BC);
		break;
	}
	case 0x0A: {
		a = map->read(BC);
		cycleCount += 8;
		break;
	}
	case 0x0B: {
		DEC16(BC);
		break;
	}
	case 0x0C: {
		INC8(c);
		break;
	}
	case 0x0D: {
		DEC8(c);
		break;
	}
	case 0x0E: {
		LoadN8(c, 8);
		break;
	}
	case 0x0F: {
		carryFlag4 = (a & 0x01);
		a >>= 1;
		a |= (carryFlag4 << 7);
		zeroFlag7 = 0;
		halfCarryFlag5 = 0;
		subtractionFlag6 = 0;
		cycleCount += 4;
		break;
	}
	case 0x10: {// stop 0
		break;
	}
	case 0x11: {//LD DE, 
		e = map->read(programCounter++);
		d = map->read(programCounter++);
		DE = (d << 8) | e;
		cycleCount += 12;
		break;
	}
	case 0x12: {// LD A, [DE]
		map->write(DE, a);
		cycleCount += 8;
		break;
	}
	case 0x13: {// INC DE
		INC16(DE);
		break;
	}
	case 0x14: {//INC D
		uint8_t tempD = d;
		d += 1;
		zeroFlag7 = (d == 0);
		subtractionFlag6 = 0;
		halfCarryFlag5 = (tempD & 0x0F) == 0;
		cycleCount += 4;
		break;
	}
	case 0x15: {//DEC D
		uint8_t tempD = d;
		d -= 1;
		zeroFlag7 = (d == 0);
		subtractionFlag6 = 1;
		halfCarryFlag5 = (tempD & 0x0F) == 0x00;
		cycleCount += 4;
		break;
	}
	case 0x16: {//LD D, n8
		LoadN8(d, 8);
		break;
	}
	case 0x17: {//RRCA
		carryFlag4 = (a & 0x01);
		a >>= 1;
		a |= (carryFlag4 << 7);
		zeroFlag7 = 0;
		halfCarryFlag5 = 0;
		subtractionFlag6 = 0;
		cycleCount += 4;
		break;
	}
	case 0x18: {
		uint8_t e8 = map->read(programCounter + 1);
		programCounter = programCounter + e8 + 1;
		cycleCount += 12;
		break;
	}
	case 0x19: {
		Add16(DE);
		break;
	}
	case 0x1A: {
		map->write(DE, a);
		cycleCount += 8;
		break;
	}
	case 0x1B: {
		DEC16(DE);
		break;
	}
	case 0x1C: {
		INC8(e);
		break;
	}
	case 0x1D: {
		DEC8(e);
		break;
	}
	case 0x1E: {
		LoadN8(e, 8);
		break;
	}
	case 0x1F: {
		uint8_t oldCarry = carryFlag4;
		carryFlag4 = a & 0x01;
		a |= (oldCarry << 7);

		zeroFlag7 = 0;
		subtractionFlag6 = 0;
		halfCarryFlag5 = 0;
		break;
	}
	case 0x20: {
		uint8_t e8 = map->read(programCounter + 1);
		if (zeroFlag7 == 0) {
			programCounter = programCounter + e8 + 1;
			cycleCount += 12;
			break;
		}
		else {
			cycleCount += 8;
			break;
		}
	}
	case 0x21: {// LD HL, n16
		l = map->read(programCounter++);
		h = map->read(programCounter++);
		HL = (h << 8) | l;
		cycleCount += 12;
		break;
	}
	case 0x22: {// LD [HL+], A
		map->write(HL, a);
		HL += 1;
		break;
	}
	case 0x23: {//INC HL
		INC16(HL);
		break;
	}
	case 0x24: {// INC H
		uint8_t tempH = h;
		h += 1;
		zeroFlag7 = (h == 0);
		subtractionFlag6 = 0;
		halfCarryFlag5 = (tempH & 0x0F) == 0;
		cycleCount += 4;
		break;
	}
	case 0x25: {//Dec H
		uint8_t tempH = h;
		h -= 1;
		zeroFlag7 = (h == 0);
		subtractionFlag6 = 1;
		halfCarryFlag5 = ((tempH & 0x0F) - 1) == 0x00;
		cycleCount += 4;
		break;
	}
	case 0x26: {//Ld H, n8
		LoadN8(h, 8);
		break;
	}
	case 0x27: {//RLA
		//uint8_t oldCarry = carryFlag4;
		//carryFlag4 = (a & 0x80) >> 7;
		//a |= oldCarry;

		//zeroFlag7 = 0;
		//subtractionFlag6 = 0;
		//halfCarryFlag5 = 0;
		if (!subtractionFlag6) {
			if ((a & 0xF) > 0x09 || halfCarryFlag5) {
				a += 6;
			}
			if (a > 0x99 || carryFlag4) {
				a += 0x60;
				carryFlag4 = 1;
			}
		}
		else {
			if (halfCarryFlag5) {
				a -= 0x06;
			}
			if (carryFlag4) {
				a -= 0x60;
			}
		}
		zeroFlag7 = (a == 0);
		halfCarryFlag5 = 0;
		cycleCount += 4;
		break;
	}
	case 0x28: {
		uint8_t e8 = map->read(programCounter + 1);
		if (zeroFlag7 == 1) {
			programCounter = programCounter + e8 + 1;
			cycleCount += 12;
			break;
		}
		else {
			cycleCount += 8;
			break;
		}

	}
	case 0x29: {
		Add16(HL);
		break;
	}
	case 0x2A: {
		map->write(HL, a);
		HL += 1;
		cycleCount += 8;
		break;
	}
	case 0x2B: {
		DEC16(HL);
		break;
	}
	case 0x2C: {
		INC8(l);
		break;
	}
	case 0x2D: {
		DEC8(l);
		break;
	}
	case 0x2E: {
		LoadN8(l, 8);
		break;
	}
	case 0x2F: {
		a = ~a;
		cycleCount += 4;
		subtractionFlag6 = 1;
		halfCarryFlag5 = 1;
		break;
	}
	case 0x30: {//JR NC, e8
		uint8_t e8 = map->read(programCounter + 1);
		if (carryFlag4 == 0) {
			programCounter = programCounter + e8 + 1;
			cycleCount += 12;
			break;
		}
		else {
			cycleCount += 8;
			break;
		}
	}
	case 0x31: {// ADD SP, DE
		uint8_t lowerBit = map->read(programCounter++);
		uint8_t upperBit = map->read(programCounter++);
		stackPointer = (upperBit << 8) | lowerBit;
		cycleCount += 12;
		break;
	}
	case 0x32: {//LD a, [DE]
		map->write(HL, a);
		HL -= 1;
		break;
	}
	case 0x33: {//DEC DE
		INC16(stackPointer);
		break;
	}
	case 0x34: {//INC SP
		uint8_t value = map->read(HL);
		uint8_t result = value + 1;
		map->write(HL, result);
		zeroFlag7 = (result == 0);
		subtractionFlag6 = 0;
		halfCarryFlag5 = ((value & 0x0F) + 1) > 0x0F;
		cycleCount += 12;
		break;
	}
	case 0x35: {//INC (HL)
		uint8_t value = map->read(HL);
		uint8_t result = value - 1;
		map->write(HL, result);
		zeroFlag7 = (result == 0);
		subtractionFlag6 = 1;
		halfCarryFlag5 = ((value & 0x0F) - 1) == 0x00;
		cycleCount += 12;
		break;
	}
	case 0x36: {//DEC (HL)
		uint8_t e8 = map->read(programCounter + 1);
		map->write(HL, e8);
		cycleCount += 12;
		break;
	}
	case 0x37: {//LD (HL), e8
		//uint8_t oldCarry = carryFlag4;
		//carryFlag4 = a & 0x01;
		//a |= (oldCarry<<7);

		//zeroFlag7 = 0;
		//subtractionFlag6 = 0;
		//halfCarryFlag5 = 0;
		//SCF
		carryFlag4 = 1;
		subtractionFlag6 = 0;
		halfCarryFlag5 = 0;
		cycleCount += 4;
		break;
	}
	case 0x38: {
		uint8_t e8 = map->read(programCounter + 1);
		if (carryFlag4 == 1) {
			programCounter = programCounter + e8 + 1;
			cycleCount += 12;
			break;
		}
		else {
			cycleCount += 8;

			break;
		}
		break;
	}
	case 0x39: {
		Add16(stackPointer);
		break;
	}
	case 0x3A: {
		a = map->read(HL);
		HL -= 1;
		cycleCount += 8;
		break;
	}
	case 0x3B: {
		DEC16(stackPointer);
		break;
	}
	case 0x3C: {
		INC8(a);
		break;
	}
	case 0x3D: {
		DEC8(a);
		break;
	}
	case 0x3E: {
		LoadN8(a, 8);
		break;
	}
	case 0x3F: {
		carryFlag4 = !carryFlag4;
		subtractionFlag6 = 0;
		halfCarryFlag5 = 0;
		cycleCount += 4;
		break;
	}
	case 0x40: {//
		LoadR8R8(b, b);
		break;
	}
	case 0x41: {//LD HL,n16
		LoadR8R8(b, c);
		break;
	}
	case 0x42: {//LD [HL+(I)] , a
		LoadR8R8(b, d);
		break;
	}
	case 0x43: {//INC HL
		LoadR8R8(b, e);
		break;
	}
	case 0x44: {//INC H
		LoadR8R8(b, h);
		break;
	}
	case 0x45: {//DEC H
		LoadR8R8(b, l);
		break;
	}
	case 0x46: {
		LoadR8Address(b);
		break;
	}
	case 0x47: {
		LoadR8R8(b, a);
		break;
	}
	case 0x48: {
		LoadR8R8(c, b);
		break;
	}
	case 0x49: {
		LoadR8R8(c, c);
		break;
	}
	case 0x4A: {
		LoadR8R8(c, d);
		break;
	}
	case 0x4B: {
		LoadR8R8(c, e);
		break;
	}
	case 0x4C: {
		LoadR8R8(c, h);
		break;
	}
	case 0x4D: {
		LoadR8R8(c, l);
		break;
	}
	case 0x4E: {
		LoadR8Address(c);
		break;
	}
	case 0x4F: {
		LoadR8R8(c, a);
		break;
	}
	case 0x50: {//JR Z, e8
		LoadR8R8(d, b);
		break;
	}
	case 0x51: {
		LoadR8R8(d, c);
		break;
	}
	case 0x52: {
		LoadR8R8(d, d);
		break;
	}
	case 0x53: {
		LoadR8R8(d, e);
		break;
	}
	case 0x54: {
		LoadR8R8(d, h);
		break;
	}
	case 0x55: {
		LoadR8R8(d, l);
		break;
	}
	case 0x56: {
		LoadR8Address(d);
		break;
	}
	case 0x57: {
		LoadR8R8(d, a);
		break;
	}
	case 0x58: {
		LoadR8R8(e, b);
		break;
	}
	case 0x59: {
		LoadR8R8(e, c);
		break;
	}
	case 0x5A: {
		LoadR8R8(e, d);
		break;
	}
	case 0x5B: {
		LoadR8R8(e, e);
		break;
	}
	case 0x5C: {
		LoadR8R8(e, h);
		break;
	}
	case 0x5D: {
		LoadR8R8(e, l);
		break;
	}
	case 0x5E: {
		LoadR8Address(e);
		break;
	}
	case 0x5F: {
		LoadR8R8(e, a);
		break;
	}
	case 0x60: {
		LoadR8R8(h, b);
		break;
	}
	case 0x61: {
		LoadR8R8(h, c);

		break;
	}
	case 0x62: {
		LoadR8R8(h, d);
		break;
	}
	case 0x63: {
		LoadR8R8(h, e);
		break;
	}
	case 0x64: {
		LoadR8R8(h, h);
		break;
	}
	case 0x65: {
		LoadR8R8(h, l);
		break;
	case 0x66: {
		LoadR8Address(h);
		break;
	}
	case 0x67: {
		LoadR8R8(h, a);
		break;
	}
	case 0x68: {
		LoadR8R8(l, b);
		break;
	}
	case 0x69: {
		LoadR8R8(l, c);
		break;
	}
	case 0x6A: {
		LoadR8R8(l, d);
		break;
	}
	case 0x6B: {
		LoadR8R8(l, e);
		break;
	}
	case 0x6C: {
		LoadR8R8(l, h);
		break;
	}
	case 0x6D: {
		LoadR8R8(l, l);
		break;
	}
	case 0x6E: {
		LoadR8Address(l);
		break;
	}
	case 0x6F: {
		LoadR8R8(l, a);
		break;
	}
	case 0x70: {
		LoadAddressR8(b);
		break;
	}
	case 0x71: {
		LoadAddressR8(c);
		break;
	}
	case 0x72: {
		LoadAddressR8(d);
		break;
	}
	case 0x73: {
		LoadAddressR8(e);
		break;
	}
	case 0x74: {
		LoadAddressR8(h);
		break;
	}
	case 0x75: {
		LoadAddressR8(l);
		break;
	}
	case 0x76: {
		//halt
		break;
	}
	case 0x77: {
		LoadAddressR8(a);
		break;
	}
	case 0x78: {
		LoadR8R8(a, b);
		break;
	}
	case 0x79: {
		LoadR8R8(a, c);
		break;
	}
	case 0x7A: {
		LoadR8R8(a, d);
		break;
	}
	case 0x7B: {
		LoadR8R8(a, e);
		break;
	}
	case 0x7C: {
		LoadR8R8(a, h);
		break;
	}
	case 0x7D: {
		LoadR8R8(a, l);
		break;
	}
	case 0x7E: {
		LoadR8Address(a);
		break;
	}
	case 0x7F: {
		LoadR8R8(a, a);
		break;
	}
	case 0x80: {
		AddR8R8(b);
		break;
	}
	case 0x81: {
		AddR8R8(c);
		break;
	}
	case 0x82: {
		AddR8R8(d);
		break;
	}
	case 0x83: {
		AddR8R8(e);
		break;
	}
	case 0x84: {
		AddR8R8(h);
		break;
	}
	case 0x85: {
		AddR8R8(l);
		break;
	}
	case 0x86: {
		uint8_t temp = map->read(HL);
		AddR8R8(temp);
		cycleCount += 4;
		break;
	}
	case 0x87: {
		AddR8R8(a);
		break;
	}
	case 0x88: {
		abcR8R8(b);
		break;
	}
	case 0x89: {
		abcR8R8(c);
		break;
	}
	case 0x8A: {
		abcR8R8(d);
		break;
	}
	case 0x8B: {
		abcR8R8(e);
		break;
	}
	case 0x8C: {
		abcR8R8(h);
		break;
	}
	case 0x8D: {
		abcR8R8(l);
		break;
	}
	case 0x8E: {
		uint8_t temp = map->read(HL);
		abcR8R8(temp);
		cycleCount += 4;
		break;
	}
	case 0x8F: {
		abcR8R8(a);
		break;
	}
	case 0x90: {
		subR8(b);
		break;
	}
	case 0x91: {
		subR8(c);
		break;
	}
	case 0x92: {
		subR8(d);
		break;
	}
	case 0x93: {
		subR8(e);
		break;
	}
	case 0x94: {
		subR8(h);
		break;
	}
	case 0x95: {
		subR8(l);
		break;
	}
	case 0x96: {
		uint8_t temp = map->read(HL);
		subR8(temp);
		cycleCount += 4;
		break;
	}
	case 0x97: {
		subR8(a);
		break;
	}
	case 0x98: {
		sbcR8R8(b);
		break;
	}
	case 0x99: {
		sbcR8R8(c);
		break;
	}
	case 0x9A: {
		sbcR8R8(d);
		break;
	}
	case 0x9B: {
		sbcR8R8(e);
		break;
	}
	case 0x9C: {
		sbcR8R8(h);
		break;
	}
	case 0x9D: {
		sbcR8R8(l);
		break;
	}
	case 0x9E: {
		uint8_t temp = map->read(HL);
		sbcR8R8(temp);
		cycleCount += 4;
		break;
	}
	case 0x9F: {
		sbcR8R8(a);
		break;
	}
	case 0xA0: {
		andR8(b);
		break;
	}
	case 0xA1: {
		andR8(c);
		break;
	}
	case 0xA2: {
		andR8(d);
		break;
	}
	case 0xA3: {
		andR8(e);
		break;
	}
	case 0xA4: {
		andR8(h);
		break;
	}
	case 0xA5: {
		andR8(l);
		break;
	}
	case 0xA6: {
		uint8_t temp = map->read(HL);
		andR8(temp);
		cycleCount += 4;
		break;
	}
	case 0xA7: {
		andR8(a);
		break;
	}
	case 0xA8: {
		xorR8(b);
		break;
	}
	case 0xA9: {
		xorR8(c);
		break;
	}
	case 0xAA: {
		xorR8(d);
		break;
	}
	case 0xAB: {
		xorR8(e);
		break;
	}
	case 0xAC: {
		xorR8(h);
		break;
	}
	case 0xAD: {
		xorR8(l);
		break;
	}
	case 0xAE: {
		uint8_t temp = map->read(HL);
		xorR8(temp);
		cycleCount += 4;
		break;
	}
	case 0xAF: {
		xorR8(a);
		break;
	}
	case 0xB0: {
		orR8(b);
		break;
	}
	case 0xB1: {
		orR8(c);
		break;
	}
	case 0xB2: {
		orR8(d);
		break;
	}
	case 0xB3: {
		orR8(e);
		break;
	}
	case 0xB4: {
		orR8(h);
		break;
	}
	case 0xB5: {
		orR8(l);
		break;
	}
	case 0xB6: {
		uint8_t temp = map->read(HL);
		orR8(temp);
		cycleCount += 4;
		break;
	}
	case 0xB7: {
		orR8(a);
		break;
	}
	case 0xB8: {
		cpR8(b);
		break;
	}
	case 0xB9: {
		cpR8(c);
		break;
	}
	case 0xBA: {
		cpR8(d);
		break;
	}
	case 0xBB: {
		cpR8(e);
		break;
	}
	case 0xBC: {
		cpR8(h);
		break;
	}
	case 0xBD: {
		cpR8(l);
		break;
	}
	case 0xBE: {
		uint8_t temp = map->read(HL);
		cpR8(temp);
		cycleCount += 4;
		break;
	}
	case 0xBF: {
		cpR8(a);
		break;
	}
	case 0xC0: {
		bool cc = (zeroFlag7 == 0);
		retCC(cc);
		break;
	}
	case 0xC1: {
		popR16(b,c,BC);
		break;
	}
	case 0xC2: {
		bool cc = (zeroFlag7 == 0);
		jumpCC(cc);
		break;
	}
	case 0xC3: {
		jumpCC(true);
		break;
	}
	case 0xC4: {
		bool cc = (zeroFlag7 == 0);
		callCC(cc);
		break;
	}
	case 0xC5: {
		pushR16(b, c, BC);
		break;
	}
	case 0xC6: {
		uint8_t n8 = map->read(programCounter++);
		AddR8R8(n8);
		break;
	}
	case 0xC7: {
		rstVec(0x00);
		break;
	}
	case 0xC8: {
		bool cc = (zeroFlag7 == 1);
		retCC(cc);
		break;
	}
	case 0xC9: {
		retCC(true);
		cycleCount -= 4;
		break;
	}
	case 0xCA: {
		bool cc = (zeroFlag7 == 1);
		jumpCC(cc);
		break;
	}
	case 0xCB: {//PREFIX CB
		break;
	}
	case 0xCC: {
		bool cc = (zeroFlag7 == 1);
		callCC(cc);
		break;
	}
	case 0xCD: {
		callCC(true);
		break;
	}
	case 0xCE: {
		uint8_t n8 = map->read(programCounter++);
		abcR8R8(n8);
		cycleCount += 4;
		break;
	}
	case 0xCF: {
		rstVec(0x08);
		break;
	}
	case 0XD0: {
		bool cc = (carryFlag4 == 0);
		retCC(cc);
		break;
	}
	case 0xD1: {
		popR16(d, e, DE);
		break;
	}
	case 0XD2: {
		bool cc = (carryFlag4 == 0);
		jumpCC(cc);
		break;
	}
	case 0xD4: {
		bool cc = (carryFlag4 == 0);
		callCC(cc);
		break;
	}
	case 0xD5: {
		pushR16(d, e, DE);
		break;
	}
	case 0xD6: {
		uint8_t n8 = map->read(programCounter++);
		subR8(n8);
		cycleCount += 4;
		break;
	}
	case 0xD7: {
		rstVec(0x10);
		break;
	}
	case 0xD8: {
		bool cc = (carryFlag4 == 1);
		retCC(cc);
		break;
	}
	case 0xD9: {
		reti();
		break;
	}
	case 0xDA: {
		bool cc = (carryFlag4 == 1);
		jumpCC(cc);
		break;
	}
	case 0xDC: {
		bool cc = (carryFlag4 == 1);
		callCC(cc);
		break;
	}
	case 0xDE: {
		uint8_t n8 = map->read(programCounter++);
		xorR8(n8);
		cycleCount += 4;
		break;
	}
	case 0xDF: {
		rstVec(0x18);
		break;
	}
	case 0xE0: {
		uint8_t n8 = map->read(programCounter++);
		uint16_t address = 0xFF00 + n8;
		map->write(address, a);
		cycleCount += 12;
		break;
	}
	case 0xE1: {
		popR16(h, l, HL);
		break;
	}
	case 0xE2: {
		
		uint16_t address = 0xFF00 + c;
		map->write(address, a);
		cycleCount += 8;
		break;
	}
	case 0xE5: {
		pushR16(h, l, HL);
		break;
	}
	case 0xE6: {
		uint8_t d8 = map->read(programCounter++);
		andR8(d8);
		cycleCount += 4;
		break;
	}
	case 0xE7: {
		rstVec(0x20);
		break;
	}
	case 0xE8: {
		int8_t offset = static_cast<int8_t>(map->read(programCounter++));
		uint16_t tempSP = stackPointer;
		stackPointer += offset;
		zeroFlag7 = 0;
		subtractionFlag6 = 0;
		halfCarryFlag5 = ((tempSP & 0x0F) + (offset & 0x0F)) > 0x0F;
		carryFlag4 = ((tempSP & 0xFF) + (offset & 0xFF)) > 0xFF;
		cycleCount += 16;
		break;
	}
	case 0xE9: {
		programCounter = HL;
		cycleCount += 4;
		break;
	}
	case 0xEA: {
		uint8_t lowerByte = map->read(programCounter++);
		uint8_t upperByte = map->read(programCounter++);
		uint16_t address = (upperByte << 8) | lowerByte;
		map->write(address, a);
		cycleCount += 16;
		break;
	}
	case 0xEE: {
		uint8_t n8 = map->read(programCounter++);
		xorR8(n8);
		cycleCount += 4;
		break;
	}
	case 0xEF: {
		rstVec(0x28);
		cycleCount += 16;
		break;
	}
	case 0xF0: {
		uint8_t n8 = map->read(programCounter++);
		uint16_t address = 0xFF00 + n8;
		a = map->read(address);
		cycleCount += 12;
		break;
	}
	case 0xF1: {
		uint8_t low = map->read(stackPointer++);
		uint8_t high = map->read(stackPointer++);
		a = high;
		zeroFlag7 = (low & 0x80) != 0;
		subtractionFlag6 = (low & 0x40) != 0;
		halfCarryFlag5 = (low & 0x20) != 0;
		carryFlag4 = (low & 0x10) != 0;
		cycleCount += 12;

		break;
	}
	case 0xF2: {
		uint16_t address = 0xFF00 + c;
		a = map->read(address);
		cycleCount += 12;
		break;
	}
	case 0xF3: {
		break;
	}
	case 0xF5: {
		uint8_t flags = 0;
		flags |= (zeroFlag7 << 7);
		flags |= (subtractionFlag6<<6);
		flags |= (halfCarryFlag5<<5);
		flags |= (carryFlag4<<4);

		map->write(--stackPointer, a);
		map->write(--stackPointer, flags);
		cycleCount += 16;
		break;
	}
	case 0xF6: {
		uint8_t d8 = map->read(programCounter++);
		orR8(d8);
		cycleCount += 4;
		break;
	}
	case 0xF7: {
		rstVec(0x30);
		break;
	}
	case 0xF8: {
		int8_t e8 = static_cast<int8_t>(map->read(programCounter++));
		uint16_t result = stackPointer + e8;
		HL = result;
		zeroFlag7 = 0;
		subtractionFlag6 = 0;
		halfCarryFlag5 = ((stackPointer & 0x0F) + (e8 & 0x0F)) > 0x0F;
		carryFlag4 = ((stackPointer & 0xFF) + (e8 & 0xFF)) > 0xFF;
		cycleCount += 12;
		break;
	}
	case 0xF9: {
		stackPointer = HL;
		cycleCount += 8;
		break;
	}
	case 0xFA: {
		uint8_t lowerByte = map->read(programCounter++);
		uint8_t upperByte = map->read(programCounter++);
		uint16_t address = (upperByte << 8) | lowerByte;
		a = map->read(address);
		cycleCount += 16;
		break;
	}
	case 0xFB: {
		break;
	}
	case 0xFE: {
		uint8_t d8 = map->read(programCounter++);
		cpR8(d8);
		cycleCount += 4;
		break;
	}
	case 0xFF: {
		rstVec(0x38);
		break;
	}

			 
	default:
		break;
	}
	}
}

void CPU::LoadN8(uint8_t &registerVal, int count){
	registerVal = map->read(programCounter++);
	cycleCount += count;
}

void CPU::Add16(uint16_t& registerVal){
	uint32_t result = HL + registerVal;
	HL = result & 0xFFFF;
	carryFlag4 = (result > 0xFFFF);
	subtractionFlag6 = 0;
	halfCarryFlag5 = ((HL & 0x0F) + (registerVal & 0x0F)) > 0x0F;
	zeroFlag7 = (HL == 0) ? 1 : 0;
	cycleCount += 8;
}

void CPU::INC16(uint16_t &registerVal){
	registerVal += 1;
	cycleCount += 8;
}

void CPU::DEC16(uint16_t& registerVal){
	registerVal -= 1;
	cycleCount += 8;
}

void CPU::INC8(uint8_t& registerVal){
	uint8_t temp = registerVal;
	registerVal += 1;
	zeroFlag7 = (registerVal == 0);
	subtractionFlag6 = 0;
	halfCarryFlag5 = ((temp & 0x0F) + 1) > 0x0F;
	cycleCount += 4;
}

void CPU::DEC8(uint8_t& registerVal){
	uint8_t temp = registerVal;
	registerVal -= 1;
	zeroFlag7 = (registerVal == 0);
	subtractionFlag6 = 1;
	halfCarryFlag5 = ((temp & 0x0F) - 1) == 0x00;
	cycleCount += 4;
}

void CPU::LoadR8R8(uint8_t& registerValLeft, uint8_t& registerValRight){
	registerValLeft = registerValRight;
	cycleCount += 4;
}

void CPU::LoadAddressR8(uint8_t& registerVal){
	map->(HL, registerVal);
	cycleCount += 8;
}

void CPU::LoadR8Address(uint8_t& registerVal){
	registerVal = map->read(HL);
	cycleCount += 8;
}

void CPU::AddR8R8(uint8_t& registerVal){
	uint16_t result = a + registerVal;
	a = result & 0xFF;
	zeroFlag7 = (a == 0);
	subtractionFlag6 = 0;
	carryFlag4 = (result > 0xFF);
	halfCarryFlag5 = ((a & 0x0F) + (registerVal & 0x0F) + (carryFlag4 ) > 0x0F);
	cycleCount += 4;
}

void CPU::abcR8R8(uint8_t& registerVal){
	uint16_t result = a + registerVal + carryFlag4;

	a = result & 0xFF;
	zeroFlag7 = (a == 0);
	subtractionFlag6 = 1;
	halfCarryFlag5 = ((a & 0x0F) < (registerVal & 0x0F) + carryFlag4);
	carryFlag4 = (registerVal + carryFlag4) > a;
	cycleCount += 4;
}

void CPU::subR8(uint8_t& registerVal){
	carryFlag4 = (registerVal > a);
	uint16_t result = a - registerVal;
	a = result & 0xFF;
	zeroFlag7 = (a == 0);
	subtractionFlag6 = 1;
	
	halfCarryFlag5 = ((a & 0x0F) < (registerVal & 0x0F));
	cycleCount += 4;
}

void CPU::sbcR8R8(uint8_t& registerVal){
	
	uint16_t result = a - registerVal - carryFlag4;
	
	a = result & 0xFF;
	zeroFlag7 = (a == 0);
	subtractionFlag6 = 1;
	halfCarryFlag5 = ((a & 0x0F) < (registerVal & 0x0F) + carryFlag4);
	carryFlag4 = (registerVal + carryFlag4) > a;
	cycleCount += 4;
}

void CPU::andR8(uint8_t& registerVal){
	a &= registerVal;
	carryFlag4 = 0;
	subtractionFlag6 = (a == 0);
	zeroFlag7 = 0;
	halfCarryFlag5 = 1;
	cycleCount += 4;

}

void CPU::xorR8(uint8_t& registerVal){
	a ^= registerVal;
	zeroFlag7 = (a == 0);
	halfCarryFlag5 = 0;
	subtractionFlag6 = 0;
	halfCarryFlag5 = 0;
	cycleCount += 4;
}

void CPU::orR8(uint8_t& registerVal){
	a |= registerVal;
	zeroFlag7 = (a == 0);
	halfCarryFlag5 = 0;
	subtractionFlag6 = 0;
	halfCarryFlag5 = 0;
	cycleCount += 4;
}

void CPU::cpR8(uint8_t& registerVal){
	zeroFlag7 = ((a - registerVal) == 0);
	carryFlag4 = (registerVal > a);
	subtractionFlag6 = 1;
	halfCarryFlag5 = ((a & 0x0F) - (registerVal & 0x0F) == 0x00);
	cycleCount += 4;
}

void CPU::retCC(bool cc){
	if (cc) {
		uint8_t lowerBit = map->read(stackPointer++);
		uint8_t upperBit = map->read(stackPointer++);
		programCounter = (upperBit << 8) | lowerBit;
		cycleCount += 20;
	}
	else {
		cycleCount += 8;
	}
}

void CPU::popR16(uint8_t& upperBit, uint8_t& lowerBit, uint16_t& registerVal){
	lowerBit = map->read(stackPointer++);
	upperBit = map->read(stackPointer++);
	registerVal = (upperBit << 8) | lowerBit;
	cycleCount += 12;
}

void CPU::jumpCC(bool cc ){
	if (cc) {
		uint8_t lowerBit = map->read(programCounter++);
		uint8_t upperBit = map->read(programCounter++);
		programCounter = (upperBit << 8) | lowerBit;
		cycleCount += 16;
	}
	else {
		cycleCount += 12;
	}
}

void CPU::callCC(bool cc){
	uint8_t lowerBits = map->read(programCounter++);
	uint8_t upperBits = map->read(programCounter++);
	uint16_t n16 = (upperBits << 8) | lowerBits;
	if (cc) {
		stackPointer--;
		map->write(stackPointer, (programCounter >> 8) & 0xFF);
		stackPointer--;
		map->write(stackPointer, programCounter & 0xFF);
		programCounter = n16;
		cycleCount += 24;
	}
	else {
		cycleCount += 12;
	}
}

void CPU::rstVec(uint8_t vec){
	stackPointer--;
	map->write(stackPointer, (programCounter >> 8) & 0xFF);
	stackPointer--;
	map->write(stackPointer, programCounter & 0xFF);
	programCounter = vec;
	cycleCount += 16;
}

void CPU::reti(){
	uint8_t lowerBit = map->read(stackPointer++);
	uint8_t upperBit = map->read(stackPointer++);
	programCounter = (upperBit << 8) | lowerBit;
	bool intrupt = true;
	cycleCount += 16;
}

void CPU::pushR16(uint8_t& upperBit, uint8_t& lowerBit, uint16_t& registerVal){
	stackPointer--;
	map->write(stackPointer, upperBit);
	stackPointer--;
	map->write(stackPointer, lowerBit);
	cycleCount += 16;
}
