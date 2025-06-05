#ifndef CHIP8_HPP
#define CHIP8_HPP
#include <string>
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
    Stack stack{};
    unsigned char delay_timer{};
    unsigned char sound_timer{}; 
    VarRegs m_regs{};

    unsigned short fetch();
    void decodeExecute(unsigned short operation);

public:
    Chip8();
    bool load(const std::string& path);
    void emulateStep();
    bool getPixel(unsigned char x, unsigned char y);
};

#endif