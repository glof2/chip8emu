#ifndef TIMER_HPP
#define TIMER_HPP
#include <chrono>

class Timer
{
private:
    unsigned char m_start_val{};
    unsigned char m_value{};
    int64_t m_time_started{};

    int64_t getTime();
public:
    Timer();
    void set(unsigned char value);
    void update();
    unsigned char get();
};

#endif