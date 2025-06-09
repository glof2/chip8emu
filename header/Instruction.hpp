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

    //  Description:    a constructor for the Instruction class
    //  Arguments:      instruction - the instruction to initialize it with
    Instruction(const Instr_t& instruction);

    //  Description:    a constructor for the Instruction class
    //  Arguments:      nibbles - Nibbles to create the Instruction with
    Instruction(const std::array<std::uint8_t, sizeof(Instr_t) * 2>& nibbles);

    //  Description:    a constructor for the Instruction class
    //  Arguments:      bytes - bytes to create the Instruction with
    Instruction(const std::array<std::uint8_t, sizeof(Instr_t)>& bytes);

    //
    std::uint16_t getAmountOfNibbles() const;

    //  
    std::uint8_t getNibble(const std::uint16_t& which) const;

    //
    std::uint64_t getNibbles(const std::uint16_t& from, const std::uint16_t& to) const;
    
    // 
    Instr_t get();

};

#include "template_defs/Instruction.tpp"

#endif