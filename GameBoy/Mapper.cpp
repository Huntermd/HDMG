#include "Mapper.h"




Mapper::~Mapper(){
	handleRTC();
}

Mapper::Mapper(std::string f){
	cart.loadFileN(f);
	loadCart(f);
	mbcType = cart.getMBC();
	
}

void Mapper::write(uint16_t address, uint8_t data){
	if (mbcType == 0x00) { nmbcWrite(address, data); return; }
	if (mbcType >= 0x01 && mbcType <= 0x03) { mbc1Write(address, data); return; }
	//if (mbcType >= 0x01 && mbcType <= 0x03)return mbc1Read(address);
	if (mbcType >= 0x0F && mbcType <= 0x13) { mbc3Write(address, data); return; }
	if (mbcType >= 0x19 && mbcType <= 0x1E) { mbc5Write(address, data); return; }
	
}

void Mapper::loadHalt(bool* h){
	this->halt = h;
}

uint8_t Mapper::read(uint16_t address){
	
	if(mbcType == 0x00)return nmbcRead(address);
	if (mbcType >= 0x01 && mbcType <= 0x03)return mbc1Read(address);
	//if (mbcType >= 0x01 && mbcType <= 0x03)return mbc1Read(address);
	if (mbcType >= 0x0F && mbcType <= 0x13)return mbc3Read(address);
	if (mbcType >= 0x19 && mbcType <= 0x1E)return mbc5Read(address);
	
	
	
	
	
}



void Mapper::handleVariables(uint8_t type){
	if (type == 0x13) {
		ifRam = false;
		ifBattery = true;
		//initTime();
		//ifRTCchoosen = false;
		
		
		

	}
}

void Mapper::loadCart(std::string f){
	cart.loadRom(f.c_str());
	ifCartLoaded = true;
	
}

