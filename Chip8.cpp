#include <iostream>
#include <fstream>
#include <iomanip>
#include <random>
#include "Chip8.hpp"

#define CHIP8_FONT_BEGIN 0

Chip8::Chip8()
{
    // Load fonts
    unsigned char m_font[]
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

    for(int i{}; i < 80; ++i)
    {
        m_memory.data[CHIP8_FONT_BEGIN + i] = m_font[i];
    }

    // Set PC
    m_PC = &(m_memory.data[0x200]);
}

bool Chip8::load(const std::string& path)
{
    std::ifstream file{path, std::ios::binary};
    if(!file.is_open())
    {
        return false;
    }

    unsigned char byte{};
    unsigned short index{0x200};
    while(file.read(reinterpret_cast<char*>(&byte), 1))
    {
        m_memory.data[index] = byte;
        ++index;
        if(index >= 4096)
        {
            break;
        }
    }

    file.close();
    return true;
}

void Chip8::jumpTo(unsigned short location)
{
    m_PC = (&(m_memory.data[0])) + location;
}

void Chip8::writeToMemory(unsigned short location, unsigned char value)
{
    *(&(m_memory.data[0]) + location) = value;
}

unsigned char Chip8::readFromMemory(unsigned short location)
{
    return *(&(m_memory.data[0]) + location);
}

unsigned short Chip8::getCurrentPCAddr()
{
    return m_PC - &(m_memory.data[0]);
}

unsigned short Chip8::fetch()
{
    unsigned short operation{  (unsigned short) (*(m_PC) * 0x100 + *(m_PC + 1)) };
    m_PC += 2;
    return operation;
}

