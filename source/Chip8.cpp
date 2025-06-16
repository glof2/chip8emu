#include <iostream>
#include <fstream>
#include <iomanip>
#include <random>
#include <array>
#include "../header/Chip8.hpp"

// ---- Emulator functions ----

// 00E0 - Clear screen, argument name omitted to make compiler shut up
void Chip8::_00E0(const Instruction<Chip8_t::Word>&)
{
    m_display.setAll(0);
}

// 00EE - Set PC to the value at top of the stack, argument name omitted to make compiler shut up
void Chip8::_00EE(const Instruction<Chip8_t::Word>&)
{
    Chip8_t::Word location{m_stack.top()};
    m_stack.pop();
    jumpTo(location);
}

// 0NNN - not implemented!, argument name omitted to make compiler shut up
void Chip8::_0NNN(const Instruction<Chip8_t::Word>&)
{
    std::cout << "UNINMPLEMENTED!\n";
}

// 1NNN - Jump to NNN
void Chip8::_1NNN(const Instruction<Chip8_t::Word>& instruction)
{
    Chip8_t::Word location{ (Chip8_t::Word)(instruction.getNibbles(1, 3)) };
    jumpTo(location);
}

// 2NNN - add current PC to stack, and jump to NNN
void Chip8::_2NNN(const Instruction<Chip8_t::Word>& instruction)
{
    m_stack.push(m_PC);
    Chip8_t::Word location{ (Chip8_t::Word)(instruction.getNibbles(1, 3)) };
    jumpTo(location);
}

// 3XNN - Skip one instruction if value in VX == NN
void Chip8::_3XNN(const Instruction<Chip8_t::Word>& instruction)
{
    Chip8_t::Byte val_1{ m_regs.read(instruction.getNibble(1)) };
    Chip8_t::Byte val_2{ (Chip8_t::Byte)(instruction.getNibbles(2, 3)) };

    if(val_1 == val_2)
    {
        m_PC += 2;
    }
}

// 4XNN - Skip one instruction if value in VX != NN
void Chip8::_4XNN(const Instruction<Chip8_t::Word>& instruction)
{
    Chip8_t::Byte val_1{ m_regs.read(instruction.getNibble(1)) };
    Chip8_t::Byte val_2{ (Chip8_t::Byte)(instruction.getNibbles(2, 3)) };

    if(val_1 != val_2)
    {
        m_PC += 2;
    }
}

// 5XY0 - Skip if values in VX == VY
void Chip8::_5XY0(const Instruction<Chip8_t::Word>& instruction)
{
    Chip8_t::Byte val_1{ m_regs.read(instruction.getNibble(1)) };
    Chip8_t::Byte val_2{ m_regs.read(instruction.getNibble(2)) };

    if(val_1 == val_2)
    {
        m_PC += 2;
    }
}

// 6XNN - set register VX to NN
void Chip8::_6XNN(const Instruction<Chip8_t::Word>& instruction)
{
    Chip8_t::Byte value{ (Chip8_t::Byte)(instruction.getNibbles(2, 3)) };
    m_regs.write(instruction.getNibble(1), value);
}

// 7XNN - add NN to register VX
void Chip8::_7XNN(const Instruction<Chip8_t::Word>& instruction)
{
    Chip8_t::Byte value{ (Chip8_t::Byte)(m_regs.read(instruction.getNibble(1))) };
    Chip8_t::Byte add{ (Chip8_t::Byte) (instruction.getNibbles(2, 3)) };
    m_regs.write(instruction.getNibble(1), value + add);
}

// 8XY0 -   set VX to VY
void Chip8::_8XY0(const Instruction<Chip8_t::Word>& instruction)
{
    Chip8_t::Byte vy{ m_regs.read(instruction.getNibble(2)) };
    m_regs.write(instruction.getNibble(1), vy);
}

// 8XY1 -   set VX to bitwise OR of VX and VY
void Chip8::_8XY1(const Instruction<Chip8_t::Word>& instruction)
{
    Chip8_t::Byte vx{ m_regs.read(instruction.getNibble(1)) };
    Chip8_t::Byte vy{ m_regs.read(instruction.getNibble(2)) };
    m_regs.write(instruction.getNibble(1), vx | vy);

    if(m_behaviour == Chip8::BehaviourType::CHIP8)
    {
        m_regs.write(0xF, 0);
    }
}

// 8XY2 -   set VX to bitwise AND of VX and VY
void Chip8::_8XY2(const Instruction<Chip8_t::Word>& instruction)
{
    Chip8_t::Byte vx{ m_regs.read(instruction.getNibble(1)) };
    Chip8_t::Byte vy{ m_regs.read(instruction.getNibble(2)) };
    m_regs.write(instruction.getNibble(1), vx & vy);

    if(m_behaviour == Chip8::BehaviourType::CHIP8)
    {
        m_regs.write(0xF, 0);
    }
}

// 8XY3 -   set VX to bitwise XOR of VX and VY
void Chip8::_8XY3(const Instruction<Chip8_t::Word>& instruction)
{
    Chip8_t::Byte vx{ m_regs.read(instruction.getNibble(1)) };
    Chip8_t::Byte vy{ m_regs.read(instruction.getNibble(2)) };
    m_regs.write(instruction.getNibble(1), vx ^ vy);

    if(m_behaviour == Chip8::BehaviourType::CHIP8)
    {
        m_regs.write(0xF, 0);
    }
}

// 8XY4 -   set VX to VX + VY, if VX+VY overflows VF is set to 1, otherwise to 0
void Chip8::_8XY4(const Instruction<Chip8_t::Word>& instruction)
{
    Chip8_t::Byte vx{ m_regs.read(instruction.getNibble(1)) };
    Chip8_t::Byte vy{ m_regs.read(instruction.getNibble(2)) };
    m_regs.write(instruction.getNibble(1), vx + vy);
    m_regs.write(0xF, vx + vy > 0xFF);
}

