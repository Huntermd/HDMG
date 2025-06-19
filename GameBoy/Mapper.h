#pragma once
#include <stdint.h>
#include "Cartridge.h"
#include "chrono"

class Mapper {
public:
	bool* halt;
	 ~Mapper();
	 Mapper();
	void write(uint16_t address, uint8_t data);
	void loadHalt(bool* h);

	uint8_t read(uint16_t address);
	inline void updateVariables(bool IframUsed, bool IfbatteryUsed);
	bool isBootRom = false;
	
	
private:
	void handleVariables(uint8_t type);
	void loadCart();
	bool ifCartLoaded = false;
	uint8_t mbcType;
	Cartridge cart;
	int bootBankNum = 0;
	int romBankNum = 1;
	int ramBankNum = 0;
	//MBC variables
	uint64_t currentTime;
	bool ifRam = false;
	bool ifBattery = false;
	inline void initTime();
	uint16_t days;
	uint8_t latchedSeconds = 0;
	uint8_t latchedMinutes = 0;
	uint8_t latchedHours = 0;
	uint8_t latchedDayLow;
	uint8_t latchedDayHigh;
	uint8_t seconds = 0;
	uint8_t minutes = 0;
	uint8_t hours = 0;
	uint8_t dayLow;
	uint8_t dayHigh;
	inline void incrementDay();
	inline void updateTime();
	int dayCounter = 0;
	
	bool carryFlag = false;
	bool ifLatched = false;
	bool ifRTCchoosen;
	uint8_t RTC;
	bool ifRTC = false;
	uint8_t upperRegister;
	uint8_t lowerRegister;
	//NMBC
	uint8_t nmbcRead(uint16_t address);
	void nmbcWrite(uint16_t address, uint8_t data);
	//MBC1
	uint8_t mbc1Read(uint16_t address);
	void mbc1Write(uint16_t address, uint8_t data);
	bool mode1 = false;
	std::vector<uint8_t> bootrom;
	void loadBoot();
	//MBC3
	uint8_t mbc3Read(uint16_t address);
	void mbc3Write(uint16_t address, uint8_t data);
	void handleRTC();

	
};
