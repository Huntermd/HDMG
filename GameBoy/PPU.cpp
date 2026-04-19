#include "PPU.h"
#include "algorithm"

SDL_Renderer* render;
SDL_Window* window;
SDL_Texture* sdlTexture;
uint32_t pixels[23040];
static bool initialized = false;



int PPU::getMode() {

	return STAT & 0x03;
}

void PPU::handleScanline(uint8_t& ie) {
	// Run every CPU cycle (4 T-cycles per M-cycle)
	if (prevLCDC != (LCDC & 0x80)) {

		//STAT &= 0xFC;
		//ifOn = true;
		//prevLCDC = LCDC & 0x80;
	}

	int mode = getMode();

	if (mode == 1) {
		//frameDotCounter++;
		if (LY == 153) {

			if (ppuCounter == 456) {
				updateDisplay(frameBuffer);
				STAT = (STAT & 0xFC) | 0x02;
				ppuCounter = 0;
				LY = 0;

				windowLineCounter = 0;

				currentFrame++;

				frameDotCounter = 0;
				checkEvents(*run, ie);
				return;
			}
		}
		if (LY < 153) {
			if (ppuCounter == 456) {
				LY++;
				//if (ifWindow) windowLineCounter++;
				if (LYC == LY) {
					STAT |= 0x04;
					ie |= 0x02;
				}
				else {
					STAT &= ~0x04;
				}
				ppuCounter = 0;
			}




		}
		//v-blank

		return;
	}
	if (mode == 2) {
		//Handle OAM scan
		if (*hBlank) {
			*hBlank = false;
		}

		oamScan();

		if (ppuCounter == (80)) {

			std::stable_sort(slOamBuffer.begin(), slOamBuffer.end(), [](const gbObject& a, const gbObject& b) {
				return a.x < b.x;
				});

			//spriteFifo.loadTransparentPixels();
			oamIndex = 0;
			STAT |= 0x03;
			stepCounter = 0;





		}

		return;
	}
	if (mode == 3) {

		//Handle drawing
		handleDraw();

		if (LCDX == 160) {
			fetcherX = 0;
			LCDX = 0;
			STAT = (STAT & 0xFC) | 0x00;
			bgFifo.clear();
			spriteFifo.clear();
			fifo.empty();
			*hBlank = true;
			pos = 0;


		}
		return;
	}
	if (mode == 0) {
		//handle h-blank
		if (ppuCounter == (456 - shorten)) {
			//frameDotCounter += ppuCounter;
			if (shorten > 0)shorten = 0;

			ppuCounter = 0;
			LY++;
			if (LY == 0x38) {
				int hey = currentFrame;

			}
			if (ifWindow) windowLineCounter++;
			//updateDisplay(frameBuffer);
			if (LYC == LY) {
				STAT |= 0x04;
				if (STAT & 0x40) {
					ie |= 0x02;
				}
			}
			else {
				STAT &= ~0x04;
			}


			//updateDisplay(frameBuffer);
			if (LY == 144) {
				STAT = (STAT & 0xFC) | 0x01;
				ie |= 0x01;
				ppuCounter = 0;
				slOamBuffer.clear();
				spriteCount = 0;
				oBufferPos = 0;
				oamIndex = 0;
				uint16_t time = frameDotCounter;
				*hBlank = false;


			}
			else {
				STAT = (STAT & 0xFC) | 0x02;
				ppuCounter = 0;
				spriteCount = 0;
				oBufferPos = 0;
				oamIndex = 0;
				slOBufferCount = 0;
				slOamBuffer.clear();
			}

		}
		return;
	}
}

void PPU::oamScan() {
	uint8_t yPos = OAM[oamIndex] - 16;//gives actual Y positon
	uint8_t xPos = OAM[oamIndex + 1] - 8; // gives actual x positon
	uint8_t tileIndex = OAM[oamIndex + 2];
	uint8_t att = OAM[oamIndex + 3];
	uint8_t height;
	if (spriteCount == 10)return;
	if ((LCDC & 0x04)) {
		height = 16;
	}
	else {
		height = 8;
	}


	if (((LY) >= yPos) && (LY) < yPos + height) {
		//output << "Current frame: " << currentFrame << " Y: " << static_cast<int>(yPos) << " X: " << static_cast<int>(xPos) << " Tileindex: " << static_cast<int>(tileIndex) << " att: " << static_cast<int>(att) << " Ly: " << static_cast<int>(LY) << std::endl;
		gbObject obj = gbObject();
		obj.load(yPos, xPos, tileIndex, att, oamIndex);
		slOamBuffer.emplace_back(obj);
		spriteCount++;



	}

	oamIndex += 4;
}

