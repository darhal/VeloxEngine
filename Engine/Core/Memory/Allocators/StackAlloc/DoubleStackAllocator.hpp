#pragma once

#include "Core/Misc/Defines/Common.hpp"
#include "StackAllocator.hpp"

TRE_NS_START

class DoubleStackAllocator : BaseAllocator
{
public:
    DoubleStackAllocator(usize size, bool autoInit = false);

    DoubleStackAllocator() : m_Current(0) {}


    ~DoubleStackAllocator() {};

	FORCEINLINE DoubleStackAllocator& Init();

	FORCEINLINE DoubleStackAllocator& Reset();

	FORCEINLINE void Free();

	FORCEINLINE void* Allocate(usize size, usize alignment) override;

	FORCEINLINE void Deallocate(void* ptr) override;

	template<typename U, typename... Args>
	FORCEINLINE U* Allocate(Args&&... args);

	template<typename T>
	FORCEINLINE void Deallocate(T* obj);

	FORCEINLINE void SetSize(usize nSize);

	FORCEINLINE void Resize(usize nSize = 0);

    FORCEINLINE void Swap();

    FORCEINLINE void SwitchTo(uint8 id);

    FORCEINLINE StackAllocator& GetCurrentStack();

    FORCEINLINE StackAllocator& GetStackAllocatorFromID(uint8 id);

    FORCEINLINE static uint8 GetMaxCount();

private:
    CONSTEXPR static uint8 MAX_STACKS = 2;

    StackAllocator m_Stacks[MAX_STACKS];
    uint8 m_Current;
};

FORCEINLINE void* DoubleStackAllocator::Allocate(usize size, usize alignment)
{
    return this->GetCurrentStack().Allocate(size, alignment);
}

FORCEINLINE void DoubleStackAllocator::Deallocate(void* ptr)
{
    return this->GetCurrentStack().Deallocate(ptr);
}

template<typename U, typename... Args>
FORCEINLINE U* DoubleStackAllocator::Allocate(Args&&... args)
{
    return this->GetCurrentStack().Allocate(std::forward<Args>(args)...);
}

template<typename T>
FORCEINLINE void DoubleStackAllocator::Deallocate(T* obj)
{
    return this->GetCurrentStack().Deallocate(obj);
}

FORCEINLINE StackAllocator& DoubleStackAllocator::GetCurrentStack()
{
    return m_Stacks[m_Current];
}

FORCEINLINE void DoubleStackAllocator::SwitchTo(uint8 id)
{
    m_Current = id % MAX_STACKS;
}

FORCEINLINE void DoubleStackAllocator::Swap()
{
    m_Current++;
    m_Current = (m_Current) % MAX_STACKS;
}

FORCEINLINE void DoubleStackAllocator::Free()
{
    for (uint8 i = 1; i < MAX_STACKS; i++){
        m_Stacks[i].Free();
    }
}

FORCEINLINE void DoubleStackAllocator::Resize(usize nSize)
{
    for (uint8 i = 1; i < MAX_STACKS; i++){
        m_Stacks[i].Resize(nSize);
    }
}

FORCEINLINE DoubleStackAllocator& DoubleStackAllocator::Reset()
{
    for (uint8 i = 1; i < MAX_STACKS; i++){
        m_Stacks[i].Reset();
    }
    return *this;
}

FORCEINLINE void DoubleStackAllocator::SetSize(usize nSize)
{
    for (uint8 i = 1; i < MAX_STACKS; i++){
        m_Stacks[i].SetSize(nSize);
    }
}

FORCEINLINE DoubleStackAllocator& DoubleStackAllocator::Init()
{
    for (uint8 i = 1; i < MAX_STACKS; i++){
        m_Stacks[i].Init();
    }
    return *this;
}

FORCEINLINE StackAllocator& DoubleStackAllocator::GetStackAllocatorFromID(uint8 id)
{
    return m_Stacks[id % MAX_STACKS];
}

FORCEINLINE uint8 DoubleStackAllocator::GetMaxCount()
{
    return MAX_STACKS;
}

TRE_NS_END