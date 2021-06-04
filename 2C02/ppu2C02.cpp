#include "ppu2C02.hpp"

uint8_t ppu2C02::cpuRead(uint16_t addr, bool rdonly) {
    uint8_t data = 0x00;

    switch (addr) {
        case 0x0000: //control
            break;
        case 0x0001: //mask
            break;
        case 0x0002: //status
            break;
        case 0x0003: //OAM address
            break;
        case 0x0004: //OAM data
            break;
        case 0x0005: //scroll
            break;
        case 0x0006: //PPU address
            break;
        case 0x0007: //PPU data
            break;
	}

    return data;
}

void ppu2C02::cpuWrite(uint16_t addr, uint8_t data) {
    switch (addr) {
        case 0x0000: //control
            break;
        case 0x0001: //mask
            break;
        case 0x0002: //status
            break;
        case 0x0003: //OAM address
            break;
        case 0x0004: //OAM data
            break;
        case 0x0005: //scroll
            break;
        case 0x0006: //PPU address
            break;
        case 0x0007: //PPU data
            break;
	}
}

uint8_t ppu2C02::ppuRead(uint16_t addr, bool rdonly) {
    uint8_t data = 0x00;

    addr &= 0x3FFF;

	// if (cart->ppuRead(addr, data)) {

	// }

    return data;
}

void ppu2C02::ppuWrite(uint16_t addr, uint8_t data) {
    addr &= 0x3FFF;

	// if (cart->ppuWrite(addr, data)) {

	// }
}

void ppu2C02::ConnectCartridge(const std::shared_ptr<Cartridge>& cartridge) {
	this->cart = cartridge;
}

void ppu2C02::clock() {
	// // Fake some noise for now
	// sprScreen.SetPixel(cycle - 1, scanline, palScreen[(rand() % 2) ? 0x3F : 0x30]);

	// // Advance renderer - it never stops, it's relentless
	// cycle++;
	// if (cycle >= 341)
	// {
	// 	cycle = 0;
	// 	scanline++;
	// 	if (scanline >= 261)
	// 	{
	// 		scanline = -1;
	// 		frame_complete = true;
	// 	}
	// }
}