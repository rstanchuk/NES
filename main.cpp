#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include <iostream>
#include <sstream>

#include "./6502/bus.hpp"
#include "./6502/cpu6502.hpp"

class NES : public olc::PixelGameEngine {
	public:
		NES() { sAppName = "NES"; }

	private: 
		// The NES
		Bus nes;
		std::shared_ptr<Cartridge> cart;
		bool bEmulationRun = false;
		float fResidualTime = 0.0f;

		uint8_t nSelectedPalette = 0x00;

	private: 
		// Support Utilities
		std::map<uint16_t, std::string> mapAsm;

		std::string hex(uint32_t n, uint8_t d) {
			std::string s(d, '0');
			for (int i = d - 1; i >= 0; i--, n >>= 4)
				s[i] = "0123456789ABCDEF"[n & 0xF];
			return s;
		};

		void DrawRam(int x, int y, uint16_t nAddr, int nRows, int nColumns) {
			int nRamX = x, nRamY = y;
			for (int row = 0; row < nRows; row++)
			{
				std::string sOffset = "$" + hex(nAddr, 4) + ":";
				for (int col = 0; col < nColumns; col++)
				{
					sOffset += " " + hex(nes.cpuRead(nAddr, true), 2);
					nAddr += 1;
				}
				DrawString(nRamX, nRamY, sOffset);
				nRamY += 10;
			}
		}

		void DrawCpu(int x, int y) {
			std::string status = "STATUS: ";
			DrawString(x , y , "STATUS:", olc::WHITE);
			DrawString(x  + 64, y, "N", nes.cpu.status & cpu6502::N ? olc::GREEN : olc::RED);
			DrawString(x  + 80, y , "V", nes.cpu.status & cpu6502::V ? olc::GREEN : olc::RED);
			DrawString(x  + 96, y , "-", nes.cpu.status & cpu6502::U ? olc::GREEN : olc::RED);
			DrawString(x  + 112, y , "B", nes.cpu.status & cpu6502::B ? olc::GREEN : olc::RED);
			DrawString(x  + 128, y , "D", nes.cpu.status & cpu6502::D ? olc::GREEN : olc::RED);
			DrawString(x  + 144, y , "I", nes.cpu.status & cpu6502::I ? olc::GREEN : olc::RED);
			DrawString(x  + 160, y , "Z", nes.cpu.status & cpu6502::Z ? olc::GREEN : olc::RED);
			DrawString(x  + 178, y , "C", nes.cpu.status & cpu6502::C ? olc::GREEN : olc::RED);
			DrawString(x , y + 10, "PC: $" + hex(nes.cpu.pc, 4));
			DrawString(x , y + 20, "A: $" +  hex(nes.cpu.a, 2) + "  [" + std::to_string(nes.cpu.a) + "]");
			DrawString(x , y + 30, "X: $" +  hex(nes.cpu.x, 2) + "  [" + std::to_string(nes.cpu.x) + "]");
			DrawString(x , y + 40, "Y: $" +  hex(nes.cpu.y, 2) + "  [" + std::to_string(nes.cpu.y) + "]");
			DrawString(x , y + 50, "Stack P: $" + hex(nes.cpu.stkp, 4));
		}

		void DrawCode(int x, int y, int nLines) {
			auto it_a = mapAsm.find(nes.cpu.pc);
			int nLineY = (nLines >> 1) * 10 + y;
			if (it_a != mapAsm.end())
			{
				DrawString(x, nLineY, (*it_a).second, olc::CYAN);
				while (nLineY < (nLines * 10) + y)
				{
					nLineY += 10;
					if (++it_a != mapAsm.end())
					{
						DrawString(x, nLineY, (*it_a).second);
					}
				}
			}

			it_a = mapAsm.find(nes.cpu.pc);
			nLineY = (nLines >> 1) * 10 + y;
			if (it_a != mapAsm.end())
			{
				while (nLineY > y)
				{
					nLineY -= 10;
					if (--it_a != mapAsm.end())
					{
						DrawString(x, nLineY, (*it_a).second);
					}
				}
			}
		}

