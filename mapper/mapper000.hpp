#ifndef MAPPER000_HPP
#define MAPPER000_HPP

#include <cstdint>
#include "mapper.hpp"

class Mapper000 : public Mapper {
    public:
        Mapper000(uint8_t prgBanks, uint8_t chrBanks);
        ~Mapper000();

        virtual bool cpuMapRead(uint16_t addr, uint32_t &mapped_addr) override;
        virtual bool cpuMapWrite(uint16_t addr, uint32_t &mapped_addr) override;
        
        virtual bool ppuMapRead(uint16_t addr, uint32_t &mapped_addr) override;
        virtual bool ppuMapWrite(uint16_t addr, uint32_t &mapped_addr) override;
};

#endif