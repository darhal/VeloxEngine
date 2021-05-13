#include "Core/Misc/Defines/Common.hpp"

TRE_NS_START

template<typename R, typename... Args>
class Function
{
public:
    Function(R(func)(Args...)) : m_Function(func)
    {
    }

    Function() : m_Function(NULL)
    {
    }

    R Call(Args... args)
    {
        return (*m_Function)(args...);
    }

    R operator()(Args... args)
    {
        return (*m_Function)(args...);
    }

public:
    R (*m_Function)(Args...);
};

TRE_NS_END