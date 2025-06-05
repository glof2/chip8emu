#include "Timer.hpp"

int64_t Timer::getTime()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

Timer::Timer()
{
    m_start_val = 0;
    m_time_started = getTime();
}

void Timer::set(unsigned char value)
{
    m_start_val = value;
    m_value = value;
    m_time_started = getTime();
}

void Timer::update()
{
    int64_t elapsed{ int64_t((getTime() - m_time_started)/1000.0 * 60.0) };
    if(elapsed > m_start_val)
    {
        elapsed = m_start_val;
    }
    m_value = m_start_val - elapsed;
}

unsigned char Timer::get()
{
    update();
    return m_value;
}