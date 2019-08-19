#pragma once

#include "Core/Misc/Defines/Common.hpp"
#include "RenderEngine/Variables/Variables/IVariable.hpp"

TRE_NS_START

template<class T = IVariable>
class VariableSet
{
public:
    typedef T IVar;
    template<typename DataType>
	class GIterator;

	typedef GIterator<IVar> Iterator;
	typedef GIterator<const IVar> CIterator;

public:
    VariableSet() : m_Root(NULL), m_TotalSize(0), m_Stide(0)
    {}

    FORCEINLINE void Add(IVar* ptr);

    usize GetTotalSize() const { return m_TotalSize; };

    usize GetStride() const { return m_Stide; };

    Iterator begin() { return Iterator(m_Root); }
	Iterator end() { return Iterator(NULL); }

	const Iterator begin() const { return Iterator(m_Root); }
	const Iterator end() const { return Iterator(NULL); }

	const CIterator cbegin() const { return CIterator(m_Root); }
	const CIterator cend() const { return CIterator(NULL); }

private:
    IVar* m_Root;
    usize m_TotalSize;
    usize m_Stide;
    
public:
    template<typename DataType>
	class GIterator
	{
	public:
		GIterator() noexcept : m_CurrentNode(m_Root) { }
		GIterator(DataType* node) noexcept : m_CurrentNode(node) { }
		bool operator!=(const Iterator& iterator) { return m_CurrentNode != iterator.m_CurrentNode; }
		
		DataType& operator*() { return *m_CurrentNode; }
		const DataType& operator*() const { return (*m_CurrentNode); }

		DataType* operator->() { return m_CurrentNode; }
		const DataType* operator->() const { return m_CurrentNode; }

		GIterator& operator=(DataType* node)
		{
			this->m_CurrentNode = node;
			return *this;
		}

		GIterator& operator++()
		{
			if (m_CurrentNode)
				m_CurrentNode = m_CurrentNode->GetNext();
			return *this;
		}

		GIterator operator++(int)
		{
			GIterator iterator = *this;
			++*this;
			return iterator;
		}

	private:
		DataType* m_CurrentNode;
	};
};

template<typename T>
FORCEINLINE void VariableSet<T>::Add(IVar* ptr)
{
    if (m_Root == NULL){
        m_Root = ptr;
    }

    m_Root->SetNext(ptr);
    m_TotalSize += ptr->GetVariableDesc().count;
    m_Stide += ptr->GetVariableDesc().size;
}

TRE_NS_END