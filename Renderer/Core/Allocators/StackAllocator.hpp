#pragma once

#include <Renderer/Common.hpp>

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
				ASSERTF(count == 0, "Can't allocate 0 element");

				if (offset + count > N)
					return NULL;

				T* ret = buffer + offset;
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

			const T* GetData() const { return buffer; }
		private:
			T buffer[N];
			size_t offset;
		};
	}
}

TRE_NS_END