void PPU::pixelFetcher() {
	if (ifWindow && !((LCDX >= (WX - 7) && LY >= WY && (LCDC & 0x20)))) {
		ifWindow = false;
	}
	if ((LCDX >= (WX - 7) && LY >= WY && (LCDC & 0x20)) && !ifWindow) {
		stepCounter = 0;
		ifWindow = true;
		bgFifo.clear();
	}
	if (!objectOn) {
		if (spriteFecthActive) {
			paused = false;
			spriteFecthActive = false;
			stepCounterP = 0;
			spriteFifo.clear();
		}
	}

	if (objectOn != 0 && !spriteFecthActive) {
		//handleSpirte fifo
		if (checkSprites()) {
			spriteFecthActive = true;
			stepCounterP = 0;
			stepCounter = 0;
			paused = true;
			if (spriteFifo.size() < 8) {
				spriteFifo.loadTransparentPixels();
			}

		}



	}



	if (!paused) {
		stepCounter++;
		switch (stepCounter) {
		case 2: {
			stepOne();
			break;
		}
		case 4: {
			stepTwo();
			break;
		}
		case 6: {
			stepThree();
			break;
		}
		case 8: {

			stepFour();
			stepCounter = 0;
			break;
		}
		}
	}





	if (spriteFecthActive) {
		stepCounterP++;
		switch (stepCounterP) {
		case 2: {
			stepOneP();
			break;
		}
		case 4: {
			stepTwoP();
			break;
		}
		case 6: {
			stepThreeP();
			break;
		}
		case 8: {

			stepFourP();
			stepCounterP = 0;
			paused = false;
			spriteFecthActive = false;
			break;
		}
		}
	}


}

void PPU::stepOne() {

	uint16_t defaultAddress;
	bool insideWindow = (fetcherX >= (WX - 7) && LY >= WY && (LCDC & 0x20));//The LCDC & 0x20 changes the access to the window
	//if ((LCDC & 0x08) != 0 && !insideWindow)defaultAddress = 0x9C00;

	defaultAddress = (LCDC & 0x08) ? 0x9C00 : 0x9800;

	if ((LCDC & 0x40) && ifWindow) defaultAddress = 0x9C00;


	if (ifWindow) {

		tileX = (fetcherX - (WX - 7)) / 8; tileY = (windowLineCounter) / 8;



	}
	else {


		tileX = (((SCX + fetcherX) / 8) & 0x1F); tileY = ((LY + SCY) & 0xFF) / 8;
	}
	//tileNumber = ppuVramRead(defaultAddress + (tileY * 32 + tileX));
	if (*isGBC) {

		tileNumber = VRAM[0][(defaultAddress + (tileY * 32 + tileX)) - 0x8000];
		uint8_t attr = VRAM[1][(defaultAddress + (tileY * 32 + tileX)) - 0x8000];
		bgPriority = (attr & 0x80) != 0;
		bgYFlip = (attr & 0x40) != 0;
		bgXFlip = (attr & 0x20) != 0;
		bgBank = ((attr & 0x08) >> 3);
		bgCPalette = (attr & 0x07);

	}
	else {
		tileNumber = ppuVramRead(defaultAddress + (tileY * 32 + tileX));
	}


}

void PPU::stepOneP() {
	uint8_t tileIndex = slOamBuffer[spriteIndex].tileIndex;
	uint8_t spriteY = slOamBuffer[spriteIndex].y;
	uint8_t att = slOamBuffer[spriteIndex].att;
	tileyP = (LY - spriteY);
	int spriteHeight = (LCDC & 0x04) ? 16 : 8;
	if (att & 0x40) {
		tileyP = (spriteHeight - 1) - tileyP;
	}

	if ((LCDC & 0x04) == 0) {
		tileNumberP = (tileIndex);
	}
	if ((LCDC & 0x04) != 0) {
		if (tileyP < 8) {
			tileNumberP = ((tileIndex & 0xFE));
		}
		else {
			tileNumberP = ((tileIndex | 0x01));
			tileyP -= 8;
		}
	}
	if (*isGBC) {
		objBank = ((att & 0x08) >> 3);
	}


}

void PPU::stepTwo() {
	uint8_t lineInTile = (LY + SCY) & 7;
	if (bgYFlip && *isGBC) {
		lineInTile = 7 - ((LY + SCY) % 8);
	}

	uint16_t addressess;
	uint16_t base;
	base = (LCDC & 0x10) == 0 && tileNumber <= 127 ? 0x9000 : 0x8000;
	addressess = base + (tileNumber * 16);
	if (*isGBC) {
		dataLow = VRAM[bgBank][(addressess + (lineInTile * 2)) - 0x8000];
	}
	else {
		dataLow = ppuVramRead(addressess + (lineInTile * 2));
	}

	//

}

