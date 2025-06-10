#ifndef CHIP8_CONSTANTS
#define CHIP8_CONSTANTS
#include <cstdint>


namespace Chip8_t
{
    typedef std::uint8_t Byte;
    typedef std::uint16_t Word;
}

namespace Chip8Const
{
    inline constexpr Chip8_t::Byte screen_height{ 32 };
    inline constexpr Chip8_t::Byte screen_width{ 64 };
    inline constexpr Chip8_t::Word mem_size{ 0xFFF + 1 };
    inline constexpr Chip8_t::Byte buttons{0xF + 1};
    inline constexpr Chip8_t::Word font_begin{ 0 };
    inline constexpr Chip8_t::Byte reg_amount{ 0xF+1 };
    inline constexpr Chip8_t::Word rom_mem_start{0x200};
}



#endif