#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Core/Utils/AlignedAlloc.hpp>
#include <vector>
#include <memory>

TRE_NS_START

namespace Renderer
{
	template<typename T>
	class ObjectPool
	{
	public:
		template<typename... P>
		T* Allocate(P&&... p)
		{
			if (empty.empty()) {
				uint32 num_objects = 64;

				T* ptr = static_cast<T*>(AlignedAlloc(num_objects * sizeof(T), MAX(64, alignof(T));

				if (!ptr)
					return NULL;

				for (uint32 i = 0; i < num_objects; i++) {
					empty.push_back(&ptr[i]);
				}

				memory.emplace_back(ptr);
			}

			T* ptr = empty.back();
			empty.pop_back();

			new (ptr) T(std::forward<P>(p)...);
			return ptr;
		}

		void Free(T* ptr)
		{
			ptr->~T();
			empty.push_back(ptr);
		}

		void clear()
		{
			empty.clear();
			memory.clear();
		}
	protected:
		struct AlignedAllocDeleter
		{
			void operator()(T* ptr)
			{
				AlignedFree(ptr);
			}
		};

		std::vector<T*> empty;
		std::vector<std::unique_ptr<T*, AlignedAllocDeleter>> memory;
	};
}

TRE_NS_END