#pragma once

#include <Renderer/Backend/Common.hpp>

TRE_NS_START

namespace Renderer
{
	namespace Utils
	{
		template <class integral>
		constexpr bool IsAligned(integral x, size_t a) noexcept
		{
			return (x & (integral(a) - 1)) == 0;
		}

		template <class integral>
		constexpr integral AlignUp(integral x, size_t a) noexcept
		{
			return integral((x + (integral(a) - 1)) & ~integral(a - 1));
		}

		template <class integral>
		constexpr integral AlignDown(integral x, size_t a) noexcept
		{
			return integral(x & ~integral(a - 1));
		}
	}
}

TRE_NS_END