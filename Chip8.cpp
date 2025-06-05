#include <iostream>
#include <fstream>
#include <iomanip>
#include "Chip8.hpp"

Chip8::Chip8()
{
    // Load fonts
    
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
            m_PC = (&(m_memory.data[0])) + location;
            break;
        }
        case 0x6:
        {
            // For now set register VX to NN (0x6XNN)
            unsigned char value{ (unsigned char)(nibbles[2] * 0x10 + nibbles[3]) };
            m_regs.write(nibbles[1], value);
            break;
        }
        case 0x7:
        {
            // For now add value to register VX
            unsigned char value{ (unsigned char)(m_regs.read(nibbles[1])) };
            unsigned char add{ nibbles[2] * 0x10 + nibbles[3]  };
            m_regs.write(nibbles[1], value + add);
            break;
        }
        case 0xA:
        {
            // For now set register I
            unsigned short value{ (unsigned short)(nibbles[1] * 0x100 + nibbles[2] * 0x10 + nibbles[3]) };
            m_I = value;
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
    }
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