void Mapper::initTime(){
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

void Mapper::incrementDay(){
	if (*halt) return;

	dayCounter++;
	if (dayCounter > 511) {
		dayCounter = 0;
		carryFlag = true;
	}


	dayHigh = (dayCounter >> 8) & 0x03;
}

void Mapper::updateTime(){
	if (*halt) {
		return;
	}
	if (!ifLatched) {
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

uint8_t Mapper::nmbcRead(uint16_t address){
	if (!ifCartLoaded) {
		std::cout << "The cartridge isn't loaded" << "\n";
		return 0xFF;
	}
	if (address >= 0x0000 && address <= 0x3FFF) {
		return cart.read(address, 0);
	}
	if (address >= 0x4000 && address <= 0x7FFF) {
		uint16_t offset = address - 0x4000;
		return cart.read(offset, 1);
	}

	return 0xFF;
}

void Mapper::nmbcWrite(uint16_t address, uint8_t data){
	return;
}

uint8_t Mapper::mbc1Read(uint16_t address){
	if (!ifCartLoaded) {
		std::cout << "The cartridge isn't loaded" << "\n";
		return 0xFF;
	}
	if (address >= 0x0000 && address <= 0x3FFF) {
		return cart.read(address, bootBankNum);
	}
	if (address >= 0x4000 && address <= 0x7FFF) {
		
		if (romBankNum == 0) {
			romBankNum = 1;
		}
		uint16_t offset = address - 0x4000;
		return cart.read(offset, romBankNum);
	}
	if (address >= 0xA000 && address <= 0xBFFF && ifRam) {
		uint16_t offset = address - 0xA000;
		return cart.readRam(offset, ramBankNum);
	}
	return 0xFF;
}

void Mapper::mbc1Write(uint16_t address, uint8_t data){
	if (!ifCartLoaded) {
		std::cout << "The cartridge isn't loaded" << "\n";
		return;
	}
	if (address >= 0x0000 && address <= 0x1FFF) {
		ifRam = (data == 0xA);
	}

	if (address >= 0x2000 && address <= 0x3FFF) {
		romBankNum = data & 0x1F;
		if (romBankNum == 0x00) {
			romBankNum = 0x1;
		}

		if (romBankNum == 0x00 && !mode1) {
			romBankNum = 0x10;
		}


	}

	if (address >= 0x4000 && address <= 0x5FFF) {
		if (mode1) {
			ramBankNum = (data & 0x03);
		}
		else {
			ramBankNum = 0x00;
		}


	}
	if (address >= 0xA000 && address <= 0xBFFF && ifRam) {
		uint16_t offset = address - 0xA000; // Calculate offset for RAM bank
		cart.write(offset, data, ramBankNum); // Write data to selected RAM bank
	}
}



uint8_t Mapper::mbc3Read(uint16_t address){
	if (!ifCartLoaded) {
		return -1;
	}
	if (address >= 0x0000 && address <= 0x3FFF) {
		return cart.read(address, bootBankNum);
	}
	if (address >= 0x4000 && address <= 0x7FFF) {
		if (romBankNum == 0) {
			romBankNum = 1;
		}
		uint16_t offset = address - 0x4000;
		return cart.read(offset, romBankNum);
	}

	if (address >= 0xA000 && address <= 0xBFFF && ifRam) {

		if (ifRTCchoosen) {
			switch (RTC) {
			case 0x08: {
				if (ifLatched) {
					return latchedSeconds;
				}
				else {
					return seconds;
				}
			}
			case 0x09: {
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
		else if (ifRam) {
			uint16_t offset = address - 0xA000;
			return cart.readRam(offset, ramBankNum);

		}
	}

	return 0xFF;
}

void Mapper::mbc3Write(uint16_t address, uint8_t data){
	if (!ifCartLoaded) {
		return;
	}
	if (address >= 0x0000 && address <= 0x1FFF) {
		if ((data& 0x0F) == 0x0A) {
			ifRam = true;
			//ifRTC = true;
		}
		else {
			ifRam = false;
			//ifRTC = false;
		}
	}
	if (address >= 0x2000 && address <= 0x3FFF) {
		int bank = (data & 0x7F);
		if (bank == 0) bank = 1;
		if (bank > cart.romBankNumber) bank %= cart.romBankNumber;

		
		romBankNum = bank;
		
		
	}
	if (address >= 0x4000 && address <= 0x5FFF) {
		if (ifRam && (data >= 0x00 && data <= 0x03)) {
			if (data >= 0x00 && data <= 0x03) {
				ramBankNum = data & 0x03;
				ifRTCchoosen = false;
				//std::cout << std::hex << ramBankNum << "\n";
			}
		}

		if (ifRTC && (data >= 0x08 && data <= 0x0C)) {
			if (data >= 0x08 && data <= 0x0C) {
				RTC = data;
				ifRTCchoosen = true;
			}
		}
	}

	if (address >= 0xA000 && address <= 0xBFFF && ifRam) {

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
		if (ifRam) {

			uint16_t offset = address - 0xA000;
			cart.write(offset, data, ramBankNum);
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

void Mapper::handleRTC(){
	//uint8_t RTCRegs[10] = { seconds,minutes,hours,dayLow,dayHigh,latchedSeconds,latchedMinutes,latchedHours,latchedDayLow,latchedDayHigh };
	//cart.loadRTC(RTCRegs);
}

uint8_t Mapper::mbc5Read(uint16_t address){
	if (address >= 0x0000 && address <= 0x3FFF) {
		return cart.read(address, bootBankNum);
	}
	if (address >= 0x4000 && address <= 0x7FFF) {

		uint16_t offset = address - 0x4000;
		return cart.read(offset, romBankNum);
	}
	if (address >= 0xA000 && address <= 0xBFFF&&ifRam) {

		uint16_t offset = address - 0xA000;
		return cart.readRam(offset, ramBankNum);
	}

	return 0xFF;
}

void Mapper::mbc5Write(uint16_t address, uint8_t data){
	if (address >= 0x0000 && address <= 0x1FFF) {
		if ((data & 0x0F) == 0x0A) {
			ifRam = true;
		}
		else {
			ifRam = false;
		}
	}
	if (address >= 0x2000 && address <= 0x2FFF) {
		romBankNum = (romBankNum & 0x100) | data;
	}
	if (address >= 0x3000 && address <= 0x3FFF) {
		romBankNum = (romBankNum & 0xFF) | ((data& 0x01)<<8);
	}
	if (address >= 0x4000 && address <= 0x5FFF) {
		ramBankNum = (data & 0x0F);
	}
	if (address >= 0xA000 && address <= 0xBFFF) {
		uint16_t offset = address - 0xA000;
		cart.write(offset, data, ramBankNum);
	}
}