void PPU::stepTwoP() {
	uint8_t lineInTile = (LY + SCY) & 7;
	uint16_t addressess = 0x8000 + (tileNumberP * 16);
	if (*isGBC) {
		dataLowP = VRAM[objBank][(addressess + (tileyP * 2)) - 0x8000];
	}
	else {
		dataLowP = ppuVramRead(addressess + (tileyP * 2));
	}

	//

}

void PPU::stepThree() {
	uint8_t lineInTile = (LY + SCY) & 7;
	if (bgYFlip && *isGBC) {
		lineInTile = 7 - ((LY + SCY) % 8);
	}
	uint16_t addressess;
	uint16_t base;
	base = (LCDC & 0x10) == 0 && tileNumber <= 127 ? 0x9000 : 0x8000;
	addressess = base + (tileNumber * 16);
	if (*isGBC) {
		dataHigh = VRAM[bgBank][(addressess + (lineInTile * 2) + 1) - 0x8000];
	}
	else {
		dataHigh = ppuVramRead(addressess + (lineInTile * 2) + 1);
	}

	//



}

void PPU::stepThreeP() {
	uint8_t lineInTile = (LY + SCY) & 7;
	uint16_t addressess = 0x8000 + ((tileNumberP * 16));
	if (*isGBC) {
		dataHighP = VRAM[objBank][(addressess + (tileyP * 2) + 1) - 0x8000];
	}
	else {
		dataHighP = ppuVramRead(addressess + (tileyP * 2) + 1);
	}

	//
}

void PPU::stepFour() {
	if (!(LCDC & 0x01) && !*isGBC) {

		for (int i = 0; i < 8; i++)
		{
			Pixel p;
			p.loadColor(0);
			p.loadPriority(0);
			bgFifo.pushPixel(p);


		}

	}
	else {

		if (bgFifo.size() == 0) {

			if (fetcherX == 0 && !ifWindow) {
				uint8_t offset = (SCX + fetcherX) % 8;
				if (bgXFlip && *isGBC) {
					for (int i = 0; i < 8 - offset; i++) {
						uint8_t low = (dataLow >> i) & 1;
						uint8_t high = (dataHigh >> i) & 1;
						uint8_t colorIndex = (high << 1) | low;
						Pixel p;
						if (*isGBC) {
							p.loadPalette(bgCPalette);
						}
						else {
							p.loadPalette(colorId);
						}
						if (*isGBC)p.loadPriority(bgPriority);
						p.loadColor(colorIndex);
						bgFifo.pushPixel(p);
						fetcherX++;

					}
					return;
				}
				for (int i = 7 - offset; i >= 0; i--) {
					uint8_t low = (dataLow >> i) & 1;
					uint8_t high = (dataHigh >> i) & 1;
					uint8_t colorIndex = (high << 1) | low;
					Pixel p;
					if (*isGBC) {
						p.loadPalette(bgCPalette);
					}
					else {
						p.loadPalette(colorId);
					}
					p.loadColor(colorIndex);
					if (*isGBC)p.loadPriority(bgPriority);
					bgFifo.pushPixel(p);
					fetcherX++;
				}
			}
			else {
				if (bgXFlip && *isGBC) {
					for (int i = 0; i < 8; i++) {
						uint8_t low = (dataLow >> i) & 1;
						uint8_t high = (dataHigh >> i) & 1;
						uint8_t colorIndex = (high << 1) | low;
						Pixel p;
						if (*isGBC) {
							p.loadPalette(bgCPalette);
						}
						else {
							p.loadPalette(colorId);
						}
						if (*isGBC)p.loadPriority(bgPriority);
						p.loadColor(colorIndex);
						bgFifo.pushPixel(p);
						fetcherX++;

					}
					return;
				}
				for (int i = 7; i >= 0; i--) {
					uint8_t low = (dataLow >> i) & 1;
					uint8_t high = (dataHigh >> i) & 1;
					uint8_t colorIndex = (high << 1) | low;
					Pixel p;
					if (*isGBC) {
						p.loadPalette(bgCPalette);
					}
					else {
						p.loadPalette(colorId);
					}
					if (*isGBC)p.loadPriority(bgPriority);
					p.loadColor(colorIndex);
					bgFifo.pushPixel(p);
					fetcherX++;
				}
			}

		}
		else {
			int size = bgFifo.size();
			//stepCounter=4;
		}

	}

}

