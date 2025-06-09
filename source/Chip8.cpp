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

Instruction<Chip8_t::Word> Chip8::fetch()
{
    Instruction<Chip8_t::Word> operation{ std::array<Chip8_t::Byte, 2>{m_memory.read(m_PC), m_memory.read(m_PC + 1)} };
    m_PC += 2;

    return operation;
}

void Chip8::decodeExecute(const Instruction<Chip8_t::Word>& instruction)
{
    // Decode & execute
    switch(instruction.getNibble(0))
    {
        case 0x0:
        {
            // Possible instructions:
            // 00E0 - Clear screen
            // 00EE - Set PC to the value at top of the stack
            switch (instruction.getNibble(3))
            {
                // 00E0 - Clear screen
                case 0x0:
                {
                    m_display.setAll(0);
                    break;
                }

                // 00EE - Set PC to the value at top of the stack
                case 0xE:
                {
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
            // Possible instructions:
            // 0x1NNN - Jump to NNN
            Chip8_t::Word location{ (Chip8_t::Word)(instruction.getNibbles(1, 3)) };
            jumpTo(location);
            break;
        }
        case 0x2:
        {
            // Possible instructions:
            // 2NNN - add current PC to stack, and jump to NNN
            m_stack.push(m_PC);
            Chip8_t::Word location{ (Chip8_t::Word)(instruction.getNibbles(1, 3)) };
            jumpTo(location);

            break;
        }
        case 0x3:
        {
            // Possible instructions:
            // 3XNN - Skip one instruction if value in VX == NN
            Chip8_t::Byte val_1{ m_regs.read(instruction.getNibble(1)) };
            Chip8_t::Byte val_2{ (Chip8_t::Byte)(instruction.getNibbles(2, 3)) };

            if(val_1 == val_2)
            {
                m_PC += 2;
            }
            break;
        }
        case 0x4:
        {
            // Possible instructions:
            // 4XNN - Skip one instruction if value in VX != NN
            Chip8_t::Byte val_1{ m_regs.read(instruction.getNibble(1)) };
            Chip8_t::Byte val_2{ (Chip8_t::Byte)(instruction.getNibbles(2, 3)) };

            if(val_1 != val_2)
            {
                m_PC += 2;
            }
            break;
        }
        case 0x5:
        {
            // Possible instructions:
            // 5XY0 - Skip if values in VX == VY
            Chip8_t::Byte val_1{ m_regs.read(instruction.getNibble(1)) };
            Chip8_t::Byte val_2{ m_regs.read(instruction.getNibble(2)) };

            if(val_1 == val_2)
            {
                m_PC += 2;
            }
            break;
        }
        case 0x6:
        {
            // Possible instructions:
            // 6XNN - set register VX to NN
            Chip8_t::Byte value{ (Chip8_t::Byte)(instruction.getNibbles(2, 3)) };
            m_regs.write(instruction.getNibble(1), value);
            break;
        }
        case 0x7:
        {
            // 7XNN - add NN to register VX
            Chip8_t::Byte value{ (Chip8_t::Byte)(m_regs.read(instruction.getNibble(1))) };
            Chip8_t::Byte add{ (Chip8_t::Byte) (instruction.getNibbles(2, 3)) };
            m_regs.write(instruction.getNibble(1), value + add);
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
            // 8XY
            Chip8_t::Byte set_value{};
            Chip8_t::Byte vx{ m_regs.read(instruction.getNibble(1)) };
            Chip8_t::Byte vy{ m_regs.read(instruction.getNibble(2)) };
            Chip8_t::Byte vf_value{ (m_regs.read(0xF)) };
            switch (instruction.getNibble(3))
            {
                // 8XY0 - VX is set to VY
                case 0x0:
                {
                    set_value = vy;
                    break;
                }

                // 8XY1 - VX is set to bitwise OR of VX and VY, if on CHIP8 Cosmac set VF to 0
                case 0x1:
                {
                    if(m_behaviour == Chip8::BehaviourType::CHIP8)
                    {
                        vf_value = 0;
                    }
                    set_value = vx | vy;
                    break;
                }

                // 8XY2 - VX is set to bitwise AND of VX and VY, if on CHIP8 Cosmac set VF to 0
                case 0x2:
                {
                    if(m_behaviour == Chip8::BehaviourType::CHIP8)
                    {
                        vf_value = 0;
                    }
                    set_value = vx & vy;
                    break;
                }

                // 8XY3 - VX is set to VX XOR VY, if on CHIP8 Cosmac set VF to 0
                case 0x3:
                {
                    if(m_behaviour == Chip8::BehaviourType::CHIP8)
                    {
                        vf_value = 0;
                    }
                    set_value = vx ^ vy;
                    break;
                }

                // 8XY4 - Set VX to VX + VY, if the addition overflows VX (VX + VY > 0xFF) set VF to 1, otherwise set VF to 1
                case 0x4:
                {
                    vf_value = Chip8_t::Byte(vx + vy > 0xFF);
                    set_value = (Chip8_t::Byte) (vx + vy);
                    break;
                }

                // 8XY5 - Set VX to VX - VY, if VX >= VY then set VF to 1 , otherwise set VF to 0
                case 0x5:
                {
                    vf_value = Chip8_t::Byte(vx >= vy);
                    set_value = (Chip8_t::Byte)(vx - vy);
                    break;
                }

                // 8XY7 - Set VX to VY - VX, if VY >= VX then set VF to 1 , otherwise set VF to 0
                case 0x7:
                {
                    vf_value = Chip8_t::Byte(vy >= vx);
                    set_value = (Chip8_t::Byte)(vy - vx);
                    break;
                }

                // 8XY6:
                // CHIP8: Set VX to VY, and shift VX one bit to the right, set VF to the bit shifted out
                // LATER: Shift VX one bit to the right, set VF to the bit shifted out
                case 0x6:
                {
                    if(m_behaviour == Chip8::BehaviourType::CHIP8)
                    {
                        vx = vy;
                    }

                    vf_value = vx & 0b00000001;

                    set_value = vx >> 1;

                    break;
                }

                // 8XYE:
                // CHIP8: Set VX to VY, and shift VX one bit to the left, set VF to the bit shifted out
                // LATER: Shift VX one bit to the left, set VF to the bit shifted out
                case 0xE:
                {
                    if(m_behaviour == Chip8::BehaviourType::CHIP8)
                    {
                        vx = vy;
                    }

                    // Get the bit shifted out (if the result of vx & 0b10000000 > 0, then the bit shifted out was 1)
                    vf_value = vx & 0b10000000;
                    if (vf_value > 0)
                    {
                        vf_value = 1;
                    }

                    set_value = vx << 1;

                    break;
                }
                
            }

            m_regs.write(instruction.getNibble(1), set_value);
            m_regs.write(0xF, vf_value);
            
            break;
        }
        case 0x9:
        {
            // Possible instructions:
            // 9XY0 - Skip one instruction if values in VX != VY
            Chip8_t::Byte val_1{ m_regs.read(instruction.getNibble(1)) };
            Chip8_t::Byte val_2{ m_regs.read(instruction.getNibble(2)) };

            if(val_1 != val_2)
            {
                m_PC += 2;
            }
            break;
        }
        case 0xA:
        {
            // Possible instructions
            // ANNN - set index I to NNN
            Chip8_t::Word value{ (Chip8_t::Word)(instruction.getNibbles(1, 3)) };
            m_I = value;
            break;
        }
        case 0xB:
        {
            // Possible instructions:
            // Legacy:
            // BNNN - Jump to NNN plus the value in V0
            // New:
            // BXNN - Jump to XNN plus the value in register VX

            // Convert NNN to a single number
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

            break;
        }
        case 0xC:
        {
            // Possible instructions:
            // CXNN - generates a random number and binary ANDs it with NN, then puts the result in VX
            Chip8_t::Byte random{ (Chip8_t::Byte) (rand() % 256) };
            Chip8_t::Byte value{ (Chip8_t::Byte) (instruction.getNibbles(2, 3)) };
            m_regs.write(instruction.getNibble(1), random & value);
            break;
        }
        case 0xD:
        {
            // Possible instructions
            // 0xDXYN - Draw a N height sprite to the screen at coordinates (VX, VY) from the location of the I registed
            //          if any of the pixels were flipped as a result of this set VF to 1, otherwise it's set to 0


            Chip8_t::Byte x{ (Chip8_t::Byte)(m_regs.read(instruction.getNibble(1)) % Chip8Const::screen_width) };
            Chip8_t::Byte y{ (Chip8_t::Byte)(m_regs.read(instruction.getNibble(2)) % Chip8Const::screen_height) };
            Chip8_t::Byte n{ (Chip8_t::Byte)(instruction.getNibble(3)) };

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
            // Possible instructions:
            // EX9E - Skip one instruction if the key corresponding to value in VX is pressed
            // EXA1 - Skip one instruction if the key corresponding to value in VX is not pressed
            Chip8_t::Word val{ (Chip8_t::Byte) (instruction.getNibbles(2, 3)) };
            Chip8_t::Byte key{ (Chip8_t::Byte)(m_regs.read(instruction.getNibble(1)) % (Chip8Const::buttons))};
            switch (val)
            {
                // EX9E - Skip one instruction if the key corresponding to value in VX is pressed
                case 0x9E:
                {
                    if(m_key_states[key] == Chip8::KeyState::DOWN) 
                    {
                        m_PC += 2;
                    }
                    break;
                }

                // EXA1 - Skip one instruction if the key corresponding to value in VX is not pressed
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
                    m_regs.write(instruction.getNibble(1), m_delay_timer.get());
                    break;
                }

                // FX15 - set delay timer to current value in VX
                case 0x15:
                {
                    m_delay_timer.set(m_regs.read(instruction.getNibble(1)));
                    break;
                }

                // FX18 - set sound timer to current value in VX
                case 0x18:
                {
                    m_sound_timer.set(m_regs.read(instruction.getNibble(1)));
                    break;
                }

                // FX1E - add VX to I
                case 0x1E:
                {
                    m_I += m_regs.read(instruction.getNibble(1));
                    break;
                }

                // FX0A - Waits until a key is pressed, if a key is pressed it's "value" is put in VX
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
                        m_regs.write(instruction.getNibble(1), key);
                    }
                    break;
                }

                // FX29 - set the I register to the address of hexadecimal character in VX
                case 0x29:
                {
                    // Get the second nibble
                    Chip8_t::Byte m_char{ m_regs.read(instruction.getNibble(1)) };
                    m_char = m_char & 0b00001111;

                    // Set I to the font location
                    m_I = Chip8Const::font_begin + m_char * 5;

                    break;
                }

                // FX33 - Take the number in VX, divide to 3 dec numbers (139 - 1, 3, 9), then store them in I, I+1, I+2
                case 0x33:
                {
                    Chip8_t::Byte num{ m_regs.read(instruction.getNibble(1)) };
                    for(char i{2}; i >= 0; --i)
                    {
                        m_memory.write(m_I + i, num % 10);
                        num /= 10;
                    }
                    break;
                }

                // FX55 - Set memory in I, to I+X with the values of registers V0 to VX
                case 0x55:
                {
                    for(int i{}; i <= instruction.getNibble(1); ++i)
                    {
                        m_memory.write(m_I + i, m_regs.read(i));
                    }

                    if(m_behaviour == Chip8::BehaviourType::CHIP8)
                    {
                        m_I += instruction.getNibble(1) + 1;
                    }

                    break;
                }

                // FX65 - Set registers V0 to VX, with the values from memory of I to I+X
                case 0x65:
                {
                    for(int i{}; i <= instruction.getNibble(1); ++i)
                    {
                        m_regs.write(i, m_memory.read(m_I + i));
                    }

                    if(m_behaviour == Chip8::BehaviourType::CHIP8)
                    {
                        m_I += instruction.getNibble(1) + 1;
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
}

// --- Member functions ---

void Chip8::setBehaviourType(Chip8::BehaviourType type)
{
    m_behaviour = type;
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

void Chip8::clearMemory()
{
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

    // Set base memory
    m_memory = {Chip8Const::mem_size};

    for(int i{}; i < 80; ++i)
    {
        m_memory.write(Chip8Const::font_begin + i,  m_font[i]);
    }

    // Set display
    m_display = {Chip8Const::screen_width, Chip8Const::screen_height};

    // Set PC
    m_PC = 0x200;

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