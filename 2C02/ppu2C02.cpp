#include "ppu2C02.hpp"

ppu2C02::ppu2C02() {
	palScreen[0x00] = olc::Pixel(84, 84, 84);
	palScreen[0x01] = olc::Pixel(0, 30, 116);
	palScreen[0x02] = olc::Pixel(8, 16, 144);
	palScreen[0x03] = olc::Pixel(48, 0, 136);
	palScreen[0x04] = olc::Pixel(68, 0, 100);
	palScreen[0x05] = olc::Pixel(92, 0, 48);
	palScreen[0x06] = olc::Pixel(84, 4, 0);
	palScreen[0x07] = olc::Pixel(60, 24, 0);
	palScreen[0x08] = olc::Pixel(32, 42, 0);
	palScreen[0x09] = olc::Pixel(8, 58, 0);
	palScreen[0x0A] = olc::Pixel(0, 64, 0);
	palScreen[0x0B] = olc::Pixel(0, 60, 0);
	palScreen[0x0C] = olc::Pixel(0, 50, 60);
	palScreen[0x0D] = olc::Pixel(0, 0, 0);
	palScreen[0x0E] = olc::Pixel(0, 0, 0);
	palScreen[0x0F] = olc::Pixel(0, 0, 0);

	palScreen[0x10] = olc::Pixel(152, 150, 152);
	palScreen[0x11] = olc::Pixel(8, 76, 196);
	palScreen[0x12] = olc::Pixel(48, 50, 236);
	palScreen[0x13] = olc::Pixel(92, 30, 228);
	palScreen[0x14] = olc::Pixel(136, 20, 176);
	palScreen[0x15] = olc::Pixel(160, 20, 100);
	palScreen[0x16] = olc::Pixel(152, 34, 32);
	palScreen[0x17] = olc::Pixel(120, 60, 0);
	palScreen[0x18] = olc::Pixel(84, 90, 0);
	palScreen[0x19] = olc::Pixel(40, 114, 0);
	palScreen[0x1A] = olc::Pixel(8, 124, 0);
	palScreen[0x1B] = olc::Pixel(0, 118, 40);
	palScreen[0x1C] = olc::Pixel(0, 102, 120);
	palScreen[0x1D] = olc::Pixel(0, 0, 0);
	palScreen[0x1E] = olc::Pixel(0, 0, 0);
	palScreen[0x1F] = olc::Pixel(0, 0, 0);

	palScreen[0x20] = olc::Pixel(236, 238, 236);
	palScreen[0x21] = olc::Pixel(76, 154, 236);
	palScreen[0x22] = olc::Pixel(120, 124, 236);
	palScreen[0x23] = olc::Pixel(176, 98, 236);
	palScreen[0x24] = olc::Pixel(228, 84, 236);
	palScreen[0x25] = olc::Pixel(236, 88, 180);
	palScreen[0x26] = olc::Pixel(236, 106, 100);
	palScreen[0x27] = olc::Pixel(212, 136, 32);
	palScreen[0x28] = olc::Pixel(160, 170, 0);
	palScreen[0x29] = olc::Pixel(116, 196, 0);
	palScreen[0x2A] = olc::Pixel(76, 208, 32);
	palScreen[0x2B] = olc::Pixel(56, 204, 108);
	palScreen[0x2C] = olc::Pixel(56, 180, 204);
	palScreen[0x2D] = olc::Pixel(60, 60, 60);
	palScreen[0x2E] = olc::Pixel(0, 0, 0);
	palScreen[0x2F] = olc::Pixel(0, 0, 0);

	palScreen[0x30] = olc::Pixel(236, 238, 236);
	palScreen[0x31] = olc::Pixel(168, 204, 236);
	palScreen[0x32] = olc::Pixel(188, 188, 236);
	palScreen[0x33] = olc::Pixel(212, 178, 236);
	palScreen[0x34] = olc::Pixel(236, 174, 236);
	palScreen[0x35] = olc::Pixel(236, 174, 212);
	palScreen[0x36] = olc::Pixel(236, 180, 176);
	palScreen[0x37] = olc::Pixel(228, 196, 144);
	palScreen[0x38] = olc::Pixel(204, 210, 120);
	palScreen[0x39] = olc::Pixel(180, 222, 120);
	palScreen[0x3A] = olc::Pixel(168, 226, 144);
	palScreen[0x3B] = olc::Pixel(152, 226, 180);
	palScreen[0x3C] = olc::Pixel(160, 214, 228);
	palScreen[0x3D] = olc::Pixel(160, 162, 160);
	palScreen[0x3E] = olc::Pixel(0, 0, 0);
	palScreen[0x3F] = olc::Pixel(0, 0, 0);

}


