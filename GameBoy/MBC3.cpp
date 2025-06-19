#include "MBC3.h"

MBC3::MBC3(){
	ifRam = false;
	ifBattery = false;
	ifCartLoaded = false;
}

MBC3::MBC3(Cartridge *cart, bool Ram, bool Battery){
	ifRam = Ram;
	ifBattery = Battery;
	cartridge = cart;
	ifCartLoaded = true;
	initTime();
	ifRTCchoosen = false;
	
	
}

MBC3::~MBC3(){
}

void MBC3::write(uint16_t address, uint8_t data){
	if (!ifCartLoaded) {
		return;
	}
	if (address >= 0x0000 && address <= 0x1FFF) {
		if (data == 0x0A) {
			ifRam = true;
			ifRTC = true;
		}
		else {
			ifRam = false;
			ifRTC = false;
		}
	}

	if (address >= 0x4000 && address <= 0x5FFF) {
		if (ifRam && (data >= 0x00 && data <= 0x03)) {
			if (data >= 0x00 && data <= 0x03) {
				ramBankNum = data;
				ifRTCchoosen = false;
				std::cout << std::hex << ramBankNum << "\n";
			}
		}
		
		if (ifRTC && (data >= 0x08 && data <= 0x0C)) {
			if (data >= 0x08 && data <= 0x0C) {
				RTC = data;
				ifRTCchoosen = true;
			}
		}
	}

	if (address >= 0xA000 && address <= 0xBFFF && ifRTC) {
		
		if (ifRTC && ifRTCchoosen) {
			switch (RTC) {
			case 0x08: seconds = data; break;
			case 0x09: minutes = data; break;
			case 0x0A: hours = data; break;
			case 0x0B:dayLow = data; break;
			case 0x0C: dayHigh = data & 0xC1; break;
			default: break; // Invalid RTC register
			}
	}
	else if(ifRam) {
		
		uint16_t offset = address - 0xA000; 
		cartridge->write(offset, data, ramBankNum);
	}
	}
	if (address >= 0x6000 && address <= 0x7FFF) {
		if (data == 0x00) {
			ifLatched = false;
			
		}
		
		if (data == 0x01 && !ifLatched) {
			ifLatched = true;
			latchedSeconds = seconds;
			latchedMinutes = minutes;
			latchedHours = hours;
			latchedDayLow = dayLow;
			latchedDayHigh = dayHigh;

		}
	}
}

uint8_t MBC3::read(uint16_t address){
	if (!ifCartLoaded) {
		return -1;
	}
	if (address >= 0x0000 && address <= 0x3FFF) {
		return cartridge->read(address, bootBankNum);
	}
	if (address >= 0x4000 && address <= 0x7FFF) {
		if (romBankNum == 0) {
			romBankNum = 1;
		}
		uint16_t offset = address - 0x4000;
		return cartridge->read(offset, romBankNum);
	}
	
	if (address >= 0xA000 && address <= 0xBFFF && ifRTC) {
		
		if (ifRTCchoosen) {
			switch(RTC){
			case 0x08: {
				if (ifLatched) {
					return latchedSeconds;
				}
				else {
					return seconds;
				}
			}
			case 0x09:{
				if (ifLatched) {
					return latchedMinutes;
				}
				else {
					return minutes;
				}
				
			}
			case 0x0A: {
				if (ifLatched) {
					return latchedHours;
				}
				else {
					return hours;
				}
			}
			case 0x0B: {
				if (ifLatched) {
					return latchedDayLow;
				}
				else {
					return dayLow;
				}
			}
			case 0x0C: {
				if (ifLatched) {
					return latchedDayHigh;
				}
				else {
					return dayHigh;
				}
			}
			default: {
				return 0xFF;
			}
			}
		}
		else if(ifRam) {
			uint16_t offset = address - 0xA000;
			return cartridge->readRam(offset, ramBankNum);

		}
	}

	return 0xFF;
}

void MBC3::loadCartridge(Cartridge* cart){
	cartridge = cart;
	ifCartLoaded = true;
	initTime();
	ifRTCchoosen = false;
}

void MBC3::updateTime() {
	if (halt) {
		return;
	}
	if(!ifLatched){
		auto now = std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now()).time_since_epoch().count();
		currentTime = now;
		seconds = (currentTime % 60);
		minutes = (currentTime / 60) % 60;
		hours = (currentTime / 3600) % 24;
		days = (currentTime / 86400);
		if (days > 511) {
			days = 0;         // Wrap around day count
			carryFlag = true;     // Set the carry flag
		}
		dayLow = days & 0xFF;
		dayHigh = (days >> 8) & 0x03;



}
}

void MBC3::incrementDay()
{
	if (halt) return; 

	dayCounter++; 
	if (dayCounter > 511) {
		dayCounter = 0; 
		carryFlag = true; 
	}

	
	dayHigh = (dayCounter >> 8) & 0x03; 
}

void MBC3::initTime(){
	auto t1 = std::chrono::system_clock::now();
	currentTime = std::chrono::time_point_cast<std::chrono::seconds>(t1).time_since_epoch().count();
	
	/*seconds = (currentTime % 60);
	minutes = (currentTime / 60) % 60;
	hours = (currentTime / 3600) % 24;
	days = (currentTime / 86400);
	dayLow = days & 0xFF;
	dayHigh = (days >> 8) & 0x03;
*/
	seconds = 0;
	minutes = 0;
	hours = 0;
	days = 0;
	dayLow = 0;
	dayHigh = 0;

	

}


