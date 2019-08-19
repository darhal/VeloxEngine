#include "DoubleStackAllocator.hpp"

TRE_NS_START

DoubleStackAllocator::DoubleStackAllocator(usize size, bool autoInit) : m_Current(0)
{
    if (autoInit){
        for (uint8 i = 1; i < MAX_STACKS; i++){
            m_Stacks[i].SetSize(size);
            m_Stacks[i].Init();
        }
    }else{
        for (uint8 i = 1; i < MAX_STACKS; i++){
            m_Stacks[i].SetSize(size);
        }
    }
}

TRE_NS_END