void PPU::stepFourP() {
	uint8_t attribute = slOamBuffer[spriteIndex].att;
	uint8_t shadeBits;
	uint8_t colorIsh = attribute & 0x07;
	if (attribute & 0x10) {
		shadeBits = obj1;
	}
	else {
		shadeBits = obj0;
	}


	if ((attribute & 0x20)) {
		for (int i = 0; i < 8; i++) {
			uint8_t low = (dataLowP >> i) & 1;
			uint8_t high = (dataHighP >> i) & 1;
			uint8_t colorIndex = (high << 1) | low;
			Pixel p;
			p.isObj = true;
			if (*isGBC) {
				p.loadPalette(colorIsh);
			}
			else {
				p.loadPalette(shadeBits);
			}
			//if (colorIndex == 0)continue;
			p.loadColor(colorIndex);
			p.loadPriority((attribute & 0x80) != 0);
			spriteFifo.handlePixelLoad(p, i, *isGBC);
			//spriteFifo.pushPixel(p);
			//spriteFifo.mixing(p);

		}
	}
	else {
		int x = 0;
		for (int i = 7; i >= 0; i--) {
			uint8_t low = (dataLowP >> i) & 1;
			uint8_t high = (dataHighP >> i) & 1;
			uint8_t colorIndex = (high << 1) | low;
			Pixel p;
			p.isObj = true;
			if (*isGBC) {
				p.loadPalette(colorIsh);
			}
			else {
				p.loadPalette(shadeBits);
			}
			//if (colorIndex == 0)continue;
			p.loadColor(colorIndex);
			p.loadPriority((attribute & 0x80) != 0);
			spriteFifo.handlePixelLoad(p, x, *isGBC);
			x++;
			//spriteFifo.pushPixel(p);
			//spriteFifo.mixing(p);

		}
	}
}

void PPU::handleDraw() {


	if (bgFifo.size() <= 8) {
		pixelFetcher();

	}
	if (bgFifo.size() > 0 && !paused) {//This creates problems with the left eye if !paused

		pushLCD();


	}

}

bool PPU::checkSprites() {
	uint8_t spriteHeight = (LCDC & 0x04) ? 16 : 8;
	int bestSpriteIndex = -1;
	uint8_t bestSpriteX = 255;
	for (int i = pos; i < slOamBuffer.size(); i++) {
		uint8_t spriteY = slOamBuffer[i].y;
		uint8_t spriteX = slOamBuffer[i].x;


		if (LY < spriteY || LY >= spriteY + spriteHeight) continue;
		if (spriteX >= 168) continue;
		if (LCDX >= spriteX) {


			spriteIndex = i;
			pos = i + 1;
			return true;
			//output << "Current frame: " << currentFrame << " Y: " << static_cast<int>(spriteY) << " X: " << static_cast<int>(spriteX) << " Tileindex: " << static_cast<int>(tileIndex) << " att: " << static_cast<int>(att) << " Ly: " << static_cast<int>(LY) << std::endl;


		};


	}

	if (bestSpriteIndex != -1) {
		spriteIndex = bestSpriteIndex;
		return true;
	}
	return false;
}

void PPU::pushLCD() {

	if (bgFifo.size() != 0) {
		if (spriteFifo.size() != 0 && objectOn) {

			mix();
		}
		else {
			fifo.pushPixel(bgFifo.popPixel());

		}
	}

	if (!fifo.isEmpty()) {


		Pixel pixel = fifo.popPixel();
		frameBuffer[LY * 160 + LCDX] = getColors(pixel);
		LCDX++;
	}

}

int PPU::initSdl() {

	if (initialized) return 0;

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {

		return -1;
	}
	window = SDL_CreateWindow("HDMG", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 576, SDL_WINDOW_SHOWN);
	if (window == NULL)
	{

		SDL_Quit();
		return -1;
	}
	render = SDL_CreateRenderer(window, -1, 0); // what renders the new pixels to screen
	if (render == NULL) {

		SDL_Quit();
		return -1;
	}
	SDL_RenderSetLogicalSize(render, 640, 576);
	sdlTexture = SDL_CreateTexture(render,
		SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_STREAMING,
		160, 144);
	initialized = true;
	return 0;
}

void PPU::checkEvents(bool& isRunning, uint8_t ie) {
	SDL_Event e;
	if (SDL_PollEvent(&e) == 0)return;
	if (e.type == SDL_QUIT) {
		freeSdl();
		SDL_Quit();
		isRunning = false;

		return;
	}

	if (e.type == SDL_KEYDOWN) {
		keyDown(e);
		if (e.key.keysym.sym == SDLK_ESCAPE) {
			freeSdl();
			SDL_Quit();
			isRunning = false;
			return;
		}
	}
	if (e.type == SDL_KEYUP) {
		keyUp(e);

	}

}

void PPU::freeSdl() {
	SDL_DestroyRenderer(render);
	SDL_DestroyTexture(sdlTexture);
	SDL_DestroyWindow(window);
}

