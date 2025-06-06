#include "../header/VarRegs.hpp"
#include <iomanip>
#include <iostream>

unsigned char VarRegs::read(unsigned char which)
{
    return m_regs[which];
}

void VarRegs::write(unsigned char which, unsigned char value)
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