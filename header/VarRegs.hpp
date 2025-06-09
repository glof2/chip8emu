#ifndef VARREGS_HPP
#define VARREGS_HPP
#include <cstdint>
#include <vector>

class VarRegs
{
private:
    std::vector<std::uint8_t> m_regs{};
public:
    //  Description:    VarRegs class constructor, creates a new VarRegs object which can hold up to 'amount' registers
    //  Arguments:      amount - the amount of regs that VarRegs can hold    
    VarRegs(std::uint8_t amount);

    //  Name:           read
    //  Description:    returns the value of the register at 'which'
    //  Arguments:      which - the index of the register
    //  Return:         the value of the register at 'which'
    std::uint8_t read(std::uint8_t  which);

    //  Name:           write
    //  Description:    writes 'value' to the register at 'which'
    //  Arguments:      which - the index of the register
    //                  value - the value to write to the register
    void write(std::uint8_t which, std::uint8_t value);
};

#endif