#include "../header/VarRegs.hpp"
#include <iomanip>
#include <iostream>

VarRegs::VarRegs(std::uint8_t amount) : m_regs(amount, 0x0) {}

std::uint8_t VarRegs::read(std::uint8_t which)
{
    if(which >= m_regs.size())
    {
        std::cout << "ATTEMPTING TO READ INVALID REG " << std::hex << (int)which << std::dec << "!\n";
        std::cout << "REG AMOUNT: " << m_regs.size() << '\n';
        return 0;
    }
    return m_regs[which];
}

void VarRegs::write(std::uint8_t which, std::uint8_t value)
{
    if(which >= m_regs.size())
    {
        std::cout << "ATTEMPTING TO WRITE TO INVALID REG " << std::hex << (int)which << std::dec << "!\n";
        std::cout << "REG AMOUNT: " << m_regs.size() << '\n';
        return;
    }
    m_regs[which] = value;
}