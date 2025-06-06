#ifndef TIMER_HPP
#define TIMER_HPP
#include <chrono>
#include <cstdint>

class Timer
{
private:
    std::uint8_t  m_start_val{};
    std::uint8_t  m_value{};
    int64_t m_time_started{};

    int64_t getTime();
public:
    Timer();
    void set(std::uint8_t  value);
    void update();
    std::uint8_t  get();
};

#endif