// 8XY5 -   set VX to VX - VY, if VX > VY set VF to 1, otherwise to 0
void Chip8::_8XY5(const Instruction<Chip8_t::Word>& instruction)
{
    Chip8_t::Byte vx{ m_regs.read(instruction.getNibble(1)) };
    Chip8_t::Byte vy{ m_regs.read(instruction.getNibble(2)) };
    m_regs.write(instruction.getNibble(1), vx - vy);
    m_regs.write(0xF, vx >= vy);
}

// 8XY7 -   set VX to VY - VX, if VY > VX set VF to 1, otherwise to 0
void Chip8::_8XY7(const Instruction<Chip8_t::Word>& instruction)
{
    Chip8_t::Byte vx{ m_regs.read(instruction.getNibble(1)) };
    Chip8_t::Byte vy{ m_regs.read(instruction.getNibble(2)) };
    m_regs.write(instruction.getNibble(1), vy - vx);
    m_regs.write(0xF, vy >= vx);
}

// 8XY6 -   
// Beh1:    set VX to VY
// Beh2:    ignore VY
// Then:    Shift VX one bit to the right, set VF to 1 if the bit shifted out was 1, or 0 if was 0
void Chip8::_8XY6(const Instruction<Chip8_t::Word>& instruction)
{
    Chip8_t::Byte vx{ m_regs.read(instruction.getNibble(1)) };
    if(m_behaviour == Chip8::BehaviourType::CHIP8)
    {
        vx = m_regs.read(instruction.getNibble(2));
    }

    m_regs.write(instruction.getNibble(1), vx >> 1);
    m_regs.write(0xF, vx & 0b00000001);
}

// 8XYE -   
// Beh1:    set VX to VY
// Beh2:    ignore VY
// Then:    Shift VX one bit to the left, set VF to 1 if the bit shifted out was 1, or 0 if was 0
void Chip8::_8XYE(const Instruction<Chip8_t::Word>& instruction)
{
    Chip8_t::Byte vx{ m_regs.read(instruction.getNibble(1)) };
    if(m_behaviour == Chip8::BehaviourType::CHIP8)
    {
        vx = m_regs.read(instruction.getNibble(2));
    }

    m_regs.write(instruction.getNibble(1), vx << 1);
    m_regs.write(0xF, (vx & 0b10000000) > 0);
}

// 9XY0 - Skip one instruction if values in VX != VY
void Chip8::_9XY0(const Instruction<Chip8_t::Word>& instruction)
{
    Chip8_t::Byte val_1{ m_regs.read(instruction.getNibble(1)) };
    Chip8_t::Byte val_2{ m_regs.read(instruction.getNibble(2)) };

    if(val_1 != val_2)
    {
        m_PC += 2;
    }
}

// ANNN - set index I to NNN
void Chip8::_ANNN(const Instruction<Chip8_t::Word>& instruction)
{
    Chip8_t::Word value{ (Chip8_t::Word)(instruction.getNibbles(1, 3)) };
    m_I = value;
}

// Legacy:
// BXNN - Jump to XNN plus the value in V0
// New:
// BXNN - Jump to XNN plus the value in register VX
void Chip8::_BXNN(const Instruction<Chip8_t::Word>& instruction)
{
    // Convert XNN to a single number
    Chip8_t::Word dest{ (Chip8_t::Word) instruction.getNibbles(1, 3)};
    
    // Add the appropriate register
    if(m_behaviour == Chip8::BehaviourType::CHIP8)
    {
        dest += m_regs.read(0x0);
    }
    else
    {
        dest += m_regs.read(instruction.getNibble(1));
    }
    
    // Jump to it
    jumpTo(dest);
}

// CXNN - generates a random number and binary ANDs it with NN, then puts the result in VX
void Chip8::_CXNN(const Instruction<Chip8_t::Word>& instruction)
{
    Chip8_t::Byte random{ (Chip8_t::Byte) (rand() % (0xFF+1)) };
    Chip8_t::Byte value{ (Chip8_t::Byte) (instruction.getNibbles(2, 3)) };
    m_regs.write(instruction.getNibble(1), random & value);
}

// 0xDXYN - Draw a N height sprite to the screen at coordinates (VX, VY) from the location of the I registed
//          if any of the pixels were flipped as a result of this set VF to 1, otherwise it's set to 0
void Chip8::_DXYN(const Instruction<Chip8_t::Word>& instruction)
{
    Chip8_t::Byte x{ (Chip8_t::Byte)(m_regs.read(instruction.getNibble(1)) % m_display.getWidth()) };
    Chip8_t::Byte y{ (Chip8_t::Byte)(m_regs.read(instruction.getNibble(2)) % m_display.getHeight()) };
    Chip8_t::Byte n{ (Chip8_t::Byte)(instruction.getNibble(3)) };

    if(m_behaviour == Chip8::BehaviourType::CHIP8)
    {
        drawRegular(x,y,n);
    }
    else if (m_behaviour == Chip8::BehaviourType::SUPERCHIP)
    {
        if (instruction.getNibble(3) == 0)
        {
            drawSChipSpecial(x, y);
        }
        else
        {
            drawRegular(x,y,n);
        }
    }

}

// EX9E - Skip one instruction if the key corresponding to value in VX is pressed
void Chip8::_EX9E(const Instruction<Chip8_t::Word>& instruction)
{
    Chip8_t::Byte key{ (Chip8_t::Byte)(m_regs.read(instruction.getNibble(1)) % (Chip8Const::buttons))};
    if(m_key_states[key] == Chip8::KeyState::DOWN) 
    {
        m_PC += 2;
    }
}

