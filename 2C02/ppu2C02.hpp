#ifndef PPU2CO2_HPP
#define PPU2CO2_HPP

#include <cstdint>
#include <memory>

#include "../cartridge/cartridge.hpp"

class ppu2C02 {
    public:
        ppu2C02();
        ~ppu2C02();

    private:
        uint8_t tblName[2][1024];
        uint8_t	tblPalette[32];
	    uint8_t tblPattern[2][4096]; //useless

    public:
        uint8_t cpuRead(uint16_t addr, bool rdonly = false);
        void cpuWrite(uint16_t addr, uint8_t  data);

        uint8_t ppuRead(uint16_t addr, bool rdonly = false);
        void ppuWrite(uint16_t addr, uint8_t data);
    
    private:
        std::shared_ptr<Cartridge> cart;

    public:
        void ConnectCartridge(const std::shared_ptr<Cartridge>& cartridge);
        void clock();
};

#endif