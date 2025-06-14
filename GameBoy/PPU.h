#pragma once
#include <stdint.h>
#include "SDL2/SDL.h"
#include "vector"


typedef struct joypad_T {
	uint8_t actionSelect=0;
	uint8_t directionSelect=0;
}JOYP;
const uint32_t gbPalette[4] = {
	0xFFFFFFFF,
	0xAAAAAAFF,
	0x555555FF,
	0x000000FF
	
};
const uint32_t objPalette[4]{
	0xFFFFFFFF,
	0XFFAAAAAA,
	0xFF555555,
	0xFF000000
};
/*
When handling backGround pixels we will only need the color and the other will be set to 0
and isObj will be set to false
But if it is object the values will be set where needed.
Palette is more of a local varible for sprite fetching
*/
typedef struct pixel_T {

	bool isObj = false;
	uint8_t color = 0x00;
	uint8_t palette = 0x00;
	uint8_t priority = 0x00;
	void loadColor(uint8_t colorId);
	bool compare(pixel_T pixel);
	void loadPriority(uint8_t Priority);
	void loadPalette(uint8_t Palette);
	void applyPalette();
	
}Pixel;
typedef struct pFIFO {
	Pixel fifo[8];
	Pixel popPixel();
	int pixelCount = 0;
	void pushPixel(Pixel pixel);
	int size();
	void clear();
	void Override(Pixel pixel);
	void loadTransparentPixels();
	void handlePixelLoad(Pixel pixel, int index);
	uint8_t peek(int index);
	void mixing(Pixel pixel);

}pixelFifo;

typedef struct gbObject_t {
	uint8_t y;
	uint8_t x;
	uint8_t tileIndex;
	uint8_t att;
	int oamIndex;
	void load(uint8_t Y, uint8_t X, uint8_t tileI, uint8_t ATT,int oam);
} gbObject;
typedef struct mainFIFO {
	Pixel fifo[8];
	Pixel popPixel();
	int pixelCount = 0;
	void pushPixel(Pixel pixel);
	bool isEmpty();
	void empty();
	uint8_t peek();

}FIFO;

class PPU {
public:
	PPU();
	~PPU() {
	};
	uint8_t joypadState = 0xFF;
	uint8_t joypadSelect = 0x30;
	void loadRun(bool* r);
	bool* run;
	JOYP jp;
	void keyUp(SDL_Event& e);
	void keyDown(SDL_Event& e);
	void loadDMA(bool* d);
	bool* dmaActive;
	int dotAmount = 3;
	std::vector<int> already;
	bool objectOn = false;
	uint8_t vRam[8192];
	uint32_t frameBuffer[160 * 144];
	uint8_t STAT = 0x84;
	uint8_t LY =0;
	uint8_t LCDC = 0x91;
	uint8_t prevLCDC = 0x00;
	uint8_t SCY = 0;
	uint8_t SCX = 0;
	uint8_t WY = 0;
	uint8_t WX = 0;
	uint8_t OAM[40 * 4];
	std::vector<gbObject> slOamBuffer;
	uint8_t LYC = 0;
	uint8_t colorId = 0xFC;
	uint16_t ppuCounter = 0;
	uint32_t frameDotCounter = 0;
	uint8_t obj0 = 0x00;
	uint8_t obj1 = 0x00;
	uint8_t joypad = 0xCF;
	int slOBufferCount = 0;
	int oamIndex = 0;
	int oBufferPos = 0;
	int spriteCount = 0;
	bool lcdJustTurnON = true;
	int LcdEnableCounter = 0;
	bool log = false;
	uint8_t fetcherX = 0;
	uint8_t LCDX = 0;
	uint8_t prevFecthX = 0;
	uint8_t fetcherY = 0;
	uint8_t tileNumber = 0;
	uint8_t tileNumberP = 0;
	uint8_t tileAddress = 0;
	uint8_t tileX = 0;
	uint8_t tileY = 0;
	uint8_t tileyP = 0;
	uint8_t dataLow = 0;
	uint8_t dataHigh = 0;
	uint8_t dataLowP = 0;
	uint8_t dataHighP = 0;
	uint8_t windowLineCounter = 0;
	bool ifOn = false;
	bool paused = false;
	bool windowPause = false;
	bool isWindow = false;
	bool spriteFecthActive = false;
	bool lcdOff = false;
	void mix();
	FIFO fifo;
	int delayOAM = 0;
	void incDelay();
	pixelFifo bgFifo;
	pixelFifo spriteFifo;
	pixelFifo bgBuffer;
	uint8_t pixelX = 0;
	int stepCounter = 0;
	int stepsUntilPause = 0;
	int stepCounterP = 0;
	int spriteIndex = 0;
	bool ifWindow = false;
	int currentFrame = 0;
	uint8_t ppuRead(uint16_t address);
	void ppuWrite(uint16_t address, uint8_t data, uint8_t& ie);
	uint8_t readOAM(uint16_t address);
	void writeOAM(uint16_t address, uint8_t data);
	uint8_t readVRAM(uint16_t address);
	void writeVRAM(uint16_t address, uint8_t data);
	void ppuInit();
	int getMode();
	void updatePPU(uint8_t &ie);
	void handleScanline(uint8_t& ie);
	void oamScan();
	void pixelFetcher();
	void stepOne();
	void stepOneP();
	void stepTwo();
	void stepTwoP();
	void stepThree();
	void stepThreeP();
	void stepFour();
	void stepFourP();
	void handleDraw();
	bool checkSprites();
	void pushLCD();
	int initSdl();
	void checkEvents(bool& isRunning,uint8_t ie);
	void freeSdl();
	uint8_t ppuVramRead(uint16_t address);
	uint16_t returnColors(Pixel pixel);
	std::vector<int> usedSprites;
	uint8_t dma = 0xFF;
	
	int delayCycle = 3;
	int bgX = 0;
	bool bgFetchEnable = true;
	uint8_t prevTile = 0xFF;
	void reset();
	int counter = 0;
	int shorten = 0;
	bool lineZero = false;
	bool initalTile = false;
	int h_blank = 460;//Line zero h-blank
	bool modeChange = false;
	int changeMode = 0;
	int cycleOffset = 1;
	void setFps();
	int currentFPS = 0;
	Uint32 fpsTimer;
};

void updateDisplay(uint32_t * frameBuffer);
//FF44 LY LCD Y coordinate (read only)]

//FF45 LYC: compare

//FF41 STAT: LCD staus:
