#pragma once
#include <stdint.h>
#include "SDL2/SDL.h"
#include "vector"



const uint32_t gbPalette[4] = {
	0xFFFFFFFF,
	0xAAAAAAFF,
	0x555555FF,
	0x000000FF
	
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
	inline void loadColor(uint8_t colorId);
	inline bool compare(pixel_T pixel);
	inline void loadPriority(uint8_t Priority);
	inline void loadPalette(uint8_t Palette);
	inline void applyPalette();
	
}Pixel;
typedef struct pFIFO {
	Pixel fifo[8];
	inline Pixel popPixel();
	int pixelCount = 0;
	inline void pushPixel(Pixel pixel);
	inline int size();
	inline void clear();
	void Override(Pixel pixel);
	void loadTransparentPixels();
	void handlePixelLoad(Pixel pixel, int index);
	inline uint8_t peek(int index);
	void mixing(Pixel pixel);

}pixelFifo;

typedef struct gbObject_t {
	uint8_t y;
	uint8_t x;
	uint8_t tileIndex;
	uint8_t att;
	int oamIndex;
	inline void load(uint8_t Y, uint8_t X, uint8_t tileI, uint8_t ATT,int oam);
} gbObject;
typedef struct mainFIFO {
	Pixel fifo[8];
	inline Pixel popPixel();
	int pixelCount = 0;
	inline void pushPixel(Pixel pixel);
	inline bool isEmpty();
	inline void empty();
	inline uint8_t peek();

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
	
	void keyUp(SDL_Event& e);
	void keyDown(SDL_Event& e);
	void loadDMA(bool* d);
	bool* dmaActive;
	
	
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
	
	uint8_t fetcherX = 0;
	uint8_t LCDX = 0;
	
	
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
	
	bool isWindow = false;
	bool spriteFecthActive = false;
	bool lcdOff = false;
	inline void mix();
	FIFO fifo;
	int delayOAM = 0;
	void incDelay();
	pixelFifo bgFifo;
	pixelFifo spriteFifo;
	
	uint8_t pixelX = 0;
	int stepCounter = 0;
	
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
	inline int getMode();
	void updatePPU(uint8_t &ie);
	void handleScanline(uint8_t& ie);
	void oamScan();
	void pixelFetcher();
	inline void stepOne();
	inline void stepOneP();
	inline void stepTwo();
	inline void stepTwoP();
	inline void stepThree();
	inline void stepThreeP();
	void stepFour();
	void stepFourP();
	void handleDraw();
	bool checkSprites();
	inline void pushLCD();
	int initSdl();
	inline void checkEvents(bool& isRunning,uint8_t ie);
	void freeSdl();
	inline uint8_t ppuVramRead(uint16_t address);
	
	
	uint8_t dma = 0xFF;
	
	int delayCycle = 3;
	
	
	
	void reset();
	
	int shorten = 0;
	bool lineZero = false;
	
	
	
	
	
	void setFps();
	int currentFPS = 0;
	Uint32 fpsTimer;
	int pos = 0;
};

inline void updateDisplay(uint32_t * frameBuffer);
//FF44 LY LCD Y coordinate (read only)]

//FF45 LYC: compare

//FF41 STAT: LCD staus:
