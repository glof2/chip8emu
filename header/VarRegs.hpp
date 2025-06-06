#ifndef VARREGS_HPP
#define VARREGS_HPP

class VarRegs
{
private:
    unsigned char m_regs[0xF+1];
public:
    unsigned char read(unsigned char which);
    void write(unsigned char which, unsigned char value);
};

#endif