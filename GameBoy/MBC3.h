#include "Mapper.h"
#include "Cartridge.h"
#include <iostream>
#include "chrono"

class MBC3 {
public:
	MBC3();
	MBC3(Cartridge *cart, bool ifRam,bool ifBattery);
	~MBC3();
	void write(uint16_t address, uint8_t data);
	uint8_t read(uint16_t address);
	void loadCartridge(Cartridge* cart);
private:
	void updateTime();
	uint64_t currentTime;
	void incrementDay();
	bool ifLatched = false;
	void initTime();
	int dayCounter = 0;
	bool halt = false;
	bool carryFlag = false;
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
	bool ifRTCchoosen;
	uint8_t RTC;
	bool ifRTC = false;
	int bootBankNum = 0;
	int romBankNum = 1;
	int ramBankNum = 0;
	bool ifRam = false;
	bool ifBattery = false;
	Cartridge* cartridge;
	bool ifCartLoaded;
	bool mode1 = false;
	uint8_t upperRegister;
	uint8_t lowerRegister;

};