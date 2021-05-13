#pragma once
#include <Renderer/Backend/Common.hpp>

TRE_NS_START

namespace Renderer
{
	namespace Utils
	{
		struct ArrayView
		{
			void*  data  = 0;
			size_t count = 0;
		};

		template<typename T>
		struct ArrayViewItr
		{
			const ArrayView& view;

			ArrayViewItr(const ArrayView& view) : view(view)
			{
			}

			T& operator[](size_t i)
			{
				return ((T*)view.data)[i];
			}

			const T& operator[](size_t i) const
			{
				return ((T*)view.data)[i];
			}

			T* begin()
			{
				return (T*)view.data;
			}

			T* end()
			{
				return (T*)view.data + view.count;
			}

			const T* begin() const
			{
				return (T*)view.data;
			}

			const T* end() const
			{
				return (T*)view.data + view.count;
			}
		};
	}
}

TRE_NS_END