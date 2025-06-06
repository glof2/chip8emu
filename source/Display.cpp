#include "../header/Display.hpp"
#include <iostream>

// --- Constructors ---

Display::Display(unsigned short width, unsigned short height) : m_data(height, std::vector<bool>(width, false)) {}

// --- Member functions ---

void Display::setPixel(unsigned short x, unsigned short y, bool state)
{
    if(x > getWidth() || y > getHeight())
    {
        std::cout << "Attempting to write out-of-bounds to a Display!\n";
        std::cout << "The display size is: (" << getWidth() << ", " << getHeight() << ")\n";
        std::cout << "Attempted to set pixel at (" << x << ", " << y << ") to " << state << '\n';
        return; 
    }
    m_data[y][x] = state;
}

bool Display::getPixel(unsigned short x, unsigned short y)
{
    if(x > getWidth() || y > getHeight())
    {
        std::cout << "Attempting to read out-of-bounds to a Display!\n";
        std::cout << "The display size is: (" << getWidth() << ", " << getHeight() << ")\n";
        std::cout << "Attempted to get pixel at (" << x << ", " << y << ")\n";
        return 0; 
    }
    return m_data[y][x];
}

void Display::flipPixel(unsigned short x, unsigned short y)
{
    if(x > getWidth() || y > getHeight())
    {
        std::cout << "Attempting to write out-of-bounds to a Display!\n";
        std::cout << "The display size is: (" << getWidth() << ", " << getHeight() << ")\n";
        std::cout << "Attempted to flip pixel at (" << x << ", " << y << ")\n";
        return; 
    }
    setPixel(x, y, !getPixel(x, y));
}

void Display::setAll(bool state)
{
    for(int x{}; x < getWidth(); ++x)
    {
        for(int y{}; y < getHeight(); ++y)
        {
            setPixel(x, y, state);
        }
    }
}

unsigned short Display::getWidth()
{
    if(getHeight() > 0)
    {
        return (m_data[0]).size();
    }
    else
    {
        std::cout << "Couldn't get Display width properly because the Display height is not > 0!\n";
        return 0;
    }
}

unsigned short Display::getHeight()
{
    return m_data.size();
}