uint8_t PPU::ppuVramRead(uint16_t address)
{
	if (*isGBC) {
		return VRAM[VBK & 0x01][address - 0x8000];
	}
	if (address >= 0x8000 && address <= 0x9FFF) {

		return vRam[address - 0x8000];
	}



}

uint32_t PPU::getColors(Pixel c) {
	if (*isGBC) {
		if (c.isObj) {

			return objCram.getColor(c.palette * 4 + c.color);
		}
		else {
			//if (c.color == 0)return bgCram.getColor(c.palette);
			return bgCram.getColor(c.palette * 4 + c.color);
		}
	}

	return gbPalette[c.color];
}



void PPU::reset() {
	LY = 0;
	fetcherX = 0;
	LCDX = 0;
	bgFifo.clear();
	spriteFifo.clear();
	fifo.empty();

	slOamBuffer.clear();
	spriteCount = 0;
	oBufferPos = 0;
	oamIndex = 0;
	stepCounter = 0;
	stepCounterP = 0;
	*hBlank = true;
}

void PPU::setFps() {
	fpsTimer = SDL_GetTicks();
}

void PPU::dmgInit() {
	for (int i = 0; i < 8192; i++) {
		vRam[i] = 0;

	}
}

void PPU::gbcInit() {
	dma = 0x00;
	//VRAM.resize(2);
	for (int i = 0; i < 2; i++) {
		std::vector<uint8_t> temp(8192, 0);
		VRAM.push_back(temp);

	}
}




void PPU::updatePPU(uint8_t& ie) {
	if (lcdOff)return;

	//if (delayCycle > 0){delayCycle--; return;
	for (size_t i = 0; i < 4; i++)
	{
		ppuCounter++;
		if (lcdJustTurnON) {
			LcdEnableCounter++;
			if (LcdEnableCounter == 80) {
				lcdJustTurnON = false;
				*hBlank = false;
				STAT = (STAT & 0xFC) | 0x03;
				shorten = 4;
				continue;
			}
			continue;
		}

		//frameDotCounter++;
		handleScanline(ie);

	}



}

PPU::PPU() {

}

void PPU::loadRun(bool* r) {
	this->run = r;
}

void PPU::loadBlank(bool* b) {
	this->hBlank = b;
}

void PPU::keyUp(SDL_Event& e) {
	switch (e.key.keysym.sym) {
	case SDLK_a: {
		joypadState |= (1 << 0);
		break;

	}
	case SDLK_b: {
		joypadState |= (1 << 1);
		break;

	}
	case SDLK_s: {
		joypadState |= (1 << 3);
		break;
	}
	case SDLK_d: {
		joypadState |= (1 << 2);
		break;
	}
	case SDLK_RIGHT: {
		joypadState |= (1 << 4);
		break;
	}
	case SDLK_LEFT: {
		joypadState |= (1 << 5);
		break;
	}
	case SDLK_UP: {
		joypadState |= (1 << 6);
		break;

	}
	case SDLK_DOWN: {
		joypadState |= (1 << 7);
		break;

	}
	}

}

void PPU::keyDown(SDL_Event& e) {
	switch (e.key.keysym.sym) {
	case SDLK_a: {
		joypadState &= ~(1 << 0);
		break;

	}
	case SDLK_b: {
		joypadState &= ~(1 << 1);
		break;

	}
	case SDLK_s: {
		joypadState &= ~(1 << 3);
		break;
	}
	case SDLK_d: {
		joypadState &= ~(1 << 2);
		break;
	}
	case SDLK_RIGHT: {
		joypadState &= ~(1 << 4);
		break;
	}
	case SDLK_LEFT: {
		joypadState &= ~(1 << 5);
		break;
	}
	case SDLK_UP: {
		joypadState &= ~(1 << 6);
		break;

	}
	case SDLK_DOWN: {
		joypadState &= ~(1 << 7);
		break;

	}
	}

}

void PPU::loadDMA(bool* d) {
	this->dmaActive = d;
}

void PPU::loadGBC(bool* c) {
	this->isGBC = c;
	ppuInit();
}

