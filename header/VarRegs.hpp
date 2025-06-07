#ifndef VARREGS_HPP
#define VARREGS_HPP
#include <cstdint>
#include <vector>

class VarRegs
{
private:
    std::vector<std::uint8_t> m_regs{};
public:
    VarRegs(std::uint8_t amount);
    std::uint8_t read(std::uint8_t  which);
    void write(std::uint8_t which, std::uint8_t value);
};

#endif