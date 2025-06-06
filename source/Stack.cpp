#include "../header/Stack.hpp"
#include <iostream>

unsigned short Stack::pop()
{
    if(m_top == nullptr)
    {
        return 0;
    }
    unsigned short val{ m_top->value };
    StackElement* element{m_top->prev };
    delete m_top;
    m_top = element;
    return val;
}

void Stack::push(unsigned short value)
{
    StackElement* new_element{ new StackElement{} };
    new_element->value = value;
    new_element->prev = m_top;
    m_top = new_element;
}