#pragma once

#include "Core/Misc/Defines/Common.hpp"
#include "RenderEngine/Variables/VariableDesc/VariableDesc.hpp"

TRE_NS_START

class IVariable
{
public:
    IVariable() : m_BasicVarDesc(NULL), m_Data(NULL), m_Next(NULL), m_IsAllocated(false)
    {}

    void* GetDataPtr() const { return m_Data; };

    VariableDesc* GetVariableDesc() const { return m_BasicVarDesc; };

    void SetNext(IVariable* next) { m_Next = next; };

    IVariable* GetNext() const { return m_Next; }

    ~IVariable() { 
        //if (m_IsAllocated)
        //    ::operator delete(m_Data);
    };

    IVariable(IVariable&& other) = default; 
    IVariable(const IVariable& other) = default; 
    IVariable& operator=(IVariable&& other) = default;
    IVariable& operator=(const IVariable& other) = default; 
protected:
    VariableDesc* m_BasicVarDesc = NULL;
    void* m_Data = NULL;
    IVariable* m_Next = NULL;
    bool m_IsAllocated = false;
};

TRE_NS_END