// EXA1 - Skip one instruction if the key corresponding to value in VX is not pressed
void Chip8::_EXA1(const Instruction<Chip8_t::Word>& instruction)
{
    Chip8_t::Byte key{ (Chip8_t::Byte)(m_regs.read(instruction.getNibble(1)) % (Chip8Const::buttons))};
    if(m_key_states[key] == Chip8::KeyState::UP || m_key_states[key] == Chip8::KeyState::JUST_RELEASED)
    {
        m_PC += 2;
    }
}

// FX07 - set VX to current value of delay timer
void Chip8::_FX07(const Instruction<Chip8_t::Word>& instruction)
{
    m_regs.write(instruction.getNibble(1), m_delay_timer.get());
}

// FX15 - set delay timer to current value in VX
void Chip8::_FX15(const Instruction<Chip8_t::Word>& instruction)
{
    m_delay_timer.set(m_regs.read(instruction.getNibble(1)));
}

// FX18 - set sound timer to current value in VX
void Chip8::_FX18(const Instruction<Chip8_t::Word>& instruction)
{
    m_sound_timer.set(m_regs.read(instruction.getNibble(1)));
}

// FX1E - add VX to I
void Chip8::_FX1E(const Instruction<Chip8_t::Word>& instruction)
{
    m_I += m_regs.read(instruction.getNibble(1));
}

// FX0A - Waits until a key is pressed, if a key is pressed it's "value" is put in VX
void Chip8::_FX0A(const Instruction<Chip8_t::Word>& instruction)
{
    Chip8_t::Byte key{Chip8Const::buttons};
    for(Chip8_t::Byte key_i{}; key_i < Chip8Const::buttons; ++key_i)
    {
        if(m_key_states[key_i] == KeyState::JUST_RELEASED)
        {
            key = key_i;
            break;
        }
    }
    if(key >= Chip8Const::buttons)
    {
        m_PC -= 2;
    }
    else
    {
        m_regs.write(instruction.getNibble(1), key);
    }
}

// FX29 - set the I register to the address of hexadecimal character in VX
void Chip8::_FX29(const Instruction<Chip8_t::Word>& instruction)
{
    // Get the second nibble (index 1)
    Chip8_t::Byte m_char{ m_regs.read(instruction.getNibble(1)) };
    m_char = m_char % (0xF+1);

    // Set I to the font location
    m_I = Chip8Const::lowres_font_begin + m_char * 5;
}

// FX33 - Take the number in VX, divide to 3 dec numbers (139 - 1, 3, 9), then store them in I, I+1, I+2
void Chip8::_FX33(const Instruction<Chip8_t::Word>& instruction)
{
    Chip8_t::Byte num{ m_regs.read(instruction.getNibble(1)) };
    for(char i{2}; i >= 0; --i)
    {
        if(m_I + i >= m_memory.getSize())
        {
            std::cout << "FX33 ATTEMPTED TO WRITE MEMORY OUT OF BOUNDS!\n";
            break;
        }
        m_memory.write(m_I + i, num % 10);
        num /= 10;
    }
}

// FX55 - Set memory in I, to I+X with the values of V0 to VX
void Chip8::_FX55(const Instruction<Chip8_t::Word>& instruction)
{
    for(int i{}; i <= instruction.getNibble(1); ++i)
    {
        if(m_I + i >= m_memory.getSize())
        {
            std::cout << "FX55 - ATTEMPTED TO WRITE MEMORY OUT OF BOUNDS!\n";
            break;
        }
        m_memory.write(m_I + i, m_regs.read(i));
    }

    if(m_behaviour == Chip8::BehaviourType::CHIP8)
    {
        m_I += instruction.getNibble(1) + 1;
    }
}

// FX65 - Set V0 to VX, with the value from memory of I to I+X
void Chip8::_FX65(const Instruction<Chip8_t::Word>& instruction)
{
    for(int i{}; i <= instruction.getNibble(1); ++i)
    {
        if(m_I + i >= m_memory.getSize())
        {
            std::cout << "FX65 - ATTEMPTED TO READ MEMORY OUT OF BOUNDS!\n";
            break;
        }
        m_regs.write(i, m_memory.read(m_I + i));
    }

    if(m_behaviour == Chip8::BehaviourType::CHIP8)
    {
        m_I += instruction.getNibble(1) + 1;
    }
}

// SUPERCHIP

// 00CN - Scroll content down N pixels
void Chip8::_00CN(const Instruction<Chip8_t::Word>& instr)
{
    if(m_behaviour != Chip8::BehaviourType::SUPERCHIP)
    {
        std::cout << "Tried to call 00CN, but behaviour mode isn't SCHIP!\n";
        return;
    }


    Chip8_t::Byte n{instr.getNibble(3)};
    if (n == 0)
    {
        return;
    }



    // Shift pixels
    // The loop goes from the bottom of the display - n to the top
    // Then it runs through all the pixels in it's current row
    // It first copies the pixel it is on into the pixel n pixels below, then clears the current pixel
    // this way the top N rows are cleared, since they aren't overwritten later
    for(int y{ (int)(m_display.getHeight() - 1 - n)}; y >= 0; --y)
    {
        for(Chip8_t::Word x{}; x < m_display.getWidth(); ++x)
        {
            m_display.setPixel(x, y+n, m_display.getPixel(x, y));
            m_display.setPixel(x, y, 0);
        }
    }
}

// 00FB - Scroll content right by 4 pixels
void Chip8::_00FB(const Instruction<Chip8_t::Word>& instr)
{
    // Guard clause, ensure instruction execution is valid
    if(m_behaviour != Chip8::BehaviourType::SUPERCHIP)
    {
        std::cout << "Tried to call 00FB, but behaviour mode isn't SCHIP!\n";
        return;
    }

    constexpr std::uint8_t shift_amount{4};

    // Shift pixels

    // The loop goes from the rightmost column of the display (minus N) to the leftmost column
    // Then it runs through all the pixels in it's current column
    // It first copies the pixel it is on into the pixel 4 pixels to the right, then clears the current pixel
    // this way the leftmost 4 columns are cleared, since they aren't overwritten later
    for(int x{ (int)(m_display.getWidth() - 1 - shift_amount)}; x >= 0; --x)
    {
        for(Chip8_t::Word y{}; y < m_display.getHeight(); ++y)
        {
            m_display.setPixel(x+shift_amount, y, m_display.getPixel(x, y));
            m_display.setPixel(x, y, 0);
        }
    }
}

