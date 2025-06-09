#include <iostream>
#include <map>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "header/Chip8.hpp"
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>

#define RENDER_SCALE 16
#define DEBUG_MENU_ADD_W 48 * RENDER_SCALE
#define DEBUG_MENU_ADD_H 24 * RENDER_SCALE
#define MAX_ROM_DIR_LEN 128

void displaySDLError(const std::string& msg)
{
    std::cout << msg << '\n';
    std::cout << "SDL_Error: " << SDL_GetError() << '\n';
}


int main()
{
    // TODO:
    // - CHIP8 lib cleanup and comments
    // - Main file cleanup and comments
    // - Add debug controls with GUI (IMGUI)
    // - Add platforms (CHIP8 variants)

    // Prepare emulator
    Chip8 emulator{};
    double emu_last_update{ (double) Timer::getTime() };
    int emu_updates_per_second{0};
    char emu_rom_dir[MAX_ROM_DIR_LEN]{};
    Chip8::SaveState emu_save_state{emulator.getSaveState()};

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
    SDL_Window* window{ SDL_CreateWindow("EMULATOR", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Chip8Const::screen_width * RENDER_SCALE + DEBUG_MENU_ADD_W, Chip8Const::screen_height * RENDER_SCALE + DEBUG_MENU_ADD_H, SDL_WINDOW_SHOWN) };
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
    SDL_Texture* target{SDL_CreateTexture(renderer, SDL_GetWindowPixelFormat(window), SDL_TEXTUREACCESS_TARGET, Chip8Const::screen_width, Chip8Const::screen_height)};
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

    // Init imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io{ ImGui::GetIO() };
    (void)io;
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

    // Create imgui settings vars
    std::string imgui_status{};
    int imgui_mem_view_follow{};

    // Play sound
    Mix_VolumeMusic(0);
    Mix_PlayMusic(beep, -1);

    // Vars for loop
    bool run{true};

    const std::map<SDL_Keycode, Chip8_t::Byte> key_translations
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

            // Pass to IMGUI
            ImGui_ImplSDL2_ProcessEvent(&ev);
        }

        // -- Update --

        // -- Emulate a step (or multiple)
        int64_t time_now{ Timer::getTime() };
        double emu_update_wait{ 1000.0 / double(emu_updates_per_second) };
        double since_last_update{ double(time_now) - emu_last_update };
        if(since_last_update >= emu_update_wait)
        {
            int64_t amount_of_updates{ (int64_t)(since_last_update / emu_update_wait) };
            double time_accounted_for{ amount_of_updates * emu_update_wait };
            for(int64_t i{}; i < amount_of_updates; ++i)
            {
                emulator.emulateStep();
            }
            double time_unaccounted_for{ since_last_update - time_accounted_for }; 
            emu_last_update = (double)time_now - time_unaccounted_for;
        }

        // -- Set just released keys to down --
        for(const std::pair<SDL_Keycode, Chip8_t::Byte>& key : key_translations)
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
                Mix_VolumeMusic(MIX_MAX_VOLUME/2);
            }
            
        }
        else
        {
            if(Mix_GetMusicVolume(beep) != 0)
            {
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
                    SDL_SetRenderDrawColor(renderer, 0x7D, 0x34, 0xEB, 0xFF);
                }
                else
                {
                    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
                }
                SDL_RenderDrawPoint(renderer, x, y);   
            }
        }
        SDL_SetRenderDrawColor(renderer, 0x42, 0x3E, 0x47, 0xFF);
        SDL_SetRenderTarget(renderer, NULL);
        
        // Draw texture on screen
        SDL_Rect scale_rect{};
        scale_rect.x = 0;
        scale_rect.y = 0;
        scale_rect.w = Chip8Const::screen_width * RENDER_SCALE; 
        scale_rect.h = Chip8Const::screen_height * RENDER_SCALE;
        SDL_RenderCopy(renderer, target, NULL, &scale_rect);

        // Handle IMGUI
        // Begin frame
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // Render GUI

        // --- Memory view ---
        ImGui::SetNextWindowSize(ImVec2(0, 0), ImGuiCond_Once);
        if(ImGui::Begin("Memory"))
        {
            // Set up table
            ImGui::BeginTable("Memview", 5);
            
            ImGui::TableSetupColumn("Addr");
            ImGui::TableSetupColumn("+0x00");
            ImGui::TableSetupColumn("+0x02");
            ImGui::TableSetupColumn("+0x04");
            ImGui::TableSetupColumn("+0x06");
            ImGui::TableHeadersRow();

            for(Chip8_t::Word i{}; i < Chip8Const::mem_size; i+=8)
            {
                ImGui::TableNextRow();

                // Set column
                ImGui::TableSetColumnIndex(0);
                ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(IM_COL32(0xE8, 0xB6, 0x02, 0xFF)), "%04X", i);

                for(uint8_t j{}; j < 8; ++j)
                {
                    ImGui::TableSetColumnIndex(j/2 + 1);
                    Chip8_t::Byte byte{ emulator.getMemoryAt(i + j) };

                    if(j%2 != 0)
                    {
                        ImGui::SameLine();
                    }

                    if(i+j == emulator.getPC() || i + j - 1 == emulator.getPC())
                    {
                        ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, IM_COL32(0x33, 0x32, 0x2F, 0xFF), -1);
                        ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(IM_COL32(0xFF, 0x00, 0x00, 0xFF)), "%02X", byte);
                        if(imgui_mem_view_follow == 1) // yay magic numbers
                        {
                            ImGui::SetScrollHereY();
                        }
                        
                    }
                    else if(i+j == emulator.getI() || i + j - 1 == emulator.getI())
                    {
                        ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, IM_COL32(0x33, 0x32, 0x2F, 0xFF), -1);
                        ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(IM_COL32(0x00, 0xFF, 0x00, 0xFF)), "%02X", byte);
                        if(imgui_mem_view_follow == 2) // yay magic numbers 2
                        {
                            ImGui::SetScrollHereY();
                        }
                    }
                    else
                    {
                        ImGui::Text("%02X", byte);
                    }
                }
                ImGui::TableNextRow();
            }

            ImGui::EndTable();
        }
        ImGui::End();
        

        // --- Second window ---
        ImGui::SetNextWindowSize(ImVec2(0, 0), ImGuiCond_Once);
        if(ImGui::Begin("Debug tools & Info"))
        {
            // -- Tools --
            ImGui::Text("Tools:");

            // Instructions per second slider
            ImGui::SliderInt("Instructions per second", &emu_updates_per_second, 0, 10000);
            
            // Next instruction
            if(ImGui::Button("Next Instruction"))
            {
                emulator.emulateStep();
            }

            ImGui::InputText("ROM file directory", emu_rom_dir, MAX_ROM_DIR_LEN);
            if(ImGui::Button("Load ROM"))
            {
                emulator.clearMemory();
                if(!emulator.loadMemory(emu_rom_dir))
                {
                    imgui_status = "FAILED TO LOAD ROM!";
                    emu_updates_per_second = 0;
                    
                }
                else
                {
                    imgui_status = "ROM LOADED!";
                }
            }

            ImGui::SameLine();
            if(ImGui::Button("Clear memory"))
            {
                imgui_status = "MEMORY CLEARED!";
                emu_updates_per_second = 0;
                emulator.clearMemory();
            }

            ImGui::SameLine();
            if(ImGui::Button("Save state"))
            {
                imgui_status = "SAVED STATE!";
                emu_save_state = emulator.getSaveState();
            }

            ImGui::SameLine();
            if(ImGui::Button("Load state"))
            {
                imgui_status = "LOADED STATE!";
                emulator.loadSaveState(emu_save_state);
            }

            ImGui::Text(("Status: " + imgui_status).c_str());

            ImGui::Combo("Memory view cursor follow", &imgui_mem_view_follow, "None\0Instruction\0I\0");

            // -- Info --
            ImGui::NewLine();
            ImGui::Text("Info:");
            
            ImGui::BeginTable("Debug Info", 2);

            // - PC -
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Current instruction index");
            ImGui::TableSetColumnIndex(1);
            ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(IM_COL32(0xFF, 0x00, 0x00, 0xFF)), "%04X", emulator.getPC());

            // - I -
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Current I index");
            ImGui::TableSetColumnIndex(1);
            ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(IM_COL32(0x00, 0xFF, 0x00, 0xFF)), "%04X", emulator.getI());

            // Delay timer
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Delay timer");
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%u", emulator.getDelayTimerValue());

            // Sound timer
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Delay timer");
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%u", emulator.getSoundTimerValue());

            ImGui::EndTable();

            // Regs:
            ImGui::BeginTable("Registers", 2);
            ImGui::TableSetupColumn("Register");
            ImGui::TableSetupColumn("Value");
            ImGui::TableHeadersRow();

            for(uint8_t i{}; i <= 0xF; ++i)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("V%X", i);
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%02X", emulator.getReg(i));
            }

            ImGui::EndTable();

            // Stack:
            ImGui::NewLine();
            ImGui::Text("Stack: ");
            ImGui::BeginTable("Stack", 2);
            ImGui::TableSetupColumn("Number (1 - top)");
            ImGui::TableSetupColumn("Value");
            ImGui::TableHeadersRow();

            std::stack<Chip8_t::Word> stack_copy{ emulator.getStackCopy() };

            int i{1};
            while(!stack_copy.empty())
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%u", i++);
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%04X", stack_copy.top());
                stack_copy.pop();
            }

            for(int j{i}; j <= 10; ++j)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%u", j);
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("N/A");
            }

            ImGui::EndTable();
        }

        ImGui::End();

        // End frame
        ImGui::EndFrame();

        // Render IMGUI
        ImGui::Render();
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);

        // Present
        SDL_RenderPresent(renderer);
    }

    // --- Cleanup ---
    // Quit imgui
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    //

    // Close SDL
    SDL_DestroyTexture(target);
    target = nullptr;
    SDL_DestroyRenderer(renderer);
    renderer = nullptr;
    SDL_DestroyWindow(window);
    window = nullptr;
    SDL_Quit();
    return 0;
}