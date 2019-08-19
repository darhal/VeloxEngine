#pragma once

#include "IVariable.hpp"
#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Maths/Maths.hpp>

TRE_NS_START

struct VertexData
{
	VertexData(const vec3& p, const vec3& n, const vec2& t) : pos(p), normal(n), texture(t)
	{}
	vec3 pos, normal;
	vec2 texture;
};

template<class T, class VarDesc = VariableDesc>
class Variable : public IVariable
{
public:
    FORCEINLINE Variable(const VarDesc& var_desc, void* data = NULL);

    virtual ~Variable() {
        //if (IVariable::m_IsAllocated)
        //    ::operator delete(IVariable::m_Data);
    }

    FORCEINLINE T* GetPointer(usize i = 0);

    template<typename... Args>
    FORCEINLINE void Emplace(usize index, Args&&... args);

    template<typename... Args>
    FORCEINLINE T* Emplace(usize index, Args&&... args);

    const VarDesc& GetConcreteDesc() const { return m_VarDesc; };

    FORCEINLINE Variable(const Variable<T, VarDesc>& other);
    FORCEINLINE Variable(Variable<T, VarDesc>&& other);
    FORCEINLINE Variable<T, VarDesc>& operator=(const Variable<T, VarDesc>& other);
    FORCEINLINE Variable<T, VarDesc>& operator=(Variable<T, VarDesc>&& other);

private:
    CONSTEXPR static usize SIZE_OF_ONE_ELEMENT = sizeof(T);

    VarDesc m_VarDesc;
};

template<typename T, typename VarDesc>
FORCEINLINE Variable<T, VarDesc>::Variable(const VarDesc& var_desc, void* data) :  IVariable(), m_VarDesc(var_desc)
{
    m_VarDesc.size = SIZE_OF_ONE_ELEMENT;
    m_Data = data;
    m_BasicVarDesc = &m_VarDesc;

    if (m_Data == NULL){
        m_Data = ::operator new(m_VarDesc.size * m_VarDesc.count);
        m_IsAllocated = true;
    }
}

template<typename T, typename VarDesc>
FORCEINLINE T* Variable<T, VarDesc>::GetPointer(usize i)
{
    return (T*) m_Data + i;
}

template<typename T, typename VarDesc>
template<typename... Args>
FORCEINLINE void Variable<T, VarDesc>::Emplace(usize index, Args&&... args)
{
    if (index < m_VarDesc.count)
        return;
    
    new (m_Data + index) T(std::forward<Args>(args)...);
}

template<typename T, typename VarDesc>
template<typename... Args>
FORCEINLINE T* Variable<T, VarDesc>::Emplace(usize index, Args&&... args)
{
    if (index < m_VarDesc.count)
        return NULL;

    new (m_Data + index) T(std::forward<Args>(args)...);
    return m_Data + index;
}

template<typename T, typename VarDesc>
FORCEINLINE Variable<T, VarDesc>::Variable(const Variable<T, VarDesc>& other) : 
    m_VarDesc(other.m_VarDesc) 
{
    m_BasicVarDesc = other.m_BasicVarDesc;
    m_Data = other.m_Data;
    m_Next = other.m_Next;
    m_IsAllocated = other.m_IsAllocated;
}

template<typename T, typename VarDesc>
FORCEINLINE Variable<T, VarDesc>::Variable(Variable<T, VarDesc>&& other) : 
    m_VarDesc(std::move(other.m_VarDesc))
{
    m_BasicVarDesc = other.m_BasicVarDesc;
    m_Data = other.m_Data;
    m_Next = other.m_Next;
    m_IsAllocated = other.m_IsAllocated;
}

template<typename T, typename VarDesc>
FORCEINLINE Variable<T, VarDesc>& Variable<T, VarDesc>::operator=(const Variable<T, VarDesc>& other)
{
    m_VarDesc = other.m_VarDesc;
    m_BasicVarDesc = other.m_BasicVarDesc;
    m_Data = other.m_Data;
    m_Next = other.m_Next;
    m_IsAllocated = other.m_IsAllocated;
    return *this;
}

template<typename T, typename VarDesc>
FORCEINLINE Variable<T, VarDesc>& Variable<T, VarDesc>::operator=(Variable<T, VarDesc>&& other)
{
    m_VarDesc = other.m_VarDesc;
    m_BasicVarDesc = other.m_BasicVarDesc;
    m_Data = other.m_Data;
    m_Next = other.m_Next;
    m_IsAllocated = other.m_IsAllocated;
    other.~Variable();
    return *this;
}

TRE_NS_END