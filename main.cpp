#include <iostream>
#include <SDL2/SDL.h>
#include "Chip8.hpp"

#define RENDER_SCALE 16

void displaySDLError(const std::string& msg)
{
    std::cout << msg << '\n';
    std::cout << "SDL_Error: " << SDL_GetError() << '\n';
}

int main()
{
    // Move this to a function for later cleanup (in case it fails in the middle!)
    // Prepare SDL
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        displaySDLError("Failed to init SDL!");
        return -1;
    }
    SDL_Window* window{ SDL_CreateWindow("EMULATOR", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, CHIP8_SCREEN_WIDTH * RENDER_SCALE, CHIP8_SCREEN_HEIGHT * RENDER_SCALE, SDL_WINDOW_SHOWN) };
    if(window == NULL)
    {
        displaySDLError("Failed to create window!");
        return -1;
    }
    
    SDL_Renderer* renderer{SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)};
    if(renderer == NULL)
    {
        displaySDLError("FAiled to create renderer!");
        return -1;
    }

    SDL_Texture* target{SDL_CreateTexture(renderer, SDL_GetWindowPixelFormat(window), SDL_TEXTUREACCESS_TARGET, CHIP8_SCREEN_WIDTH, CHIP8_SCREEN_HEIGHT)};
    if(target == NULL)
    {
        displaySDLError("Failed to create texture for render scaling!");
        return -1;
    }

    // Prepare emulator
    Chip8 emulator{};
    std::cout << "Loading ibm logo ROM...\n";
    if(!emulator.load("ibm.ch8"))
    {
        std::cout << "Failed to load ROM!\n";
        return -1;
    }
    std::cout << "Loaded ibm logo ROM succesfuly!\n";

    bool run{true};
    while (run)
    {
        // -- Poll Events --
        SDL_Event ev{};
        while(SDL_PollEvent(&ev))
        {
            if(ev.type == SDL_QUIT)
            {
                run = false;
            }
        }

        // -- Update --
        emulator.emulateStep();

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