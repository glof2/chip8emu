#include "../header/Memory.hpp"
#include <iostream>


// -- Constructors --

Memory::Memory(std::uint16_t  size) : m_data(size, 0) {}


// -- Member functions --

void Memory::write(std::uint16_t where, std::uint8_t what)
{
    if(where >= m_data.size())
    {
        std::cout << "Attempting to write out of Memory bounds!\n";
        std::cout << "The memory is of size (dec): " << (int)m_data.size() << '\n';
        std::cout << "Attempted to write \'" << std::hex << what << std::dec << "\' to address (dec): " << where << '\n'; 
        return;
    }

    m_data[where] = what;
}

std::uint8_t  Memory::read(std::uint16_t where)
{
    if(where >= m_data.size())
    {
        std::cout << "Attempting to read out of Memory bounds!\n";
        std::cout << "The memory is of size (dec): " << (int)m_data.size() << '\n';
        std::cout << "Attempted to read address (dec): " << where << '\n'; 
        return 0;
    }
    return m_data[where];
}

std::uint16_t  Memory::getSize()
{
    return m_data.size();
}