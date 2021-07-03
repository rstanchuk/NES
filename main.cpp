#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include <SDL2/SDL.h>
#include <iomanip>
#include <cstring>

#include <iostream>
#include <sstream>

#include "6502/bus.h"
#include "6502/cpu6502.h"

class NES {
    public:
        NES(int scale, std::string rom) {
			// SDL setup
            SDL_Init(SDL_INIT_EVERYTHING);
            window = SDL_CreateWindow("NES", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 256*scale, 240*scale, SDL_WINDOW_SHOWN);
            renderer = SDL_CreateRenderer (window, -1, SDL_RENDERER_ACCELERATED);

            SDL_GetRendererInfo(renderer, &info);
            std::cout << "Renderer name: " << info.name << std::endl;
            std::cout << "Texture formats: " << std::endl;
            for( Uint32 i = 0; i < info.num_texture_formats; i++ ) {
                std::cout << SDL_GetPixelFormatName( info.texture_formats[i] ) << std::endl;
            }

            texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, texWidth, texHeight);

			// Cartridge setup
			cart = std::make_shared<Cartridge>(rom);
			
			if (!cart->ImageValid())
				exit(1);

			nes.insertCartridge(cart);	
			nes.reset();
        }

        ~NES() {
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            SDL_Quit();
        }

    private:
		// SDL
        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_RendererInfo info;
        SDL_Texture* texture;
        const unsigned int texWidth = 256;
        const unsigned int texHeight = 240;

		// NES hardware
		Bus nes;
		std::shared_ptr<Cartridge> cart;
		uint8_t prevControl = 0x00;
        
    public:
        void updateScreen(unsigned char* screen, olc::Pixel* frame) {
			int framePixelIndex = 0;
            for(int x = 0; x < texWidth; x++) {
                for(int y = 0; y < texHeight; y++) {
                    int offset = (x * 4 * texHeight) + (y * 4);
                    screen[offset + 0] = (int)frame[framePixelIndex].r;		// b
                    screen[offset + 1] = (int)frame[framePixelIndex].g;		// g
                    screen[offset + 2] = (int)frame[framePixelIndex].b;		// r
                    screen[offset + 3] = (int)frame[framePixelIndex].a;		// a
					framePixelIndex++;
                }
            }
        }

		bool processInput() {
			bool run = true;
			SDL_Event event;
			while(SDL_PollEvent(&event)) {
				switch (event.type) {
					case SDL_QUIT:
						run = false;
						break;
					case SDL_KEYDOWN:
						switch (event.key.keysym.sym) {
							case SDLK_r:	// Reset
								nes.reset();
								break;
							case SDLK_ESCAPE:
								run = false;
								break;
							case SDLK_a:		// Select
								nes.controller[0] = 0x20;
								break;
							case SDLK_s:		// Start
								nes.controller[0] = 0x10;
								break;
							case SDLK_x:		// A
								nes.controller[0] = 0x80;
								break;
							case SDLK_z:		// B
								nes.controller[0] = 0x40;
								break;
							case SDLK_UP:
								nes.controller[0] = 0x08;
								break;
							case SDLK_DOWN:
								nes.controller[0] = 0x04;
								break;
							case SDLK_LEFT:
								nes.controller[0] = 0x02;
								break;
							case SDLK_RIGHT:
								nes.controller[0] = 0x01;
								break;
						}
						break;
					case SDL_KEYUP:
						switch (event.key.keysym.sym) {
							case SDLK_a: // Select
							case SDLK_s: // Start
							case SDLK_x: // A
							case SDLK_z: // B
							case SDLK_UP:
							case SDLK_DOWN:
							case SDLK_LEFT:
							case SDLK_RIGHT:
								nes.controller[0] = 0x00;
								break;
						}
						break;
				}
			}

			return run;
		}

        void run() {
            unsigned char screen[texWidth * texHeight * 4];
            SDL_Event event;
            bool run = true;
            bool useLocktexture = false;
            
            unsigned int frames = 0;
            Uint64 start = SDL_GetPerformanceCounter();

			clock_t fElapsedTime = clock();

            while(run) {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
                SDL_RenderClear(renderer);

				run = processInput();

				do { 
					nes.clock(); 
				} while (!nes.ppu.frame_complete);
				nes.ppu.frame_complete = false;
            
				olc::Pixel* frame = nes.ppu.GetScreen().GetData();

                // This draws creates a screen texture
                updateScreen(screen, frame);

                if(useLocktexture) {
                    unsigned char* lockedPixels = nullptr;
                    int pitch = 0;
                    SDL_LockTexture(texture, NULL, reinterpret_cast< void** >( &lockedPixels ), &pitch);
                    std::memcpy(lockedPixels, screen, sizeof(screen)/sizeof(screen[0]));
                    SDL_UnlockTexture( texture );
                } else {
                    SDL_UpdateTexture(texture, NULL, screen, texWidth * 4);
                }

                SDL_RenderCopy(renderer, texture, NULL, NULL);
                SDL_RenderPresent( renderer );
            
                frames++;
                const Uint64 end = SDL_GetPerformanceCounter();
                const static Uint64 freq = SDL_GetPerformanceFrequency();
                const double seconds = ( end - start ) / static_cast< double >( freq );
                if(seconds > 2.0) {
                    std::cout << std::setprecision(0) << std::fixed << frames / seconds << " FPS\n";
                    start = end;
                    frames = 0;
                }
            }
        }
};

int main(int argc, char* argv[]) {
	if(argc < 2) {
		std::cout << "./nes <rom>\n";
		exit(1);
	}

	NES nes(2, argv[1]);
	nes.run();

	return 0;
}