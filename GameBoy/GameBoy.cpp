#include "GameBoy.h"
#include "iomanip"

std::ofstream outputFile;
int lineCount = 0;
bool eiDelay = false;
GameBoy::GameBoy(){
	
	if (!testEnable) {
		cpuInit();
	}
	
	
	
	
}

GameBoy::~GameBoy(){

}

void GameBoy::testInstuctions(){
	ime = 0;
	ie = 0;
	cycleCount = 0;
	a = 164;
	b = 197;
	c = 214;
	d = 62;
	e = 232;
	zeroFlag7 = (0 & 0x80) != 0;
	subtractionFlag6 = (80 & 0x40) != 0;
	halfCarryFlag5 = (80 & 0x20) != 0;
	carryFlag4 = (80 & 0x10) != 0;
	h = 191;
	l =2;
	BC = (b << 8) | c;
	
	programCounter = 17742;
	stackPointer = 32175;

	
	uint8_t flags = 0;
	do
	{
		getOpcode();
		InstructionSet();

	} while (cycleCount != 12);//programCounter < 45834);
	flags |= (zeroFlag7 << 7);
	flags |= (subtractionFlag6 << 6);
	flags |= (halfCarryFlag5 << 5);
	flags |= (carryFlag4 << 4);
	std::cout << static_cast<int>(a) << "\n";
	std::cout << static_cast<int>(b) << "\n";
	std::cout << static_cast<int>(c) << "\n";
	std::cout << static_cast<int>(d) << "\n";
	std::cout << static_cast<int>(e) << "\n";
	std::cout << static_cast<int>(flags) << "\n";
	std::cout << static_cast<int>(h) << "\n";
	std::cout << static_cast<int>(l) << "\n";
	for (size_t i = 0; i < testRam.size(); i++){
		std::cout << static_cast<int>(testRam[i][0])<<","<<static_cast<int>(testRam[i][1]) << "\n";

	}
	std::cout << static_cast<int>(programCounter) << "\n";
	std::cout << static_cast<int>(stackPointer) << "\n";
}

void GameBoy::testRom(){
	if (dmaActive) {
		mCycle();
		return;
	}
	if (ifHalt) {
		mCycle();
		
	}
	else {
		if (eiDelay) {
			ime = 1;
			eiDelay = false;
		}
		


		getOpcode();

	}
	
	

	
	
	InstructionSet();
	handleInterruptions();
	
}

void GameBoy::loadComponets(Mapper* m, PPU* p){
	this->map = m;
	this->ppu = p;
	ppu->loadRun(&isRunning);


}

void GameBoy::callResult(uint8_t data){
	results += static_cast<char>(data);
	std::cout << results << "\n";
	std::cout << "Cycle count is: " << cycleCount << "\n";
}

void GameBoy::closeTxt(){
	outputFile.close();
}



void GameBoy::logData(){
	uint8_t temp1 = cpuRead(programCounter);
	uint8_t temp2 = cpuRead(programCounter + 1);
	uint8_t temp3 = cpuRead(programCounter + 2);
	uint8_t temp4 = cpuRead(programCounter + 3);
	uint8_t flags = 0;
	flags |= (zeroFlag7 << 7);
	flags |= (subtractionFlag6 << 6);
	flags |= (halfCarryFlag5 << 5);
	flags |= (carryFlag4 << 4);
	outputFile << "A: " << std::setw(2) << std::setfill('0') <<  std::hex << std::uppercase << static_cast<int>(a)
		<< " F: " << std::setw(2) << std::setfill('0') << std::hex<<std::uppercase << static_cast<int>(flags)
		 << " B: " << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << static_cast<int>(b)
		<< " C: " << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << static_cast<int>(c)
		<< " D: " << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << static_cast<int>(d)
		<< " E: " << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << static_cast<int>(e)
		<< " H: " << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << static_cast<int>(h)
		<< " L: " << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << static_cast<int>(l)
		<< " SP: " << std::setw(4) << std::setfill('0') << std::hex << std::uppercase << static_cast<int>(stackPointer)
		<< " PC: " <<"00:" << std::setw(4) << std::setfill('0') << std::hex << std::uppercase << static_cast<int>(programCounter)
		<<" (" 
		<< std::setw(2) << std::setfill('0') << std::hex << std::uppercase << static_cast<int>(temp1)<< " "
		<< std::setw(2) << std::setfill('0') << std::hex << std::uppercase << static_cast<int>(temp2) << " "
		<< std::setw(2) << std::setfill('0') << std::hex << std::uppercase << static_cast<int>(temp3) << " "
		<< std::setw(2) << std::setfill('0') << std::hex << std::uppercase << static_cast<int>(temp4)
		<<")"
		<< std::endl;
	//<<" op: "<<std::setw(2) << std::setfill('0') << std::hex << std::uppercase << static_cast<int>(Opcode)
}

void GameBoy::initLogging(){
	outputFile.open("myLogs.txt");
	if (!outputFile.is_open()) {
		std::cerr<< "Error opening file logs" << "\n";
		exit(1);
	}
}



void GameBoy::getOpcode(){
	Opcode = cpuRead(programCounter);
	//std::cout << "Opcode: " << std::hex << static_cast<int>(Opcode) << " and HL is : " << std::hex << static_cast<int>(HL) << " and program counter is " << std::hex << static_cast<int>(programCounter)<<"\n";
	programCounter++;
}



