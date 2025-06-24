#pragma once
#include <vector>
#include <iostream>
#include <fstream>
#include <stdint.h>

class Cartridge {
public:
	uint8_t getMBC();
	int romSize;
	int ramSize;
	int size;
	bool ifLoaded;
	
	Cartridge() {
		ifLoaded = false;
		size = 0;
	};
	Cartridge(const char* filePath) {
		loadRom(filePath);
		
		
	}
	~Cartridge();
	uint8_t read(uint16_t address, uint16_t bankNumber);
	uint8_t readRam(uint16_t address, uint16_t bankNumber);
	void loadRom(const char* filePath);
	int romBankNumber = 0;
	
	std::vector<unsigned char> getHeader();
	void getHeaderData();
	int cartridgeType;
	void write(uint16_t address, uint8_t data, uint16_t bankNumber);
	void loadRTC(uint8_t* r);
	uint8_t rtcRegs[18];
	std::vector<uint8_t> rtc; 
	std::string fileN;
	void loadFileN(std::string s);
private:
	uint8_t savable[10] = { 0x02,0x03,0x0F,0x10,0x12,0x13,0x1A,0x1B,0x1D,0x1E };
	uint64_t getUnixTimeStamp();
	bool ramLoaded = false;
	void getRamRomsize();
	
	int numBanks;
	const uint16_t bankSize = 16 * 1024;
	const uint16_t OpitonalbankSize1 = 32 * 1024;
	const uint16_t OpitonalbankSize2 = 8 * 1024;
	std::vector<std::vector<uint8_t>> romBanks;
	std::vector<std::vector<uint8_t>> ramBanks;
	void getRomBankSize(std::vector<unsigned char> file);
	void getRamBankSize(std::vector<unsigned char> file);
	
	int ramBankNumber = 0;
	void saveRam();
	void loadRam(const char* fileName);

};