#ifndef CHIP8_HPP
#define CHIP8_HPP
#include <string>
#include "Chip8Common.hpp"
#include "Timer.hpp"
#include "Stack.hpp"
#include "VarRegs.hpp"
#include "Memory.hpp"



struct Display
{
    bool data[Chip8Const::screen_height][Chip8Const::screen_width]{};
};

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
    Display m_display{};
    unsigned short m_PC{};
    unsigned short m_I{};
    Stack m_stack{};
    Timer m_delay_timer{};
    Timer m_sound_timer{}; 
    VarRegs m_regs{};
    KeyState m_key_states[Chip8Const::buttons];
    bool m_legacy_beh{true};

    void jumpTo(unsigned short location);
    void writeToMemory(unsigned short location, unsigned char value);
    unsigned char readFromMemory(unsigned short location);
    unsigned short getCurrentPCAddr();
    unsigned short fetch();
    void decodeExecute(unsigned short operation);
public:
    Chip8();
    void setLegacyBeh(bool value);
    bool load(const std::string& path);
    void emulateStep();
    bool getPixel(unsigned char x, unsigned char y);
    void setKeyState(unsigned char which, KeyState state);
    KeyState getKeyState(unsigned char which);
    bool shouldBeep();
};

#endif