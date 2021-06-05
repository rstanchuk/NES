#ifndef CARTRIDGE_HPP
#define CARTRIDGE_HPP

#include <cstdint>
#include <vector>
#include <fstream>
#include <string>

#include "../mapper/mapper000.hpp"

class Cartridge {
    public:
        Cartridge(const std::string& sFileName);
        ~Cartridge();

        enum MIRROR {
            HORIZONTAL,
            VERTICAL,
            ONESCREEN_LO,
            ONESCREEN_HI,
        } mirror = HORIZONTAL;

        bool ImageValid();
        
    private:
        std::vector<uint8_t> vPRGMemory;
	    std::vector<uint8_t> vCHRMemory;

        uint8_t nMapperID = 0;
        uint8_t nPRGBanks = 0;
        uint8_t nCHRBanks = 0;

        bool bImageValid = false;
        std::shared_ptr<Mapper> pMapper;

    public:
        bool cpuRead(uint16_t addr, uint8_t &data);
        bool cpuWrite(uint16_t addr, uint8_t data);

        bool ppuRead(uint16_t addr, uint8_t &data);
        bool ppuWrite(uint16_t addr, uint8_t data);
};

#endif