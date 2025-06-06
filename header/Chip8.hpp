#ifndef CHIP8_HPP
#define CHIP8_HPP
#include <string>
#include <stack>
#include "Chip8Common.hpp"
#include "Timer.hpp"
#include "VarRegs.hpp"
#include "Memory.hpp"
#include "Display.hpp"

class Chip8
{
public:
    enum class KeyState
    {
        UP,
        DOWN,
        JUST_RELEASED,
        INVALID
    };

private:
    Memory m_memory{Chip8Const::mem_size};
    Display m_display{Chip8Const::screen_width, Chip8Const::screen_height};
    Chip8_t::Word m_PC{};
    Chip8_t::Word m_I{};
    std::stack<Chip8_t::Word> m_stack{};
    Timer m_delay_timer{};
    Timer m_sound_timer{}; 
    VarRegs m_regs{};
    KeyState m_key_states[Chip8Const::buttons];
    bool m_legacy_beh{true};

    void jumpTo(Chip8_t::Word location); 
    Chip8_t::Word fetch();
    void decodeExecute(Chip8_t::Word operation);
public:
    Chip8();
    void setLegacyBeh(bool value);
    bool load(const std::string& path);
    void emulateStep();
    bool getPixel(Chip8_t::Byte x, Chip8_t::Byte y);
    void setKeyState(Chip8_t::Byte which, KeyState state);
    KeyState getKeyState(Chip8_t::Byte which);
    bool shouldBeep();
};

#endif