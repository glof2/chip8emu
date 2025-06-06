#include "../header/Memory.hpp"
#include <iostream>


// -- Constructors --

Memory::Memory(unsigned short size) : m_data(size, 0) {}


// -- Member functions --

void Memory::write(unsigned short where, unsigned char what)
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

unsigned char Memory::read(unsigned short where)
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

unsigned short Memory::getSize()
{
    return m_data.size();
}