void PPU::mix() {

	Pixel sp = spriteFifo.popPixel();
	Pixel bg = bgFifo.popPixel();

	if (*isGBC) {
		// FIX: rewrite GBC priority rules from scratch. Previous code treated
		// `sp.priority == 1` (OAM attr bit 7) as "push sprite", which is the
		// opposite of what the hardware does. Also, when LCDC.0 == 0 on GBC
		// the BG loses all master priority and sprites always win.

		// 1) Transparent sprite pixel -> BG wins.
		if (sp.color == 0) {
			fifo.pushPixel(bg);
			return;
		}
		// 2) LCDC.0 == 0 on GBC: BG master priority disabled -> sprite wins.
		if (!(LCDC & 0x01)) {
			fifo.pushPixel(sp);
			return;
		}
		// 3) BG color 0 is always "behind" any opaque sprite pixel.
		if (bg.color == 0) {
			fifo.pushPixel(sp);
			return;
		}
		// 4) Opaque on opaque: if either bg-map priority (bit 7 of BG attr)
		//    OR sprite priority (bit 7 of OAM attr) is set, BG wins.
		if (bg.priority || sp.priority) {
			fifo.pushPixel(bg);
		}
		else {
			fifo.pushPixel(sp);
		}
		return;
	}

	// DMG path unchanged
	if (bg.compare(sp)) {
		fifo.pushPixel(bg);
	}
	else {
		if (!*isGBC) sp.applyPalette();
		fifo.pushPixel(sp);
	}

}

void PPU::incDelay() {
	delayOAM = 0;
}

uint8_t PPU::ppuRead(uint16_t address)
{
	switch (address) {
	case 0xFF00: {
		uint8_t result = 0xCF;
		if (!(joypadSelect & 0x10)) {
			result &= ((joypadState >> 4) | 0xF0);
		}
		if (!(joypadSelect & 0x20)) {
			result &= ((joypadState & 0x0F) | 0xF0);
		}
		result = (result & 0x0F) | joypadSelect;
		return result;
	}
	case 0xFF40: {
		return LCDC;
	}
	case 0xFF41: {
		return STAT;
	}
	case 0xFF42: {
		return SCY;
	}
	case 0xFF43: {
		return SCX;
	}
	case 0xFF44: {
		return LY;
	}
	case 0xFF45: {

		return LYC;
	}
	case 0xFF46: {
		return dma;
	}
	case 0xFF47: {
		return colorId;
	}
	case 0xFF48: {
		return obj0;
	}
	case 0xFF49: {
		return obj1;
	}
	case 0xFF4A: {
		return WY;
	}
	case 0xFF4B: {
		return WX;
	}
	case 0xFF4C: {
		return 0x00;
	}
	case 0xFF4D: {
		return KEY1 | 0x7E;
	}
	case 0xFF4F: {
		return VBK | 0xFE;
	}
	case 0xFF51: {
		return 0x00;
	}
	case 0xFF52: {
		return 0x00;
	}
	case 0xFF53: {
		return 0x00;
	}
	case 0xFF56: {
		return 0x00;
	}
	case 0xFF54: {
		return 0x00;
	}
	case 0xFF55: {
		return HDMA5;
	}
	case 0xFF68: {
		return bgCram.readByte();
	}
	case 0xFF69: {
		return bgCram.read();
	}
	case 0xFF6A: {
		return objCram.readByte();
	}
	case 0xFF6B: {
		return objCram.read();
	}
	case 0xFF6C: {
		return OPRI;
	}
	}
	return 0xFF;
}

void PPU::ppuWrite(uint16_t address, uint8_t data, uint8_t& ie) {

	switch (address) {
	case 0xFF00: {
		joypadSelect = data & 0x30;

		break;
	}
	case 0xFF40: {
		bool wasOn = (LCDC & 0x80);
		bool nowOn = (data & 0x80);
		bool wasOnS = (LCDC & 0x02);
		bool nowOns = (data & 0x02);
		if (!wasOnS && nowOns) {
			objectOn = true;
		}
		if (wasOnS && !nowOns) {
			objectOn = false;

			//STAT = (STAT & 0xFC) | 0x00;
		}
		if (!wasOn && nowOn) {
			lcdJustTurnON = true;
			lcdOff = false;
			LcdEnableCounter = 0;
			STAT = (STAT & 0xFC) | 0x00;
			delayCycle = 2;
		}
		if (wasOn && !nowOn) {
			lcdOff = true;
			*hBlank = true;
			reset();
			//STAT = (STAT & 0xFC) | 0x00;
		}
		LCDC = data;

		return;
	}
	case 0xFF41: {
		STAT = (STAT & 0x07) | (data & 0xF8);
		return;
	}
	case 0xFF42: {
		SCY = data;
		return;
	}
	case 0xFF43: {
		SCX = data;
		return;
	}

	case 0xFF45: {
		LYC = data;

		if (LYC == LY) {
			STAT |= 0x04;
			if (STAT & 0x40) {
				ie |= 0x02;
			}
		}
		else {
			STAT &= ~0x04;
		}

		return;
	}
	case 0xFF46: {
		dma = data;
		return;
	}
	case 0xFF47: {
		colorId = data;
		return;
	}
	case 0xFF48: {
		obj0 = data;
		return;
	}
	case 0xFF49: {
		obj1 = data;
		return;
	}
	case 0xFF4A: {
		WY = data;
		return;
	}
	case 0xFF4B: {
		WX = data;
		return;
	}
	case 0xFF4C: {
		KEY0 = data;
		return;
	}
	case 0xFF4D: {
		KEY1 = (KEY1 & 0x80) | (data & 0x01);
		return;
	}
	case 0xFF4F: {
		VBK = data & 0x01;
		return;
	}
	case 0xFF51: {
		HDMA1 = data;
		return;
	}
	case 0xFF52: {
		HDMA2 = data & 0xF0;
		return;
	}
	case 0xFF53: {
		HDMA3 = data & 0x1F;
		return;
	}
	case 0xFF56: {
		return;
	}
	case 0xFF54: {
		HDMA4 = data & 0xF0;
		return;
	}
	case 0xFF55: {
		// FIX #1: Previously this was "HDMA5 == data;" -- a comparison whose
		// result was discarded. It was also the wrong place to handle HDMA5
		// because GameBoy::cpuWrite intercepts 0xFF55 before ppuWrite ever
		// sees it. Just return.
		return;
	}
	case 0xFF68: {
		bgCram.updateVariables(data);
		return;
	}
	case 0xFF69: {
		bgCram.write(data);
		return;
	}
	case 0xFF6A: {
		objCram.updateVariables(data);
		return;
	}
	case 0xFF6B: {
		objCram.write(data);
		return;
	}
	case 0xFF6C: {
		int hey = 0;
		return;
	}

	}

}

