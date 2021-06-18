#ifndef Mapper000_H
#define Mapper000_H

#include "mapper.h"

class Mapper000 : public Mapper
{
public:
	Mapper000(uint8_t prgBanks, uint8_t chrBanks);
	~Mapper000();

public:
	bool cpuMapRead(uint16_t addr, uint32_t &mapped_addr) override;
	bool cpuMapWrite(uint16_t addr, uint32_t &mapped_addr, uint8_t data = 0) override;
	bool ppuMapRead(uint16_t addr, uint32_t &mapped_addr) override;
	bool ppuMapWrite(uint16_t addr, uint32_t &mapped_addr) override;
	void reset() override;

	// No local equipment required
};

#endif