		bool OnUserCreate() {
			// Load the cartridge
			cart = std::make_shared<Cartridge>("roms/Super Mario Bros.nes");
			
			if (!cart->ImageValid())
				return false;

			// Insert into NES
			nes.insertCartridge(cart);
						
			// Extract dissassembly
			mapAsm = nes.cpu.disassemble(0x0000, 0xFFFF);

			// Reset NES
			nes.reset();
			return true;
		}

		bool OnUserUpdate(float fElapsedTime) {
			Clear(olc::DARK_BLUE);

			// Sneaky peek of controller input in next video! ;P
			nes.controller[0] = 0x00;
			nes.controller[0] |= GetKey(olc::Key::X).bHeld ? 0x80 : 0x00;
			nes.controller[0] |= GetKey(olc::Key::Z).bHeld ? 0x40 : 0x00;
			nes.controller[0] |= GetKey(olc::Key::A).bHeld ? 0x20 : 0x00;
			nes.controller[0] |= GetKey(olc::Key::S).bHeld ? 0x10 : 0x00;
			nes.controller[0] |= GetKey(olc::Key::UP).bHeld ? 0x08 : 0x00;
			nes.controller[0] |= GetKey(olc::Key::DOWN).bHeld ? 0x04 : 0x00;
			nes.controller[0] |= GetKey(olc::Key::LEFT).bHeld ? 0x02 : 0x00;
			nes.controller[0] |= GetKey(olc::Key::RIGHT).bHeld ? 0x01 : 0x00;

			if (GetKey(olc::Key::SPACE).bPressed) bEmulationRun = !bEmulationRun;
			if (GetKey(olc::Key::R).bPressed) nes.reset();
			if (GetKey(olc::Key::P).bPressed) (++nSelectedPalette) &= 0x07;

			if (bEmulationRun) {
				if (fResidualTime > 0.0f)
					fResidualTime -= fElapsedTime;
				else {
					fResidualTime += (1.0f / 60.0f) - fElapsedTime;
					do { nes.clock(); } while (!nes.ppu.frame_complete);
					nes.ppu.frame_complete = false;
				}
			} else {
				// Emulate code step-by-step
				if (GetKey(olc::Key::C).bPressed) {
					// Clock enough times to execute a whole CPU instruction
					do { nes.clock(); } while (!nes.cpu.complete());
					// CPU clock runs slower than system clock, so it may be
					// complete for additional system clock cycles. Drain
					// those out
					do { nes.clock(); } while (nes.cpu.complete());
				}

				// Emulate one whole frame
				if (GetKey(olc::Key::F).bPressed) {
					// Clock enough times to draw a single frame
					do { nes.clock(); } while (!nes.ppu.frame_complete);
					// Use residual clock cycles to complete current instruction
					do { nes.clock(); } while (!nes.cpu.complete());
					// Reset frame completion flag
					nes.ppu.frame_complete = false;
				}
			}

			DrawCpu(516, 2);
			DrawCode(516, 72, 26);

			// Draw Palettes & Pattern Tables ==============================================
			const int nSwatchSize = 6;
			for (int p = 0; p < 8; p++) // For each palette
				for(int s = 0; s < 4; s++) // For each index
					FillRect(516 + p * (nSwatchSize * 5) + s * nSwatchSize, 340, 
						nSwatchSize, nSwatchSize, nes.ppu.GetColourFromPaletteRam(p, s));
			
			// Draw selection reticule around selected palette
			DrawRect(516 + nSelectedPalette * (nSwatchSize * 5) - 1, 339, (nSwatchSize * 4), nSwatchSize, olc::WHITE);

			// Generate Pattern Tables
			DrawSprite(516, 348, &nes.ppu.GetPatternTable(0, nSelectedPalette));
			DrawSprite(648, 348, &nes.ppu.GetPatternTable(1, nSelectedPalette));

			// Draw rendered output ========================================================
			DrawSprite(0, 0, &nes.ppu.GetScreen(), 2);
			return true;
		}
};

int main() {
	NES nes;
	nes.Construct(780, 480, 1, 1);
	nes.Start();
	return 0;
}
