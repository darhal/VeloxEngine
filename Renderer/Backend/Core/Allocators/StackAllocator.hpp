#pragma once

#include <Renderer/Backend/Common.hpp>

TRE_NS_START

namespace Renderer
{
	namespace Utils
	{
		template<typename T, size_t N = 512>
		class StackAllocator
		{
		public:
			StackAllocator() : offset(0) {}

			T* Allocate(size_t count = 1)
			{
				if (count == 0)
					return NULL;
			
				if (offset + count > N)
					return NULL;

				T* ret = (T*)buffer + offset;
				offset += count;
				return ret;
			}

			T* AllocateInit(size_t count, const T& obj = T())
			{
				T* ret = this->Allocate(count);

				if (ret)
					std::fill(ret, ret + count, obj);

				return ret;
			}

			void Reset()
			{
				offset = 0;
				// TODO: do specilized function is T is not pod then call objects dtor
			}

			void SetOffset(size_t off) { offset = off; }

			size_t GetElementCount() const { return offset; }

			const T* GetData() const { return (T*)buffer; }
		private:
			alignas(T) char buffer[sizeof(T) * N];
			size_t offset;
		};
	}
}

TRE_NS_END