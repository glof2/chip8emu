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
    inline constexpr Chip8_t::Byte lowres_screen_width{ 64 };
    inline constexpr Chip8_t::Byte lowres_screen_height{ 32 };

    inline constexpr Chip8_t::Byte highres_screen_width{ 128 };
    inline constexpr Chip8_t::Byte highres_screen_height{ 64 };

    inline constexpr Chip8_t::Word mem_size{ 0xFFF + 1 };
    inline constexpr Chip8_t::Byte buttons{0xF + 1};
    inline constexpr Chip8_t::Word lowres_font_begin{ 0 };
    inline constexpr Chip8_t::Word highres_font_begin{0x100};
    inline constexpr Chip8_t::Byte reg_amount{ 0xF+1 };
    inline constexpr Chip8_t::Word rom_mem_start{0x200};
}



#endif
