#include "../header/Timer.hpp"
#include <iostream>

int64_t Timer::getTime()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

Timer::Timer()
{
    m_start_val = 0;
    m_time_started = getTime();
}

void Timer::set(std::uint8_t value)
{
    m_start_val = value;
    m_value = value;
    m_time_started = getTime();
}

void Timer::update()
{
    //std::cout << "Timer updating.\n";
    int64_t now{ getTime() };
    double elapsed{ ((now - m_time_started)/1000.0) };
    //std::cout << "Elapsed since start: " << elapsed << "s\n";
    //std::cout << "Calculated by doing (" << now << " - "  << m_time_started << ")/1000.0\n"; 
    //std::cout << "Timer started at " << (int)m_start_val << ", removing " << (int)elapsed * 60 << " val\n";  
    int64_t to_remove{ elapsed*60 };
    if(to_remove > m_start_val)
    {
        to_remove = m_start_val;
    }
    m_value = m_start_val - to_remove;
    //std::cout << "FInal m_value: " << (int)m_value << '\n';

}

std::uint8_t Timer::get()
{
    update();
    return m_value;
}