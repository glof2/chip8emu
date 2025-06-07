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

    struct SaveState
    {
        Memory memory{Chip8Const::mem_size};
        Display display{Chip8Const::screen_width, Chip8Const::screen_height};
        Chip8_t::Word PC{};
        Chip8_t::Word I{};
        std::stack<Chip8_t::Word> stack{};
        VarRegs regs{Chip8Const::reg_amount};
    };
private:
    Memory m_memory{Chip8Const::mem_size};
    Display m_display{Chip8Const::screen_width, Chip8Const::screen_height};
    Chip8_t::Word m_PC{};
    Chip8_t::Word m_I{};
    std::stack<Chip8_t::Word> m_stack{};
    Timer m_delay_timer{};
    Timer m_sound_timer{}; 
    VarRegs m_regs{Chip8Const::reg_amount};
    KeyState m_key_states[Chip8Const::buttons];
    bool m_legacy_beh{true};


    // --- Private member functions ---
    
    //  Name:           jumpTo
    //  Description:    makes the current instruction pointer jump to provided location
    //  Arguments:      location - the location in memory to jump to
    void jumpTo(Chip8_t::Word location);

    //  Name:           fetch
    //  Description:    returns the byte code for the current instruction
    //  Return:         the byte code of the instruction that the instruction pointer is currently on
    Chip8_t::Word fetch();

    //  Name:           decodeExecute
    //  Description:    decodes and executes the instruction given
    //  Arguments:      operation - the instruction to decode and execute
    void decodeExecute(Chip8_t::Word operation);

public:  
    // --- Constructors ---

    //  Description: Returns a Chip8 emulator class object, to load a ROM use Chip8::load
    Chip8();

    // --- Member functions ---

    //  Name:           setLegacyBeh
    //  Description:    switches the current legacy behaviour settings
    //  Arguments:      value - the value to switch to: true - legacy behaviour is on, false - legacy behaviour is off
    void setLegacyBeh(bool value);

    //  Name:           loadMemory
    //  Description:    loads the file in provided path to the memory, the file should be a CHIP8 rom file
    //  Arguments:      path - the path to the CHIP8 file
    bool loadMemory(const std::string& path);

    //  Name:           clearMemory
    //  Description:    clears the memory of the emulator
    void clearMemory();

    //  Name:           getSaveState
    //  Description:    Copies the current emulator state and returns it
    //  Return:         The save state
    SaveState getSaveState();

    //  Name:           loadSaveState
    //  Description:    Loads the saved state
    //  Arguments:      state - the save state to load
    void loadSaveState(SaveState state);

    //  Name:           emulateStep
    //  Description:    emulates a single instruction (fetch, decode and execute) and updates the emulator state
    void emulateStep();

    //  Name:           getPixel()
    //  Description;    returns the Display pixel state for the provided coordinates
    //  Arguments:      x - the X coordinate
    //                  y - the Y coordinate
    //  Return:         the state of the pixel at provided coordinates
    bool getPixel(Chip8_t::Byte x, Chip8_t::Byte y);

    //  Name:           setKeyState
    //  Description:    sets the state of the provided key
    //  Arguments:      which - the key to set the state of
    //                  state - the state to set the key to
    void setKeyState(Chip8_t::Byte which, KeyState state);

    //  Name:           getKeyState
    //  Description:    returns the state of the provided key
    //  Arguments:      which - the key to set the state of
    //  Return:         the state of the key
    KeyState getKeyState(Chip8_t::Byte which);

    //  Name:           shouldBeep
    //  Description:    returns whether or not the emulator thinks that sound should be playing
    //  Return:         true if emulator should be emiting sound, false otherwise
    bool shouldBeep();

    //  Name:           getMemoryAt
    //  Description:    returns the memory stored in provided index
    //  Arguments:      where - the index (of memory) to get
    //  Return:         the memory stored in provided index
    Chip8_t::Byte getMemoryAt(Chip8_t::Word where);

    //  Name:           getPC
    //  Description:    returns the current pc index
    //  Return:         the current PC index
    Chip8_t::Word getPC();

    //  Name:           getI
    //  Description:    returns the current I index
    //  Return:         the current I index
    Chip8_t::Word getI();

    //  Name:           getReg
    //  Description:    returns the value of provided Reg
    //  Arguments:      which - which register to get (0x0 - 0xF)
    //  Return:         the value of the reg
    Chip8_t::Byte getReg(Chip8_t::Byte which);

    //  Name:           getStackCopy
    //  Description:    returns a copy of the stack
    //  Return:         a copy of the stack
    std::stack<Chip8_t::Word> getStackCopy();

    //  Name:           getDelayTimerValue
    //  Description:    returns the delay timer value
    //  Return:         the delay timer value
    std::uint8_t getDelayTimerValue();

    //  Name:           getSoundTimerValue
    //  Description:    returns the sound timer value
    //  Return:         the sound timer value
    std::uint8_t getSoundTimerValue();
};

#endif