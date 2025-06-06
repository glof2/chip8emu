#include "Stack.hpp"
#include <iostream>

unsigned short Stack::pop()
{
    if(m_top == nullptr)
    {
        std::cout << "WTF BRO STACK IS EMPTY\n";
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