// Scroll screen content left by 4 pixels
void Chip8::_00FC(const Instruction<Chip8_t::Word>& instr)
{
    // Guard clause, ensure instruction execution is valid
    if(m_behaviour != Chip8::BehaviourType::SUPERCHIP)
    {
        std::cout << "Tried to call 00FC, but behaviour mode isn't SCHIP!\n";
        return;
    }

    constexpr std::uint8_t shift_amount{4};

    // Shift pixels
    // The loop goes from the leftmost column of the display (plus N) to the rightmost column
    // Then it runs through all the pixels in it's current column
    // It first copies the pixel it is on into the pixel 4 pixels to the left, then clears the current pixel
    // this way 4 rightmost columns are cleared, since they aren't overwritten later
    for(int x{ (int)(shift_amount)}; x < m_display.getWidth(); ++x)
    {
        for(Chip8_t::Word y{}; y < m_display.getHeight(); ++y)
        {
            m_display.setPixel(x-shift_amount, y, m_display.getPixel(x, y));
            m_display.setPixel(x, y, 0);
        }
    }
}

// 00FD - Exit interpreter
void Chip8::_00FD(const Instruction<Chip8_t::Word>& instr)
{
    // Guard clause, ensure instruction execution is valid
    if(m_behaviour != Chip8::BehaviourType::SUPERCHIP)
    {
        std::cout << "Tried to call 00FD, but behaviour mode isn't SCHIP!\n";
        return;
    }
    // For emulation purposes, just enter an infinite loop
    m_PC -= 2;
}

// 00FE - Switch to low res mode and (SCHIP modern) clear the screen
void Chip8::_00FE(const Instruction<Chip8_t::Word>& instr)
{
    // Guard clause, ensure instruction execution is valid
    if(m_behaviour != Chip8::BehaviourType::SUPERCHIP)
    {
        std::cout << "Tried to call 00FE, but behaviour mode isn't SCHIP!\n";
        return;
    }
    m_display.setSize(Chip8Const::lowres_screen_width, Chip8Const::lowres_screen_height);
    m_display.setAll(0);
}

// 00FF - Switch to high res mode and (SCHIP modern) clear the screen
void Chip8::_00FF(const Instruction<Chip8_t::Word>& instr)
{
    // Guard clause, ensure instruction execution is valid
    if(m_behaviour != Chip8::BehaviourType::SUPERCHIP)
    {
        std::cout << "Tried to call 00FF, but behaviour mode isn't SCHIP!\n";
        return;
    }
    m_display.setSize(Chip8Const::highres_screen_width, Chip8Const::highres_screen_height);
    m_display.setAll(0);
}

// FX30 - Set I to the 10 lines high font sprite for VX
void Chip8::_FX30(const Instruction<Chip8_t::Word>& instr)
{
    // Guard clause, ensure instruction execution is valid
    if(m_behaviour != Chip8::BehaviourType::SUPERCHIP)
    {
        std::cout << "Tried to call FX30, but behaviour mode isn't SCHIP!\n";
        return;
    }

    // Get the second nibble (index 1)
    Chip8_t::Byte m_char{ m_regs.read(instr.getNibble(1)) };
    m_char = m_char % (0xF+1);

    // Set I to the font location
    m_I = Chip8Const::highres_font_begin + m_char * 10;
}

// FX75 - Store the content of the registers V0 to VX into flags storage
void Chip8::_FX75(const Instruction<Chip8_t::Word>& instr)
{
    // Guard clause, ensure instruction execution is valid
    if(m_behaviour != Chip8::BehaviourType::SUPERCHIP)
    {
        std::cout << "Tried to call FX75, but behaviour mode isn't SCHIP!\n";
        return;
    }
    std::cout << "FX75 - NOT IMPLEMENTED!\n";
}

// FX85 - Store the content of the registers V0 to VX into flags storage
void Chip8::_FX85(const Instruction<Chip8_t::Word>& instr)
{
    // Guard clause, ensure instruction execution is valid
    if(m_behaviour != Chip8::BehaviourType::SUPERCHIP)
    {
        std::cout << "Tried to call FX85, but behaviour mode isn't SCHIP!\n";
        return;
    }
    std::cout << "FX85 - NOT IMPLEMENTED!\n";
}

// --- Private member functions ---

void Chip8::drawRegular(const Chip8_t::Byte& x, const Chip8_t::Byte& y, const Chip8_t::Byte& n)
{
    // Set VF register
    m_regs.write(0xF, 0);

    for(Chip8_t::Byte byte_i{}; byte_i < n; ++byte_i)
    {
        // Exit condition
        if(y + byte_i >= m_display.getHeight())
        {
            break;
        }

        drawByte(m_memory.read(m_I + byte_i), x, y + byte_i);
    }
}

void Chip8::drawSChipSpecial(const Chip8_t::Byte& x, const Chip8_t::Byte& y)
{
    // Set VF register
    m_regs.write(0xF, 0);

    // Draw a 16x16 spire
    for(Chip8_t::Byte byte_i{}; byte_i < 32; byte_i += 2)
    {
        Chip8_t::Byte y_{ byte_i/2 + y };
        // Exit condition
        if(y_ >= m_display.getHeight())
        {
            break;
        }

        drawByte(m_memory.read(m_I + byte_i), x, y_);
        drawByte(m_memory.read(m_I + byte_i + 1), x + 8, y_);
    }
}

