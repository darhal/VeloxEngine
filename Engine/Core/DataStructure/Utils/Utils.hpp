#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/DataStructure/Bitset/Bitset.hpp>
#include <Core/DataStructure/String/String.hpp>

TRE_NS_START

// Compile time counter resources: http://b.atch.se/posts/constexpr-counter/
namespace Detail
{
#if defined(COMPILER_MSVC)
	template<uint32 N>
	struct flag
	{
		friend constexpr uint32 adl_flag(flag<N>);
	};
	template<uint32 N>
	struct writer
	{
		friend constexpr uint32 adl_flag(flag<N>)
		{
			return N;
		}

		static constexpr uint32 value = N;
	};
	template<uint32 N, class = char[noexcept(adl_flag(flag<N>())) ? +1 : -1]>
	uint32 constexpr reader(uint32, flag<N>)
	{
		return N;
	}
	template<uint32 N>
	uint32 constexpr reader(float, flag<N>, uint32 R = reader(0, flag<N - 1>()))
	{
		return R;
	}
	uint32 constexpr reader(float, flag<0>)
	{
		return 0;
	}
#else
	template<uint32 N>
	struct flag
	{
		friend constexpr uint32 adl_flag(flag<N>);
	};
	template<uint32 N>
	struct writer
	{
		friend constexpr uint32 adl_flag(flag<N>)
		{
			return N;
		}

		static constexpr uint32 value = N;
	};
	template<uint32 N, uint32 = adl_flag(flag<N> {}) >
	uint32 constexpr reader(uint32, flag<N>)
	{
		return N;
	}

	template<uint32 N>
	uint32 constexpr reader(float, flag<N>, uint32 R = reader(0, flag<N - 1> {}))
	{
		return R;
	}

	uint32 constexpr reader(float, flag<0>)
	{
		return 0;
	}

#endif
}

struct Utils
{
	FORCEINLINE static String ToString(const Bitset& bitset)
	{
		uint32 len = (uint32) bitset.Length();
		String res(len + 1);

		for (uint32 i = 0; i < len; i++) {
			res[i] = (bitset[i] ? '1' : '0');
		}

		return res;
	}


#if defined(COMPILER_MSVC)
	template<uint32 N = 0, uint32 C = Detail::reader(uint32(0), Detail::flag<32>())>
	static uint32 constexpr NextID(uint32 R = Detail::writer<C + N>::value)
	{
		return R;
	}
#else
	template<uint32 N = 0>
	static uint32 constexpr NextID(uint32 R = Detail::writer < Detail::reader(0, Detail::flag<32> {}) + N >::value)
	{
		return R;
	}
#endif

};

TRE_NS_END