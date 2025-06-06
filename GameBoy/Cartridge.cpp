#include "Cartridge.h"
#include <iomanip>
uint8_t Cartridge::getMBC(){
	return romBanks[0][0x0147];
}
Cartridge::~Cartridge(){}

uint8_t Cartridge::read(uint16_t address,int bankNumber){
	if (!ifLoaded) {
		return -1;
	}
	return romBanks[bankNumber][address];
}

uint8_t Cartridge::readRam(uint16_t address, int bankNumber){
	if (!ifLoaded) {
		return -1;
	}
	return ramBanks[bankNumber][address];
}





void Cartridge::loadRom(const char* filePath){
	std::ifstream file; 
	std::streampos sizeBuffer; //streampos is the type returned by tellg()
	std::streampos begin; 
	std::streampos end; 
	
	file.open(filePath, std::ifstream::binary | std::ios::ate); // ios::ate flag, which means that the get pointer will be positioned at the end of the file.
	if (!file.is_open()) {
		return;
	}
	ifLoaded = true;
	sizeBuffer = file.tellg();
	size = static_cast<int>(sizeBuffer);
	file.seekg(0, std::ios::beg);
	std::vector<uint8_t> buffer(sizeBuffer);
	file.read(reinterpret_cast<char*>(buffer.data()), sizeBuffer);
	
	
	file.close();
	
	
	getRomBankSize(buffer);
	getRamBankSize(buffer);
	
	int ramOffset = romBankNumber * OpitonalbankSize2;
	
	
	
	
	for (int i = 0; i < romBankNumber; i++) {
		romBanks.emplace_back(buffer.begin() + i * bankSize,buffer.begin() + (i+1) * bankSize);
	}
	for (int i = 0; i < ramBankNumber; i++) {
		ramBanks.emplace_back(buffer.begin() + ramOffset + i * OpitonalbankSize2, buffer.begin() + ramOffset + (i + 1) * OpitonalbankSize2);
	}
}




std::vector<unsigned char> Cartridge::getHeader(){
	if (!ifLoaded) {
		exit(1);
	}
	return std::vector<unsigned char>(romBanks[0]);
}

void Cartridge::getHeaderData(){
	if (!ifLoaded) {
		std::cerr << "Error: ROM not loaded.\n";
		return;
	}
	std::cout << "Game title" << "\n";
	std::string gameTitle = "";
	for (int i = 0x0134; i <= 0x0143; i++) {
		if (romBanks[0][i] != 0) {
			gameTitle += static_cast<char>(romBanks[0][i]);
		}
		


	}
	std::cout << gameTitle;
	std::cout << "\n";
	cartridgeType = romBanks[0][0x0147];
	std::cout << "0x" << std::hex<< cartridgeType << "\n";
	int romSize = romBanks[0][0x0148];
	std::cout << "0x" << std::hex << romSize << "\n";
	int ramSize = romBanks[0][0x0149];
	std::cout << "0x" << std::hex << ramSize << "\n";
	uint8_t checksum = 0;
	for (uint16_t address = 0x0134; address <= 0x014C; address++) {
		checksum = checksum - romBanks[0][address] - 1;
		
	}
	std::cout << "0x" << std::hex << (int)checksum;


}

void Cartridge::write(uint16_t address, uint8_t data, int bankNumber){
	if (!ifLoaded) {
		return;
	}
	ramBanks[bankNumber][address] = data;
}

void Cartridge::getRamRomsize(){
	romSize = static_cast<int>(romBanks[0][0x148]);
	ramSize = static_cast<int>(romBanks[0][0x149]);
	
	std::cout << std::hex << romSize << "\n";
	
	exit(1);
}



void Cartridge::getRomBankSize(std::vector<unsigned char> file){
	romSize = static_cast<int>(file[0x0148]);
	int MBC = static_cast<int>(file[0x0147]);

	//for (uint16_t pc = 0x460B; pc <=0x460D; pc++){
	//	std::cout << std::hex << static_cast<int>(file[pc]) << "\n";
	//}

	
	
	
	switch (romSize) {
	case 0: {
		romBankNumber = 2;
		break;
	}
	case 1: {
		romBankNumber = 4;
		break;
	}
	case 2:{
		romBankNumber = 8;
		break;
	}
	case 3: {
		romBankNumber = 16;
		break;
	}
	case 4: {
		romBankNumber = 32;
		break;
	}
	case 5: {
		romBankNumber = 64;
		break;
	}
	case 6: {
		romBankNumber = 128;
		break;
	}
	case 7: {
		romBankNumber = 256;
		break;
	}
	case 8: {
		romBankNumber = 512;
		break;
	}
	default: {
		
		break;
	}
	}

	
}

void Cartridge::getRamBankSize(std::vector<unsigned char> file){
	ramSize = static_cast<int>(file[0x0149]);
	switch(ramSize){
	case 0: {
		ramBankNumber = 0;
		break;
	}
	case 1: {
		ramBankNumber : 0;
		break;
	}
	case 2: {
		ramBankNumber = 1;
		break;
	}
	case 3: {
		ramBankNumber = 4;
		break;
	}
	case 4: {
		ramBankNumber = 16;
		break;
	}
	case 5: {
		ramBankNumber = 8;
		break;
	}
	default: {
		ramBankNumber = 0;
		break;
	}
	}
}


