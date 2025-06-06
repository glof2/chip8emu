#ifndef VARREGS_HPP
#define VARREGS_HPP
#include <cstdint>

class VarRegs
{
private:
    std::uint8_t m_regs[0xF+1];
public:
    std::uint8_t read(std::uint8_t  which);
    void write(std::uint8_t which, std::uint8_t value);
};

#endif