void Chip8::drawByte(const Chip8_t::Byte& sprite, Chip8_t::Byte x, Chip8_t::Byte y)
{
    // Go through each bit in byte
    for(signed char i{7}; i >= 0; --i)
    {
        Chip8_t::Byte bit_i{ (Chip8_t::Byte)(7 - i) };
        Chip8_t::Byte mask{ (Chip8_t::Byte) (1 << i) };
        Chip8_t::Byte masked_number{ (Chip8_t::Byte) (sprite & mask) };
        bool bit{ (bool) ((Chip8_t::Byte)(masked_number >> i)) };

        // Exit condition
        if ( x + bit_i >= m_display.getWidth())
        {
            break;
        }

        // Draw
        if(bit == true)
        {
            if(getPixel(x + bit_i, y) == true)
            {
                m_regs.write(0xF, 1);
            }
            m_display.flipPixel(x + bit_i, y);
        }

    }
}

void Chip8::jumpTo(Chip8_t::Word location)
{
    m_PC = location;
}

Instruction<Chip8_t::Word> Chip8::fetch() // Connect
{
    Instruction<Chip8_t::Word> operation{ std::array<Chip8_t::Byte, 2>{m_memory.read(m_PC), m_memory.read(m_PC + 1)} };
    m_PC += 2;

    return operation;
}

