#ifndef MEMORY_HPP
#define MEMORY_HPP
#include <vector>
#include <cstdint>

class Memory
{
private:
    std::vector<std::uint8_t > m_data{};
public:
    // --- Constructors ---

    //  Name:           Memory class constructor
    //  Description:    Creates a new Memory object
    //  Arguments:       size - the amount of bytes that the memory can hold
    Memory(std::uint16_t size);

    // --- Member functions ---

    //  Name:           write
    //  Description:    writes a byte to the address provided
    //  Arguments:      where   - the address of the byte to write to
    //                  what    - the byte to write in the address 
    void write(std::uint16_t  where, std::uint8_t  what);

    //  Name:           read
    //  Description:    reads and returns the byte at the provided address
    //  Arguments:      where   - the address of the byte to write to
    //  Return:         the Byte stored at the provided address
    std::uint8_t read(std::uint16_t  where);

    //  Name:           getSize
    //  Description:    returns the size of the memory
    //  Return:         the size of the memory (in Bytes)
    std::uint16_t  getSize();
};

#endif