uint8_t PPU::readOAM(uint16_t address) {
	int mode = getMode();
	if (mode == 2 || mode == 3)return 0xFF;
	if (*dmaActive)return 0xFF;
	if (delayOAM > 0) {
		delayOAM--;
		return 0xFF;
	}
	return OAM[address - 0xFE00];
}

void PPU::writeOAM(uint16_t address, uint8_t data) {


	OAM[address - 0xFE00] = data;
}

uint8_t PPU::readVRAM(uint16_t address) {

	if (*isGBC) {
		return VRAM[VBK & 0x01][address - 0x8000];
	}

	if (address >= 0x8000 && address <= 0x9FFF) {
		return vRam[address - 0x8000];
	}


}

void PPU::writeVRAM(uint16_t address, uint8_t data) {
	if (*isGBC) {
		if (address > 0x9FFF) {
			int hey = 0;
		}
		VRAM[VBK & 0x01][address - 0x8000] = data;
		return;
	}

	if (address >= 0x8000 && address <= 0x9FFF) {
		vRam[address - 0x8000] = data;
		return;
	}

}

void PPU::ppuInit() {
	if (*isGBC) {
		gbcInit();
	}
	else {
		dmgInit();
	}

	for (int i = 0; i < 23040; i++) {
		frameBuffer[i] = 0;

	}
	for (size_t i = 0; i < 160; i++) {
		OAM[i] = 0;

	}

}

void updateDisplay(uint32_t* frameBuffer) {

	SDL_UpdateTexture(sdlTexture, NULL, frameBuffer, 160 * sizeof(Uint32));

	SDL_RenderClear(render); // clears screen?? or maybe just the renderer
	SDL_RenderCopy(render, sdlTexture, NULL, NULL);
	// Up until now everything was drawn behind the scenes.
	// This will show the new, red contents of the window.
	SDL_RenderPresent(render);//Updates the window

}

Pixel pFIFO::popPixel() {


	Pixel pixel = fifo[0];

	pixelCount--;
	for (int i = 0; i < pixelCount; i++) {
		fifo[i] = fifo[i + 1];

	}
	return pixel;
}

void pFIFO::pushPixel(Pixel pixel) {
	if (pixelCount == 8)return;
	fifo[pixelCount] = pixel;
	pixelCount++;
}

int pFIFO::size() {
	return pixelCount;
}

void pFIFO::clear() {
	pixelCount = 0;

}

void pFIFO::Override(Pixel pixel) {
}

void pFIFO::loadTransparentPixels() {
	for (int i = pixelCount; i < 8; i++) {
		Pixel p;
		p.isObj = true;
		p.loadColor(0);
		p.loadPriority(0);
		p.loadPalette(0);
		pushPixel(p);
	}
}

void pFIFO::handlePixelLoad(Pixel pixel, int index, bool gbc) {
	if (gbc) {
		fifo[index] = pixel;
		return;
	}

	if ((fifo[index].color == 0 && pixel.color != 0) || (pixel.priority < fifo[index].priority)) {
		fifo[index] = pixel;
	}
}

uint8_t pFIFO::peek(int index)
{
	return fifo[index].color;
}