ppu2C02::~ppu2C02() { }

olc::Sprite& ppu2C02::GetScreen() {
	return sprScreen;
}

olc::Sprite & ppu2C02::GetNameTable(uint8_t i) {
	return sprNameTable[i];
}

olc::Sprite & ppu2C02::GetPatternTable(uint8_t i, uint8_t palette) {
	for (uint16_t nTileY = 0; nTileY < 16; nTileY++) {
		for (uint16_t nTileX = 0; nTileX < 16; nTileX++) {
			uint16_t nOffset = nTileY * 256 + nTileX * 16;

			for (uint16_t row = 0; row < 8; row++) {
				uint8_t tile_lsb = ppuRead(i * 0x1000 + nOffset + row + 0x0000);
				uint8_t tile_msb = ppuRead(i * 0x1000 + nOffset + row + 0x0008);

				for (uint16_t col = 0; col < 8; col++) {
					uint8_t pixel = (tile_lsb & 0x01) + (tile_msb & 0x01);

					tile_lsb >>= 1; tile_msb >>= 1;

					sprPatternTable[i].SetPixel(nTileX * 8 + (7 - col), nTileY * 8 + row, GetColourFromPaletteRam(palette, pixel));
				}
			}
		}
	}

	return sprPatternTable[i];
}

olc::Pixel& ppu2C02::GetColourFromPaletteRam(uint8_t palette, uint8_t pixel) {
	return palScreen[ppuRead(0x3F00 + (palette << 2) + pixel) & 0x3F];
}

uint8_t ppu2C02::cpuRead(uint16_t addr, bool rdonly) {
    uint8_t data = 0x00;

	if (rdonly) {
		// Reading from PPU registers can affect their contents
		// so this read only option is used for examining the
		// state of the PPU without changing its state. This is
		// really only used in debug mode.
		switch (addr) {
			case 0x0000: // Control
				data = control.reg;
				break;
			case 0x0001: // Mask
				data = mask.reg;
				break;
			case 0x0002: // Status
				data = status.reg;
				break;
			case 0x0003: // OAM Address
				break;
			case 0x0004: // OAM Data
				break;
			case 0x0005: // Scroll
				break;
			case 0x0006: // PPU Address
				break;
			case 0x0007: // PPU Data
				break;
		}
	} else {
		switch (addr) {
				// Control - Not readable
			case 0x0000: break;
			
				// Mask - Not Readable
			case 0x0001: break;
			
				// Status
			case 0x0002:
				data = (status.reg & 0xE0) | (ppu_data_buffer & 0x1F);

				// Clear the vertical blanking flag
				status.vertical_blank = 0;

				// Reset Loopy's Address latch flag
				address_latch = 0;
				break;

				// OAM Address
			case 0x0003: break;

				// OAM Data
			case 0x0004: break;

				// Scroll - Not Readable
			case 0x0005: break;

				// PPU Address - Not Readable
			case 0x0006: break;

				// PPU Data
			case 0x0007: 
				data = ppu_data_buffer;
				ppu_data_buffer = ppuRead(vram_addr.reg);
				if (vram_addr.reg >= 0x3F00) data = ppu_data_buffer;
				vram_addr.reg += (control.increment_mode ? 32 : 1);
				break;
		}
	}

	return data;
}

