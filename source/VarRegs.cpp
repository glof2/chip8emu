#include "../header/VarRegs.hpp"
#include <iomanip>
#include <iostream>

std::uint8_t VarRegs::read(std::uint8_t which)
{
    return m_regs[which];
}

void VarRegs::write(std::uint8_t which, std::uint8_t value)
{
    if(which <= 0xF)
    {
        m_regs[which] = value;
    }
    else
    {
        std::cout << "ATTEMPTING TO WRITE TO INVALID REG " << std::hex << (int)which << std::dec << "!\n";
    }
    
}