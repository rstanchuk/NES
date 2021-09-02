#include <SDL.h>
#include <iostream>

#include "bus.h"
#include "cpu6502.h"

#define TEX_WIDTH 256
#define TEX_HEIGHT 240

class NES {
	public:
		NES(int scale, std::string rom) {
			// SDL setup
			SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);
			window = SDL_CreateWindow("NES", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, TEX_WIDTH*scale, TEX_HEIGHT*scale, SDL_WINDOW_SHOWN);
			renderer = SDL_CreateRenderer (window, -1, SDL_RENDERER_ACCELERATED);
			texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, TEX_WIDTH, TEX_HEIGHT);

			// Graphical API info
			SDL_RendererInfo info;
			SDL_GetRendererInfo(renderer, &info);
			std::cout << "Renderer name: " << info.name << std::endl;
			std::cout << "Texture formats: " << std::endl;
			for(int i = 0; i < info.num_texture_formats; i++) {
				std::cout << SDL_GetPixelFormatName( info.texture_formats[i] ) << std::endl;
			}

			// Cartridge setup
			cart = std::make_shared<Cartridge>(rom);

			if (!cart->ImageValid())
			exit(1);

			nes.insertCartridge(cart);	
			nes.reset();

			// Controller setup
			nes.controller[0] = 0x00;
			nes.controller[1] = 0x00;

			SDL_GameControllerAddMappingsFromFile("gamecontrollerdb.txt");
			ctrl = SDL_GameControllerOpen(0);
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
		SDL_Texture* texture;

		// NES hardware
		Bus nes;
		std::shared_ptr<Cartridge> cart;

		// Controllers
		SDL_GameController *ctrl;
	public:
		bool processInput() {
			bool run = true;
			SDL_Event event;

			while(SDL_PollEvent(&event)) {	
				switch(event.type) {
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
								nes.controller[0] |= 0x80;
								//nes.controller[1] |= 0x80;
								break;
							case SDLK_z:		// B
								nes.controller[0] |= 0x40;
								//nes.controller[1] |= 0x40;
								break;
							case SDLK_UP:
								nes.controller[0] |= 0x08;
								//nes.controller[1] |= 0x08;
								break;
							case SDLK_DOWN:
								nes.controller[0] |= 0x04;
								//nes.controller[1] |= 0x04;
								break;
							case SDLK_LEFT:
								nes.controller[0] |= 0x02;
								//nes.controller[1] |= 0x02;
								break;
							case SDLK_RIGHT:
								nes.controller[0] |= 0x01;
								//nes.controller[1] |= 0x01;
								break;
						}
						break;
					case SDL_KEYUP:
						switch (event.key.keysym.sym) {
							case SDLK_x: // A
							case SDLK_z: // B
								nes.controller[0] &= 0x0f;
								break;
							case SDLK_a: // Select
							case SDLK_s: // Start
							case SDLK_UP:
							case SDLK_DOWN:
							case SDLK_LEFT:
							case SDLK_RIGHT:
								nes.controller[0] = 0x00;
								break;
						}
						break;
					case SDL_CONTROLLERBUTTONDOWN:
						switch(event.cbutton.button) {
							case SDL_CONTROLLER_BUTTON_A:
								nes.controller[1] |= 0x80;
								break;
							case SDL_CONTROLLER_BUTTON_B:
								nes.controller[1] |= 0x40;
								break;
							case SDL_CONTROLLER_BUTTON_DPAD_UP:
								nes.controller[1] |= 0x08;
								break;
							case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
								nes.controller[1] |= 0x04;
								break;
							case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
								nes.controller[1] |= 0x02;
								break;
							case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
								nes.controller[1] |= 0x01;
								break;
						}
						break;
					case SDL_CONTROLLERBUTTONUP:
						switch(event.cbutton.button) {
							case SDL_CONTROLLER_BUTTON_A:
							case SDL_CONTROLLER_BUTTON_B:
								nes.controller[1] &= 0x0f;
								break;
							case SDL_CONTROLLER_BUTTON_DPAD_UP:
							case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
							case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
							case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
								nes.controller[1] = 0x00;
								break;
						}
						break;
				}
			}

			return run;
		}

		void run() {
			SDL_Event event;
			bool run = true;
			bool useLocktexture = false;

			unsigned int frames = 0;
			Uint64 start = SDL_GetPerformanceCounter();

			clock_t fElapsedTime = clock();

			while(run) {
				run = processInput();

				SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
				SDL_RenderClear(renderer);

				do { 
					nes.clock(); 
				} while (!nes.ppu.frame_complete);
				nes.ppu.frame_complete = false;

				uint8_t* screen = nes.ppu.GetScreen();

				// Makes a texture out of screen in order to display
				if(useLocktexture) {
					unsigned char* lockedPixels = nullptr;
					int pitch = 0;
					SDL_LockTexture(texture, NULL, reinterpret_cast<void**>(&lockedPixels), &pitch);
					std::memcpy(lockedPixels, screen, sizeof(screen)/sizeof(*screen));
					SDL_UnlockTexture(texture);
				} else {
					SDL_UpdateTexture(texture, NULL, screen, TEX_WIDTH * 4);
				}

				SDL_RenderCopy(renderer, texture, NULL, NULL);
				SDL_RenderPresent(renderer);

				// FPS calculation
				frames++;
				const Uint64 end = SDL_GetPerformanceCounter();
				const static Uint64 freq = SDL_GetPerformanceFrequency();
				const double seconds = (end - start) / (double)freq;
				if(seconds > 2.0) {
					std::cout << std::setprecision(0) << std::fixed << frames/seconds << " FPS\n";
					start = end;
					frames = 0;
				}
			}

			nes.controller[0] = 0x00;
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