std::string Chip8::decode(const Instruction<Chip8_t::Word>& instruction)
{
    std::string result{};
    // Decode & execute
    switch(instruction.getNibble(0))
    {
        case 0x0:
        {
            // Possible instructions:
            // 00E0 - Clear screen
            // 00EE - Set PC to the value at top of the stack
            // 00CN - scroll content down N pixels
            // 00FB - scroll content right 4 pixels
            // 00FC - scroll content right 4 pixels
            // 00FD - exit interpreter
            // 00FE - switch to lores mode
            // 00FF - switch to hires mode
            switch (instruction.getNibbles(2, 3))
            {
                // 00E0 - Clear screen
                case 0xE0:
                {
                    result = "00E0";
                    break;
                }

                // 00EE - Set PC to the value at top of the stack
                case 0xEE:
                {
                    result = "00EE";
                    break;
                }

                // 00FB - scroll content right 4 pixels
                case 0xFB:
                {
                    result = "00FB";
                    break;
                }

                // 00FC - scroll content right 4 pixels
                case 0xFC:
                {
                    result = "00FC";
                    break;
                }

                // 00FD - exit interpreter
                case 0xFD:
                {
                    result = "00FD";
                    break;
                }

                // 00FE - switch to lores mode
                case 0xFE:
                {
                    result = "00FE";
                    break;
                }

                // 00FF - switch to hires mode
                case 0xFF:
                {
                    result = "00FF";
                    break;
                }

                // 00CN - scroll content down N pixels
                default:
                {
                    if(instruction.getNibble(2) != 0xC)
                    {
                        break;
                    }
                    result = "00CN";
                    break;
                }
            }
            
            break;
        }
        case 0x1:
        {
            // Possible instructions:
            // 0x1NNN - Jump to NNN
            result = "1NNN";
            break;
        }
        case 0x2:
        {
            // Possible instructions:
            // 2NNN - add current PC to stack, and jump to NNN
            result = "2NNN";

            break;
        }
        case 0x3:
        {
            // Possible instructions:
            // 3XNN - Skip one instruction if value in VX == NN
            result = "3XNN";
            break;
        }
        case 0x4:
        {
            // Possible instructions:
            // 4XNN - Skip one instruction if value in VX != NN
            result = "4XNN";
            break;
        }
        case 0x5:
        {
            // Possible instructions:
            // 5XY0 - Skip if values in VX == VY
            if(instruction.getNibble(3) != 0x0)
            {
                break;
            }

            result = "5XY0";
            break;
        }
        case 0x6:
        {
            // Possible instructions:
            // 6XNN - set register VX to NN
            result = "6XNN";

            break;
        }
        case 0x7:
        {
            // 7XNN - add NN to register VX
            result = "7XNN";
            break;
        }
        case 0x8:
        {
            // Possible instructions:
            // 8XY0 -   set VX to VY
            // 8XY1 -   set VX to bitwise OR of VX and VY
            // 8XY2 -   set VX to bitwise AND of VX and VY
            // 8XY3 -   set VX to bitwise XOR of VX and VY
            // 8XY4 -   set VX to VX + VY, if VX+VY overflows VF is set to 1, otherwise to 0
            // 8XY5 -   set VX to VX - VY, if VX > VY set VF to 1, otherwise to 0
            // 8XY7 -   set VX to VY - VX, if VY > VX set VF to 1, otherwise to 0
            // 8XY6 -   
            // Beh1:    set VX to VY
            // Beh2:    ignore VY
            // Then:    Shift VX one bit to the right, set VF to 1 if the bit shifted out was 1, or 0 if was 0
            // 8XYE -   
            // Beh1:    set VX to VY
            // Beh2:    ignore VY
            // Then:    Shift VX one bit to the left, set VF to 1 if the bit shifted out was 1, or 0 if was 0
            switch (instruction.getNibble(3))
            {
                // 8XY0 - VX is set to VY
                case 0x0:
                {
                    result = "8XY0";
                    break;
                }

                // 8XY1 - VX is set to bitwise OR of VX and VY, if on CHIP8 Cosmac set VF to 0
                case 0x1:
                {
                    result = "8XY1";
                    break;
                }

                // 8XY2 - VX is set to bitwise AND of VX and VY, if on CHIP8 Cosmac set VF to 0
                case 0x2:
                {
                    result = "8XY2";
                    break;
                }

                // 8XY3 - VX is set to VX XOR VY, if on CHIP8 Cosmac set VF to 0
                case 0x3:
                {
                    result = "8XY3";
                    break;
                }

                // 8XY4 - Set VX to VX + VY, if the addition overflows VX (VX + VY > 0xFF) set VF to 1, otherwise set VF to 0
                case 0x4:
                {
                    result = "8XY4";
                    break;
                }

                // 8XY5 - Set VX to VX - VY, if VX >= VY then set VF to 1 , otherwise set VF to 0
                case 0x5:
                {
                    result = "8XY5";
                    break;
                }

                // 8XY7 - Set VX to VY - VX, if VY >= VX then set VF to 1 , otherwise set VF to 0
                case 0x7:
                {
                    result = "8XY7";
                    break;
                }

                // 8XY6:
                // CHIP8: Set VX to VY, and shift VX one bit to the right, set VF to the bit shifted out
                // LATER: Shift VX one bit to the right, set VF to the bit shifted out
                case 0x6:
                {
                    result = "8XY6";
                    break;
                }

                // 8XYE:
                // CHIP8: Set VX to VY, and shift VX one bit to the left, set VF to the bit shifted out
                // LATER: Shift VX one bit to the left, set VF to the bit shifted out
                case 0xE:
                {
                    result = "8XYE";
                    break;
                }
            }
            break;
        }
        case 0x9:
        {
            // Possible instructions:
            // 9XY0 - Skip one instruction if values in VX != VY
            if(instruction.getNibble(3) != 0x0)
            {
                break;
            }
            result = "9XY0";
            break;
        }
        case 0xA:
        {
            // Possible instructions
            // ANNN - set index I to NNN
            result = "ANNN";
            break;
        }
        case 0xB:
        {
            // Possible instructions:
            // BXNN - jump to XNN + V0, or jump to XNN + VX depending on quirks
            result = "BXNN";
            break;
        }
        case 0xC:
        {
            // Possible instructions:
            // CXNN - generates a random number and binary ANDs it with NN, then puts the result in VX
            result = "CXNN";
            break;
        }
        case 0xD:
        {
            // Possible instructions
            // 0xDXYN - Draw a N height sprite to the screen at coordinates (VX, VY) from the location of the I registed
            //          if any of the pixels were flipped as a result of this set VF to 1, otherwise it's set to 0
            result = "DXYN";
            break;
        }
        case 0xE:
        {
            // Possible instructions:
            // EX9E - Skip one instruction if the key corresponding to value in VX is pressed
            // EXA1 - Skip one instruction if the key corresponding to value in VX is not pressed

            Chip8_t::Word val{ (Chip8_t::Byte) (instruction.getNibbles(2, 3)) };
            switch (val)
            {
                // EX9E - Skip one instruction if the key corresponding to value in VX is pressed
                case 0x9E:
                {
                    result = "EX9E";
                    break;
                }

                // EXA1 - Skip one instruction if the key corresponding to value in VX is not pressed
                case 0xA1:
                {
                    result = "EXA1";
                    break;
                }
            }
            break;
        }
        case 0xF:
        {
            // Possible instructions
            // FX07 - set VX to current value of delay timer
            // FX15 - set delay timer to current value in VX
            // FX18 - set sound timer to current value in VX
            // FX1E - add VX to I
            // FX0A - Waits until a key is pressed, if a key is pressed it's "value" is put in VX
            // FX29 - set the I register to the address of hexadecimal character in VX
            // FX33 - Take the number in VX, divide to 3 dec numbers (139 - 1, 3, 9), then store them in I, I+1, I+2
            // FX55 - Set memory in I, to I+X with the values of V0 to VX
            // FX65 - Set V0 to VX, with the value from memory of I to I+X
            Chip8_t::Byte val{ (Chip8_t::Byte) (instruction.getNibbles(2, 3)) };

            switch (val)
            {
                // FX07 - set VX to current value of delay timer
                case 0x07:
                {
                    result = "FX07";
                    break;
                }

                // FX15 - set delay timer to current value in VX
                case 0x15:
                {
                    result = "FX15";
                    break;
                }

                // FX18 - set sound timer to current value in VX
                case 0x18:
                {
                    result = "FX18";
                    break;
                }

                // FX1E - add VX to I
                case 0x1E:
                {
                    result = "FX1E";
                    break;
                }

                // FX0A - Waits until a key is pressed, if a key is pressed it's "value" is put in VX
                case 0x0A:
                {
                    result = "FX0A";
                    break;
                }

                // FX29 - set the I register to the address of hexadecimal character in VX
                case 0x29:
                {
                    result = "FX29";
                    break;
                }

                // FX33 - Take the number in VX, divide to 3 dec numbers (139 - 1, 3, 9), then store them in I, I+1, I+2
                case 0x33:
                {
                    result = "FX33";
                    break;
                }

                // FX55 - Set memory in I, to I+X with the values of registers V0 to VX
                case 0x55:
                {
                    result = "FX55";
                    break;
                }

                // FX65 - Set registers V0 to VX, with the values from memory of I to I+X
                case 0x65:
                {
                    result = "FX65";
                    break;
                }

                // -- SUPER CHIP --
                case 0x30:
                {
                    result = "FX30";
                    break;
                }
                case 0x75:
                {
                    result = "FX75";
                    break;
                }
                case 0x85:
                {
                    result = "FX85";
                    break;
                }
            }
            break;
        }
    }

    return result;
}

void Chip8::execute(const std::string& which, const Instruction<Chip8_t::Word>& instruction)
{
    if(m_exec_map.find(which) == m_exec_map.end())
    {
        printf("INVALID INSTRUCTION %s ORIGINATING FROM %04X\n", which.c_str(), instruction.get());
        return;
    }

    (m_exec_map.at(which))(instruction);
}

// --- Constructors ----

