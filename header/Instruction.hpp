#ifndef INSTRUCTION_HPP
#define INSTRUCTION_HPP
#include <cstdint>
#include <string>
#include <array>

// The type to use for the instruction (eg. uint16_t for an instruction that can contain 16 bits like 0xFFFF)
template <typename Instr_t>
class Instruction
{
private:
    Instr_t m_instruction{};
public:
    // --- Constructors ---

    //  Description:    a constructor for the Instruction class, takes in the instruction to create the class with
    //  Arguments:      instruction - the instruction to initialize it with
    Instruction(const Instr_t& instruction);

    //  Description:    a constructor for the Instruction class, takes in an array of nibbles to create the instruction
    //  Arguments:      nibbles - Nibbles to create the Instruction with
    Instruction(const std::array<std::uint8_t, sizeof(Instr_t) * 2>& nibbles);

    //  Description:    a constructor for the Instruction class, takes in an array of bytes to create the instruction
    //  Arguments:      bytes - bytes to create the Instruction with
    Instruction(const std::array<std::uint8_t, sizeof(Instr_t)>& bytes);

    //  Name:           getAmountOfNibbles
    //  Description:    returns the amount of nibbles that the instruction can  hold
    //  Return:         the amount of nibbles that the instruction can hold
    std::uint16_t getAmountOfNibbles() const;

    //  Name:           getNibble
    //  Description:    gets the nibble at specified position
    //  Arguments:      which - the position of the nibble, valid positions: 0 to (n-1) where n is the amount of nibbles that the instruction can hold
    std::uint8_t getNibble(const std::uint16_t& which) const;

    //  Name:           getNibbles
    //  Description:    returns the nibbles from 'from' to 'to'
    //  Arguments:      from - starting nibble
    //                  to - ending nibble
    //                  valid 'from' & 'to': 0 to (n-1) where n is the amount of nibbles that the instruction can hold
    //                  'to' must be >= 'from' 
    //  Return:         The nibbles from 'from' to 'to' in a single number
    //                  for example for instruction: 0xDEAD
    //                  getNibbles(1,2) = 0xEA
    std::uint64_t getNibbles(const std::uint16_t& from, const std::uint16_t& to) const;
    
    //  Name:           get
    //  Description:    returns the full instruction held by the class object
    //  Return:         the instruction held by the class object
    Instr_t get() const;

};

#include "template_defs/Instruction.tpp"

#endif