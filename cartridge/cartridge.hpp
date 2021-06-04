#ifndef CARTRIDGE_HPP
#define CARTRIDGE_HPP

#include <cstdint>
#include <vector>

class Cartridge {
    public:
        Cartridge(const std::string& sFileName);
        ~Cartridge();
        
    private:
        std::vector<uint8_t> vPRGMemory;
	    std::vector<uint8_t> vCHRMemory;

        uint8_t nMapperID = 0;
        uint8_t nPRGBanks = 0;
        uint8_t nCHRBanks = 0;

    public:
        uint8_t cpuRead(uint16_t addr, bool rdonly = false);
        void cpuWrite(uint16_t addr, uint8_t  data);

        uint8_t ppuRead(uint16_t addr, bool rdonly = false);
        void ppuWrite(uint16_t addr, uint8_t data);
};

#endif