Chip8::Chip8() : 
//    m_memory{Chip8Const::mem_size},
//    m_display{Chip8Const::screen_width, Chip8Const::screen_height},
//    m_PC{},
//    m_I{},/
//    m_stack{},
//    m_delay_timer{},
//    m_sound_timer{}, 
//    m_regs{},
//    m_key_states{},
    m_behaviour{Chip8::BehaviourType::CHIP8}
{
    clearMemory();

    // Initalize exec map
    m_exec_map = 
    {
        // CHIP8
        {"00E0", std::bind(&Chip8::_00E0, this, std::placeholders::_1)},
        {"00EE", std::bind(&Chip8::_00EE, this, std::placeholders::_1)},
        {"0NNN", std::bind(&Chip8::_0NNN, this, std::placeholders::_1)},
        {"1NNN", std::bind(&Chip8::_1NNN, this, std::placeholders::_1)},
        {"2NNN", std::bind(&Chip8::_2NNN, this, std::placeholders::_1)},
        {"3XNN", std::bind(&Chip8::_3XNN, this, std::placeholders::_1)},
        {"4XNN", std::bind(&Chip8::_4XNN, this, std::placeholders::_1)},
        {"5XY0", std::bind(&Chip8::_5XY0, this, std::placeholders::_1)},
        {"6XNN", std::bind(&Chip8::_6XNN, this, std::placeholders::_1)},
        {"7XNN", std::bind(&Chip8::_7XNN, this, std::placeholders::_1)},
        {"8XY0", std::bind(&Chip8::_8XY0, this, std::placeholders::_1)},
        {"8XY1", std::bind(&Chip8::_8XY1, this, std::placeholders::_1)},
        {"8XY2", std::bind(&Chip8::_8XY2, this, std::placeholders::_1)},
        {"8XY3", std::bind(&Chip8::_8XY3, this, std::placeholders::_1)},
        {"8XY4", std::bind(&Chip8::_8XY4, this, std::placeholders::_1)},
        {"8XY5", std::bind(&Chip8::_8XY5, this, std::placeholders::_1)},
        {"8XY6", std::bind(&Chip8::_8XY6, this, std::placeholders::_1)},
        {"8XY7", std::bind(&Chip8::_8XY7, this, std::placeholders::_1)},
        {"8XYE", std::bind(&Chip8::_8XYE, this, std::placeholders::_1)},
        {"9XY0", std::bind(&Chip8::_9XY0, this, std::placeholders::_1)},
        {"ANNN", std::bind(&Chip8::_ANNN, this, std::placeholders::_1)},
        {"BXNN", std::bind(&Chip8::_BXNN, this, std::placeholders::_1)},
        {"CXNN", std::bind(&Chip8::_CXNN, this, std::placeholders::_1)},
        {"DXYN", std::bind(&Chip8::_DXYN, this, std::placeholders::_1)},
        {"EX9E", std::bind(&Chip8::_EX9E, this, std::placeholders::_1)},
        {"EXA1", std::bind(&Chip8::_EXA1, this, std::placeholders::_1)},
        {"FX07", std::bind(&Chip8::_FX07, this, std::placeholders::_1)},
        {"FX0A", std::bind(&Chip8::_FX0A, this, std::placeholders::_1)},
        {"FX15", std::bind(&Chip8::_FX15, this, std::placeholders::_1)},
        {"FX18", std::bind(&Chip8::_FX18, this, std::placeholders::_1)},
        {"FX1E", std::bind(&Chip8::_FX1E, this, std::placeholders::_1)},
        {"FX29", std::bind(&Chip8::_FX29, this, std::placeholders::_1)},
        {"FX33", std::bind(&Chip8::_FX33, this, std::placeholders::_1)},
        {"FX55", std::bind(&Chip8::_FX55, this, std::placeholders::_1)},
        {"FX65", std::bind(&Chip8::_FX65, this, std::placeholders::_1)},

        // SCHIP
        {"00CN", std::bind(&Chip8::_00CN, this, std::placeholders::_1)},
        {"00FB", std::bind(&Chip8::_00FB, this, std::placeholders::_1)},
        {"00FC", std::bind(&Chip8::_00FC, this, std::placeholders::_1)},
        {"00FD", std::bind(&Chip8::_00FD, this, std::placeholders::_1)},
        {"00FE", std::bind(&Chip8::_00FE, this, std::placeholders::_1)},
        {"00FF", std::bind(&Chip8::_00FF, this, std::placeholders::_1)},
        {"FX30", std::bind(&Chip8::_FX30, this, std::placeholders::_1)},
        {"FX75", std::bind(&Chip8::_FX75, this, std::placeholders::_1)},
        {"FX85", std::bind(&Chip8::_FX85, this, std::placeholders::_1)},
    };
}

// --- Member functions ---

void Chip8::setBehaviourType(Chip8::BehaviourType type)
{
    m_behaviour = type;
}

Chip8::BehaviourType Chip8::getBehaviourType()
{
    return m_behaviour;
}

bool Chip8::loadMemory(const std::string& path)
{
    // Open file
    std::ifstream file{path, std::ios::binary};
    if(!file.is_open())
    {
        return false;
    }

    // Write file contents to memory
    Chip8_t::Byte byte{};
    Chip8_t::Word index{Chip8Const::rom_mem_start};
    while(file.read(reinterpret_cast<char*>(&byte), 1))
    {
        m_memory.write(index, byte);
        ++index;
        if(index >= 4096)
        {
            break;
        }
    }

    file.close();
    return true;
}