void ppu2C02::cpuWrite(uint16_t addr, uint8_t data) {
    switch (addr) {
		case 0x0000: // Control
			control.reg = data;
			tram_addr.nametable_x = control.nametable_x;
			tram_addr.nametable_y = control.nametable_y;
			break;
		case 0x0001: // Mask
			mask.reg = data;
			break;
		case 0x0002: // Status
			break;
		case 0x0003: // OAM Address
			break;
		case 0x0004: // OAM Data
			break;
		case 0x0005: // Scroll
			if (address_latch == 0) {
				fine_x = data & 0x07;
				tram_addr.coarse_x = data >> 3;
				address_latch = 1;
			} else {
				tram_addr.fine_y = data & 0x07;
				tram_addr.coarse_y = data >> 3;
				address_latch = 0;
			}
			break;
		case 0x0006: // PPU Address
			if (address_latch == 0) {
				tram_addr.reg = (uint16_t)((data & 0x3F) << 8) | (tram_addr.reg & 0x00FF);
				address_latch = 1;
			} else {
				tram_addr.reg = (tram_addr.reg & 0xFF00) | data;
				vram_addr = tram_addr;
				address_latch = 0;
			}
			break;
		case 0x0007: // PPU Data
			ppuWrite(vram_addr.reg, data);
			vram_addr.reg += (control.increment_mode ? 32 : 1);
			break;
	}
}

uint8_t ppu2C02::ppuRead(uint16_t addr, bool rdonly) {
    uint8_t data = 0x00;
	addr &= 0x3FFF;

	if (cart->ppuRead(addr, data)) {

	} else if (addr >= 0x0000 && addr <= 0x1FFF) {
		// If the cartridge cant map the address, have
		// a physical location ready here
		data = tblPattern[(addr & 0x1000) >> 12][addr & 0x0FFF];
	}
	else if (addr >= 0x2000 && addr <= 0x3EFF) {
		addr &= 0x0FFF;

		if (cart->mirror == Cartridge::MIRROR::VERTICAL) {
			// Vertical
			if (addr >= 0x0000 && addr <= 0x03FF)
				data = tblName[0][addr & 0x03FF];
			if (addr >= 0x0400 && addr <= 0x07FF)
				data = tblName[1][addr & 0x03FF];
			if (addr >= 0x0800 && addr <= 0x0BFF)
				data = tblName[0][addr & 0x03FF];
			if (addr >= 0x0C00 && addr <= 0x0FFF)
				data = tblName[1][addr & 0x03FF];
		} else if (cart->mirror == Cartridge::MIRROR::HORIZONTAL) {
			// Horizontal
			if (addr >= 0x0000 && addr <= 0x03FF)
				data = tblName[0][addr & 0x03FF];
			if (addr >= 0x0400 && addr <= 0x07FF)
				data = tblName[0][addr & 0x03FF];
			if (addr >= 0x0800 && addr <= 0x0BFF)
				data = tblName[1][addr & 0x03FF];
			if (addr >= 0x0C00 && addr <= 0x0FFF)
				data = tblName[1][addr & 0x03FF];
		}
	} else if (addr >= 0x3F00 && addr <= 0x3FFF) {
		addr &= 0x001F;
		if (addr == 0x0010) addr = 0x0000;
		if (addr == 0x0014) addr = 0x0004;
		if (addr == 0x0018) addr = 0x0008;
		if (addr == 0x001C) addr = 0x000C;
		data = tblPalette[addr] & (mask.grayscale ? 0x30 : 0x3F);
	}

	return data;
}

void ppu2C02::ppuWrite(uint16_t addr, uint8_t data) {
    addr &= 0x3FFF;

	if (cart->ppuWrite(addr, data)) {

	} else if (addr >= 0x0000 && addr <= 0x1FFF) {
		tblPattern[(addr & 0x1000) >> 12][addr & 0x0FFF] = data;
	} else if (addr >= 0x2000 && addr <= 0x3EFF) {
		addr &= 0x0FFF;
		if (cart->mirror == Cartridge::MIRROR::VERTICAL) {
			// Vertical
			if (addr >= 0x0000 && addr <= 0x03FF)
				tblName[0][addr & 0x03FF] = data;
			if (addr >= 0x0400 && addr <= 0x07FF)
				tblName[1][addr & 0x03FF] = data;
			if (addr >= 0x0800 && addr <= 0x0BFF)
				tblName[0][addr & 0x03FF] = data;
			if (addr >= 0x0C00 && addr <= 0x0FFF)
				tblName[1][addr & 0x03FF] = data;
		} else if (cart->mirror == Cartridge::MIRROR::HORIZONTAL) {
			// Horizontal
			if (addr >= 0x0000 && addr <= 0x03FF)
				tblName[0][addr & 0x03FF] = data;
			if (addr >= 0x0400 && addr <= 0x07FF)
				tblName[0][addr & 0x03FF] = data;
			if (addr >= 0x0800 && addr <= 0x0BFF)
				tblName[1][addr & 0x03FF] = data;
			if (addr >= 0x0C00 && addr <= 0x0FFF)
				tblName[1][addr & 0x03FF] = data;
		}
	} else if (addr >= 0x3F00 && addr <= 0x3FFF) {
		addr &= 0x001F;
		if (addr == 0x0010) addr = 0x0000;
		if (addr == 0x0014) addr = 0x0004;
		if (addr == 0x0018) addr = 0x0008;
		if (addr == 0x001C) addr = 0x000C;
		tblPalette[addr] = data;
	}
}

