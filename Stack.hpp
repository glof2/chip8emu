#ifndef STACK_HPP
#define STACK_HPP

struct StackElement
{
    unsigned short value{};
    StackElement* prev{};
};

class Stack
{
private:
    StackElement* m_top{};
public:
    Stack() = default;
    void push(unsigned short value);
    unsigned short pop();
};

#endif