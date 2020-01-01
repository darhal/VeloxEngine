#pragma once
#include <Core/Misc/Defines/Common.hpp>
#include <Core/ECS/Component/BaseComponent.hpp>

TRE_NS_START

template<typename Component>
class ArchetypeChunkIterator
{
public:
	template<typename PointerType>
	class GenericIterator;

	typedef GenericIterator<Component> Iterator;
	typedef GenericIterator<const Component> CIterator;
public:
	ArchetypeChunkIterator(Component* comp_buff, uint32 count) : m_ComponentBuffer(comp_buff), m_EntitiesCount(count)
	{}

	Iterator begin() { return Iterator(m_ComponentBuffer, 0); }
	Iterator end() { return Iterator(m_ComponentBuffer, m_EntitiesCount); }

	const Iterator begin() const { return Iterator(m_ComponentBuffer, 0); }
	const Iterator end() const { return Iterator(m_ComponentBuffer, m_EntitiesCount); }

	const CIterator cbegin() const { return CIterator(m_ComponentBuffer, 0); }
	const CIterator cend() const { return CIterator(m_ComponentBuffer, m_EntitiesCount); }
private:
	Component* m_ComponentBuffer;
	uint32 m_EntitiesCount;

private:
	template<typename DataType>
	class GenericIterator
	{
	public:
		GenericIterator(Component* comp_buff) noexcept : m_ComponentBuffer(comp_buff), m_CurrentComponentIndex(0) {}
		GenericIterator(Component* comp_buff, uint32 index) noexcept : m_ComponentBuffer(comp_buff), m_CurrentComponentIndex(index) {}
		bool operator!=(const Iterator& iterator) { return m_CurrentComponentIndex != iterator.m_CurrentComponentIndex; }

		DataType& operator*() { return (*(Component*) &m_ComponentBuffer[m_CurrentComponentIndex]); }
		const DataType& operator*() const { return (*(Component*) &m_ComponentBuffer[m_CurrentComponentIndex]); }

		DataType* operator->() { return (Component*) &m_ComponentBuffer[m_CurrentComponentIndex]; }
		const DataType* operator->() const { return (Component*) &m_ComponentBuffer[m_CurrentComponentIndex]; }

		GenericIterator& operator++()
		{
			m_CurrentComponentIndex++;
			return *this;
		}

		GenericIterator operator++(int)
		{
			GenericIterator iterator = *this;
			++*this;
			return iterator;
		}

	private:
		Component* m_ComponentBuffer;
		uint32 m_CurrentComponentIndex;
	};
};

TRE_NS_END