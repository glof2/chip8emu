#include <iostream>
#include <map>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "header/Chip8.hpp"

#define RENDER_SCALE 16

void displaySDLError(const std::string& msg)
{
    std::cout << msg << '\n';
    std::cout << "SDL_Error: " << SDL_GetError() << '\n';
}

int main()
{
    // TODO:
    // - General code cleanup
    // - CHIP8 lib cleanup and comments
    // - Main file cleanup and comments
    // - Add debug controls with GUI (IMGUI)

    // Prepare emulator
    Chip8 emulator{};
    std::cout << "Input rom file directory: ";
    std::string rom_dir{};
    std::cin >> rom_dir;
    if(!emulator.load(rom_dir))
    {
        std::cout << "Failed to load ROM!\n";
        return -1;
    }

    char legacy_beh{};
    std::cout << "Legacy beh? (y for yes): ";
    std::cin >> legacy_beh;
    emulator.setLegacyBeh(legacy_beh == 'y');

    std::cout << "Loaded ROM succesfuly!\n";

    // Move this to a function for later cleanup (in case it fails in the middle!)
    // Prepare SDL
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        displaySDLError("Failed to init SDL!");
        return -1;
    }

    //Initialize SDL_mixer
    if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
    {
        std::cout << "SDL_mixer could not initialize! SDL_mixer Error: " <<  Mix_GetError() << '\n';
        return -1;
    }

    // Prepare window
    SDL_Window* window{ SDL_CreateWindow("EMULATOR", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, CHIP8_SCREEN_WIDTH * RENDER_SCALE, CHIP8_SCREEN_HEIGHT * RENDER_SCALE, SDL_WINDOW_SHOWN) };
    if(window == NULL)
    {
        displaySDLError("Failed to create window!");
        return -1;
    }
    
    // Prepare renderer
    SDL_Renderer* renderer{SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED)};
    if(renderer == NULL)
    {
        displaySDLError("FAiled to create renderer!");
        return -1;
    }

    // Prepare target texture
    SDL_Texture* target{SDL_CreateTexture(renderer, SDL_GetWindowPixelFormat(window), SDL_TEXTUREACCESS_TARGET, CHIP8_SCREEN_WIDTH, CHIP8_SCREEN_HEIGHT)};
    if(target == NULL)
    {
        displaySDLError("Failed to create texture for render scaling!");
        return -1;
    }

    // Prepare sound
    Mix_Music* beep{ Mix_LoadMUS("beep.wav") };
    if (beep == NULL)
    {
        std::cout << "Could not load beep.wav! SDL_mixer Error: " <<  Mix_GetError() << '\n';
        return -1;
    }

    // Play sound
    Mix_VolumeMusic(0);
    Mix_PlayMusic(beep, -1);

    // Vars for loop
    bool run{true};
    const std::map<SDL_Keycode, unsigned char> key_translations
    {
        {SDLK_1, 0x1},
        {SDLK_2, 0x2},
        {SDLK_3, 0x3},
        {SDLK_4, 0xC},
        {SDLK_q, 0x4},
        {SDLK_w, 0x5},
        {SDLK_e, 0x6},
        {SDLK_r, 0xD},
        {SDLK_a, 0x7},
        {SDLK_s, 0x8},
        {SDLK_d, 0x9},
        {SDLK_f, 0xE},
        {SDLK_z, 0xA},
        {SDLK_x, 0x0},
        {SDLK_c, 0xB},
        {SDLK_v, 0xF},
    };

    while (run)
    {
        // -- Poll Events --
        SDL_Event ev{};
        while(SDL_PollEvent(&ev))
        {
            switch (ev.type)
            {
                case SDL_QUIT:
                {
                    run = false;
                    break;
                }
                case SDL_KEYDOWN:
                {
                    if(key_translations.find(ev.key.keysym.sym) != key_translations.end())
                    {
                        emulator.setKeyState(key_translations.at(ev.key.keysym.sym), Chip8::KeyState::DOWN);
                    }
                    break;
                }
                case SDL_KEYUP:
                {
                    if(key_translations.find(ev.key.keysym.sym) != key_translations.end())
                    {
                        emulator.setKeyState(key_translations.at(ev.key.keysym.sym), Chip8::KeyState::JUST_RELEASED);
                    }
                    break;
                }
            }
        }

        // -- Update --

        // -- Emulate a step
        emulator.emulateStep();

        // -- Set just released keys to down --
        for(const std::pair<SDL_Keycode, unsigned char>& key : key_translations)
        {
            if(emulator.getKeyState(key.second) == Chip8::KeyState::JUST_RELEASED)
            {
                emulator.setKeyState(key.second, Chip8::KeyState::UP); 
            }
        }

        // -- Play sound --
        if(emulator.shouldBeep())
        {
            if(Mix_GetMusicVolume(beep) != MIX_MAX_VOLUME/2)
            {
                std::cout << "Shoudl beep and is not max vol\n";
                Mix_VolumeMusic(MIX_MAX_VOLUME/2);
            }
            
        }
        else
        {
            if(Mix_GetMusicVolume(beep) != 0)
            {
                std::cout << "Shouldn't beep and is not 0 vol\n";
                Mix_VolumeMusic(0);
            }
        }

        // -- Render -- 
        // Clear
        SDL_RenderClear(renderer);

        // Draw to texture
        SDL_SetRenderTarget(renderer, target);
        for(int x{}; x < 64; ++x)
        {
            for(int y{}; y < 32; ++y)
            {
                bool pixel{ emulator.getPixel(x, y) };
                if(pixel)
                {
                    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                }
                else
                {
                    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
                }
                SDL_RenderDrawPoint(renderer, x, y);   
            }
        }
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_SetRenderTarget(renderer, NULL);
        
        // Draw texture on screen
        SDL_Rect scale_rect{};
        scale_rect.x = 0;
        scale_rect.y = 0;
        scale_rect.w = CHIP8_SCREEN_WIDTH * RENDER_SCALE; 
        scale_rect.h = CHIP8_SCREEN_HEIGHT * RENDER_SCALE;
        SDL_RenderCopy(renderer, target, NULL, &scale_rect);

        // Present
        SDL_RenderPresent(renderer);
        SDL_Delay(1000/600);
    }


    SDL_DestroyTexture(target);
    target = nullptr;
    SDL_DestroyRenderer(renderer);
    renderer = nullptr;
    SDL_DestroyWindow(window);
    window = nullptr;
    SDL_Quit();
    return 0;
}