#ifndef MEMORY_HPP
#define MEMORY_HPP
#include <vector>

class Memory
{
private:
    std::vector<unsigned char> m_data{};
public:
    // --- Constructors ---

    //  Name:           Memory class constructor
    //  Description:    Creates a new Memory object
    //  Arguments:       size - the amount of bytes that the memory can hold
    Memory(unsigned short size);

    // --- Member functions ---

    //  Name:           write
    //  Description:    writes a byte to the address provided
    //  Arguments:      where   - the address of the byte to write to
    //                  what    - the byte to write in the address 
    void write(unsigned short where, unsigned char what);

    //  Name:           read
    //  Description:    reads and returns the byte at the provided address
    //  Arguments:      where   - the address of the byte to write to
    //  Return:         the Byte stored at the provided address
    unsigned char read(unsigned short where);

    //  Name:           getSize
    //  Description:    returns the size of the memory
    //  Return:         the size of the memory (in Bytes)
    unsigned short getSize();
};

#endif