uint8_t GameBoy::cpuRead(uint16_t address){
	mCycle();
	if (dmaActive)return byte;
	if (testEnable) {
		for (int i = 0; i < testRam.size(); i++) {
			if (address == testRam[i][0]) {
				return testRam[i][1];
			}
		}

	}
	if (address >= 0x0000 && address <= 0x3FFF&& !dmaActive) {
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

void GameBoy::cpuWrite(uint16_t address, uint8_t data){
	mCycle();
	if (dmaActive)return;
	if (testEnable) {
		bool includes = false;
		int index = 0;
		for (int i = 0; i < testRam.size(); i++) {
			if (address == testRam[i][0]) {
				includes = true;
				index = i;
				break;
			}
		}
		if (includes) {
			testRam[index][1] = data;
		}
		else {
			testRam.emplace_back(address, data);
		}
	}
	else {
		if (address >= 0x0000 && address <= 0x3FFF && !dmaActive) {
			map->write(address, data);
		}
		if (address >= 0x4000 && address <= 0x7FFF && !dmaActive) {
			map->write(address, data);
		}
		if (address >= 0x8000 && address <= 0x9FFF && !dmaActive) {
			ppu->writeVRAM(address,data);
		}
		if (address >= 0xA000 && address <= 0xBFFF && !dmaActive) {
			map->write(address, data);
		}
		if (address >= 0xC000 && address <= 0xDFFF && !dmaActive) {
			wram[address - 0xC000] = data;
		}
		if (address >= 0xFE00 && address <= 0xFE9F && !dmaActive) {
			
			ppu->writeOAM(address,data);
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
			return ;
		}
		if (address == 0xFF07 && !dmaActive) {
			uint8_t myLastBit = lastBit;
			lastBit &= ((data & 4) >> 2);
			detectEdge(myLastBit, lastBit);
			tac = data;
			return ;
		}
		if (address >= 0xFF40 && address <= 0xFF4B && !dmaActive) {
			if (address == 0xFF46) {
				dmaActive = true;
				sourceBase = data << 8;
				dmaIndex = 0;
				dmaDelay = 2;
			}
			ppu->ppuWrite(address,data,iF);
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
		if (address == 0xFFFF ) {
			ie = data;
		}
	}
	
}

void GameBoy::tCycle(int num){
	//cycleCount += num;
	//updateTimer((num / 4));
	
	
	
}

void GameBoy::init(){

	
}

void GameBoy::cpuInit(){
	
	a = 0x01;
	b = 0x00;
	c = 0x13;
	d = 0x00;
	e = 0xD8;
	zeroFlag7 = (0xB0 & 0x80) != 0;
	subtractionFlag6 = (0xB0 & 0x40) != 0;
	halfCarryFlag5 = (0xB0 & 0x20) != 0;
	carryFlag4 = (0xB0 & 0x10) != 0;
	h = 0x01;
	l = 0x4D;
	BC = (b << 8) | c;
	HL = (h << 8) | l;
	DE = (d << 8) | e;
	programCounter = 0x0100;
	stackPointer = 0xFFFE;
	cycleCount = 0;
	for (int i = 0; i < 8192; i++){
		wram[i] = 0;

	}
	for (int i = 0; i < 127; i++) {
		hram[i] = 0;

	}
	for (int i = 0; i < 0x80; i++) {
		ioRegisters[i] = 0;
	}
	Opcode = 0;
	
	iF = 0xE1;
	ie = 0;
	
}

void GameBoy::InstructionSet() {
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
	case 0x00: {//NOP
		//tCycle(4);
		break;
	}
	case 0x01: {// LD BC, n16
		c = cpuRead(programCounter++);
		b = cpuRead(programCounter++);
		BC = (b << 8) | c;
		tCycle(12);
		break;
	}
	case 0x02: {// LD [BC], a
		cpuWrite(BC, a);
		tCycle(8);
		break;
	}
	case 0x03: {// inc BC
		INC16(BC,b,c);
		tCycle(8);
		break;
	}
	case 0x04: {//INC B
		halfCarryFlag5 = ((b & 0x0F) + 1) > 0x0F;
		b += 1;
		zeroFlag7 = (b == 0);
		subtractionFlag6 = 0;
		
		BC = (b << 8) | c;
		tCycle(4);
		break;
	}
	case 0x05: {//DEC b
		halfCarryFlag5 = (static_cast<int>((b & 0x0F)) - 1) < 0;
		b -= 1;
		zeroFlag7 = (b == 0);
		subtractionFlag6 = 1;
		
		BC = (b << 8) | c;
		tCycle(4);
		break;
	}
	case 0x06: {//LD b, n8
		LoadN8(b, 8);
		BC = (b << 8) | c;
		tCycle(8);
		break;
	}
	case 0x07: { //RLCA
		carryFlag4 = (a & 0x80) >> 7;
		a <<= 1;
		a |= carryFlag4;
		zeroFlag7 = 0;
		halfCarryFlag5 = 0;
		subtractionFlag6 = 0;
		tCycle(4);
		break;
	}
	case 0x08: {//LD[a16],SP
		uint8_t lowerBit = cpuRead(programCounter++);
		uint8_t upperBit = cpuRead(programCounter++);
		uint16_t a16 = (upperBit << 8) | lowerBit;
		uint8_t lowerSp = (stackPointer & 0x00FF);
		uint8_t upperSp = (stackPointer >> 8);
		cpuWrite(a16, lowerSp);
		cpuWrite(a16 + 1, upperSp);
		tCycle(20);
		break;
	}
	case 0x09: {
		Add16(BC);
		tCycle(8);
		break;
	}
	case 0x0A: {
		a = cpuRead(BC);
		tCycle(8);
		break;
	}
	case 0x0B: {
		DEC16(BC,b,c);
		tCycle(8);
		break;
	}
	case 0x0C: {
		INC8(c);
		BC = (b << 8) | c;
		tCycle(4);
		break;
	}
	case 0x0D: {
		DEC8(c);
		BC = (b << 8) | c;
		tCycle(4);
		break;
	}
	case 0x0E: {
		LoadN8(c, 8);
		BC = (b << 8) | c;
		tCycle(8);
		break;
	}
	case 0x0F: {
		carryFlag4 = (a & 0x01);
		a >>= 1;
		a |= (carryFlag4 << 7);
		zeroFlag7 = 0;
		halfCarryFlag5 = 0;
		subtractionFlag6 = 0;
		tCycle(4);
		break;
	}
	case 0x10: {// stop 0
		programCounter++;
		tCycle(4);
		break;
	}
	case 0x11: {//LD DE, 
		e = cpuRead(programCounter++);
		d = cpuRead(programCounter++);
		DE = (d << 8) | e;
		tCycle(12);
		break;
	}
	case 0x12: {// LD A, [DE]
		cpuWrite(DE, a);
		tCycle(8);
		break;
	}
	case 0x13: {// INC DE
		INC16(DE,d,e);
		tCycle(8);
		break;
	}
	case 0x14: {//INC D
		halfCarryFlag5 = ((d & 0x0F) + 1) > 0x0F;
		d += 1;
		zeroFlag7 = (d == 0);
		subtractionFlag6 = 0;
		
		DE = (d << 8) | e;
		tCycle(4);
		break;
	}
	case 0x15: {//DEC D
		halfCarryFlag5 = (static_cast<int>((d & 0x0F)) - 1) < 0;
		d -= 1;
		zeroFlag7 = (d == 0);
		subtractionFlag6 = 1;
		
		DE = (d << 8) | e;
		tCycle(4);
		break;
	}
	case 0x16: {//LD D, n8
		LoadN8(d, 8);
		DE = (d << 8) | e;
		tCycle(8);
		break;
	}
	case 0x17: {
		//RLA
		uint8_t oldCarry = carryFlag4;
		carryFlag4 = (a & 0x80) >> 7;
		a = (a << 1) | oldCarry;

		zeroFlag7 = 0;
		subtractionFlag6 = 0;
		halfCarryFlag5 = 0;
		tCycle(4);
		break;
	}
	case 0x18: {
		int8_t e8 = static_cast<int8_t>(cpuRead(programCounter++));
		programCounter = programCounter + e8;
		mCycle();
		tCycle(12);
		break;
	}
	case 0x19: {
		DE = (d << 8) | e;
		Add16(DE);
		tCycle(8);
		break;
	}
	case 0x1A: {
		a = cpuRead(DE);
		tCycle(8);
		break;
	}
	case 0x1B: {
		
		DEC16(DE,d,e);
		tCycle(8);
		break;
	}
	case 0x1C: {
		INC8(e);
		DE = (d << 8) | e;
		tCycle(4);
		break;
	}
	case 0x1D: {
		DEC8(e);
		DE = (d << 8) | e;
		tCycle(4);
		break;
	}
	case 0x1E: {
		LoadN8(e, 8);
		DE = (d << 8) | e;
		tCycle(8);
		break;
	}
	case 0x1F: {
		uint8_t temp = a;
		
		a >>= 1;
		a = a | (carryFlag4 << 7);
		carryFlag4 = (temp & 0x01);
		
		

		zeroFlag7 = 0;
		subtractionFlag6 = 0;
		halfCarryFlag5 = 0;
		tCycle(4);
		break;
	}
	case 0x20: {
		int8_t e8 = static_cast<int8_t>(cpuRead(programCounter++));
		if (zeroFlag7 == 0) {
			programCounter = programCounter + e8;
			mCycle();
			tCycle(12);
			break;
		}
		else {
			tCycle(8);
			break;
		}
	}
	case 0x21: {// LD HL, n16
		l = cpuRead(programCounter++);
		h = cpuRead(programCounter++);
		HL = (h << 8) | l;
		tCycle(12);
		break;
	}
	case 0x22: {// LD [HL+], A
		cpuWrite(HL, a);
		HL += 1;
		h = (HL >> 8) & 0xFF;
		l = HL & 0xFF;
		tCycle(8);
		break;
	}
	case 0x23: {//INC HL
		INC16(HL,h,l);
		tCycle(8);
		break;
	}
	case 0x24: {// INC H
		halfCarryFlag5 = ((h & 0x0F) + 1) > 0x0F;
		h += 1;
		zeroFlag7 = (h == 0);
		subtractionFlag6 = 0;
		
		HL = (h << 8) | l;
		tCycle(4);
		break;
	}
	case 0x25: {//Dec H
		halfCarryFlag5 = (static_cast<int>((h & 0x0F)) - 1) < 0;
		h -= 1;
		zeroFlag7 = (h == 0);
		subtractionFlag6 = 1;
		
		HL = (h << 8) | l;
		tCycle(4);
		break;
	}
	case 0x26: {//Ld H, n8
		LoadN8(h, 8);
		HL = (h << 8) | l;
		tCycle(8);
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
			if (a > 0x99 || carryFlag4) {
				a += 0x60;
				carryFlag4 = 1;
			}
			if ((a & 0x0F) > 0x09 || halfCarryFlag5) {
				a += 6;
			}
			
		}
		else {
			if (carryFlag4) {
				a -= 0x60;
			}
			if (halfCarryFlag5) {
				a -= 0x06;
			}
			
		}
		zeroFlag7 = (a == 0);
		halfCarryFlag5 = 0;
		tCycle(4);
		break;
	}
	case 0x28: {
		int8_t e8 = static_cast<int8_t>(cpuRead(programCounter++));
		if (zeroFlag7 == 1) {
			programCounter = programCounter + e8;
			mCycle();
			tCycle(12);
			break;
		}
		else {
			tCycle(8);
			break;
		}

	}
	case 0x29: {
		HL = (h << 8) | l;
		Add16(HL);
		tCycle(8);
		break;
	}
	case 0x2A: {
		a = cpuRead(HL);
		HL += 1; 
		h = (HL >> 8) & 0xFF;
		l = HL & 0xFF;
		tCycle(8);
		break;
	}
	case 0x2B: {
		DEC16(HL,h,l);
		tCycle(8);
		break;
	}
	case 0x2C: {
		INC8(l);
		HL = (h << 8) | l;
		tCycle(4);
		break;
	}
	case 0x2D: {
		DEC8(l);
		HL = (h << 8) | l;
		tCycle(4);
		break;
	}
	case 0x2E: {
		LoadN8(l, 8);
		HL = (h << 8) | l;
		tCycle(8);
		break;
	}
	case 0x2F: {
		a = ~a;
		
		subtractionFlag6 = 1;
		halfCarryFlag5 = 1;
		tCycle(4);
		break;
	}
	case 0x30: {//JR NC, e8
		int8_t e8 = static_cast<int8_t>(cpuRead(programCounter++));
		if (carryFlag4 == 0) {
			programCounter = programCounter + e8;
			mCycle();
			tCycle(12);
			break;
		}
		else {
			tCycle(8);
			break;
		}
	}
	case 0x31: {// ADD SP, DE
		uint8_t lowerBit = cpuRead(programCounter++);
		uint8_t upperBit = cpuRead(programCounter++);
		stackPointer = (upperBit << 8) | lowerBit;
		tCycle(12);
		break;
	}
	case 0x32: {//LD a, [DE]
		cpuWrite(HL, a);
		HL -= 1;
		h = (HL >> 8) & 0xFF;
		l = HL & 0xFF;
		tCycle(8);
		break;
	}
	case 0x33: {//DEC DE
		stackPointer += 1;
		mCycle();
		tCycle(8);
		break;
	}
	case 0x34: {//INC SP
		uint8_t value = cpuRead(HL);
		uint8_t result = value + 1;
		cpuWrite(HL, result);
		zeroFlag7 = (result == 0);
		subtractionFlag6 = 0;
		halfCarryFlag5 = ((value & 0x0F) + 1) > 0x0F;
		tCycle(12);
		
		break;
	}
	case 0x35: {//INC (HL)
		uint8_t value = cpuRead(HL);
		uint8_t result = value - 1;
		cpuWrite(HL, result);
		zeroFlag7 = (result == 0);
		subtractionFlag6 = 1;
		halfCarryFlag5 = (static_cast<int>(value & 0x0F) - 1) < 0x00;
		tCycle(12);
		break;
	}
	case 0x36: {//DEC (HL)
		uint8_t e8 = cpuRead(programCounter++);
		HL = (h << 8) | l;
		cpuWrite(HL, e8);
		tCycle(12);
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
		tCycle(4);
		break;
	}
	case 0x38: {
		int8_t e8 = static_cast<int8_t>(cpuRead(programCounter++));
		if (carryFlag4 == 1) {
			programCounter = programCounter + e8;
			mCycle();
			tCycle(12);
			break;
		}
		else {
			tCycle(8);

			break;
		}
		break;
	}
	case 0x39: {
		Add16(stackPointer);
		tCycle(8);
		break;
	}
	case 0x3A: {
		a = cpuRead(HL);
		HL -= 1;
		h = (HL >> 8) & 0xFF;
		l = HL & 0xFF;
		tCycle(8);
		break;
	}
	case 0x3B: {
		stackPointer -= 1;
		mCycle();
		tCycle(8);
		break;
	}
	case 0x3C: {
		INC8(a);
		tCycle(4);
		break;
	}
	case 0x3D: {
		DEC8(a);
		tCycle(4);
		break;
	}
	case 0x3E: {
		LoadN8(a, 8);
		tCycle(8);
		break;
	}
	case 0x3F: {
		carryFlag4 ^= 0x01;

		subtractionFlag6 = 0;
		halfCarryFlag5 = 0;
		
		tCycle(4);
		break;
	}
	case 0x40: {//
		LoadR8R8(b, b);
		BC = (b << 8) | c;
		break;
	}
	case 0x41: {//LD HL,n16
		LoadR8R8(b, c);
		BC = (b << 8) | c;
		break;
	}
	case 0x42: {//LD [HL+(I)] , a
		LoadR8R8(b, d);
		BC = (b << 8) | c;
		break;
	}
	case 0x43: {//INC HL
		LoadR8R8(b, e);
		BC = (b << 8) | c;
		break;
	}
	case 0x44: {//INC H
		LoadR8R8(b, h);
		BC = (b << 8) | c;
		break;
	}
	case 0x45: {//DEC H
		LoadR8R8(b, l);
		BC = (b << 8) | c;
		break;
	}
	case 0x46: {
		LoadR8Address(b);
		BC = (b << 8) | c;
		break;
	}
	case 0x47: {
		LoadR8R8(b, a);
		BC = (b << 8) | c;
		break;
	}
	case 0x48: {
		LoadR8R8(c, b);
		BC = (b << 8) | c;
		break;
	}
	case 0x49: {
		LoadR8R8(c, c);
		BC = (b << 8) | c;
		break;
	}
	case 0x4A: {
		LoadR8R8(c, d);
		BC = (b << 8) | c;
		break;
	}
	case 0x4B: {
		LoadR8R8(c, e);
		BC = (b << 8) | c;
		break;
	}
	case 0x4C: {
		LoadR8R8(c, h);
		BC = (b << 8) | c;
		break;
	}
	case 0x4D: {
		LoadR8R8(c, l);
		BC = (b << 8) | c;
		break;
	}
	case 0x4E: {
		LoadR8Address(c);
		BC = (b << 8) | c;
		break;
	}
	case 0x4F: {
		LoadR8R8(c, a);
		BC = (b << 8) | c;
		break;
	}
	case 0x50: {//JR Z, e8
		LoadR8R8(d, b);
		DE = (d << 8) | e;
		break;
	}
	case 0x51: {
		LoadR8R8(d, c);
		DE = (d << 8) | e;
		break;
	}
	case 0x52: {
		LoadR8R8(d, d);
		DE = (d << 8) | e;
		break;
	}
	case 0x53: {
		LoadR8R8(d, e);
		DE = (d << 8) | e;
		break;
	}
	case 0x54: {
		LoadR8R8(d, h);
		DE = (d << 8) | e;
		break;
	}
	case 0x55: {
		LoadR8R8(d, l);
		DE = (d << 8) | e;
		break;
	}
	case 0x56: {
		LoadR8Address(d);
		DE = (d << 8) | e;
		break;
	}
	case 0x57: {
		LoadR8R8(d, a);
		DE = (d << 8) | e;
		break;
	}
	case 0x58: {
		LoadR8R8(e, b);
		DE = (d << 8) | e;
		break;
	}
	case 0x59: {
		LoadR8R8(e, c);
		DE = (d << 8) | e;
		break;
	}
	case 0x5A: {
		LoadR8R8(e, d);
		DE = (d << 8) | e;
		break;
	}
	case 0x5B: {
		LoadR8R8(e, e);
		DE = (d << 8) | e;
		break;
	}
	case 0x5C: {
		LoadR8R8(e, h);
		DE = (d << 8) | e;
		break;
	}
	case 0x5D: {
		LoadR8R8(e, l);
		DE = (d << 8) | e;
		break;
	}
	case 0x5E: {
		LoadR8Address(e);
		DE = (d << 8) | e;
		break;
	}
	case 0x5F: {
		LoadR8R8(e, a);
		DE = (d << 8) | e;
		break;
	}
	case 0x60: {
		LoadR8R8(h, b);
		HL = (h << 8) | l;
		break;
	}
	case 0x61: {
		LoadR8R8(h, c);
		HL = (h << 8) | l;
		break;
	}
	case 0x62: {
		LoadR8R8(h, d);
		HL = (h << 8) | l;
		break;
	}
	case 0x63: {
		LoadR8R8(h, e);
		HL = (h << 8) | l;
		break;
	}
	case 0x64: {
		LoadR8R8(h, h);
		HL = (h << 8) | l;
		break;
	}
	case 0x65: {
		LoadR8R8(h, l);
		HL = (h << 8) | l;
		break;
	case 0x66: {
		LoadR8Address(h);
		HL = (h << 8) | l;
		break;
	}
	case 0x67: {
		LoadR8R8(h, a);
		HL = (h << 8) | l;
		break;
	}
	case 0x68: {
		LoadR8R8(l, b);
		HL = (h << 8) | l;
		break;
	}
	case 0x69: {
		LoadR8R8(l, c);
		HL = (h << 8) | l;
		break;
	}
	case 0x6A: {
		LoadR8R8(l, d);
		HL = (h << 8) | l;
		break;
	}
	case 0x6B: {
		LoadR8R8(l, e);
		HL = (h << 8) | l;
		break;
	}
	case 0x6C: {
		LoadR8R8(l, h);
		HL = (h << 8) | l;
		break;
	}
	case 0x6D: {
		LoadR8R8(l, l);
		HL = (h << 8) | l;
		break;
	}
	case 0x6E: {
		LoadR8Address(l);
		HL = (h << 8) | l;
		break;
	}
	case 0x6F: {
		LoadR8R8(l, a);
		HL = (h << 8) | l;
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
		if (ime && (ie & iF) != 0) {
			ifHalt = false;
			return;
		}
		else if (!ime) {
			if ((ie & iF) == 0) {
				ifHalt = true;
				return;
			}
			else {
				ifHalt = false;
				return;
			}
		}
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
		BC = (b << 8) | c;
		break;
	}
	case 0x81: {
		AddR8R8(c);
		BC = (b << 8) | c;
		break;
	}
	case 0x82: {
		AddR8R8(d);
		DE = (d << 8) | e;
		break;
	}
	case 0x83: {
		AddR8R8(e);
		DE = (d << 8) | e;
		break;
	}
	case 0x84: {
		AddR8R8(h);
		HL = (h << 8) | l;
		break;
	}
	case 0x85: {
		AddR8R8(l);
		HL = (h << 8) | l;
		break;
	}
	case 0x86: {
		uint8_t temp = cpuRead(HL);
		AddR8R8(temp);
		tCycle(4);
		
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
		uint8_t temp = cpuRead(HL);
		abcR8R8(temp);
		tCycle(4);
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
		uint8_t temp = cpuRead(HL);
		subR8(temp);
		tCycle(4);
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
		uint8_t temp = cpuRead(HL);
		sbcR8R8(temp);
		tCycle(4);
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
		uint8_t temp = cpuRead(HL);
		andR8(temp);
		tCycle(4);
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
		uint8_t temp = cpuRead(HL);
		xorR8(temp);
		tCycle(4);
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
		uint8_t temp = cpuRead(HL);
		orR8(temp);
		tCycle(4);
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
		uint8_t temp = cpuRead(HL);
		cpR8(temp);
		tCycle(4);
		break;
	}
	case 0xBF: {
		zeroFlag7 = 1;
		carryFlag4 =0;
		subtractionFlag6 = 1;
		halfCarryFlag5 = 0;
		tCycle(4);
		break;
	}
	case 0xC0: {
		bool cc = (zeroFlag7 == 0);
		retCC(cc);
		break;
	}
	case 0xC1: {
		popR16(b, c, BC);
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
		uint8_t n8 = cpuRead(programCounter++);
		AddR8R8(n8);
		tCycle(4);
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
		uint8_t lowerBit = cpuRead(stackPointer++);
		uint8_t upperBit = cpuRead(stackPointer++);
		programCounter = (upperBit << 8) | lowerBit;
		mCycle();
		
		break;
	}
	case 0xCA: {
		bool cc = (zeroFlag7 == 1);
		jumpCC(cc);
		break;
	}
	case 0xCB: {//PREFIX CB
		cbInstructionSet();
		tCycle(4);
		break;
	}
	case 0xCC: {
		bool cc = (zeroFlag7 == 1);
		callCC(cc);
		break;
	}
	case 0xCD: {
		//rember to break
		callCC(true);
		break;
	}
	case 0xCE: {
		uint8_t n8 = cpuRead(programCounter++);
		abcR8R8(n8);
		tCycle(4);
		break;
	}
	case 0xCF: {
		rstVec(0x08);
		break;
	}
	case 0xD0: {
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
		uint8_t n8 = cpuRead(programCounter++);
		subR8(n8);
		tCycle(4);
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
		uint8_t n8 = cpuRead(programCounter++);
		sbcR8R8(n8);
		tCycle(4);
		break;
	}
	case 0xDF: {
		rstVec(0x18);
		break;
	}
	case 0xE0: {
		uint8_t n8 = cpuRead(programCounter++);
		uint16_t address = 0xFF00 + n8;
		cpuWrite(address, a);
		tCycle(12);
		break;
	}
	case 0xE1: {
		popR16(h, l, HL);
		break;
	}
	case 0xE2: {

		uint16_t address = 0xFF00 + c;
		cpuWrite(address, a);
		tCycle(8);
		break;
	}
	case 0xE5: {
		pushR16(h, l, HL);
		break;
	}
	case 0xE6: {
		uint8_t d8 = cpuRead(programCounter++);
		andR8(d8);
		tCycle(4);
		break;
	}
	case 0xE7: {
		rstVec(0x20);
		break;
	}
	case 0xE8: {
		int8_t offset = static_cast<int8_t>(cpuRead(programCounter++));
		halfCarryFlag5 = ((stackPointer & 0x0F) + (offset & 0x0F)) > 0x0F;
		uint8_t lowerByte = (stackPointer & 0xFF);
		uint16_t result = lowerByte + static_cast<uint8_t>(offset);
		carryFlag4 = (result > 0xFF);
		stackPointer += offset;
		zeroFlag7 = 0;
		subtractionFlag6 = 0;
		mCycle();
		mCycle();
		tCycle(16);
		
		
		break;
	}
	case 0xE9: {
		programCounter = HL;
		tCycle(4);
		break;
	}
	case 0xEA: {
		uint8_t lowerByte = cpuRead(programCounter++);
		uint8_t upperByte = cpuRead(programCounter++);
		uint16_t address = (upperByte << 8) | lowerByte;
		cpuWrite(address, a);
		tCycle(16);
		break;
	}
	case 0xEE: {
		uint8_t n8 = cpuRead(programCounter++);
		xorR8(n8);
		tCycle(4);
		break;
	}
	case 0xEF: {
		rstVec(0x28);
		
		break;
	}
	case 0xF0: {
		uint8_t n8 = cpuRead(programCounter++);
		uint16_t address = 0xFF00 + n8;
		a = cpuRead(address);
		tCycle(12);
		break;
	}
	case 0xF1: {
		uint8_t low = cpuRead(stackPointer++);
		uint8_t high = cpuRead(stackPointer++);
		a = high;
		zeroFlag7 = (low & 0x80) != 0;
		subtractionFlag6 = (low & 0x40) != 0;
		halfCarryFlag5 = (low & 0x20) != 0;
		carryFlag4 = (low & 0x10) != 0;
		tCycle(12);

		break;
	}
	case 0xF2: {
		uint16_t address = 0xFF00 + c;
		a = cpuRead(address);
		tCycle(8);
		break;
	}
	case 0xF3: {
		ime = 0;
		tCycle(4);
		break;
	}
	case 0xF5: {
		uint8_t flags = 0;
		flags |= (zeroFlag7 << 7);
		flags |= (subtractionFlag6 << 6);
		flags |= (halfCarryFlag5 << 5);
		flags |= (carryFlag4 << 4);

		cpuWrite(--stackPointer, a);
		cpuWrite(--stackPointer, flags);
		mCycle();
		tCycle(16);
		break;
	}
	case 0xF6: {
		uint8_t d8 = cpuRead(programCounter++);
		orR8(d8);
		tCycle(8);
		break;
	}
	case 0xF7: {
		rstVec(0x30);
		break;
	}
	case 0xF8: {
		int8_t e8 = static_cast<int8_t>(cpuRead(programCounter++));
		uint16_t result = stackPointer + e8;
		HL = result;
		zeroFlag7 = 0;
		subtractionFlag6 = 0;
		halfCarryFlag5 = ((stackPointer & 0x0F) + (e8 & 0x0F)) > 0x0F;
		carryFlag4 = ((stackPointer & 0xFF) + (e8 & 0xFF)) > 0xFF;
		
		h = (HL >> 8) & 0xFF;
		l = HL & 0xFF;
		mCycle();
		tCycle(12);
		break;
	}
	case 0xF9: {
		stackPointer = HL;
		mCycle();
		tCycle(8);
		break;
	}
	case 0xFA: {
		uint8_t lowerByte = cpuRead(programCounter++);
		uint8_t upperByte = cpuRead(programCounter++);
		uint16_t address = (upperByte << 8) | lowerByte;
		a = cpuRead(address);
		tCycle(16);
		break;
	}
	case 0xFB: {
		eiDelay = true;
		tCycle(4);
		break;
	}
	case 0xFE: {
		uint8_t d8 = cpuRead(programCounter++);
		cpR8(d8);
		tCycle(4);
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

void GameBoy::cbInstructionSet(){
	getOpcode();

	switch (Opcode) {
	case 0x00: {// RLC B
		rlc(b);
		BC = (b << 8) | c;
		
		break;
	}
	case 0x01: {// LD BC, n16
		rlc(c);
		BC = (b << 8) | c;
		
		break;
	}
	case 0x02: {// LD [BC], a
		rlc(d);
		DE = (d << 8) | e;
		
		break;
	}
	case 0x03: {// inc BC
		rlc(e);
		DE = (d << 8) | e;
		
		break;
	}
	case 0x04: {//INC B
		rlc(h);
		HL = (h << 8) | l;
		
		break;
	}
	case 0x05: {//DEC b
		rlc(l);
		HL = (h << 8) | l;
		
		break;
	}
	case 0x06: {//LD b, n8
		HL = (h << 8) | l;
		uint8_t n8 = cpuRead(HL);
		rlc(n8);
		cpuWrite(HL, n8);
		tCycle(8);
		break;
	}
	case 0x07: { //RLCA
		rlc(a);
		
		break;
	}
	case 0x08: {//LD[a16],SP
		rrc(b);
		BC = (b << 8) | c;
		
		break;
	}
	case 0x09: {
		rrc(c);
		BC = (b << 8) | c;
		
		break;
	}
	case 0x0A: {
		rrc(d);
		DE = (d << 8) | e;
		
		break;
	}
	case 0x0B: {
		rrc(e);
		DE = (d << 8) | e;
		
		break;
	}
	case 0x0C: {
		rrc(h);
		HL = (h << 8) | l;
		
		break;
	}
	case 0x0D: {
		rrc(l);
		HL = (h << 8) | l;
		
		break;
	}
	case 0x0E: {
		HL = (h << 8) | l;
		uint8_t n8 = cpuRead(HL);
		rrc(n8);
		cpuWrite(HL, n8);
		tCycle(8);
		break;
	}
	case 0x0F: {
		rrc(a);
		
		break;
	}
	case 0x10: {// stop 0
		rl(b);
		BC = (b << 8) | c;
		
		break;
	}
	case 0x11: {//LD DE, 
		rl(c);
		BC = (b << 8) | c;
		
		break;
	}
	case 0x12: {// LD A, [DE]
		rl(d);
		DE = (d << 8) | e;
		
		break;
	}
	case 0x13: {// INC DE
		rl(e);
		DE = (d << 8) | e;
		
		break;
	}
	case 0x14: {//INC D
		rl(h);
		HL = (h << 8) | l;
		
		break;
	}
	case 0x15: {//DEC D
		rl(l);
		HL = (h << 8) | l;
		
		break;
	}
	case 0x16: {//LD D, n8
		HL = (h << 8) | l;
		uint8_t n8 = cpuRead(HL);
		rl(n8);
		cpuWrite(HL, n8);
		tCycle(8);
		break;
	}
	case 0x17: {
		rl(a);
		
		break;
	}
	case 0x18: {
		rr(b);
		BC = (b << 8) | c;
		
		break;
	}
	case 0x19: {
		rr(c);
		BC = (b << 8) | c;
		
		break;
	}
	case 0x1A: {
		rr(d);
		DE = (d << 8) | e;
		
		break;
	}
	case 0x1B: {
		rr(e);
		DE = (d << 8) | e;
		
		break;
	}
	case 0x1C: {
		rr(h);
		HL = (h << 8) | l;
		
		break;
	}
	case 0x1D: {
		rr(l);
		HL = (h << 8) | l;
		
		break;
	}
	case 0x1E: {
		HL = (h << 8) | l;
		uint8_t n8 = cpuRead(HL);
		rr(n8);
		cpuWrite(HL, n8);
		tCycle(8);
		break;
	}
	case 0x1F: {
		rr(a);
		
		break;
	}
	case 0x20: {
		sla(b);
		BC = (b << 8) | c;
		
		break;
	}
	case 0x21: {// LD HL, n16
		sla(c);
		BC = (b << 8) | c;
		
		break;
	}
	case 0x22: {// LD [HL+], A
		sla(d);
		DE = (d << 8) | e;
		
		break;
	}
	case 0x23: {//INC HL
		sla(e);
		DE = (d << 8) | e;
		
		break;
	}
	case 0x24: {// INC H
		sla(h);
		HL = (h << 8) | l;
		
		break;
	}
	case 0x25: {//Dec H
		sla(l);
		HL = (h << 8) | l;
		
		break;
	}
	case 0x26: {//Ld H, n8
		HL = (h << 8) | l;
		uint8_t n8 = cpuRead(HL);
		sla(n8);
		cpuWrite(HL, n8);
		tCycle(8);
		break;
	}
	case 0x27: {//RLA
		sla(a);
		
		break;
	}
	case 0x28: {
		sra(b);
		BC = (b << 8) | c;
		
		break;

	}
	case 0x29: {
		sra(c);
		BC = (b << 8) | c;
		
		break;
	}
	case 0x2A: {
		sra(d);
		DE = (d << 8) | e;
		
		break;
	}
	case 0x2B: {
		sra(e);
		DE = (d << 8) | e;
		
		break;
	}
	case 0x2C: {
		sra(h);
		HL = (h << 8) | l;
		
		break;
	}
	case 0x2D: {
		sra(l);
		HL = (h << 8) | l;
		
		break;
	}
	case 0x2E: {
		HL = (h << 8) | l;
		uint8_t n8 = cpuRead(HL);
		sra(n8);
		cpuWrite(HL, n8);
		tCycle(8);
		break;
	}
	case 0x2F: {
		sra(a);
		
		break;
	}
	case 0x30: {//JR NC, e8
		swap(b);
		BC= (b << 8) | c;
		
		break;
	}
	case 0x31: {// ADD SP, DE
		swap(c);
		BC= (b<< 8) | c;
		
		break;
	}
	case 0x32: {//LD a, [DE]
		swap(d);
		DE= (d<< 8) | e;
		
		break;
	}
	case 0x33: {//DEC DE
		swap(e);
		DE= (d<< 8) | e;
		
		break;
	}
	case 0x34: {//INC SP
		swap(h);
		HL = (h << 8) | l;
		
		break;
	}
	case 0x35: {//INC (HL)
		swap(l);
		HL = (h << 8) | l;
		
		break;
	}
	case 0x36: {//DEC (HL)
		HL = (h << 8) | l;
		uint8_t n8 = cpuRead(HL);
		swap(n8);
		cpuWrite(HL, n8);
		tCycle(8);
		break;
	}
	case 0x37: {//LD (HL), e8
		swap(a);
		
		break;
	}
	case 0x38: {
		srl(b);
		
		BC = (b << 8) | c;
		
		break;
	}
	case 0x39: {
		srl(c);
		BC = (b << 8) | c;
		
		break;
	}
	case 0x3A: {
		srl(d);
		DE = (d << 8) | e;
		
		break;
	}
	case 0x3B: {
		srl(e);
		DE = (d << 8) | e;
		
		break;
	}
	case 0x3C: {
		srl(h);
		HL = (h << 8) | l;
		
		break;
	}
	case 0x3D: {
		srl(l);
		HL = (h << 8) | l;
		
		break;
	}
	case 0x3E: {
		HL = (h << 8) | l;
		uint8_t n8 = cpuRead(HL);
		srl(n8);
		cpuWrite(HL, n8);
		tCycle(8);
		break;
	}
	case 0x3F: {
		srl(a);
		
		break;
	}
	case 0x40: {//
		bit(0x01, b);
		
		break;
	}
	case 0x41: {//LD HL,n16
		bit(0x01, c);
		
		break;
	}
	case 0x42: {//LD [HL+(I)] , a
		bit(0x01,d );
		
		break;
	}
	case 0x43: {//INC HL
		bit(0x01, e);
		
		break;
	}
	case 0x44: {//INC H
		bit(0x01, h);
		
		break;
	}
	case 0x45: {//DEC H
		bit(0x01, l);
		
		break;
	}
	case 0x46: {
		HL = (h << 8) | l;
		uint8_t n8 = cpuRead(HL);
		bit(0x01,n8 );
		tCycle(4);
		break;
	}
	case 0x47: {
		bit(0x01,a );
		
		break;
	}
	case 0x48: {
		bit(0x02, b);
		
		break;
	}
	case 0x49: {
		bit(0x02, c);
		
		break;
	}
	case 0x4A: {
		bit(0x02, d);
		
		break;
	}
	case 0x4B: {
		bit(0x02, e);
		
		break;
	}
	case 0x4C: {
		bit(0x02, h);
		
		break;
	}
	case 0x4D: {
		bit(0x02, l);
		
		break;
	}
	case 0x4E: {
		HL = (h << 8) | l;
		uint8_t n8 = cpuRead(HL);
		bit(0x02, n8);
		tCycle(4);
		break;
	}
	case 0x4F: {
		bit(0x02, a);
		
		break;
	}
	case 0x50: {//JR Z, e8
		bit(0x04, b);
		
		break;
	}
	case 0x51: {
		bit(0x04, c);
		
		break;
	}
	case 0x52: {
		bit(0x04, d);
		
		break;
	}
	case 0x53: {
		bit(0x04, e);
		
		break;
	}
	case 0x54: {
		bit(0x04, h);
		
		break;
	}
	case 0x55: {
		bit(0x04, l);
		
		break;
	}
	case 0x56: {
		HL = (h << 8) | l;
		uint8_t n8 = cpuRead(HL);
		bit(0x04, n8);
		tCycle(4);
		break;
	}
	case 0x57: {
		bit(0x04, a);
		
		break;
	}
	case 0x58: {
		bit(0x08, b);
		
		break;
	}
	case 0x59: {
		bit(0x08, c);
		
		break;
	}
	case 0x5A: {
		bit(0x08, d);
		
		break;
	}
	case 0x5B: {
		bit(0x08, e);
		
		break;
	}
	case 0x5C: {
		bit(0x08, h);
		
		break;
	}
	case 0x5D: {
		bit(0x08, l);
		
		break;
	}
	case 0x5E: {
		HL = (h << 8) | l;
		uint8_t n8 = cpuRead(HL);
		bit(0x08, n8);
		tCycle(4);
		break;
	}
	case 0x5F: {
		bit(0x08, a);
		
		break;
	}
	case 0x60: {
		bit(0x10, b);
		
		break;
	}
	case 0x61: {
		bit(0x10, c);
		
		break;
	}
	case 0x62: {
		bit(0x10, d);
		
		break;
	}
	case 0x63: {
		bit(0x10, e);
		
		break;
	}
	case 0x64: {
		bit(0x10, h);
		
		break;
	}
	case 0x65: {
		bit(0x10, l);
		
		break;
	case 0x66: {
		HL = (h << 8) | l;
		uint8_t n8 = cpuRead(HL);
		bit(0x10, n8);
		tCycle(4);
		break;
	}
	case 0x67: {
		bit(0x10, a);
		
		break;
	}
	case 0x68: {
		bit(0x20, b);
		
		break;
	}
	case 0x69: {
		bit(0x20, c);
		
		break;
	}
	case 0x6A: {
		bit(0x20, d);
		
		break;
	}
	case 0x6B: {
		bit(0x20, e);
		
		break;
	}
	case 0x6C: {
		bit(0x20, h);
		
		break;
	}
	case 0x6D: {
		bit(0x20, l);
		
		break;
	}
	case 0x6E: {
		HL = (h << 8) | l;
		uint8_t n8 = cpuRead(HL);
		bit(0x20, n8);
		tCycle(4);
		break;
	}
	case 0x6F: {
		bit(0x20, a);
		
		break;
	}
	case 0x70: {
		bit(0x40, b);
		
		break;
	}
	case 0x71: {
		bit(0x40, c);
		
		break;
	}
	case 0x72: {
		bit(0x40, d);
		
		break;
	}
	case 0x73: {
		bit(0x40, e);
		
		break;
	}
	case 0x74: {
		bit(0x40, h);
		
		break;
	}
	case 0x75: {
		bit(0x40, l);
		
		break;
	}
	case 0x76: {
		HL = (h << 8) | l;
		uint8_t n8 = cpuRead(HL);
		bit(0x40, n8);
		tCycle(4);
		break;
	}
	case 0x77: {
		bit(0x40, a);
		
		break;
	}
	case 0x78: {
		bit(0x80, b);
		
		break;
	}
	case 0x79: {
		bit(0x80, c);
		
		break;
	}
	case 0x7A: {
		bit(0x80, d);
		
		break;
	}
	case 0x7B: {
		bit(0x80, e);
		
		break;
	}
	case 0x7C: {
		bit(0x80, h);
		
		break;
	}
	case 0x7D: {
		bit(0x80, l);
		
		break;
	}
	case 0x7E: {
		HL = (h << 8) | l;
		uint8_t n8 = cpuRead(HL);
		bit(0x80, n8);
		tCycle(4);
		break;
	}
	case 0x7F: {
		bit(0x80, a);
		
		break;
	}
	case 0x80: {
		res(0xFE, b);
		BC = (b << 8) | c;
		
		break;
	}
	case 0x81: {
		res(0xFE, c);
		BC = (b << 8) | c;
		
		break;
	}
	case 0x82: {
		res(0xFE, d);
		
		break;
	}
	case 0x83: {
		res(0xFE, e);
		
		break;
	}
	case 0x84: {
		res(0xFE, h);
		HL = (h << 8) | l;
		
		break;
	}
	case 0x85: {
		res(0xFE, l);
		HL = (h << 8) | l;
		
		break;
	}
	case 0x86: {
		HL = (h << 8) | l;
		uint8_t n8 = cpuRead(HL);
		res(0xFE, n8);
		cpuWrite(HL, n8);
		tCycle(8);
		break;
	}
	case 0x87: {
		res(0xFE, a);
		
		break;
	}
	case 0x88: {
		res(0xFD, b);
		BC = (b << 8) | c;
		
		break;
	}
	case 0x89: {
		res(0xFD, c);
		BC = (b << 8) | c;
		
		break;
	}
	case 0x8A: {
		res(0xFD, d);
		DE = (d << 8) | e;
		
		break;
	}
	case 0x8B: {
		res(0xFD, e);
		DE = (d << 8) | e;
		
		break;
	}
	case 0x8C: {
		res(0xFD, h);
		
		HL = (h << 8) | l;
		break;
	}
	case 0x8D: {
		res(0xFD, l);
		HL = (h << 8) | l;
		
		break;
	}
	case 0x8E: {
		HL = (h << 8) | l;
		uint8_t n8 = cpuRead(HL);
		res(0xFD, n8);
		cpuWrite(HL, n8);
		tCycle(8);
		break;
	}
	case 0x8F: {
		res(0xFD, a);
		
		break;
	}
			 //Beginning of 0x90
	case 0x90: {
		res(0xFb, b);
		BC = (b << 8) | c;
		
		break;
	}
	case 0x91: {
		res(0xFb, c);
		BC = (b << 8) | c;
		
		break;
	}
	case 0x92: {
		res(0xFB, d);
		DE = (d << 8) | e;
		
		break;
	}
	case 0x93: {
		res(0xFB, e);
		DE = (d << 8) | e;
		
		break;
	}
	case 0x94: {
		res(0xFB, h);
		HL = (h << 8) | l;
		
		break;
	}
	case 0x95: {
		res(0xFB, l);
		HL = (h << 8) | l;
		
		break;
	}
	case 0x96: {
		HL = (h << 8) | l;
		uint8_t n8 = cpuRead(HL);
		res(0xFB, n8);
		cpuWrite(HL, n8);
		tCycle(8);
		break;
	}
	case 0x97: {
		res(0xFB, a);
		
		break;
	}
	case 0x98: {
		res(0xF7, b);
		BC = (b << 8) | c;
		
		break;
	}
	case 0x99: {
		res(0xF7, c);
		BC = (b << 8) | c;
		
		break;
	}
	case 0x9A: {
		res(0xF7, d);
		DE = (d << 8) | e;
		
		break;
	}
	case 0x9B: {
		res(0xF7, e);
		DE = (d << 8) | e;
		
		break;
	}
	case 0x9C: {
		res(0xF7, h);
		HL = (h << 8) | l;
		
		break;
	}
	case 0x9D: {
		res(0xF7, l);
		HL = (h << 8) | l;
		
		break;
	}
	case 0x9E: {
		HL = (h << 8) | l;
		uint8_t n8 = cpuRead(HL);
		res(0xF7, n8);
		cpuWrite(HL, n8);
		tCycle(8);
		break;
	}
	case 0x9F: {
		res(0xF7, a);
		
		break;
	}
			 //End of 0x90
			 //Beginning of A
	case 0xA0: {
		res(0xEF, b);
		BC = (b << 8) | c;
		
		break;
	}
	case 0xA1: {
		res(0xEF, c);
		BC = (b << 8) | c;
		
		break;
	}
	case 0xA2: {
		res(0xEF, d);
		DE = (d << 8) | e;
		
		break;
	}
	case 0xA3: {
		res(0xEF, e);
		DE = (d << 8) | e;
		
		break;
	}
	case 0xA4: {
		res(0xEF, h);
		HL = (h << 8) | l;
		
		break;
	}
	case 0xA5: {
		res(0xEF, l);
		HL = (h << 8) | l;
		
		break;
	}
	case 0xA6: {
		HL = (h << 8) | l;
		uint8_t n8 = cpuRead(HL);
		res(0xEF, n8);
		cpuWrite(HL, n8);
		tCycle(8);
		break;
	}
	case 0xA7: {
		res(0xEF, a);
		
		break;
	}
	case 0xA8: {
		res(0xDF, b);
		BC = (b << 8) | c;
		
		break;
	}
	case 0xA9: {
		res(0xDF, c);
		BC = (b << 8) | c;
		
		break;
	}
	case 0xAA: {
		res(0xDF, d);
		DE = (d << 8) | e;
		
		break;
	}
	case 0xAB: {
		res(0xDF, e);
		DE = (d << 8) | e;
		
		break;
	}
	case 0xAC: {
		res(0xDF, h);
		HL = (h << 8) | l;
		
		break;
	}
	case 0xAD: {
		res(0xDF, l);
		HL = (h << 8) | l;
		
		break;
	}
	case 0xAE: {
		HL = (h << 8) | l;
		uint8_t n8 = cpuRead(HL);
		res(0xDF, n8);
		cpuWrite(HL, n8);
		tCycle(8);
		break;
	}
	case 0xAF: {
		res(0xDF, a);
		
		break;
	}
			 //End of A
			 //Beginning of B
	case 0xB0: {
		res(0xBF, b);
		BC = (b << 8) | c;
		
		break;
	}
	case 0xB1: {
		res(0xBF, c);
		BC = (b << 8) | c;
		
		break;
	}
	case 0xB2: {
		res(0xBF, d);
		DE = (d << 8) | e;
		
		break;
	}
	case 0xB3: {
		res(0xBF, e);
		DE = (d << 8) | e;
		
		break;
	}
	case 0xB4: {
		res(0xBF, h);
		HL = (h << 8) | l;
		
		break;
	}
	case 0xB5: {
		res(0xBF, l);
		HL = (h << 8) | l;
		
		break;
	}
	case 0xB6: {
		HL = (h << 8) | l;
		uint8_t n8 = cpuRead(HL);
		res(0xBF, n8);
		cpuWrite(HL, n8);
		tCycle(8);
		break;
	}
	case 0xB7: {
		res(0xBF, a);
		
		break;
	}
	case 0xB8: {
		res(0x7F, b);
		BC = (b << 8) | c;
		
		break;
	}
	case 0xB9: {
		res(0x7F, c);
		BC = (b << 8) | c;
		
		break;
	}
	case 0xBA: {
		res(0x7F, d);
		DE = (d << 8) | e;
		
		break;
	}
	case 0xBB: {
		res(0x7F, e);
		DE = (d << 8) | e;
		
		break;
	}
	case 0xBC: {
		res(0x7F, h);
		HL = (h << 8) | l;
		
		break;
	}
	case 0xBD: {
		res(0x7F, l);
		HL = (h << 8) | l;
		
		break;
	}
	case 0xBE: {
		HL = (h << 8) | l;
		uint8_t n8 = cpuRead(HL);
		res(0x7F, n8);
		cpuWrite(HL, n8);
		tCycle(8);
		break;
	}
	case 0xBF: {
		res(0x7F, a);
		
		break;
	}
			 //End of B
			 //Beginning of C
	case 0xC0: {
		set(0x01, b);
		BC = (b << 8) | c;
		
		break;
	}
	case 0xC1: {
		set(0x01, c);
		BC = (b << 8) | c;
		
		break;
	}
	case 0xC2: {
		set(0x01, d);
		DE = (d << 8) | e;
		
		break;
	}
	case 0xC3: {
		set(0x01, e);
		DE = (d << 8) | e;
		
		break;
	}
	case 0xC4: {
		set(0x01, h);
		HL = (h << 8) | l;
		
		break;
	}
	case 0xC5: {
		set(0x01, l);
		HL = (h << 8) | l;
		
		break;
	}
	case 0xC6: {
		HL = (h << 8) | l;
		uint8_t n8 = cpuRead(HL);
		set(0x01, n8);
		cpuWrite(HL, n8);
		tCycle(8);
		break;
	}
	case 0xC7: {
		set(0x01, a);
		
		break;
	}
	case 0xC8: {
		set(0x02, b);
		BC = (b << 8) | c;
		
		break;
	}
	case 0xC9: {
		set(0x02, c);
		BC = (b << 8) | c;
		
		break;
	}
	case 0xCA: {
		set(0x02, d);
		DE = (d << 8) | e;
		
		break;
	}
	case 0xCB: {
		set(0x02, e);
		DE = (d << 8) | e;
		
		break;
	}
	case 0xCC: {
		set(0x02, h);
		HL = (h << 8) | l;
		
		break;
	}
	case 0xCD: {
		set(0x02, l);
		HL = (h << 8) | l;
		
		break;
	}
	case 0xCE: {
		HL = (h << 8) | l;
		uint8_t n8 = cpuRead(HL);
		set(0x02, n8);
		cpuWrite(HL, n8);
		tCycle(8);
		break;
	}
	case 0xCF: {
		set(0x02, a);
		
		break;
	}
			 //END of c
	case 0xD0: {
		set(0x04, b);
		BC = (b << 8) | c;
		
		break;
	}
	case 0xD1: {
		set(0x04, c);
		BC = (b << 8) | c;
		
		break;
	}
	case 0xD2: {
		set(0x04, d);
		DE = (d << 8) | e;
		
		break;
	}
	case 0xD3: {
		set(0x04, e);
		DE = (d << 8) | e;
		
		break;
	}
	case 0xD4: {
		set(0x04, h);
		HL = (h << 8) | l;
		
		break;
	}
	case 0xD5: {
		set(0x04, l);
		HL = (h << 8) | l;
		
		break;
	}
	case 0xD6: {
		HL = (h << 8) | l;
		uint8_t n8 = cpuRead(HL);
		set(0x04, n8);
		cpuWrite(HL, n8);
		tCycle(8);
		break;
	}
	case 0xD7: {
		set(0x04, a);
		
		break;
	}
	case 0xD8: {
		set(0x08, b);
		BC = (b << 8) | c;
		
		break;
	}
	case 0xD9: {
		set(0x08, c);
		BC = (b << 8) | c;
		
		break;
	}
	case 0xDA: {
		set(0x08, d);
		DE = (d << 8) | e;
		
		break;
	}
	case 0xDB: {
		set(0x08, e);
		DE = (d << 8) | e;
		
		break;
	}
	case 0xDC: {
		set(0x08, h);
		HL = (h << 8) | l;
		
		break;
	}
	case 0xDD: {
		set(0x08, l);
		HL = (h << 8) | l;
		
		break;
	}
	case 0xDE: {
		HL = (h << 8) | l;
		uint8_t n8 = cpuRead(HL);
		set(0x08, n8);
		cpuWrite(HL, n8);
		tCycle(8);
		break;
	}
	case 0xDF: {
		set(0x08, a);
		
		break;
	}
	case 0xE0: {
		set(0x10, b);
		BC = (b << 8) | c;
		
		break;
	}
	case 0xE1: {
		set(0x10, c);
		BC = (b << 8) | c;
		
		break;
	}
	case 0xE2: {
		set(0x10, d);
		DE = (d << 8) | e;
		
		break;
	}
	case 0xE3: {
		set(0x10, e);
		DE = (d << 8) | e;
		
		break;
	}
	case 0xE4: {
		set(0x10, h);
		HL = (h << 8) | l;
		
		break;
	}
	case 0xE5: {
		set(0x10, l);
		HL = (h << 8) | l;
		
		break;
	}
	case 0xE6: {
		HL = (h << 8) | l;
		uint8_t n8 = cpuRead(HL);
		set(0x10, n8);
		cpuWrite(HL, n8);
		tCycle(8);
		break;
	}
	case 0xE7: {
		set(0x10, a);
		
		break;
	}
	case 0xE8: {
		set(0x20, b);
		BC = (b << 8) | c;
		
		break;
	}
	case 0xE9: {
		set(0x20, c);
		BC = (b << 8) | c;
		
		break;
	}
	case 0xEA: {
		set(0x20, d);
		DE = (d << 8) | e;
		
		break;
	}
	case 0xEB: {
		set(0x20, e);
		DE = (d << 8) | e;
		
		break;
	}
	case 0xEC: {
		set(0x20, h);
		HL = (h << 8) | l;
		
		break;
	}
	case 0xED: {
		set(0x20, l);
		HL = (h << 8) | l;
		
		break;
	}
	case 0xEE: {
		HL = (h << 8) | l;
		uint8_t n8 = cpuRead(HL);
		set(0x20, n8);
		cpuWrite(HL, n8);
		tCycle(8);
		break;
	}
	case 0xEF: {
		set(0x20, a);
		
		break;
	}
	case 0xF0: {
		set(0x40, b);
		BC = (b << 8) | c;
		
		break;
	}
	case 0xF1: {
		set(0x40, c);
		BC = (b << 8) | c;
		
		break;
	}
	case 0xF2: {
		set(0x40, d);
		DE = (d << 8) | e;
		
		break;
	}
	case 0xF3: {
		set(0x40, e);
		DE = (d << 8) | e;
		
		break;
	}
	case 0xF4: {
		set(0x40, h);
		HL = (h << 8) | l;
		
		break;
	}
	case 0xF5: {
		set(0x40, l);
		HL = (h << 8) | l;
		
		break;
	}
	case 0xF6: {
		HL = (h << 8) | l;
		uint8_t n8 = cpuRead(HL);
		set(0x40, n8);
		cpuWrite(HL, n8);
		tCycle(8);
		break;
	}
	case 0xF7: {
		set(0x40, a);
		
		break;
	}
	case 0xF8: {
		set(0x80, b);
		BC = (b << 8) | c;
		
		break;
	}
	case 0xF9: {
		set(0x80, c);
		BC = (b << 8) | c;
		
		break;
	}
	case 0xFA: {
		set(0x80, d);
		DE = (d << 8) | e;
		
		break;
	}
	case 0xFB: {
		set(0x80, e);
		DE = (d << 8) | e;
		
		break;
	}
	case 0xFC: {
		set(0x80, h);
		HL = (h << 8) | l;
		
		break;
	}
	case 0xFD: {
		set(0x80, l);
		HL = (h << 8) | l;
		
		break;
	}
	case 0xFE: {
		HL = (h << 8) | l;
		uint8_t n8 = cpuRead(HL);
		set(0x80, n8);
		cpuWrite(HL, n8);
		tCycle(8);
		break;
	}
	case 0xFF: {
		set(0x80, a);
		
		break;
	}


	default:
		break;
	}
	}

}

void GameBoy::LoadN8(uint8_t& registerVal, int count) {
	registerVal = cpuRead(programCounter++);
	
}

void GameBoy::Add16(uint16_t& registerVal) {
	HL = (h << 8) | l;
	halfCarryFlag5 = ((HL & 0xFFF) + (registerVal & 0xFFF)) > 0xFFF;
	uint32_t result = HL + registerVal;
	HL += registerVal;
	
	h = (HL >> 8) & 0xFF;
	l = HL & 0xFF;
	
	carryFlag4 = (result > 0xFFFF);
	
	subtractionFlag6 = 0;
	
	
	
	mCycle();
}

void GameBoy::INC16(uint16_t& registerVal,uint8_t& upperBit, uint8_t& lowerBit) {
	registerVal = (upperBit << 8) | lowerBit;
	registerVal += 1;
	upperBit = (registerVal >> 8) & 0xFF;
	lowerBit = registerVal & 0xFF;
	mCycle();
	
}

void GameBoy::DEC16(uint16_t& registerVal, uint8_t& upperBit, uint8_t& lowerBit) {
	registerVal = (upperBit << 8) | lowerBit;
	registerVal -= 1;
	upperBit = (registerVal >> 8) & 0xFF;
	lowerBit = registerVal & 0xFF;
	mCycle();
}

void GameBoy::INC8(uint8_t& registerVal) {
	halfCarryFlag5 = ((registerVal & 0x0F) + 1) > 0x0F;
	registerVal++;
	
	
	zeroFlag7 = (registerVal == 0);
	subtractionFlag6 = 0;
	
	
}

void GameBoy::DEC8(uint8_t& registerVal) {
	halfCarryFlag5 = (static_cast<int>((registerVal & 0x0F)) - 1) < 0;
	registerVal -= 1;
	zeroFlag7 = (registerVal == 0);
	subtractionFlag6 = 1;
	
	
}

void GameBoy::LoadR8R8(uint8_t& registerValLeft, uint8_t& registerValRight) {
	registerValLeft = registerValRight;
	tCycle(4);
	
}

void GameBoy::LoadAddressR8(uint8_t& registerVal) {
	cpuWrite(HL, registerVal);
	tCycle(8);
	
}

void GameBoy::LoadR8Address(uint8_t& registerVal) {
	registerVal = cpuRead(HL);
	tCycle(8);
	
}

void GameBoy::AddR8R8(uint8_t& registerVal) {
	//halfCarryFlag5 = (((a & 0x0F) + (registerVal & 0x0F)) & 0x10) == 0x10;
	halfCarryFlag5 = ((a & 0x0F) + (registerVal & 0x0F)) > 0x0F;
	uint16_t result = a + registerVal;
	a += registerVal;
	zeroFlag7 = (a == 0);
	subtractionFlag6 = 0;
	carryFlag4 = (result > 0xFF);
	tCycle(4);
	
	
	
	
}

void GameBoy::abcR8R8(uint8_t& registerVal) {
	halfCarryFlag5 = ((a & 0x0F) + (registerVal & 0x0F) + (carryFlag4 & 0x0F)) > 0x0F;
	uint16_t result = a + registerVal + carryFlag4;

	a = a + registerVal + carryFlag4;
	zeroFlag7 = (a == 0);
	subtractionFlag6 = 0;
	
	carryFlag4 = (result > 0x00FF);

	tCycle(4);
	
}

void GameBoy::subR8(uint8_t& registerVal) {
	carryFlag4 = (registerVal > a);
	halfCarryFlag5 = ((a & 0x0F) - (registerVal & 0x0F)) < 0;
	a = a - registerVal;
	zeroFlag7 = (a == 0);
	subtractionFlag6 = 1;
	tCycle(4);
	
	
}

void GameBoy::sbcR8R8(uint8_t& registerVal) {
	uint8_t temp = carryFlag4;
	carryFlag4 = (registerVal + carryFlag4) > a;
	uint16_t result = a - registerVal - carryFlag4;

	
	
	
	subtractionFlag6 = 1;
	halfCarryFlag5 = (static_cast<int>(a & 0x0F) - static_cast<int>(registerVal & 0x0F) - static_cast<int>(temp & 0x0F)) < 0;
	a = a - registerVal - temp;

	
	zeroFlag7 = (a == 0);
	tCycle(4);
}

void GameBoy::andR8(uint8_t& registerVal) {
	a &= registerVal;
	carryFlag4 = 0;
	subtractionFlag6 = 0;
	zeroFlag7 = (a == 0);
	halfCarryFlag5 = 1;
	tCycle(4);

}

void GameBoy::xorR8(uint8_t& registerVal) {
	a ^= registerVal;
	zeroFlag7 = (a == 0);
	carryFlag4 = 0;
	subtractionFlag6 = 0;
	halfCarryFlag5 = 0;
	tCycle(4);
}

void GameBoy::orR8(uint8_t& registerVal) {
	a |= registerVal;
	zeroFlag7 = (a == 0);
	halfCarryFlag5 = 0;
	subtractionFlag6 = 0;
	carryFlag4 = 0;
	tCycle(4);
}

void GameBoy::cpR8(uint8_t& registerVal) {
	zeroFlag7 = ((a - registerVal) == 0);
	carryFlag4 = (registerVal > a);
	subtractionFlag6 = 1;
	halfCarryFlag5 = (static_cast<int>((a & 0x0F)) - static_cast<int>((registerVal & 0x0F))) < 0x00;
	tCycle(4);
}

void GameBoy::retCC(bool cc) {
	if (cc) {
		uint8_t lowerBit = cpuRead(stackPointer++);
		uint8_t upperBit = cpuRead(stackPointer++);
		programCounter = (upperBit << 8) | lowerBit;
		mCycle();
		//mCycle();
		tCycle(20);
	}
	else {
		mCycle();
		tCycle(8);
	}
}

void GameBoy::popR16(uint8_t& upperBit, uint8_t& lowerBit, uint16_t& registerVal) {
	lowerBit = cpuRead(stackPointer++);
	upperBit = cpuRead(stackPointer++);
	registerVal = (upperBit << 8) | lowerBit;
	tCycle(12);
	
}

void GameBoy::jumpCC(bool cc) {
	uint8_t lowerBit = cpuRead(programCounter);
	programCounter++;
	uint8_t upperBit = cpuRead(programCounter);
	programCounter++;
	if (cc) {
		
		programCounter = (upperBit << 8) | lowerBit;
		mCycle();
		tCycle(16);
	}
	else {
		tCycle(12);
	}
}

void GameBoy::callCC(bool cc) {
	uint8_t lowerBits = cpuRead(programCounter++);
	uint8_t upperBits = cpuRead(programCounter++);
	uint16_t n16 = (upperBits << 8) | lowerBits;
	if (cc) {

		stackPointer--;
		cpuWrite(stackPointer, (programCounter >> 8) & 0xFF);
		stackPointer--;
		cpuWrite(stackPointer, programCounter & 0xFF);
		programCounter = n16;
		mCycle();
		tCycle(24);
		
	}
	else {
		tCycle(12);
	}
}

void GameBoy::rstVec(uint8_t vec) {
	stackPointer--;
	cpuWrite(stackPointer, (programCounter >> 8) & 0xFF);
	stackPointer--;
	cpuWrite(stackPointer, programCounter & 0xFF);
	programCounter = vec;
	mCycle();
	tCycle(16);
}

void GameBoy::reti() {
	ime = 1;
	uint8_t lowerBit = cpuRead(stackPointer++);
	uint8_t upperBit = cpuRead(stackPointer++);
	programCounter = (upperBit << 8) | lowerBit;
	mCycle();
	tCycle(16);
	
}

void GameBoy::pushR16(uint8_t& upperBit, uint8_t& lowerBit, uint16_t& registerVal) {
	stackPointer--;
	cpuWrite(stackPointer, upperBit);
	stackPointer--;
	cpuWrite(stackPointer, lowerBit);
	mCycle();
	tCycle(16);
	
}

void GameBoy::rlc(uint8_t& r8){
	carryFlag4 = (r8 & 0x80) >> 7;
	r8<<= 1;
	r8 |= carryFlag4;
	zeroFlag7 = (r8 == 0);
	halfCarryFlag5 = 0;
	subtractionFlag6 = 0;
	
	tCycle(4);

}

void GameBoy::rrc(uint8_t& r8){
	carryFlag4 = (r8 & 0x01);
	r8 >>= 1;
	r8 |= (carryFlag4 << 7);
	zeroFlag7 = (r8 == 0);
	halfCarryFlag5 = 0;
	subtractionFlag6 = 0;
	
	tCycle(4);
}

void GameBoy::rl(uint8_t& r8){
	uint8_t oldCarry = carryFlag4;
	carryFlag4 = (r8 & 0x80) >> 7;
	r8 = (r8 << 1) | oldCarry;

	zeroFlag7 = (r8 == 0);
	subtractionFlag6 = 0;
	halfCarryFlag5 = 0;

	tCycle(4);
}

void GameBoy::rr(uint8_t& r8){
	uint8_t temp = r8;
	
	r8 >>= 1;
	r8 = r8 | (carryFlag4 << 7);
	carryFlag4 = (temp & 0x01);
	


	zeroFlag7 = (r8 == 0);
	subtractionFlag6 = 0;
	halfCarryFlag5 = 0;
	
	tCycle(4);
}

void GameBoy::sla(uint8_t& r8){
	carryFlag4 = (r8 & 0x80) >> 7;
	r8 = (r8 << 1);

	zeroFlag7 = (r8== 0);
	subtractionFlag6 = 0;
	halfCarryFlag5 = 0;
	
	tCycle(4);
}

void GameBoy::sra(uint8_t& r8){
	carryFlag4 = (r8 & 0x01);
	uint8_t temp = (r8 & 0x80);
	r8 >>= 1;
	r8 |= temp;
	
	tCycle(4);


	zeroFlag7 = (r8 == 0);
	subtractionFlag6 = 0;
	halfCarryFlag5 = 0;
	
	tCycle(4);
}

void GameBoy::swap(uint8_t& r8){
	uint8_t upperBits = (r8 & 0xF0) >> 4;
	r8 = ((r8 & 0x0F) << 4) | upperBits;
	carryFlag4 = 0;


	zeroFlag7 = (r8 == 0);
	subtractionFlag6 = 0;
	halfCarryFlag5 = 0;

	tCycle(4);
}

void GameBoy::srl(uint8_t& r8){
	carryFlag4 = (r8 & 0x01) ;
	r8 >>= 1;
	zeroFlag7 = (r8 == 0);
	subtractionFlag6 = 0;
	halfCarryFlag5 = 0;
	
	tCycle(4);
}

void GameBoy::bit(uint8_t u3,uint8_t& r8){
	uint8_t temp = (r8 & u3);
	zeroFlag7 = (temp == 0);
	subtractionFlag6 = 0;
	halfCarryFlag5 = 1;

	tCycle(4);

}

void GameBoy::res(uint8_t u3, uint8_t& r8){
	r8 &= u3;
	
	tCycle(4);
}

void GameBoy::set(uint8_t u3, uint8_t& r8){
	r8 |= u3;
	
	tCycle(4);

}

void GameBoy::jumpInterruptions(uint8_t address){
	stackPointer--;
	cpuWrite(stackPointer, (programCounter >> 8) & 0xFF);
	stackPointer--;
	cpuWrite(stackPointer, programCounter & 0xFF);
	programCounter = address;
	mCycle();
}

void GameBoy::handleInterruptions(){
	if ((ime == 1) && ((ie & iF) != 0)) {
		if ((iF & 0x01) && (ie & 0x01)) {
			//VBlank interruption
			iF &= ~0x01;
			
			ime = 0;
			for (size_t i = 0; i < 2; i++)
			{
				mCycle();
			}
			jumpInterruptions(0x40);
		}
		if ((iF & 0x02) && (ie& 0x02)) {
			//LCD interruption
			iF &= ~0x02;
			
			ime = 0;
			for (size_t i = 0; i < 2; i++)
			{
				mCycle();
			}
			jumpInterruptions(0x48);
		}
		if ((iF & 0x04) && (ie& 0x04)) {
			//timer interruption
			iF &= ~0x04;
			
			ime = 0;
			for (size_t i = 0; i < 2; i++)
			{
				mCycle();
			}
			jumpInterruptions(0x50);
		}
		if ((iF & 0x08) && (ie& 0x08)) {
			//Serial interruption
			iF &= ~0x08;
			
			ime = 0;
			for (size_t i = 0; i < 2; i++)
			{
				mCycle();
			}
			jumpInterruptions(0x58);
		}
		if ((iF & 0x10) && (ie& 0x10)) {
			//Joypad interruption
			iF &= ~0x10;
			
			ime = 0;
			for (size_t i = 0; i < 2; i++)
			{
				mCycle();
			}
			jumpInterruptions(0x60);
		}

	}
}
/*
	16 bit counter increments every t-cycle (usually min of 4 per instruction)
	after the increment
	n = amount of increments
	 loop n times:
	counter++
	The bit poistion is determined based on the least 2 signicant bits in TAC then stored for later
	bitPostiton = (2 least bits)
	bit 2 extracted out of TAC (1/enabled and 0/disabled)
	timerEnable = 1 or 0
	currentResult =bitPostion & timerEnable
	(prevResult == 1 && currentResult == 0)
		then tima++



*/
void GameBoy::updateTimer(int num){
	prevDCounter = divCounter & 0x3FFF;
	divCounter = (divCounter & 0x3FFF) + 1 ;

	bool timerEnabled = false;
	switch (tac & 0b11){
	case 0b00: {
		timerEnabled = (prevDCounter & (1 << 7)) && (!(divCounter & (1 << 7)));
		break;
	}
	case 0b01: {
		timerEnabled = (prevDCounter & (1 << 1)) && (!(divCounter & (1 << 1)));
		break;
	}
	case 0b10: {
		timerEnabled = (prevDCounter & (1 << 3)) && (!(divCounter & (1 << 3)));
		break;
	}
	case 0b11: {
		timerEnabled = (prevDCounter & (1 << 5)) && (!(divCounter & (1 << 5)));
		break;
	}
	default:
		break;
	}
	if (timerEnabled && tac & (1<<2))
	{
		tima++;
		if (tima > 0xFF) {
			tima = tma;
			iF |= 0x04;
			

		}

	}
	

}

void GameBoy::incState(){
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

void GameBoy::sysClkChange(uint16_t new_value){
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

void GameBoy::detectEdge(uint8_t before, uint8_t after){
	if ((before == 1) && (after == 0)) {
		tima = (tima + 1) & 0xFF;
		if (tima == 0) {
			cyclesTilTimaIRQ = 1;
		}
	}
}

void GameBoy::mCycle(){
	//ppu->checkEvents(isRunning, iF);
	dma();
	incState();
	ppu->updatePPU(iF);

}

uint8_t GameBoy::readByte(uint16_t address){
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

void GameBoy::dma()
{
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