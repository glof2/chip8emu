#ifndef CHIP8_HPP
#define CHIP8_HPP
#include <string>
#include "Timer.hpp"
#include "Stack.hpp"
#include "VarRegs.hpp"

#define CHIP8_SCREEN_HEIGHT 32
#define CHIP8_SCREEN_WIDTH 64

struct Memory
{
    unsigned char data[4096]{};
};

struct Display
{
    bool data[CHIP8_SCREEN_HEIGHT][CHIP8_SCREEN_WIDTH];
};

class Chip8
{
private:
    Memory m_memory{};
    Display m_display{};
    unsigned char* m_PC{};
    unsigned short m_I{};
    Stack m_stack{};
    Timer m_delay_timer{};
    Timer m_sound_timer{}; 
    VarRegs m_regs{};
    bool m_key_states[0xF + 1]{};
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
    void updateKeyState(unsigned char which, bool state);
};

#endif