void pFIFO::mixing(Pixel pixel) {
	Pixel p = popPixel();
	if ((p.color == 0 && pixel.color != 0) || (pixel.priority < p.priority)) {
		pushPixel(pixel);
	}
	else {
		pushPixel(p);
	}

}

Pixel mainFIFO::popPixel() {


	Pixel pixel = fifo[0];
	pixelCount--;
	for (int i = 0; i < pixelCount; i++) {
		fifo[i] = fifo[i + 1];

	}
	return pixel;
}

void mainFIFO::pushPixel(Pixel pixel) {
	if (pixelCount == 8)return;
	fifo[pixelCount] = pixel;
	pixelCount++;
}

bool mainFIFO::isEmpty() {
	return (pixelCount == 0);
}

void mainFIFO::empty()
{
	pixelCount = 0;
}

uint8_t mainFIFO::peek()
{
	return 0;
}

void pixel_T::loadColor(uint8_t colorId) {
	color = colorId;

}

bool pixel_T::compare(pixel_T pixel) {
	if (pixel.isObj) {
		if (pixel.color == 0) {
			return true;
		}
		if (pixel.priority != 0 && color != 0) {
			return true;
		}




		return false;
	}
	return true;
}

void pixel_T::loadPriority(uint8_t Priority) {
	priority = Priority;
}

void pixel_T::loadPalette(uint8_t Palette) {
	palette = Palette;
}

void pixel_T::applyPalette() {


	uint8_t shadeBit = (palette >> (color * 2)) & 0x03;
	color = shadeBit;

}

void gbObject_t::load(uint8_t Y, uint8_t X, uint8_t tileI, uint8_t ATT, int oam) {
	att = ATT;
	y = Y;
	x = X;
	tileIndex = tileI;
	oamIndex = oam;
}

uint32_t color_T::getColor(uint8_t c)
{
	uint32_t color = palette[c];
	if (color == 0x0000FFFF) {
		int hey = 0;
	}
	return palette[c];
}

uint8_t color_T::read() {
	// FIX #7: Per GBC spec, BCPD/OCPD auto-increment only happens on WRITES,
	// not on reads. The previous version incremented on every read, which
	// could silently shift the palette index the next time the CPU wrote.
	return ram[address & 0x3F];
}

uint8_t color_T::readByte() {
	// FIX #7: bit 6 of BCPS/OCPS is unused and always reads back as 1.
	return autoIncrement | address | 0x40;
}

void color_T::write(uint8_t c) {
	// FIX #9: CRAM is only 64 bytes (32 colors x 2 bytes). Without masking,
	// autoIncrement walks `address` off the end of both `ram[64]` and
	// (via loadColor's `colorIndex = (address-1)/2`) `palette[32]`, which is
	// a real out-of-bounds write -- very likely cause of the crash after
	// character creation, where Crystal hammers CRAM with many palette writes.
	ram[address & 0x3F] = c;
	if ((address & 1) == 1) {
		loadColor();
	}
	if (autoIncrement) address = (address + 1) & 0x3F;
}

void color_T::updateVariables(uint8_t d) {
	autoIncrement = (d & 0x80);
	address = (d & 0x3F);
}

void color_T::loadColor() {
	// FIX #8b: previously the function used `address` directly, assuming
	// the high byte had just been written. That's true for the common
	// "write low, write high" auto-increment pattern, but we call this
	// from write() only on odd addresses anyway, so `pair = address & ~1`
	// (= address-1) names the low byte and `pair+1` names the high byte.
	// This is clearer and also safe if the address is ever even.
	uint8_t pair = address & 0x3E;           // low byte of color pair
	uint16_t color = (ram[pair + 1] << 8) | ram[pair];
	// Real CGB stores BGR555: bits 0-4 = R, 5-9 = G, 10-14 = B.
	uint8_t r = ((color >> 0) & 0x1F);
	uint8_t g = ((color >> 5) & 0x1F);
	uint8_t b = ((color >> 10) & 0x1F);

	// NOTE on Bug #8a (CGB color correction): the `* 255 / 31` expansion
	// below is the naive conversion. Real CGB LCDs are dimmer/warmer, and
	// games were designed for that. If colors look over-saturated, apply a
	// correction matrix (Gambatte / SameBoy publish the coefficients). Left
	// as naive for now since it isn't a crash cause.
	uint8_t R = r * 255 / 31;
	uint8_t G = g * 255 / 31;
	uint8_t B = b * 255 / 31;

	uint8_t colorIndex = pair / 2;           // 0..31
	uint32_t packed = (R << 24) | (G << 16) | (B << 8) | 0xFF;
	palette[colorIndex] = packed;
}