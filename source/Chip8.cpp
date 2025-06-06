#include <iostream>
#include <fstream>
#include <iomanip>
#include <random>
#include "../header/Chip8.hpp"

// --- Private member functions ---

void Chip8::jumpTo(Chip8_t::Word location)
{
    m_PC = location;
}

Chip8_t::Word Chip8::fetch()
{
    Chip8_t::Word operation{  (Chip8_t::Word) (m_memory.read(m_PC) * 0x100 + m_memory.read(m_PC + 1)) };
    m_PC += 2;
    return operation;
}

void Chip8::decodeExecute(Chip8_t::Word operation)
{
    // Extract nibbles
    Chip8_t::Byte nibbles[4]{};
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
                    /*
                    for(int y{}; y < m_display.getHeight(); ++y)
                    {
                        for(int x{}; x < m_display.getWidth(); ++x)
                        {
                            m_display.setPixel(x, y, 0);
                        }
                    }
                    */
                    m_display.setAll(0);
                    break;
                }
                case 0xE:
                {
                    // Return to PC from stack
                    Chip8_t::Word location = m_stack.top();
                    m_stack.pop();
                    jumpTo(location);

                    break;
                }
            }
            
            break;
        }
        case 0x1:
        {
            // For now only jump NNN
            Chip8_t::Word location{ (Chip8_t::Word)(nibbles[1] * 0x100 + nibbles[2] * 0x10 + nibbles[3]) };
            jumpTo(location);
            break;
        }
        case 0x2:
        {
            // For now 2NNN - add current pc to stack, and jump to NNN
            m_stack.push(m_PC);
            Chip8_t::Word location{ (Chip8_t::Word)(nibbles[1] * 0x100 + nibbles[2] * 0x10 + nibbles[3]) };
            jumpTo(location);


            break;
        }
        case 0x3:
        {
            // For now 3XNN - Skip one instruction if value in VX == NN
            Chip8_t::Byte val_1{ m_regs.read(nibbles[1]) };
            Chip8_t::Byte val_2{ (Chip8_t::Byte)(nibbles[2] * 0x10 + nibbles[3]) };

            if(val_1 == val_2)
            {
                m_PC += 2;
            }
            break;
        }
        case 0x4:
        {
            // For now 4XNN - Skip one instruction if value in VX != NN
            Chip8_t::Byte val_1{ m_regs.read(nibbles[1]) };
            Chip8_t::Byte val_2{ (Chip8_t::Byte)(nibbles[2] * 0x10 + nibbles[3]) };

            if(val_1 != val_2)
            {
                m_PC += 2;
            }
            break;
        }
        case 0x5:
        {
            // For now 5XY0 - Skip if values in VX == VY
            Chip8_t::Byte val_1{ m_regs.read(nibbles[1]) };
            Chip8_t::Byte val_2{ m_regs.read(nibbles[2]) };

            if(val_1 == val_2)
            {
                m_PC += 2;
            }
            break;
        }
        case 0x6:
        {
            // For now 6XNN - set register VX to NN
            Chip8_t::Byte value{ (Chip8_t::Byte)(nibbles[2] * 0x10 + nibbles[3]) };
            m_regs.write(nibbles[1], value);
            break;
        }
        case 0x7:
        {
            // For now 7XNN - add NN to register VX
            Chip8_t::Byte value{ (Chip8_t::Byte)(m_regs.read(nibbles[1])) };
            Chip8_t::Byte add{ (Chip8_t::Byte) (nibbles[2] * 0x10 + nibbles[3]) };
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
            Chip8_t::Byte set_value{};
            Chip8_t::Byte vx{ m_regs.read(nibbles[1]) };
            Chip8_t::Byte vy{ m_regs.read(nibbles[2]) };
            bool vf_value{ (bool) (m_regs.read(0xF)) };
            switch (nibbles[3])
            {
                case 0x0:
                {
                    set_value = vy;
                    break;
                }
                case 0x1:
                {
                    if(m_legacy_beh)
                    {
                        vf_value = 0;
                    }
                    set_value = vx | vy;
                    break;
                }
                case 0x2:
                {
                    if(m_legacy_beh)
                    {
                        vf_value = 0;
                    }
                    set_value = vx & vy;
                    break;
                }
                case 0x3:
                {
                    if(m_legacy_beh)
                    {
                        vf_value = 0;
                    }
                    set_value = vx xor vy;
                    break;
                }
                case 0x4:
                {
                    vf_value = 0;
                    if (vx + vy > 255)
                    {
                        vf_value = 1;
                    }
                    set_value = (Chip8_t::Byte) (vx + vy);
                    break;
                }
                case 0x5:
                {
                    vf_value = 0;
                    if(vx >= vy)
                    {
                        vf_value = 1;
                    }
                    set_value = vx - vy;
                    break;
                }
                case 0x7:
                {
                    vf_value = 0;
                    if(vy >= vx)
                    {
                        vf_value = 1;
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

                    vf_value = vx & 0b00000001;

                    set_value = vx >> 1;

                    break;
                }
                case 0xE:
                {
                    if(m_legacy_beh)
                    {
                        vx = vy;
                    }

                    vf_value = vx & 0b10000000;

                    set_value = vx << 1;

                    break;
                }
                
            }

            m_regs.write(nibbles[1], set_value);
            m_regs.write(0xF, vf_value);
            
            break;
        }
        case 0x9:
        {
            // For now 9XY0 - Skip if values in VX != VY
            Chip8_t::Byte val_1{ m_regs.read(nibbles[1]) };
            Chip8_t::Byte val_2{ m_regs.read(nibbles[2]) };

            if(val_1 != val_2)
            {
                m_PC += 2;
            }
            break;
        }
        case 0xA:
        {
            // For now set register I
            Chip8_t::Word value{ (Chip8_t::Word)(nibbles[1] * 0x100 + nibbles[2] * 0x10 + nibbles[3]) };
            m_I = value;
            break;
        }
        case 0xB:
        {
            // Legacy:
            // BNNN - Jump to NNN plus the value in V0
            // New:
            // BXNN - Jump to NN plus the value in VX
            Chip8_t::Word dest{};
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
            Chip8_t::Byte random{ (Chip8_t::Byte) (rand() % 256) };
            Chip8_t::Byte value{ (Chip8_t::Byte) (nibbles[2]* 0x10 + nibbles[3] * 0x1) };
            m_regs.write(nibbles[1], random & value);
            break;
        }
        case 0xD:
        {
            // For now draw
            Chip8_t::Byte x{ (Chip8_t::Byte)(m_regs.read(nibbles[1]) % Chip8Const::screen_width) };
            Chip8_t::Byte y{ (Chip8_t::Byte)(m_regs.read(nibbles[2]) % Chip8Const::screen_height) };
            Chip8_t::Byte n{ (Chip8_t::Byte)(nibbles[3]) };

            // Set VF register
            m_regs.write(0xF, 0);

            for(Chip8_t::Byte byte_i{}; byte_i < n; ++byte_i)
            {
                // Exit contition
                if(y + byte_i >= m_display.getHeight())
                {
                    break;
                }

                // Go through each bit in byte
                for(char i{7}; i >= 0; --i)
                {
                    Chip8_t::Byte bit_i{ (Chip8_t::Byte)(7 - i) };
                    Chip8_t::Byte mask{ (Chip8_t::Byte) (1 << i) };
                    Chip8_t::Byte masked_number{ (Chip8_t::Byte) (m_memory.read(m_I + byte_i) & mask) };
                    bool bit{ (bool) ((Chip8_t::Byte)(masked_number >> i)) };
                    
                    // Exit condition
                    if ( x + bit_i >= m_display.getWidth())
                    {
                        break;
                    }

                    // Draw
                    if(bit == true)
                    {
                        if(getPixel(x + bit_i, y + byte_i) == true)
                        {
                            m_regs.write(0xF, 1);
                        }

                        m_display.flipPixel(x + bit_i, y + byte_i);
                        //m_display[y + byte_i][x + bit_i] = !(m_display.data[y + byte_i][x + bit_i]);
                    }

                    
                }
            }
            break;
        }
        case 0xE:
        {
            // EX9E - Skip one instruction if the key corresponding to value in VX is pressed
            // EXA1 - Skip one instruction if the key corresponding to value in VX is not pressed
            Chip8_t::Byte val{ (Chip8_t::Byte) (nibbles[2] * 0x10 + nibbles[3] * 0x1) };
            Chip8_t::Byte key{ m_regs.read(nibbles[1]) };
            if (key > 0xF)
            {
                std::cout << "Attempted to grab wrong key!\n";
                return;
            }
            switch (val)
            {
                case 0x9E:
                {
                    if(m_key_states[key] == Chip8::KeyState::DOWN) 
                    {
                        m_PC += 2;
                    }
                    break;
                }
                case 0xA1:
                {
                    if(m_key_states[key] == Chip8::KeyState::UP || m_key_states[key] == Chip8::KeyState::JUST_RELEASED)
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
            Chip8_t::Byte val{ (Chip8_t::Byte) (nibbles[2] * 0x10 + nibbles[3] * 0x1) };

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
                    Chip8_t::Byte key{0xF+1};
                    for(Chip8_t::Byte key_i{}; key_i <= 0xF; ++key_i)
                    {
                        if(m_key_states[key_i] == KeyState::JUST_RELEASED)
                        {
                            key = key_i;
                            break;
                        }
                    }
                    if(key > 0xF)
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
                    Chip8_t::Byte m_char{ m_regs.read(nibbles[1]) };
                    m_char = m_char & 0b00001111;

                    m_I = Chip8Const::font_begin + m_char * 5;

                    break;
                }
                case 0x33:
                {
                    Chip8_t::Byte num{ m_regs.read(nibbles[1]) };
                    for(char i{2}; i >= 0; --i)
                    {
                        m_memory.write(m_I + i, num % 10);
                        num /= 10;
                    }
                    break;
                }
                case 0x55:
                {
                    for(int i{}; i <= nibbles[1]; ++i)
                    {
                        m_memory.write(m_I + i, m_regs.read(i));
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
                        m_regs.write(i, m_memory.read(m_I + i));
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

// --- Constructors ----

Chip8::Chip8() : 
    m_memory{Chip8Const::mem_size},
    m_display{Chip8Const::screen_width, Chip8Const::screen_height},
    m_PC{},
    m_I{},
    m_stack{},
    m_delay_timer{},
    m_sound_timer{}, 
    m_regs{},
    m_key_states{},
    m_legacy_beh{true}
{
    // Defaults vars


    // Load fonts
    Chip8_t::Byte m_font[]
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
        m_memory.write(Chip8Const::font_begin + i,  m_font[i]);
    }

    // Set PC
    m_PC = 0x200;
}

// --- Member functions ---

void Chip8::setLegacyBeh(bool value)
{
    m_legacy_beh = value;
}

bool Chip8::load(const std::string& path)
{
    // Open file
    std::ifstream file{path, std::ios::binary};
    if(!file.is_open())
    {
        return false;
    }

    // Write file contents to memory
    Chip8_t::Byte byte{};
    Chip8_t::Word index{0x200};
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

void Chip8::emulateStep()
{
    // Fetch
    Chip8_t::Word operation{fetch()};

    // Decode & Execute
    decodeExecute(operation);
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