void ppu2C02::ConnectCartridge(const std::shared_ptr<Cartridge>& cartridge) {
	this->cart = cartridge;
}

void ppu2C02::reset() {
	fine_x = 0x00;
	address_latch = 0x00;
	ppu_data_buffer = 0x00;
	scanline = 0;
	cycle = 0;
	bg_next_tile_id = 0x00;
	bg_next_tile_attrib = 0x00;
	bg_next_tile_lsb = 0x00;
	bg_next_tile_msb = 0x00;
	bg_shifter_pattern_lo = 0x0000;
	bg_shifter_pattern_hi = 0x0000;
	bg_shifter_attrib_lo = 0x0000;
	bg_shifter_attrib_hi = 0x0000;
	status.reg = 0x00;
	mask.reg = 0x00;
	control.reg = 0x00;
	vram_addr.reg = 0x0000;
	tram_addr.reg = 0x0000;
}

void ppu2C02::clock() {
	auto IncrementScrollX = [&]() {
		// Ony if rendering is enabled
		if (mask.render_background || mask.render_sprites) {
			// A single name table is 32x30 tiles. As we increment horizontally
			// we may cross into a neighbouring nametable, or wrap around to
			// a neighbouring nametable
			if (vram_addr.coarse_x == 31) {
				// Leaving nametable so wrap address round
				vram_addr.coarse_x = 0;
				// Flip target nametable bit
				vram_addr.nametable_x = ~vram_addr.nametable_x;
			} else {
				// Staying in current nametable, so just increment
				vram_addr.coarse_x++;
			}
		}
	};

	auto IncrementScrollY = [&]() {
		// Ony if rendering is enabled
		if (mask.render_background || mask.render_sprites) {
			// If possible, just increment the fine y offset
			if (vram_addr.fine_y < 7) {
				vram_addr.fine_y++;
			} else {
				// Reset fine y offset
				vram_addr.fine_y = 0;

				// Check if we need to swap vertical nametable targets
				if (vram_addr.coarse_y == 29) {
					// We do, so reset coarse y offset
					vram_addr.coarse_y = 0;
					// And flip the target nametable bit
					vram_addr.nametable_y = ~vram_addr.nametable_y;
				} else if (vram_addr.coarse_y == 31) {
					// In case the pointer is in the attribute memory, we
					// just wrap around the current nametable
					vram_addr.coarse_y = 0;
				} else {
					// None of the above boundary/wrapping conditions apply
					// so just increment the coarse y offset
					vram_addr.coarse_y++;
				}
			}
		}
	};

	auto TransferAddressX = [&]() {
		// Ony if rendering is enabled
		if (mask.render_background || mask.render_sprites)
		{
			vram_addr.nametable_x = tram_addr.nametable_x;
			vram_addr.coarse_x    = tram_addr.coarse_x;
		}
	};

	auto TransferAddressY = [&]() {
		// Ony if rendering is enabled
		if (mask.render_background || mask.render_sprites) {
			vram_addr.fine_y      = tram_addr.fine_y;
			vram_addr.nametable_y = tram_addr.nametable_y;
			vram_addr.coarse_y    = tram_addr.coarse_y;
		}
	};

	auto LoadBackgroundShifters = [&]() {	
		bg_shifter_pattern_lo = (bg_shifter_pattern_lo & 0xFF00) | bg_next_tile_lsb;
		bg_shifter_pattern_hi = (bg_shifter_pattern_hi & 0xFF00) | bg_next_tile_msb;

		bg_shifter_attrib_lo  = (bg_shifter_attrib_lo & 0xFF00) | ((bg_next_tile_attrib & 0b01) ? 0xFF : 0x00);
		bg_shifter_attrib_hi  = (bg_shifter_attrib_hi & 0xFF00) | ((bg_next_tile_attrib & 0b10) ? 0xFF : 0x00);
	};


	auto UpdateShifters = [&]() {
		if (mask.render_background) {
			// Shifting background tile pattern row
			bg_shifter_pattern_lo <<= 1;
			bg_shifter_pattern_hi <<= 1;

			// Shifting palette attributes by 1
			bg_shifter_attrib_lo <<= 1;
			bg_shifter_attrib_hi <<= 1;
		}
	};

	if (scanline >= -1 && scanline < 240) {		
		if (scanline == 0 && cycle == 0) {
			// "Odd Frame" cycle skip
			cycle = 1;
		}

		if (scanline == -1 && cycle == 1) {
			// Effectively start of new frame, so clear vertical blank flag
			status.vertical_blank = 0;
		}


		if ((cycle >= 2 && cycle < 258) || (cycle >= 321 && cycle < 338)) {
			UpdateShifters();
			
			switch ((cycle - 1) % 8) {
				case 0:
					LoadBackgroundShifters();
					bg_next_tile_id = ppuRead(0x2000 | (vram_addr.reg & 0x0FFF));
					break;
				case 2:			
					bg_next_tile_attrib = ppuRead(0x23C0 | (vram_addr.nametable_y << 11) 
														| (vram_addr.nametable_x << 10) 
														| ((vram_addr.coarse_y >> 2) << 3) 
														| (vram_addr.coarse_x >> 2));			
					if (vram_addr.coarse_y & 0x02) bg_next_tile_attrib >>= 4;
					if (vram_addr.coarse_x & 0x02) bg_next_tile_attrib >>= 2;
					bg_next_tile_attrib &= 0x03;
					break;
				case 4: 
					bg_next_tile_lsb = ppuRead((control.pattern_background << 12) 
											+ ((uint16_t)bg_next_tile_id << 4) 
											+ (vram_addr.fine_y) + 0);
					break;
				case 6:
					bg_next_tile_msb = ppuRead((control.pattern_background << 12)
											+ ((uint16_t)bg_next_tile_id << 4)
											+ (vram_addr.fine_y) + 8);
					break;
				case 7:
					IncrementScrollX();
					break;
			}
		}

		if (cycle == 256) {
			IncrementScrollY();
		}

		if (cycle == 257) {
			LoadBackgroundShifters();
			TransferAddressX();
		}

		// Superfluous reads of tile id at end of scanline
		if (cycle == 338 || cycle == 340) {
			bg_next_tile_id = ppuRead(0x2000 | (vram_addr.reg & 0x0FFF));
		}

		if (scanline == -1 && cycle >= 280 && cycle < 305) {
			// End of vertical blank period so reset the Y address ready for rendering
			TransferAddressY();
		}
	}

	if (scanline == 240) {
		// Post Render Scanline - Do Nothing!
	}

	if (scanline >= 241 && scanline < 261) {
		if (scanline == 241 && cycle == 1) {
			status.vertical_blank = 1;
			if (control.enable_nmi) 
				nmi = true;
		}
	}

	uint8_t bg_pixel = 0x00;   // The 2-bit pixel to be rendered
	uint8_t bg_palette = 0x00; // The 3-bit index of the palette the pixel indexes

	if (mask.render_background) {
		uint16_t bit_mux = 0x8000 >> fine_x;
		uint8_t p0_pixel = (bg_shifter_pattern_lo & bit_mux) > 0;
		uint8_t p1_pixel = (bg_shifter_pattern_hi & bit_mux) > 0;
		bg_pixel = (p1_pixel << 1) | p0_pixel;
		uint8_t bg_pal0 = (bg_shifter_attrib_lo & bit_mux) > 0;
		uint8_t bg_pal1 = (bg_shifter_attrib_hi & bit_mux) > 0;
		bg_palette = (bg_pal1 << 1) | bg_pal0;
	}

	sprScreen.SetPixel(cycle - 1, scanline, GetColourFromPaletteRam(bg_palette, bg_pixel));

	cycle++;
	if (cycle >= 341) {
		cycle = 0;
		scanline++;
		if (scanline >= 261)
		{
			scanline = -1;
			frame_complete = true;
		}
	}
}