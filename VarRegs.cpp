#include "VarRegs.hpp"

unsigned char VarRegs::read(unsigned char which)
{
    return m_regs[which];
}

void VarRegs::write(unsigned char which, unsigned char value)
{
    m_regs[which] = value;
}