void Chip8::clearMemory()
{
    std::array lowres_font
    {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    std::array highres_font
    {
        // 0
        0x3C, 0x42, 0x81, 0x81, 0x81,
        0x81, 0x81, 0x81, 0x42, 0x3C,
        // 1
        0x18, 0x38, 0x58, 0x18, 0x18,
        0x18, 0x18, 0x18, 0x18, 0x7E,
        // 2
        0x3C, 0x42, 0x81, 0x01, 0x02,
        0x0C, 0x30, 0x40, 0x80, 0xFF,
        // 3
        0x3C, 0x42, 0x81, 0x01, 0x06,
        0x06, 0x01, 0x81, 0x42, 0x3C,
        // 4
        0x0C, 0x14, 0x24, 0x44, 0x84,
        0xFF, 0x04, 0x04, 0x04, 0x04,
        // 5
        0xFF, 0x80, 0x80, 0xFC, 0x02,
        0x01, 0x01, 0x81, 0x42, 0x3C,
        // 6
        0x3C, 0x42, 0x81, 0x80, 0xBC,
        0xC2, 0x81, 0x81, 0x42, 0x3C,
        // 7
        0xFF, 0x01, 0x02, 0x04, 0x08,
        0x10, 0x20, 0x40, 0x80, 0x80,
        // 8
        0x3C, 0x42, 0x81, 0x42, 0x3C,
        0x42, 0x81, 0x81, 0x42, 0x3C,
        // 9
        0x3C, 0x42, 0x81, 0x81, 0x43,
        0x3D, 0x01, 0x81, 0x42, 0x3C,
        // A
        0x3C, 0x42, 0x81, 0x81, 0xFF,
        0x81, 0x81, 0x81, 0x42, 0x3C,
        // B
        0xFC, 0x42, 0x41, 0x42, 0x7C,
        0x42, 0x41, 0x41, 0x42, 0xFC,
        // C
        0x3C, 0x42, 0x81, 0x80, 0x80,
        0x80, 0x80, 0x81, 0x42, 0x3C,
        // D
        0xFC, 0x42, 0x41, 0x41, 0x41,
        0x41, 0x41, 0x41, 0x42, 0xFC,
        // E
        0xFF, 0x80, 0x80, 0x80, 0xFC,
        0x80, 0x80, 0x80, 0x80, 0xFF,
        // F
        0xFF, 0x80, 0x80, 0x80, 0xFC,
        0x80, 0x80, 0x80, 0x80, 0x80
    };

    // Set base memory
    m_memory = {Chip8Const::mem_size};

    for(int i{}; i < lowres_font.size(); ++i)
    {
        m_memory.write(Chip8Const::lowres_font_begin + i,  lowres_font[i]);
    }

    for(int i{}; i < highres_font.size(); ++i)
    {
        m_memory.write(Chip8Const::highres_font_begin + i,  highres_font[i]);
    }

    // Set display
    m_display = {Chip8Const::lowres_screen_width, Chip8Const::lowres_screen_height};

    // Set PC
    m_PC = Chip8Const::rom_mem_start;

    // Set I
    m_I = 0;

    // Set stack
    m_stack = {};

    // Set timers
    m_delay_timer = {};
    m_sound_timer = {};

    // Set regs
    m_regs = {Chip8Const::reg_amount};

    // Key states
    for(int i{}; i < Chip8Const::buttons; ++i)
    {
        m_key_states[i] = Chip8::KeyState::UP;
    }
}

Chip8::SaveState Chip8::getSaveState()
{
    SaveState state{};

    // Save memory
    /*
    for(Chip8_t::Word i{}; i < Chip8Const::mem_size; ++i)
    {
        state.memory.write(i, m_memory.read(i));
    }
    */
    state.memory = m_memory;

    // Save display
    /*
    for(Chip8_t::Word y{}; y < Chip8Const::screen_height; ++y)
    {
        for(Chip8_t::Word x{}; x < Chip8Const::screen_width; ++x)
        {
            state.display.setPixel(x, y, m_display.getPixel(x, y));
        }
    }
    */
    state.display = m_display;

    // Save PC & I
    state.PC = m_PC;
    state.I = m_I;

    // Save stack
    state.stack = m_stack;

    // Save regs
    state.regs = m_regs;

    return state;
}

void Chip8::loadSaveState(Chip8::SaveState state)
{
    m_memory = state.memory;
    m_display = state.display;
    m_PC = state.PC;
    m_I = state.I;
    m_stack = state.stack;
    m_regs = state.regs;

}

void Chip8::emulateStep()
{
    // Fetch
    Instruction<Chip8_t::Word> operation{fetch()};

    // Ensure PC and I validness
    if(m_PC >= Chip8Const::mem_size)
    {
        printf("PC (%04X) out of bounds! Setting it to 0xFFF - 1!\n", m_PC);
        m_PC = Chip8Const::mem_size - 2;
    }
    if(m_I >= Chip8Const::mem_size)
    {
        printf("I (%04X) out of bounds! Setting it to 0xFFF - 1!\n", m_PC);
        m_I = Chip8Const::mem_size - 2;
    }

    // Decode & Execute
    execute(decode(operation), operation);
}

bool Chip8::getPixel(Chip8_t::Byte x, Chip8_t::Byte y)
{
    return m_display.getPixel(x, y);
}

void Chip8::setKeyState(Chip8_t::Byte which, Chip8::KeyState state)
{
    m_key_states[which] = state;
}

Chip8::KeyState Chip8::getKeyState(Chip8_t::Byte which)
{
    return m_key_states[which];
}

bool Chip8::shouldBeep()
{
    return m_sound_timer.get() > 0;
}

Chip8_t::Byte Chip8::getMemoryAt(Chip8_t::Word where)
{
    return m_memory.read(where);
}

Chip8_t::Word Chip8::getPC()
{
    return m_PC;
}

Chip8_t::Word Chip8::getI()
{
    return m_I;
}

Chip8_t::Byte Chip8::getReg(Chip8_t::Byte which)
{
    return m_regs.read(which);
}

std::stack<Chip8_t::Word> Chip8::getStackCopy()
{
    return m_stack;
}

std::uint8_t Chip8::getDelayTimerValue()
{
    return m_delay_timer.get();
}

std::uint8_t Chip8::getSoundTimerValue()
{
    return m_sound_timer.get();
}

std::uint8_t Chip8::getScreenWidth()
{
    return m_display.getWidth();
}

std::uint8_t Chip8::getScreenHeight()
{
    return m_display.getHeight();
}
