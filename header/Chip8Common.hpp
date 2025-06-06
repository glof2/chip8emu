#ifndef CHIP8_CONSTANTS
#define CHIP8_CONSTANTS
#include <stdint.h>

namespace Chip8Const
{
    inline constexpr unsigned char screen_height{ 32 };
    inline constexpr unsigned char screen_width{ 64 };
    inline constexpr unsigned short mem_size{ 4096 };
    inline constexpr unsigned char buttons{0xF + 1};
}

namespace Chip8_t
{
    typedef uint8_t Byte;
    typedef uint16_t Word;
}

#endif