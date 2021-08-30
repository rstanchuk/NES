#include "bus.h"

Bus::Bus() {
	// Connect CPU to communication bus
	cpu.ConnectBus(this);
	
}

Bus::~Bus() { }

void Bus::cpuWrite(uint16_t addr, uint8_t data) {	
	if (cart->cpuWrite(addr, data)) {
		// The cartridge "sees all" and has the facility to veto
		// the propagation of the bus transaction if it requires.
		// This allows the cartridge to map any address to some
		// other data, including the facility to divert transactions
		// with other physical devices. The NES does not do this
		// but I figured it might be quite a flexible way of adding
		// "custom" hardware to the NES in the future!
	} else if (addr >= 0x0000 && addr <= 0x1FFF) {
		// System RAM Address Range. The range covers 8KB, though
		// there is only 2KB available. That 2KB is "mirrored"
		// through this address range. Using bitwise AND to mask
		// the bottom 11 bits is the same as addr % 2048.
		cpuRam[addr & 0x07FF] = data;

	} else if (addr >= 0x2000 && addr <= 0x3FFF) {
		// PPU Address range. The PPU only has 8 primary registers
		// and these are repeated throughout this range. We can
		// use bitwise AND operation to mask the bottom 3 bits, 
		// which is the equivalent of addr % 8.
		ppu.cpuWrite(addr & 0x0007, data);
	} else if (addr == 0x4014) {
		// A write to this address initiates a DMA transfer
		dma_page = data;
		dma_addr = 0x00;
		dma_transfer = true;						
	} else if (addr >= 0x4016 && addr <= 0x4017) {
		// "Lock In" controller state at this time
		controller_state[addr & 0x0001] = controller[addr & 0x0001];
	}
	
}

uint8_t Bus::cpuRead(uint16_t addr, bool bReadOnly) {
	uint8_t data = 0x00;	
	if (cart->cpuRead(addr, data)) {
		// Cartridge Address Range
	} else if (addr >= 0x0000 && addr <= 0x1FFF) {
		// System RAM Address Range, mirrored every 2048
		data = cpuRam[addr & 0x07FF];
	} else if (addr >= 0x2000 && addr <= 0x3FFF) {
		// PPU Address range, mirrored every 8
		data = ppu.cpuRead(addr & 0x0007, bReadOnly);
	} else if (addr >= 0x4016 && addr <= 0x4017) {
		// Read out the MSB of the controller status word
		data = (controller_state[addr & 0x0001] & 0x80) > 0;
		controller_state[addr & 0x0001] <<= 1;
	}

	return data;
}

void Bus::insertCartridge(const std::shared_ptr<Cartridge>& cartridge) {
	// Connects cartridge to both Main Bus and CPU Bus
	this->cart = cartridge;
	ppu.ConnectCartridge(cartridge);
}

void Bus::reset() {
	cart->reset();
	cpu.reset();
	ppu.reset();
	nSystemClockCounter = 0;
	dma_page = 0x00;
	dma_addr = 0x00;
	dma_data = 0x00;
	dma_dummy = true;
	dma_transfer = false;
}

void Bus::clock() {
	// Clocking. The heart and soul of an emulator. The running
	// frequency is controlled by whatever calls this function.
	// So here we "divide" the clock as necessary and call
	// the peripheral devices clock() function at the correct
	// times.

	// The fastest clock frequency the digital system cares
	// about is equivalent to the PPU clock. So the PPU is clocked
	// each time this function is called.
	ppu.clock();

	// The CPU runs 3 times slower than the PPU so we only call its
	// clock() function every 3 times this function is called. We
	// have a global counter to keep track of this.
	if (nSystemClockCounter % 3 == 0) {
		// Is the system performing a DMA transfer form CPU memory to 
		// OAM memory on PPU?...
		if (dma_transfer) {
			// ...Yes! We need to wait until the next even CPU clock cycle
			// before it starts...
			if (dma_dummy) {
				// ...So hang around in here each clock until 1 or 2 cycles
				// have elapsed...
				if (nSystemClockCounter % 2 == 1) {
					// ...and finally allow DMA to start
					dma_dummy = false;
				}
			} else {
				// DMA can take place!
				if (nSystemClockCounter % 2 == 0) {
					// On even clock cycles, read from CPU bus
					dma_data = cpuRead(dma_page << 8 | dma_addr);
				} else {
					// On odd clock cycles, write to PPU OAM
					ppu.pOAM[dma_addr] = dma_data;
					// Increment the lo byte of the address
					dma_addr++;
					// If this wraps around, we know that 256
					// bytes have been written, so end the DMA
					// transfer, and proceed as normal
					if (dma_addr == 0x00) {
						dma_transfer = false;
						dma_dummy = true;
					}
				}
			}
		} else {
			// No DMA happening, the CPU is in control of its
			// own destiny. Go forth my friend and calculate
			// awesomeness for many generations to come...
			cpu.clock();
		}		
	}

	// The PPU is capable of emitting an interrupt to indicate the
	// vertical blanking period has been entered. If it has, we need
	// to send that irq to the CPU.
	if (ppu.nmi) {
		ppu.nmi = false;
		cpu.nmi();
	}

	nSystemClockCounter++;
}
