#pragma once
#include <stdint.h>
#include "Cartridge.h"
#include "chrono"

class Mapper {
public:
	bool* halt;
	 ~Mapper();
	 Mapper(std::string f);
	inline void write(uint16_t address, uint8_t data);
	void loadHalt(bool* h);

	inline uint8_t read(uint16_t address);
	
	
	
	
private:
	void handleVariables(uint8_t type);
	void loadCart(std::string f);
	bool ifCartLoaded = false;
	uint8_t mbcType;
	Cartridge cart;
	uint16_t bootBankNum = 0;
	uint16_t romBankNum = 1;
	uint16_t ramBankNum = 0;
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
	
	
	//MBC3
	uint8_t mbc3Read(uint16_t address);
	void mbc3Write(uint16_t address, uint8_t data);
	void handleRTC();
	//MBC5
	uint8_t mbc5Read(uint16_t address);
	void mbc5Write(uint16_t address, uint8_t data);
	
};
