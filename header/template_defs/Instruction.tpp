#include "./../Instruction.hpp"

#include <sstream>

template <typename Instr_t>
Instruction<Instr_t>::Instruction(const Instr_t& instruction) : m_instruction{instruction} {}

template <typename Instr_t>
Instruction<Instr_t>::Instruction(const std::array<std::uint8_t, sizeof(Instr_t) * 2>& nibbles) : m_instruction{}
{
    std::uint64_t current_multiplier{ 1 };
    for(int i{ (int)nibbles.size() - 1 }; i >= 0; --i)
    {
        m_instruction += nibbles[i] * current_multiplier;
        current_multiplier *= 0x10;
    }
}

template <typename Instr_t>
Instruction<Instr_t>::Instruction(const std::array<std::uint8_t, sizeof(Instr_t)>& bytes) : m_instruction{}
{
    std::uint64_t current_multiplier{ 1 };
    for(int i{ (int)bytes.size() - 1 }; i >= 0; --i)
    {
        m_instruction += bytes[i] * current_multiplier;
        current_multiplier *= 0x100;
    }
}

template <typename Instr_t>
std::uint16_t Instruction<Instr_t>::getAmountOfNibbles() const
{
    // sizeof(Instr_t) * 8 - size in bits
    // so, sizeof(Instr_t) * 2 - amount of nibbles
    return sizeof(Instr_t) * 2;
}

template <typename Instr_t>
std::uint8_t Instruction<Instr_t>::getNibble(const std::uint16_t& which) const
{
    std::uint16_t amount{getAmountOfNibbles()};
    if(which >= amount)
    {
        std::cout << "Attempted to grab invalid nibble " << which << "! Amount of nibbles in instr: " << amount << '\n';
        return 0;
    }

    std::uint16_t shift = (amount - 1 - which) * 4;
    return (m_instruction >> shift) & 0xF;
}

template <typename Instr_t>
Instr_t Instruction<Instr_t>::getNibbles(const std::uint16_t& from, const std::uint16_t& to) const
{
    // Check if nibbles are valid
    // sizeof(Instr_t) * 8 - size in bits
    // so, sizeof(Instr_t) * 4 - amount of nibbles
    std::uint16_t amount { getAmountOfNibbles() };
    if(from >= amount || to >= amount || from > to)
    {
        std::cout << "Attempted to grab invalid nibbles from " << from  << " to " << to << "! Amount of nibbles in instr: " << amount << '\n';
        return 0;
    }

    std::stringstream ss{};
    ss << std::hex;
    for(std::uint16_t i{from}; i <= to; ++i)
    {
        ss << (unsigned int)getNibble(i);
    }

    std::uint64_t ret{};
    ss >> ret;
    return ret;
}

template <typename Instr_t>
Instr_t Instruction<Instr_t>::get() const
{
    return m_instruction;
}