void Chip8::decodeExecute(unsigned short operation)
{
    // Extract nibbles
    unsigned char nibbles[4]{};
    for(int i{3}; i >= 0; --i)
    {
        nibbles[i] = operation & 0xF;
        operation = operation >> 4;
    }

    // Decode & execute
    switch(nibbles[0])
    {
        case 0x0:
        {
            // 00E0 - Clear screen
            // 00EE - Return to PC from stack
            switch (nibbles[3])
            {
                case 0x0:
                {
                    // Clear screen
                    for(int y{}; y < CHIP8_SCREEN_HEIGHT; ++y)
                    {
                        for(int x{}; x < CHIP8_SCREEN_WIDTH; ++x)
                        {
                            m_display.data[y][x] = 0;
                        }
                    }
                    break;
                }
                case 0xE:
                {
                    // Return to PC from stack
                    unsigned short location = m_stack.pop();
                    jumpTo(location);

                    break;
                }
                default:
                {
                    std::cout << "INVALID OPERATION 0x0NNN" << '\n';
                    break;
                }
            }
            
            break;
        }
        case 0x1:
        {
            // For now only jump NNN
            unsigned short location{ (unsigned short)(nibbles[1] * 0x100 + nibbles[2] * 0x10 + nibbles[3]) };
            jumpTo(location);
            break;
        }
        case 0x2:
        {
            // For now 2NNN - add current pc to stack, and jump to NNN
            m_stack.push(getCurrentPCAddr());
            unsigned short location{ (unsigned short)(nibbles[1] * 0x100 + nibbles[2] * 0x10 + nibbles[3]) };
            jumpTo(location);


            break;
        }
        case 0x3:
        {
            // For now 3XNN - Skip one instruction if value in VX == NN
            unsigned char val_1{ m_regs.read(nibbles[1]) };
            unsigned char val_2{ nibbles[2] * 0x10 + nibbles[3] };

            if(val_1 == val_2)
            {
                m_PC += 2;
            }
            break;
        }
        case 0x4:
        {
            // For now 4XNN - Skip one instruction if value in VX != NN
            unsigned char val_1{ m_regs.read(nibbles[1]) };
            unsigned char val_2{ nibbles[2] * 0x10 + nibbles[3] };

            if(val_1 != val_2)
            {
                m_PC += 2;
            }
            break;
        }
        case 0x5:
        {
            // For now 5XY0 - Skip if values in VX == VY
            unsigned char val_1{ m_regs.read(nibbles[1]) };
            unsigned char val_2{ m_regs.read(nibbles[2]) };

            if(val_1 == val_2)
            {
                m_PC += 2;
            }
            break;
        }
        case 0x6:
        {
            // For now 6XNN - set register VX to NN
            unsigned char value{ (unsigned char)(nibbles[2] * 0x10 + nibbles[3]) };
            m_regs.write(nibbles[1], value);
            break;
        }
        case 0x7:
        {
            // For now 7XNN - add NN to register VX
            unsigned char value{ (unsigned char)(m_regs.read(nibbles[1])) };
            unsigned char add{ nibbles[2] * 0x10 + nibbles[3]  };
            m_regs.write(nibbles[1], value + add);
            break;
        }
        case 0x8:
        {
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
            // 8XY8 -   
            // Beh1:    set VX to VY
            // Beh2:    ignore VY
            // Then:    Shift VX one bit to the left, set VF to 1 if the bit shifted out was 1, or 0 if was 0
            unsigned char set_value{};
            unsigned char vx{ m_regs.read(nibbles[1]) };
            unsigned char vy{ m_regs.read(nibbles[2]) };
            switch (nibbles[3])
            {
                case 0x0:
                {
                    set_value = vy;
                    break;
                }
                case 0x1:
                {
                    set_value = vx | vy;
                    break;
                }
                case 0x2:
                {
                    set_value = vx & vy;
                    break;
                }
                case 0x3:
                {
                    set_value = vx xor vy;
                    break;
                }
                case 0x4:
                {
                    m_regs.write(0xF, 0);
                    if (vx + vy > 255)
                    {
                        m_regs.write(0xF, 1);
                    }
                    set_value = (unsigned char) (vx + vy);
                    break;
                }
                case 0x5:
                {
                    m_regs.write(0xF, 0);
                    if(vx > vy)
                    {
                        m_regs.write(0XF, 1);
                    }
                    set_value = vx - vy;
                    break;
                }
                case 0x7:
                {
                    m_regs.write(0xF, 0);
                    if(vy > vx)
                    {
                        m_regs.write(0XF, 1);
                    }
                    set_value = vy - vx;
                    break;
                }
                case 0x6:
                {
                    if(m_legacy_beh)
                    {
                        vx = vy;
                    }

                    m_regs.write(0xF, vx & 0b00000001);

                    set_value = vx >> 1;

                    break;
                }
                case 0xE:
                {
                    if(m_legacy_beh)
                    {
                        vx = vy;
                    }

                    m_regs.write(0xF, vx & 0b10000000);

                    set_value = vx << 1;

                    break;
                }
                
            }

            m_regs.write(nibbles[1], set_value);
            break;
        }
        case 0x9:
        {
            // For now 9XY0 - Skip if values in VX != VY
            unsigned char val_1{ m_regs.read(nibbles[1]) };
            unsigned char val_2{ m_regs.read(nibbles[2]) };

            if(val_1 != val_2)
            {
                m_PC += 2;
            }
            break;
        }
        case 0xA:
        {
            // For now set register I
            unsigned short value{ (unsigned short)(nibbles[1] * 0x100 + nibbles[2] * 0x10 + nibbles[3]) };
            m_I = value;
            break;
        }
        case 0xB:
        {
            // Legacy:
            // BNNN - Jump to NNN plus the value in V0
            // New:
            // BXNN - Jump to NN plus the value in VX
            unsigned short dest{};
            if(m_legacy_beh)
            {
                dest = nibbles[1] * 0x100 + nibbles[2] * 0x10 + nibbles[3] * 0x1 + m_regs.read(0x0);
            }
            else
            {
                dest = nibbles[2] * 0x10 + nibbles[3] * 0x1 + m_regs.read(nibbles[1]);
            }
            
            jumpTo(dest);

            break;
        }
        case 0xC:
        {
            // CXNN - generates a random number and binary ANDs it with NN, then puts the result in VX
            unsigned char random{ rand() % 256 };
            unsigned char value{ nibbles[2]* 0x10 + nibbles[3] * 0x1 };
            m_regs.write(nibbles[1], random & value);
            break;
        }
        case 0xD:
        {
            // For now draw
            unsigned char x{ (unsigned char)(m_regs.read(nibbles[1]) % CHIP8_SCREEN_WIDTH) };
            unsigned char y{ (unsigned char)(m_regs.read(nibbles[2]) % CHIP8_SCREEN_HEIGHT) };
            unsigned char n{ (unsigned char)(nibbles[3]) };

            // Set VF register
            m_regs.write(0xF, 0);

            unsigned char* start_byte{ &(m_memory.data[0]) + m_I };
            for(unsigned char byte_i{}; byte_i < n; ++byte_i)
            {
                // Go through each bit in byte
                for(char i{7}; i >= 0; --i)
                {
                    unsigned char bit_i{ (unsigned char)(7 - i) };
                    unsigned char mask{ 1 << i};
                    unsigned char masked_number{ *(start_byte + byte_i) & mask };
                    bool bit{ masked_number >> i };
                    
                    // Draw
                    if(bit == true)
                    {
                        if(m_display.data[y + byte_i][x + bit_i] == true)
                        {
                            m_regs.write(0xF, 1);
                        }
                        m_display.data[y + byte_i][x + bit_i] = !(m_display.data[y + byte_i][x + bit_i]);
                    }

                    // Exit condition
                    if ( x + bit_i >= CHIP8_SCREEN_WIDTH)
                    {
                        break;
                    }
                }

                // Exit contition
                if(y + byte_i >= CHIP8_SCREEN_HEIGHT)
                {
                    break;
                }
            }
            break;
        }
        case 0xE:
        {
            // EX9E - Skip one instruction if the key corresponding to value in VX is pressed
            // EXA1 - Skip one instruction if the key correspondint to value in VX is not pressed
            unsigned char val{ nibbles[2] * 0x10 + nibbles[3] * 0x1 };
            unsigned char key{ m_regs.read(nibbles[1]) };
            switch (val)
            {
                case 0x9E:
                {
                    if(m_key_states[key] == true)
                    {
                        m_PC += 2;
                    }
                    break;
                }
                case 0xA1:
                {
                    if(m_key_states[key] == false)
                    {
                        m_PC += 2;
                    }
                    break;
                }
            }
            break;
        }
        case 0xF:
        {
            // FX07 - set VX to current value of delay timer
            // FX15 - set delay timer to current value in VX
            // FX18 - set sound timer to current value in VX
            // FX1E - add VX to I
            // FX0A - Waits until a key is pressed, if a key is pressed it's "value" is put in VX
            // FX29 - set the I register to the address of hexadecimal character in VX
            // FX33 - Take the number in VX, divide to 3 dec numbers (139 - 1, 3, 9), then store them in I, I+1, I+2
            // FX55 - Set memory in I, to I+X with the values of V0 to VX
            // FX65 - Set V0 to VX, with the value from memory of I to I+X
            unsigned char val{ nibbles[2] * 0x10 + nibbles[3] * 0x1 };

            switch (val)
            {
                case 0x07:
                {
                    m_regs.write(nibbles[1], m_delay_timer.get());
                    break;
                }
                case 0x15:
                {
                    m_delay_timer.set(m_regs.read(nibbles[1]));
                    break;
                }
                case 0x18:
                {
                    m_sound_timer.set(m_regs.read(nibbles[1]));
                    break;
                }
                case 0x1E:
                {
                    m_I += m_regs.read(nibbles[1]);
                    break;
                }
                case 0x0A:
                {
                    unsigned char key{0xF+1};
                    for(unsigned char key_i{}; key_i <= 0xF; ++key_i)
                    {
                        if(m_key_states[key_i] == true)
                        {
                            key = key_i;
                            break;
                        }
                    }
                    if(key == 0xF+1)
                    {
                        m_PC -= 2;
                    }
                    else
                    {
                        m_regs.write(nibbles[1], key);
                    }
                    break;
                }
                case 0x29:
                {
                    unsigned char m_char{ m_regs.read(nibbles[1]) };
                    m_char = m_char & 0b00001111;

                    m_I = CHIP8_FONT_BEGIN + m_char * 5;

                    break;
                }
                case 0x33:
                {
                    unsigned char num{ m_regs.read(nibbles[1]) };
                    for(char i{2}; i >= 0; --i)
                    {
                        writeToMemory(m_I + i, num % 10);
                        num /= 10;
                    }
                    break;
                }
                case 0x55:
                {
                    for(int i{}; i <= nibbles[1]; ++i)
                    {
                        writeToMemory(m_I + i, m_regs.read(i));
                    }

                    if(m_legacy_beh)
                    {
                        m_I = nibbles[1] + 1;
                    }
                    break;
                }
                case 0x65:
                {
                    for(int i{}; i <= nibbles[1]; ++i)
                    {
                        m_regs.write(i, readFromMemory(m_I + i));
                    }

                    if(m_legacy_beh)
                    {
                        m_I = nibbles[1] + 1;
                    }
                    break;
                }
            }

            break;
        }
    }
}

void Chip8::setLegacyBeh(bool value)
{
    m_legacy_beh = value;
}

void Chip8::emulateStep()
{
    // Fetch
    unsigned short operation{fetch()};

    // Decode & Execute
    decodeExecute(operation);
}

bool Chip8::getPixel(unsigned char x, unsigned char y)
{
    return (m_display.data)[y][x];
}